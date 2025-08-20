// Chương trình esp32 sử dụng 2 lõi, CPU 0 dùng đê xử lý ngắt, CPU 1 dùng để xử lý vòng lặp chính: các task esp now , uart
// có 2 task tương ứng với 2 lõi, task xử lý ngắt(InterruptTask) và task chính(main_loopTask())
// Chương trình này sử dụng ESP-NOW để gửi dữ liệu giữa các thiết bị ESP32, nhận dữ liệu từ UART và xử lý ngắt từ encoder
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Cấu hình phần cứng
#define INTERRUPT_PIN 18  // Chân ngắt encoder
#define LED_PIN 2         // Chân LED báo trạng thái
#define SEND_INTERVAL 50  // Gửi dữ liệu mỗi 50ms (20Hz)

// Cấu trúc dữ liệu ESP-NOW
#pragma pack(push, 1)
typedef struct {
    uint16_t speed_shot;
    uint16_t pwm_shot;
    uint16_t speed_set;
    uint16_t value_4;
    uint16_t new_value;
    uint8_t checksum;
} espnow_data_t;
#pragma pack(pop)

// Cấu trúc dữ liệu UART
#pragma pack(push, 1)
typedef struct {
    uint16_t speed_set;
    uint16_t pwm;
    uint16_t var_1;
    uint8_t status;
} uart_data_t;
#pragma pack(pop)

// Biến toàn cục
volatile uint32_t pulse_count = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
espnow_data_t send_data;
uart_data_t uart_received;
uint8_t broadcast_mac[] = {0x5C, 0x01, 0x3B, 0x33, 0x9C, 0x84};
unsigned long last_rpm_calc_time = 0;
unsigned long last_espnow_send = 0;

// Task handle
TaskHandle_t InterruptTaskHandle = NULL;
// ==================== TÍNH TOÁN RPM ====================
uint16_t calculate_rpm() {
    uint32_t current_time = millis();
    uint32_t interval = current_time - last_rpm_calc_time;
    if (interval < 1000) return 0;
    portENTER_CRITICAL(&mux);
    uint32_t current_pulse = pulse_count;
    pulse_count = 0;
    portEXIT_CRITICAL(&mux);
    uint32_t rpm = (current_pulse * 60000UL) / (13 * interval);
    last_rpm_calc_time = current_time;
    static uint16_t filtered_rpm = 0;
    filtered_rpm = (filtered_rpm * 3 + rpm) / 4;
    return (filtered_rpm > 12000) ? 12000 : filtered_rpm;
}
// ==================== HÀM XỬ LÝ NGẮT ====================
void IRAM_ATTR handleInterrupt() {
    portENTER_CRITICAL_ISR(&mux);
    pulse_count++;
    portEXIT_CRITICAL_ISR(&mux);
}

void InterruptTask(void *parameter) {
    while (1) {
        calculate_rpm();
        vTaskDelay(50 / portTICK_PERIOD_MS);  // Đợi 50ms để cập nhật RPM
    }
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

void receive_uart() {
    static uint8_t buffer[sizeof(uart_data_t)];
    static uint8_t idx = 0;
    while (Serial2.available() > 0) {
        buffer[idx++] = Serial2.read();
        if (idx >= sizeof(uart_data_t)) {
            portENTER_CRITICAL(&mux);
            memcpy(&uart_received, buffer, sizeof(uart_data_t));
            portEXIT_CRITICAL(&mux);
            //Serial.printf("ESP32 Nhận: Set=%u, PWM=%u, Var1=%u, Status=%u\n",
            //    uart_received.speed_set, uart_received.pwm, uart_received.var_1, uart_received.status);
            idx = 0;
        }
    }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    digitalWrite(LED_PIN, status == ESP_NOW_SEND_SUCCESS);
}

void main_loopTask(void *parameter) {
    while (1) {
        uint32_t current_time = millis();
        receive_uart();
        /*uint16_t current_rpm = calculate_rpm();
        Serial.printf("RPM: %u\n", current_rpm);
        // Gửi dữ liệu ESP-NOW định kỳ*/
        if(current_time - last_espnow_send >= SEND_INTERVAL) {
          uint16_t current_rpm = calculate_rpm();
          Serial.printf("RPM: %u\n", current_rpm);
            last_espnow_send = current_time;
            send_data.speed_shot = current_rpm;
            send_data.pwm_shot = uart_received.pwm;
            send_data.speed_set = uart_received.speed_set;
            send_data.value_4 = uart_received.var_1;
            send_data.new_value = uart_received.status;
            send_data.checksum = calculate_checksum(&send_data);
            esp_now_send(broadcast_mac, (uint8_t*)&send_data, sizeof(send_data));
            //Serial.printf("Đã gửi: RPM=%u, PWM=%u, Set=%u\n", send_data.speed_shot, send_data.pwm_shot, send_data.speed_set);
        }
        vTaskDelay(1 / portTICK_PERIOD_MS); //// Đợi 1ms trước khi lặp lại
    }
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 17, 16); // RX, TX
    pinMode(LED_PIN, OUTPUT);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    if(esp_now_init() != ESP_OK) {
        ESP.restart();
    }
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcast_mac, 6);
    esp_now_add_peer(&peerInfo);
    esp_now_register_send_cb(OnDataSent);

    // Tạo task xử lý ngắt trên CPU 0
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), handleInterrupt, RISING);
    xTaskCreatePinnedToCore(InterruptTask,  // Hàm xử lý task
                        "InterruptTask", // Tên task
                        1000,            // Kích thước stack (bytes)
                        NULL,            // Tham số truyền vào task (không dùng)
                        2,               // Độ ưu tiên (ưu tiên cao hơn loop)
                        &InterruptTaskHandle, // Handle để quản lý task
                        0);              // Gán task vào lõi 0

    // Tạo task chính trên CPU 1
    xTaskCreatePinnedToCore(main_loopTask, "main_loopTask", 4000, NULL, 1, NULL, 1);
}

void loop() { vTaskDelete(NULL); }  // sẽ xoá task loop() ngay khi ESP32 khởi động
// không cần dùng đến, vì đã tạo task riêng cho vòng lặp chính
// Chương trình sẽ chạy trên task main_loopTask() và xử lý ngắt trên task InterruptTask()
