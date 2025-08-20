#ifndef UART_ST_H
#define UART_ST_H

#include "set_ball_and_area.h"

uint8_t shoot_speed = 80;
bool flag_auto_pwm = false;

uint8_t data_rx[2];
uint8_t data_rx_angle[2];
uint16_t but_data = 0x00;
int16_t cur_angle;
uint32_t time_but = 0;
int16_t joy_LX;int16_t joy_LY;int16_t joy_RX;int16_t joy_RY;

struct moto_data{
  uint8_t dir;
  uint8_t speed;
  uint8_t motor_num;
};
moto_data m1;



void UART_get_button(){
  Serial2.write('d');
  if(Serial2.available()){
    Serial2.readBytes((char*)&but_data, 2);
    Serial2.readBytes((char*)&joy_LX, 2);
    Serial2.readBytes((char*)&joy_LY, 2);
    Serial2.readBytes((char*)&joy_RX, 2);
    Serial2.readBytes((char*)&joy_RY, 2);
    Serial.println(but_data); 
  }
}
void UART_get_angle(){
  Serial3.write('z');
  delay(5);
  if(Serial3.available()){
    data_rx_angle[0] = Serial3.read();
    data_rx_angle[1] = Serial3.read();
    cur_angle = data_rx_angle[0] << 8 | data_rx_angle[1];
    //Serial.println(cur_angle);
  }
}
// mix code for me
// select : 8192 ; start : 16384
// Cấu hình UART
#define UART_START_BYTE 0xAA
#define UART_END_BYTE 0x55
#define SEND_INTERVAL 50
#define MAX_RETRIES 3

uint8_t auto_pwm = 0;
// Biến logic 
bool sta_auto_pwm = false;
bool flag_auto_1 = true;
bool flag_auto_2 = true;
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

uart_data_t tx_data;
unsigned long last_send_time = 0;
uint8_t retry_count = 0;

// Hàm tính checksum
uint8_t calculate_checksum(void* data, size_t size) {
    uint8_t* bytes = (uint8_t*)data;
    uint8_t sum = 0;
    for(size_t i = 0; i < size; i++) {
        sum += bytes[i];
    }
    return 255 - sum;
}

void ctrl_mode() {
  // =========== xử lý nút bấm ; chuyển đổi mode ==== //
  static bool flag_select = true;  // Sử dụng static để giữ trạng thái giữa các lần gọi hàm
  static bool flag_start = true;   // Sử dụng static để giữ trạng thái giữa các lần gọi hàm

  // Xử lý nút select
  if (but_data == 8192) {
    if (flag_select) {
      flag_auto_pwm = true;
      tx_data.mode_ball += 1;
      if (tx_data.mode_ball >= 5) tx_data.mode_ball = 1;
      flag_select = false;  // Chặn không cho tăng liên tục khi giữ nút
    }
  } else {
    flag_select = true;     // Reset cờ khi thả nút
  }

  // Xử lý nút start
  if (but_data == 16384) {
    flag_auto_pwm = true;
    if (flag_start) {
      tx_data.mode_area += 1;
      if (tx_data.mode_area >= 5) tx_data.mode_area = 1;
      flag_start = false;   // Chặn không cho tăng liên tục khi giữ nút
    }
  } else {
    flag_start = true;      // Reset cờ khi thả nút
  }
  // Serial.print("mode_ball: ");Serial.println(tx_data.mode_ball);
  // Serial.print("mode_area: ");Serial.println(tx_data.mode_area);
}
 // ================ gán các giá trị để gửi ========================// 
void update_data_screen(){
  static unsigned long time_update_screen = 0;
  if (millis() - time_update_screen >= 50){
  switch(tx_data.mode_ball){
    case 1 :
            tx_data.default_pwm[0] = DEFAULT_PWM_11;
            tx_data.default_speed[0] = DEFAULT_SPEED_11;

            tx_data.default_pwm[1] = DEFAULT_PWM_12;
            tx_data.default_speed[1] = DEFAULT_SPEED_12;

            tx_data.default_pwm[2] = DEFAULT_PWM_13;
            tx_data.default_speed[2] = DEFAULT_SPEED_13;

            tx_data.default_pwm[3] = DEFAULT_PWM_14;
            tx_data.default_speed[3] = DEFAULT_SPEED_14;
                   
        break;
    case 2:                     
            tx_data.default_pwm[0] = DEFAULT_PWM_21;
            tx_data.default_speed[0] = DEFAULT_SPEED_21;
                            
            tx_data.default_pwm[1] = DEFAULT_PWM_22;
            tx_data.default_speed[1] = DEFAULT_SPEED_22;
                 
            tx_data.default_pwm[2] = DEFAULT_PWM_23;
            tx_data.default_speed[2] = DEFAULT_SPEED_23;
                         
            tx_data.default_pwm[3] = DEFAULT_PWM_24;
            tx_data.default_speed[3] = DEFAULT_SPEED_24;
                 
        break;
    case 3:    
            tx_data.default_pwm[0] = DEFAULT_PWM_31;
            tx_data.default_speed[0] = DEFAULT_SPEED_31;
            
            tx_data.default_pwm[1] = DEFAULT_PWM_32;
            tx_data.default_speed[1] = DEFAULT_SPEED_32;
            
            tx_data.default_pwm[2] = DEFAULT_PWM_33;
            tx_data.default_speed[2] = DEFAULT_SPEED_33;
            
            tx_data.default_pwm[3] = DEFAULT_PWM_34;
            tx_data.default_speed[3] = DEFAULT_SPEED_34;
                        
        break;
    case 4:       
            tx_data.default_pwm[0] = DEFAULT_PWM_41;
            tx_data.default_speed[0] = DEFAULT_SPEED_41;
                
            tx_data.default_pwm[1] = DEFAULT_PWM_42;
            tx_data.default_speed[1] = DEFAULT_SPEED_42;
                
            tx_data.default_pwm[2] = DEFAULT_PWM_43;
            tx_data.default_speed[2] = DEFAULT_SPEED_43;
                
            tx_data.default_pwm[3] = DEFAULT_PWM_44;
            tx_data.default_speed[3] = DEFAULT_SPEED_44;
                         
        break;
      
  }
  
  #if 0
  if (sta_auto_pwm){
    switch(tx_data.mode_ball){
      case 1:
        switch(tx_data.mode_area){
          case 1: 
            auto_pwm = DEFAULT_PWM_11;
            break;
          case 2:
            auto_pwm = DEFAULT_PWM_12;
            break;
          case 3:
            auto_pwm = DEFAULT_PWM_13;
            break;
          case 4:
            auto_pwm = DEFAULT_PWM_14;
            break;
        }
        break;
      case 2:
        switch(tx_data.mode_area){
          case 1: 
            auto_pwm = DEFAULT_PWM_21;
            break;
          case 2:
            auto_pwm = DEFAULT_PWM_22;
            break;
          case 3:
            auto_pwm = DEFAULT_PWM_23;
            break;
          case 4:
            auto_pwm = DEFAULT_PWM_24;
            break;
        }
        break;
      case 3:
        switch(tx_data.mode_area){
          case 1: 
            auto_pwm = DEFAULT_PWM_31;
            break;
          case 2:
            auto_pwm = DEFAULT_PWM_32;
            break;
          case 3:
            auto_pwm = DEFAULT_PWM_33;
            break;
          case 4:
            auto_pwm = DEFAULT_PWM_34;
            break;
        }
        break;
      case 4:
        switch(tx_data.mode_area){
          case 1: 
            auto_pwm = DEFAULT_PWM_41;
            break;
          case 2:
            auto_pwm = DEFAULT_PWM_42;
            break;
          case 3:
            auto_pwm = DEFAULT_PWM_43;
            break;
          case 4:
            auto_pwm = DEFAULT_PWM_44;
            break;
        }
        break;
    }


  }

  else {

  }
  #endif
  time_update_screen = millis();
  }
}

// Hàm gửi dữ liệu qua UART
void send_data_to_debug() {
    if (millis() - last_send_time >= SEND_INTERVAL) {
        last_send_time = millis();
        
        // Cập nhật dữ liệu

        //tx_data.pwm = output;
        tx_data.pwm = shoot_speed;
        //update_data_screen();
        // Tạo buffer
        uint8_t buffer[sizeof(tx_data) + 3];
        buffer[0] = UART_START_BYTE;
        memcpy(&buffer[1], &tx_data, sizeof(tx_data));
        buffer[sizeof(buffer)-2] = calculate_checksum(&tx_data, sizeof(tx_data));
        buffer[sizeof(buffer)-1] = UART_END_BYTE;
        
        // Gửi dữ liệu với cơ chế retry
        retry_count = 0;
        while (retry_count < MAX_RETRIES) {
            Serial1.write(buffer, sizeof(buffer));
            Serial1.flush();
            
            // Đợi một chút trước khi retry
            delay(2);
            retry_count++;
        }
        
        #if 0
        Serial.print("Sent: ");
        for (size_t i = 0; i < sizeof(buffer); i++) {
            Serial.print(buffer[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
        #endif
    }
}

// Hàm phụ trợ gán giá trị
void set_values(uint8_t pwm, uint16_t speed) {
    tx_data.default_pwm[tx_data.mode_area-1] = pwm;
    tx_data.default_speed[tx_data.mode_area-1] = speed;
}

void debug_huhu(){
  static unsigned long time_delay = 0;
  if (millis() - time_delay >= 2000){
  for(int i = 0; i<5;i++){
    Serial.println(tx_data.default_pwm[i]);
    Serial.println(tx_data.default_speed[i]);
    }
  }
}

const int DEFAULT_PWM[4][4] = {
  {DEFAULT_PWM_11, DEFAULT_PWM_12, DEFAULT_PWM_13, DEFAULT_PWM_14},
  {DEFAULT_PWM_21, DEFAULT_PWM_22, DEFAULT_PWM_23, DEFAULT_PWM_24},
  {DEFAULT_PWM_31, DEFAULT_PWM_32, DEFAULT_PWM_33, DEFAULT_PWM_34},
  {DEFAULT_PWM_41, DEFAULT_PWM_42, DEFAULT_PWM_43, DEFAULT_PWM_44}
};
void auto_pwm_shoot(){
  static unsigned long time_delay = 0;
  if (millis() -  time_delay >= 50){
    time_delay = millis();
    if (flag_auto_pwm){
      if (tx_data.mode_ball >= 1 && tx_data.mode_ball <= 4 &&
          tx_data.mode_area >= 1 && tx_data.mode_area <= 4) {
        shoot_speed = DEFAULT_PWM[tx_data.mode_ball - 1][tx_data.mode_area - 1];
      }
    }
  }
}

#endif


