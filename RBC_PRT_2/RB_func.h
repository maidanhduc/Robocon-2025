#ifndef RB_FUNC_H
#define RB_FUNC_H
#include <math.h>
#include "HardwareSerial.h"
#include "Arduino.h"
#include <stdint.h>
#include "RB_MAP.h"
#include "DRIVER_base.h"
#include "cocau.h"
#include "ps3_but.h"
#include "uart_st.h"

#define MAX_SPEED 180
#define MIN_SPEED 2

uint8_t speed = 0;

int8_t status = 0;
int16_t angle_ro ;



Motor DC[4] = { Motor(M0_DIR,M0_PWM),Motor(M1_DIR,M1_PWM),
                Motor(M2_DIR,M2_PWM),Motor(M3_DIR,M3_PWM) };

Motor dc_ban = Motor(shoot_dir,shoot_pwm);
Motor dc_goc = Motor(dcgoc_dir,dcgoc_pwm);

uint8_t v_plus;
uint8_t v_minus;

double Kp=0.02;
double Ki=0.01;
double Kd=0.25;
uint32_t time = 0;
int16_t last_Error = 0;
int16_t error;

void Speed_up(uint8_t target_v, uint8_t t){
  static uint32_t time_str = 0;
  if(millis() - time_str >= t){
    if(speed < target_v){
      speed++;
      time_str = millis();
    }

  }
}
void Speed_down(uint8_t target_v, uint8_t t){
  static uint32_t time_sto = 0;
  if(millis() - time_sto >= t){
    if(speed > target_v){
      speed-=5;
      time_sto = millis();
    }
    if(speed < MIN_SPEED + 6){
      speed = MIN_SPEED;
    }
  }
}

double PI_const = 3.141592654; 
int16_t angle_update = 0;

int pid_cal(int set_point, int input,int constrain_p,  double pP, double pI, double pD){   // goc quay, dau vao, toc do quay , 3 he so 
  int output = 0; 
  static long error_p, last_error_p; 
  error_p = set_point - input; 
  output = pP*error_p + pI*(error_p +  last_error_p) + pD*(error_p - last_error_p); // tỉ lệ ; tích phân ;  đạo hàm
  last_error_p = error_p; 
  output = constrain(output, - constrain_p, constrain_p); 
  return output; 
}

void rundc0(int v, int vec){ v = abs(v);   if(vec == 1 ){  DC[0].goForward(v);  }else if(vec == 0){  DC[0].goReverse(v);}}
void rundc1(int v, int vec){ v = abs(v);   if(vec == 1 ){  DC[1].goForward(v);  }else if(vec == 0){  DC[1].goReverse(v);}}
void rundc2(int v, int vec){ v = abs(v);   if(vec == 1 ){  DC[2].goForward(v);  }else if(vec == 0){  DC[2].goReverse(v);}}
void rundc3(int v, int vec){ v = abs(v);   if(vec == 1 ){  DC[3].goForward(v);  }else if(vec == 0){  DC[3].goReverse(v);}}


void move(int angle_rotare, int v_rotare, int angle_tt, int v_tt , int soft_start , int soft_stopp  ){   // angle tt là góc * 10
  int angle_compass = cur_angle; 
  int real_v_rotare = pid_cal(angle_rotare/10,angle_compass/10, v_rotare, 0.5, 0.01, 0.04); 
  int pwm0, pwm1, pwm2, pwm3; 
  int vec0, vec1, vec2, vec3; 
  pwm0 = real_v_rotare + v_tt*cos((angle_tt - angle_compass - 450)*(PI_const/1800)); 
  pwm1 = real_v_rotare + v_tt*cos((angle_tt - angle_compass - 1350)*(PI_const/1800)); // +900
  pwm2 = real_v_rotare + v_tt*cos((angle_tt - angle_compass - 2250)*(PI_const/1800)); // +900
  pwm3 = real_v_rotare + v_tt*cos((angle_tt - angle_compass - 3150)*(PI_const/1800)); // +900

  if(pwm0 >  0 ){ vec0 = 1; }else { vec0 = 0 ; }
  if(pwm1 >  0 ){ vec1 = 1; }else { vec1 = 0 ; }
  if(pwm2 >  0 ){ vec2 = 1; }else { vec2 = 0 ; }
  if(pwm3 >  0 ){ vec3 = 1; }else { vec3 = 0 ; }

  rundc0(pwm0, vec0); 
  rundc1(pwm1, vec1); 
  rundc2(pwm2, vec2); 
  rundc3(pwm3, vec3);
  Serial.print("pwm0:" );Serial.print(pwm0);
  Serial.print("  pwm1:" );Serial.print(pwm1);
  Serial.print("  pwm2:" );Serial.print(pwm2);
  Serial.print("  pwm3:" );Serial.println(pwm3); 
  //Serial.println(pwm4); 
}


/*

1        4        1  0
                  2  3


2        3

*/ 

void ps3_move(){
static int run_stt = 0;
static int ang_rot  = 0;
static int ang_tt  = 0;
  if(abs(joy_LX)>20 || abs(joy_LY)>20 ||  abs(joy_RX)>20 || abs(joy_RY)>20   ){
    if(abs(joy_LX)>100 || abs(joy_LY)>100){
      ang_tt = atan2(joy_LX, joy_LY)*10*RAD_TO_DEG;
      Speed_up(120, 10);
    }else{
      speed = 0;
    }
    if(abs(joy_RX) > 100 || abs(joy_RY) >100){
      ang_rot = atan2(joy_RX, joy_RY)*10*RAD_TO_DEG;
      Speed_up(120, 10);
    }
     Serial.println(ang_rot); 
     
     move(ang_rot,60,ang_tt,speed,1,1);
  }else if(button_up){
    run_stt = 1;
    ang_tt = ang_rot + 0;
    Speed_up(MAX_SPEED, 5);
    move(ang_rot,90,ang_tt,speed,1,1);
  }else if(button_down){
    run_stt = 2;
    Speed_up(MAX_SPEED, 5);
    ang_tt = ang_rot + 1800;
    move(ang_rot,90,ang_tt,speed,1,1);
  }else if(button_left){
    run_stt = 3;
    Speed_up(MAX_SPEED, 5);
    ang_tt = ang_rot + 900;
    move(ang_rot,90,ang_tt,speed,1,1);
  }else if(button_right){
    run_stt = 4;
    Speed_up(MAX_SPEED, 5);
    ang_tt = ang_rot - 900;
    move(ang_rot,9+0,ang_tt,speed,1,1);
  }else if(button_L1){
    //move(ang_rot,70,ang_tt,0,1,1);
    Speed_up(30, 1);
    DC[0].goForward(speed);
    DC[1].goForward(speed);
    DC[2].goForward(speed);
    DC[3].goForward(speed);
  }else if(button_R1){
    //move(ang_rot,70,ang_tt,0,1,1);
    Speed_up(30, 1);
    DC[0].goReverse(speed);
    DC[1].goReverse(speed);
    DC[2].goReverse(speed);
    DC[3].goReverse(speed);
  }else{
    if(speed != 0){
      Speed_down(2,2);
      switch(run_stt){
        case 1: DC[0].goForward(speed);DC[1].goReverse(speed);DC[2].goReverse(speed);DC[3].goForward(speed); break;
        case 2: DC[1].goForward(speed);DC[0].goReverse(speed);DC[3].goReverse(speed);DC[2].goForward(speed); break;
        case 3: DC[0].goForward(speed);DC[1].goForward(speed);DC[2].goReverse(speed);DC[3].goReverse(speed); break;
        case 4: DC[2].goForward(speed);DC[3].goForward(speed);DC[0].goReverse(speed);DC[1].goReverse(speed); break;
      }
    }else{
      DC[0].goForward(2);DC[1].goReverse(2);DC[2].goReverse(2);DC[3].goForward(2);
    }
    // Speed_down(2, 1);
    // move(ang_rot,0,ang_tt,0,1,1); 
    ang_rot = cur_angle;
  }
  
}



pid_param K2;
int16_t error1;
int16_t last_err1 = 0;
int16_t output1;
uint32_t time2 = 0;
void PID_rolate(int16_t set_angle, uint8_t speed_ro, uint8_t sample_t){
  K2.Kp = 0.1; K2.Ki = 0.15; K2.Kd = 0.01;
  if(millis() - time2 >= sample_t){
    error1 = set_angle - cur_angle;
    output1 = K2.Kp*error1 + K2.Ki*((error1-last_err1)/sample_t) + K2.Kd*(error1-last_err1);
    last_err1 = error1;
    if(abs(output1) > speed_ro){
      output1 = speed_ro;
    }
    output1 = abs(output1);
    if(output1==2) output1 = 3;
    if(error1 > 0){
      DC[0].goForward(output1);
      DC[1].goForward(output1);
      DC[2].goForward(output1);
      DC[3].goForward(output1);
    }else{
      DC[0].goReverse(output1);
      DC[1].goReverse(output1);
      DC[2].goReverse(output1);
      DC[3].goReverse(output1);
    }
  }
}



bool gun_ss = true;
uint8_t gun_cc = 0;
void gun_ctrl(){
  if(but_data == 1024 && gun_ss){ // nut vuong
    gun_ss = false;
    gun_cc += 1;
    if(gun_cc > 1) gun_cc = 0;
  }else if(but_data != 1024){
    gun_ss = true;
  }
  static bool fl1 = true;
  if(but_data == 2048 && fl1){ // nut tam giac
    flag_auto_pwm = false;
    fl1 = false;
    shoot_speed+=5;
  }else if(but_data == !2048){
    fl1 = true;
  }
  if(but_data == 256 && fl1){ // nut x
    flag_auto_pwm = false;
    fl1 = false;
    shoot_speed-=5;
  }else if(but_data == !256){
    fl1 = true;
  }
  if(shoot_speed < 80) shoot_speed = 80;
  if(shoot_speed>=255) shoot_speed = 255;
  if(gun_cc == 1){
    dc_ban.goForward(shoot_speed);
  }else{
    dc_ban.goForward(0);
  }
  
}
void ang_ctrl(uint8_t speed){
  if(but_data == 128){ // l2
    dc_goc.goForward(speed);
  }else if(but_data == 64){ // r2
    dc_goc.goReverse(speed);
  }else{
    dc_goc.goReverse(0);
  }
}

#endif