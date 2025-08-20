#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Cấu hình phần cứng
#define INTERRUPT_PIN 18
#define LED_PIN 2
#define SEND_INTERVAL 50
#define PULSES_PER_REVOLUTION 13
#define ENCODER_MAX_RPM 15000
#define MIN_PULSE_INTERVAL 5

// Cấu hình UART
#define UART_START_BYTE 0xAA
#define UART_END_BYTE 0x55
#define UART_TIMEOUT_MS 100
#define UART_BUFFER_SIZE (sizeof(uart_data_t) + 3) // +3 cho start, end, checksum

// Cấu trúc dữ liệu ESP-NOW
#pragma pack(push, 1)
typedef struct {
    uint16_t speed_shot;
    uint8_t pwm_shot;
    uint8_t mode_ball;
    uint8_t mode_area;
    uint8_t default_pwm[4];
    uint16_t default_speed[4];
    uint8_t checksum;
} espnow_data_t;
#pragma pack(pop)

// Cấu trúc dữ liệu UART
#pragma pack(push, 1)
typedef struct {
    uint8_t pwm;
    uint8_t mode_ball;
    uint8_t mode_area;
    uint8_t default_pwm[4];
    uint16_t default_speed[4];
} uart_data_t;
#pragma pack(pop)

// Biến toàn cục
volatile uint32_t pulse_count = 0;
volatile uint32_t last_pulse_time = 0;
volatile uint32_t pulse_interval_sum = 0;
volatile uint16_t pulse_interval_count = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

espnow_data_t send_data;
uart_data_t uart_received;
uint8_t broadcast_mac[] = {0x5C, 0x01, 0x3B, 0x33, 0x9C, 0x84};

unsigned long last_espnow_send = 0;
uint16_t speed_send = 0;

// Ngắt encoder
void IRAM_ATTR handleInterrupt() {
    uint32_t current_time = micros();
    uint32_t interval = current_time - last_pulse_time;
    last_pulse_time = current_time;

    if (interval > MIN_PULSE_INTERVAL) {
        portENTER_CRITICAL_ISR(&mux);
        pulse_count++;
        pulse_interval_sum += interval;
        pulse_interval_count++;
        portEXIT_CRITICAL_ISR(&mux);
    }
}

// Tính RPM
uint16_t calculate_rpm() {
    static uint32_t last_calculation_time = 0;
    uint32_t current_time = micros();

    if (current_time - last_calculation_time < SEND_INTERVAL * 1000) {
        return speed_send;
    }

    uint32_t pulses, total_interval;
    uint16_t pulse_count_local;

    portENTER_CRITICAL(&mux);
    pulses = pulse_count;
    pulse_count = 0;
    total_interval = pulse_interval_sum;
    pulse_count_local = pulse_interval_count;
    pulse_interval_sum = 0;
    pulse_interval_count = 0;
    portEXIT_CRITICAL(&mux);

    last_calculation_time = current_time;

    uint16_t rpm_pulse = 0;
    uint16_t rpm_interval = 0;

    if (pulses > 0) {
        rpm_pulse = (pulses * 60000000UL) / (PULSES_PER_REVOLUTION * SEND_INTERVAL * 1000);
    }

    if (pulse_count_local > 0) {
        uint32_t avg_interval = total_interval / pulse_count_local;
        if (avg_interval > 0) {
            rpm_interval = 60000000UL / (PULSES_PER_REVOLUTION * avg_interval);
        }
    }

    uint16_t current_rpm = (rpm_interval > 3000) ? rpm_interval : ((rpm_pulse + rpm_interval) / 2);

    static uint16_t filtered_rpm = 0;
    uint8_t filter_factor = (current_rpm > 5000) ? 1 : 3;
    filtered_rpm = (filtered_rpm * filter_factor + current_rpm) / (filter_factor + 1);

    filtered_rpm = min(filtered_rpm, (uint16_t)ENCODER_MAX_RPM);
    speed_send = filtered_rpm;

    return filtered_rpm;
}

// Tính checksum
uint8_t calculate_checksum(void* data, size_t size) {
    uint8_t* bytes = (uint8_t*)data;
    uint8_t sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += bytes[i];
    }
    return 255 - sum;
}

// Callback ESP-NOW
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    digitalWrite(LED_PIN, status == ESP_NOW_SEND_SUCCESS);
}

// Kiểm tra dữ liệu UART hợp lệ
bool validate_uart_data(const uart_data_t* data) {
    if (data->mode_ball < 1 || data->mode_ball > 4) return false;
    if (data->mode_area < 1 || data->mode_area > 4) return false;
    if (data->pwm > 255) return false;
    
    for (int i = 0; i < 4; i++) {
        if (data->default_pwm[i] > 255) return false;
        if (data->default_speed[i] > ENCODER_MAX_RPM) return false;
    }
    
    return true;
}

// Nhận UART với cơ chế mới
void receive_uart() {
    static uint8_t buffer[UART_BUFFER_SIZE];
    static uint8_t idx = 0;
    static bool receiving = false;
    static unsigned long last_byte_time = 0;

    while (Serial2.available() > 0) {
        uint8_t incoming = Serial2.read();
        unsigned long current_time = millis();

        // Timeout reset
        if (receiving && (current_time - last_byte_time > UART_TIMEOUT_MS)) {
            receiving = false;
            idx = 0;
            Serial.println("UART timeout - resetting");
        }

        last_byte_time = current_time;

        // Bắt đầu frame
        if (!receiving && incoming == UART_START_BYTE) {
            receiving = true;
            buffer[0] = incoming;
            idx = 1;
            continue;
        }

        // Nhận dữ liệu
        if (receiving) {
            // Kiểm tra kích thước buffer
            if (idx >= UART_BUFFER_SIZE) {
                receiving = false;
                idx = 0;
                continue;
            }

            buffer[idx++] = incoming;

            // Kết thúc frame
            if (incoming == UART_END_BYTE && idx == UART_BUFFER_SIZE) {
                // Kiểm tra checksum
                
                uint8_t received_checksum = buffer[UART_BUFFER_SIZE - 2];
                uint8_t calculated_checksum = calculate_checksum(&buffer[1], sizeof(uart_data_t));
                
                if (received_checksum == calculated_checksum) {
                    // Sao chép dữ liệu hợp lệ
                    uart_data_t temp_data;
                    memcpy(&temp_data, &buffer[1], sizeof(uart_data_t));
                    
                    if (validate_uart_data(&temp_data)) {
                        portENTER_CRITICAL(&mux);
                        memcpy(&uart_received, &temp_data, sizeof(uart_data_t));
                        portEXIT_CRITICAL(&mux);
                        
                        //Serial.println("UART data received and validated");
                    } else {
                        Serial.println("Invalid data values");
                    }
                } else {
                    Serial.println("Checksum error");
                }
                
                receiving = false;
                idx = 0;
            }
        }
    }
}

// Hiển thị thông tin
void displayInfo() {
    static unsigned long last_display_time = 0;

    if (millis() - last_display_time > 500) {
        last_display_time = millis();

        Serial.println("\n===== SYSTEM INFO =====");
        Serial.printf("Speed: %d RPM\n", speed_send);
        Serial.printf("PWM: %d\n", uart_received.pwm);
        Serial.printf("Mode Ball: %d, Mode Area: %d\n",
            uart_received.mode_ball, uart_received.mode_area);

        Serial.println("Default Values:");
        for (int i = 0; i < 4; i++) {
            Serial.printf("Area %d: PWM=%d, Speed=%d\n",
                i + 1, uart_received.default_pwm[i], uart_received.default_speed[i]);
        }
        Serial.println("=====================");
    }
}

// Khởi tạo
void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 17, 16); //  RX, TX

    pinMode(LED_PIN, OUTPUT);
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), handleInterrupt, RISING);

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Lỗi khởi tạo ESP-NOW");
        ESP.restart();
    }

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcast_mac, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Lỗi thêm peer");
        ESP.restart();
    }

    esp_now_register_send_cb(OnDataSent);

    memset(&uart_received, 0, sizeof(uart_data_t));
    uart_received.mode_ball = 1;
    uart_received.mode_area = 1;

    Serial.println("Khởi động hoàn tất");
}

// Vòng lặp chính
void loop() {
    uint32_t current_time = millis();

    receive_uart();

    if (current_time - last_espnow_send >= SEND_INTERVAL) {
        last_espnow_send = current_time;

        uint16_t current_rpm = calculate_rpm();

        send_data.speed_shot = current_rpm;
        send_data.pwm_shot = uart_received.pwm;
        send_data.mode_ball = uart_received.mode_ball;
        send_data.mode_area = uart_received.mode_area;
        memcpy(send_data.default_pwm, uart_received.default_pwm, 4);
        memcpy(send_data.default_speed, uart_received.default_speed, 8);
        send_data.checksum = calculate_checksum(&send_data, sizeof(send_data) - 1);

        esp_now_send(broadcast_mac, (uint8_t*)&send_data, sizeof(send_data));
    }

    displayInfo();

    delay(1);
}



  