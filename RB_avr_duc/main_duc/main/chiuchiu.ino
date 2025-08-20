#include "Uart.h"
//==================công tắc hành trình,cơ cấu nâng================//
uint32_t shoot_speed = 80; // shoot speed bay gio la toc doc quay cua dong co mong muon khong phai xung pwm
bool stt_ball = true;
uint8_t c_ball = 0;
bool oii = true;
bool ok = true;
bool on_lazer = false;
volatile unsigned long dem = 0;
// pid toc

unsigned long count_xung;
unsigned long thoigian;
unsigned long hientai=0;
int timecho=1000;
// Tham số PID
//double Kp = 0.8, Ki = 0.3, Kd = 0.01;  // Chỉnh các giá trị này
double dtt;
unsigned long last_time;
double integral, previous;
void pid_shot(int16_t setpoint) {
    static double integral = 0, previous = 0; // Biến trạng thái PID
    static unsigned long last_time = 0;       // Lưu thời gian cập nhật PID trước đó
    const unsigned long SAMPLE_TIME = 100;    // Cập nhật PID mỗi 100ms

    // Kiểm tra nếu chưa đủ thời gian thì không thực hiện PID
    unsigned long now = millis();
    if (now - last_time < SAMPLE_TIME) {
        return (uint16_t)output; // Giữ nguyên giá trị đầu ra cũ
    }
    last_time = now;

    // Đọc giá trị thực tế và tính sai số
    double actual = toc_do; // Giả sử `toc_do` là biến toàn cục
    double error = setpoint - actual;

    // Tham số PID
    double Kp = 0.005, Ki = 0.001, Kd = 0.01;
    double proportional = error;
    
    // Tính toán PID
    double dt = SAMPLE_TIME / 1000.0; // Đổi mili giây sang giây
    integral += error * dt;
    integral = constrain(integral, -100, 100); // Giới hạn chống windup

    double derivative = (error - previous) / dt;
    previous = error;

    // Tính toán đầu ra PID
    int16_t adjustment = Kp * proportional + Ki * integral + Kd * derivative;
    output = output + adjustment;
    output = constrain(output, 0, 255);
    Serial.print("output: ");Serial.println(output);
    //Serial.print("Adjustment: "); Serial.println(adjustment);
    //Serial.print("error: ");Serial.println(error);
}


void ctrl_ball(){     // bts bridge
  uart_get_button();
  if (but_data == button_cricle && oii){  // button circle
    oii = false;
    c_ball+= 1;
    if (c_ball == 3 ) c_ball=0;
  }else if(but_data != button_cricle){
    oii = true;
  }
  if (c_ball == 1 && ok ){
    DC_rise.turnRight(150); // len
    if (digitalRead(CTup)){
      ok = false;
      DC_rise.stop();
    }
  }else if (c_ball == 2){
    ok = true;
    DC_rise.turnLeft(245); // xuong
    if(digitalRead(CTdown)){
      c_ball =0;
    }
  }else{
    DC_rise.stop();
  }

//  Serial.print("CTup: ");Serial.println(status_ctup);
//  Serial.print("CTdown: ");Serial.println(status_ctdown);

}
bool gun_ss = true;
uint8_t gun_cc = 0;
void ctrl_gun(){
  uart_get_button();
  if(but_data == button_square && gun_ss){ // nut vuong
    on_lazer == true;
    gun_ss = false;
    gun_cc += 1;
    if (gun_cc > 1) gun_cc =0;
  } else if(but_data != button_square){
    gun_ss = true;
  }
  static bool fl1 = true;
  if(but_data == button_cross && fl1){ //nut x
    fl1 = false;
    shoot_speed+= 15;
    output += 15;
  }else if(but_data == !button_cross){
    fl1 = true;
  }
  if(but_data == button_triangle && fl1){ // nut tam giac
    fl1 = false;
    shoot_speed -=5;
    output -= 5;
  }else if(but_data == !button_triangle){
    fl1 = true;
  }
  if(output < 80) output = 80;  // 
  if(output>=255) output = 255;
  if(gun_cc == 1){
    //pid_shot(5000); // shoot speed bay gio la toc doc quay cua dong co mong muon khong phai xung pwm
    DC_shot.quaythuan(output);
    //Serial.println(output);
  }else{

    DC_shot.quaythuan(0);
  }
  //Serial.print("gun_cc: ");Serial.println(gun_cc);
  if (but_data == button_L1){
    DC_rotate.quaynghich(45);
  }
  else if (but_data == button_R1){
    DC_rotate.quaythuan(45);
  }
  else DC_rotate.stop();
}
// ====================cơ cấu xoay - bắn =============================
void rotate_left(uint8_t pwm){
  DC_rotate.quaythuan(pwm);
}
void rotate_right(uint8_t pwm){
  DC_rotate.quaynghich(pwm);
}
void shot(uint8_t pwm){
  DC_shot.quaynghich(pwm);
}
bool laze_sta = true;

void lazer() {
  static uint32_t last_toggle_time = 0;
  static bool laze_state = false; // Trạng thái hiện tại của laze

  if (laze_state && millis() - last_toggle_time > 660) {
    laze_sta = false;
    laze_state = false;
    last_toggle_time = millis();
  } else if (!laze_state && millis() - last_toggle_time > 340) {
    laze_sta = true;
    laze_state = true;
    last_toggle_time = millis();
  }
  return laze_sta;
}
void ctr_laze(){
  if (ctr_laze){
    digitalWrite(laze1,HIGH);
    digitalWrite(laze2,HIGH);
  }
  else {
    digitalWrite(laze1,LOW);
    digitalWrite(laze2,LOW);
  }
}
