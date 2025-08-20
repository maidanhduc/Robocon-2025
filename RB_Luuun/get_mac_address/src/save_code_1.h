/*
// 14:33:5C:0A:9B:20 -> {0x14, 0x33, 0x5C, 0x0A, 0x9B, 0x20} -> {20, 51, 92, 10, 155, 32}
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Cấu hình phần cứng
#define INTERRUPT_PIN 18         // Chân ngắt encoder
#define LED_PIN 2                // Chân LED báo trạng thái
#define SEND_INTERVAL 50         // Gửi dữ liệu mỗi 50ms (20Hz)
#define PULSES_PER_REVOLUTION 13 // Số xung mỗi vòng quay encoder
#define ENCODER_MAX_RPM 15000    // RPM tối đa dự kiến
#define MIN_PULSE_INTERVAL 5     // Khoảng cách tối thiểu giữa các xung (µs)

// Cấu trúc dữ liệu ESP-NOW
#pragma pack(push, 1)
typedef struct {
    uint16_t speed_shot;  // Tốc độ động cơ (RPM)
    uint16_t pwm_shot;    // Giá trị PWM (0-1000)
    uint16_t speed_set;   // Tốc độ đặt (từ UART)
    uint16_t value_4;     // Giá trị tùy chọn 4 (từ UART)
    uint16_t new_value;   // Giá trị tùy chọn 5 (từ UART)
    uint8_t checksum;     // Checksum kiểm tra
} espnow_data_t;
#pragma pack(pop)

// Cấu trúc dữ liệu UART
#pragma pack(push, 1)
typedef struct {
    uint16_t speed_set;   // 2 byte
    uint16_t pwm;         // 2 byte
    uint16_t var_1;       // 2 byte
    uint8_t status;       // 1 byte
} uart_data_t;           // Tổng: 7 byte
#pragma pack(pop)

// Biến toàn cục
volatile uint32_t pulse_count = 0;
volatile uint32_t last_pulse_time = 0;
volatile uint32_t pulse_interval_sum = 0;
volatile uint16_t pulse_interval_count = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

espnow_data_t send_data;
uart_data_t uart_received;
uint8_t broadcast_mac[] = {0x14, 0x33, 0x5C, 0x0A, 0x9B, 0x20}; // MAC của ESP32 (thay đổi theo thiết bị của bạn) a truong
//uint8_t broadcast_mac[] = {0x5C, 0x01, 0x3B, 0x33, 0x9C, 0x84}; mac của esp32 me
unsigned long last_espnow_send = 0;
uint16_t speed_send = 0;

// ==================== HÀM XỬ LÝ NGẮT CẢI TIẾN ====================
void IRAM_ATTR handleInterrupt() {
    uint32_t current_time = micros();
    uint32_t interval = current_time - last_pulse_time;
    last_pulse_time = current_time;
    
    // Lọc nhiễu - bỏ qua các xung quá gần nhau
    if (interval > MIN_PULSE_INTERVAL) {
        portENTER_CRITICAL_ISR(&mux);
        pulse_count++;
        pulse_interval_sum += interval;
        pulse_interval_count++;
        portEXIT_CRITICAL_ISR(&mux);
    }
}

// ==================== TÍNH TOÁN RPM CẢI TIẾN ====================
uint16_t calculate_rpm() {
    static uint32_t last_calculation_time = 0;
    uint32_t current_time = micros();
    
    // Chỉ tính toán mỗi SEND_INTERVAL ms
    if (current_time - last_calculation_time < SEND_INTERVAL * 1000) {
        return speed_send; // Trả về giá trị cũ nếu chưa đến thời điểm tính
    }
    
    uint32_t pulses;
    uint32_t total_interval;
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
    uint32_t elapsed_time = current_time - last_calculation_time;
    
    // Tính RPM theo 2 phương pháp
    uint16_t rpm_pulse = 0;
    uint16_t rpm_interval = 0;
    
    // Phương pháp 1: Đếm xung trong khoảng thời gian
    if (pulses > 0 && elapsed_time > 0) {
        rpm_pulse = (pulses * 60000000UL) / (PULSES_PER_REVOLUTION * elapsed_time);
    }
    
    // Phương pháp 2: Tính trung bình khoảng cách giữa các xung
    if (pulse_count_local > 0) {
        uint32_t avg_interval = total_interval / pulse_count_local;
        if (avg_interval > 0) {
            rpm_interval = 60000000UL / (PULSES_PER_REVOLUTION * avg_interval);
        }
    }
    
    // Kết hợp 2 phương pháp (ưu tiên phương pháp khoảng cách khi tốc độ cao)
    uint16_t current_rpm = (rpm_interval > 3000) ? rpm_interval : 
                          ((rpm_pulse + rpm_interval) / 2);
    
    // Bộ lọc thích ứng
    static uint16_t filtered_rpm = 0;
    uint8_t filter_factor = (current_rpm > 5000) ? 1 : 3;
    filtered_rpm = (filtered_rpm * filter_factor + current_rpm) / (filter_factor + 1);
    
    // Giới hạn giá trị RPM
    filtered_rpm = min(filtered_rpm, (uint16_t)ENCODER_MAX_RPM);
    speed_send = filtered_rpm;
    
    return filtered_rpm;
}

// ==================== TÍNH CHECKSUM ====================
uint8_t calculate_checksum(const espnow_data_t* data) {
    const uint8_t* bytes = (const uint8_t*)data;
    uint8_t sum = 0;
    for(size_t i = 0; i < sizeof(espnow_data_t)-1; i++) {
        sum += bytes[i];
    }
    return 255 - sum;
}

// ==================== CALLBACK ESP-NOW ====================
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    digitalWrite(LED_PIN, status == ESP_NOW_SEND_SUCCESS);
}

// ==================== GỬI TỐC ĐỘ QUA UART ==================== // khong gửi tốc độ qua uart
void send_speed() {
    uint8_t data_tx[2];
    if (Serial2.available() > 0) {
        data_tx[0] = Serial2.read();
        data_tx[1] = Serial2.read();
        Serial2.write(data_tx, sizeof(data_tx));
        Serial.printf("Gửi tốc độ: %u\n", (data_tx[0] << 8) | data_tx[1]);
    }
}

// ==================== NHẬN UART Từ Arduino ====================
void receive_uart() {
    static uint8_t buffer[sizeof(uart_data_t)];
    static uint8_t idx = 0;
    static unsigned long last_byte_time = 0;

    while (Serial2.available() > 0) {
        buffer[idx++] = Serial2.read();
        last_byte_time = millis();

        if (idx >= sizeof(uart_data_t)) {
            portENTER_CRITICAL(&mux);
            memcpy(&uart_received, buffer, sizeof(uart_data_t));
            portEXIT_CRITICAL(&mux);

            Serial.printf("ESP32 Nhận: Set=%u, PWM=%u, Var1=%u, Status=%u\n",
                uart_received.speed_set,
                uart_received.pwm,
                uart_received.var_1,
                uart_received.status);

            idx = 0;
        }
    }

    if (idx > 0 && millis() - last_byte_time > 100) {
        Serial.println("Lỗi: Timeout, yêu cầu gửi lại!");
        Serial2.write("ERROR");  // Gửi tín hiệu lỗi về Arduino
        idx = 0;
    }
}

// ==================== CẤU HÌNH BAN ĐẦU ====================
void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 17, 16); // RX, TX
    
    pinMode(LED_PIN, OUTPUT);
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), handleInterrupt, RISING);

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    
    if(esp_now_init() != ESP_OK) {
        Serial.println("Lỗi khởi tạo ESP-NOW");
        ESP.restart();
    }

    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, broadcast_mac, 6);
    if(esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Lỗi thêm peer");
        ESP.restart();
    }

    esp_now_register_send_cb(OnDataSent);
    Serial.println("Khởi động hoàn tất");
}

// ==================== VÒNG LẶP CHÍNH ====================
void loop() {
    uint32_t current_time = millis();
    
    // 1. Nhận dữ liệu UART
    receive_uart();
    
    // 2. Gửi ESP-NOW định kỳ
    if(current_time - last_espnow_send >= SEND_INTERVAL) {
        last_espnow_send = current_time;
        
        // Tính toán RPM
        uint16_t current_rpm = calculate_rpm();
        
        // Cập nhật dữ liệu gửi
        send_data.speed_shot = current_rpm;
        send_data.pwm_shot = uart_received.pwm;
        send_data.speed_set = uart_received.speed_set;
        send_data.value_4 = uart_received.var_1;
        send_data.new_value = uart_received.status;
        send_data.checksum = calculate_checksum(&send_data);
        
        // Gửi dữ liệu
        esp_err_t result = esp_now_send(broadcast_mac, (uint8_t*)&send_data, sizeof(send_data));
        
        // Debug (có thể bỏ khi hoạt động ổn định)
        static uint32_t last_debug_time = 0;
        if (millis() - last_debug_time > 1000) {
            last_debug_time = millis();
            Serial.printf("RPM: %u, PWM: %u, Set: %u\n", 
                current_rpm, uart_received.pwm, uart_received.speed_set);
        }
    }
    
    // 3. Xử lý gửi tốc độ nếu có yêu cầu từ UART
   // send_speed();
    
    delay(1);
}
// bây giờ yêu cầu hiển thị 2 mode : mode_ball and mode_area
// nut select : mode_ball , nut_start : mode_area
/*
   ý tưởng thiết kế giao diện :
                    speed:xxx
                    pwm:xxx
        mode_ball : {1,2,3,4}; mode_area : {1,2,3,4}
        default_pwm_1 : xx ; default_speed_1:xxxxx;
        default_pwm_2 : xx ; default_speed_2:xxxxx;
        default_pwm_3 : xx ; default_speed_3:xxxxx;
        default_pwm_4 : xx ; default_speed_4:xxxxx;
    ý tưởng điều khiển :
        +--------về phần arduino---------+
          khi chuyển các mode_are tự động gán các giá trị : default_pwm_1, default_pwm_2, default_pwm_3, default_pwm_4  
                                                            default_speed_1, default_speed_2, default_speed_3, default_speed_4     theo từng mode_area{1,2,3,4}
                                                            các giá trị default theo mode_area sẽ được người dùng gán khi test sân và test bóng 
          truyền các  giá trị : pwm, default_pwm_1, default_pwm_2, default_pwm_3, default_pwm_4 theo mode_area{1,2,3,4}
        +--------về phần esp32---------+
          nhận các giá trị :  default_speed_1, default_speed_2, default_speed_3, default_speed_4  
                              default_pwm_1, default_pwm_2, default_pwm_3, default_pwm_4           theo mode_ball{1,2,3,4} và mode khu vựcvực
                              pwm, mode_ball, mode_area 
            
          gửi các giá trị : speed,pwm, defaultdefaulttheo mode_ball{1,2,3,4} và mode khu vực
*/

