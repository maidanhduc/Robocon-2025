#include "HardwareSerial.h"
#include "Arduino.h"
#include <stdint.h>
#include "RB_MAP.h"
#include "DRIVER_base.h"
#include "uart_st.h"
#include "cocau.h"
#include "ps3_but.h"


Motor DC[4] = { Motor(M0_DIR,M0_PWM),Motor(M1_DIR,M1_PWM),
                Motor(M2_DIR,M2_PWM),Motor(M3_DIR,M3_PWM) };


bool drb_stt = false;
uint8_t speed_mo = 10;

struct motor_uart{
  uint8_t dir;
  uint8_t speed;
};
void motor_run(uint8_t id, int8_t dir, uint8_t speed){
  if(dir == 1){
    DC[id].goForward(speed);
  }else if(dir == -1){
    DC[id].goReverse(speed);
  }else{
    DC[id].goReverse(0);
  }
}




void motor_uart(){
  if(stt_motor){
    motor_run(0,arr[0][0],arr[0][1]);
    motor_run(1,arr[1][0],arr[1][1]);
    motor_run(2,arr[2][0],arr[2][1]);
    motor_run(3,arr[3][0],arr[3][1]);
  }
}
/*
  

  
  motor(m[0].dir, m[1].speed)
  motor(m[1].dir, m[1].speed)
  motor(m[2].dir, m[1].speed)
  motor(m[3].dir, m[1].speed)



*/
void speed_up_motor(uint8_t speed){
    


}

uint8_t gun_ctrl = 0;
bool oii = true;
//uint8_t speed=50;
uint32_t dr_time = 0;

bool step_dr[10] = {true,false,false,false,false,false,false,false,false,true};

void auto_ball(){
  if(but_data == 256 && digitalRead(ctp_duoi)){
    v2.day();  
  }else{
    v2.thu();
  }
}


void  rolate(){
  if(button_L1){
    DC[3].goForward(7);
  }else if(button_R1){
    DC[3].goReverse(7);
  }else{
    DC[3].goReverse(0);
  }
}


void shoot(){ // dieu khien 2 dong co ban
  if(but_data == 1024 && oii){
    oii = false;
    gun_ctrl += 1;
    if(gun_ctrl == 2) gun_ctrl = 0;
  }else if(but_data != 1024){
    oii = true;
  }
 // Serial.println(gun_ctrl);
  static bool fl1 = true;
  if(but_data == 512 && fl1){ // nut o
    flag_auto_pwm = false;
    fl1 = false;
    speed -= 2;
  }else if(but_data == !2){
    fl1 = true;
  }
  if(but_data == 2048 && fl1){ // nut tam giac
    flag_auto_pwm = false;
    fl1 = false;
    speed+=10;
  }else if(but_data == ! 8){
    fl1 = true;
  }
  if(speed < 50) speed = 50;
  if(speed>=255) speed = 255;

  if(gun_ctrl){
    DC[0].goReverse(speed);
    DC[1].goReverse(speed);
  }else{
    DC[0].goReverse(0);
    DC[1].goReverse(0);
  }
}


void dribbing(){  // an X de dap bong
  static bool fl2 = true;
  static uint8_t drb = 0;
  if(but_data == 256 &&  fl2 ){ // && digitalRead(ctp_tren)
    drb += 1;
    fl2 = false;
  }else if(but_data != 256){
    fl2 = true;
  }
  if(drb==2) drb = 0;
  if(drb==1){
    v2.day();
    if(!digitalRead(ss_bal) && drb_stt){
      v1.thu();
    }else{
      v1.day();
    }
    if(step_dr[0]){
      step_dr[1] = true;
      step_dr[0] = false;
    }
    if(step_dr[1]){
      dr_time = millis();
      step_dr[2] = true;
      step_dr[1] = false;
    }
    if(step_dr[2]){
      if(millis()-dr_time >= 1200){
        dr_time = millis();
        step_dr[0] = false;
        step_dr[3] = true;
        step_dr[2] = false;
      }
    }
    if(step_dr[3]){
      drb_stt = true;
      v2.thu();
      v3.day();
    }
  }else if(drb == 0){
    drb_stt = false;
    //v2.thu();
    v3.thu();
    step_dr[3] = false;
    step_dr[0] = true;
  }
}

void dribbing_2(){  // an X de dap bong
  
  static bool fl2 = true;
  static uint8_t drb = 0;
  //Serial.println(drb);
  if(but_data == 256 &&  fl2){
    drb += 1;
    fl2 = false;
  }else if(but_data != 256){
    fl2 = true;
  }
  if(drb==2) drb = 0;
  //Serial.println(drb);
  if(drb==1){
    v2.day();
    v1.thu();
    if(step_dr[0]){
      step_dr[1] = true;
      step_dr[0] = false;
    }
    if(step_dr[1]){
      dr_time = millis();
      //Serial.println("save");
      step_dr[2] = true;
      step_dr[1] = false;
      
    }
    if(step_dr[2]){
      if(millis()-dr_time >= 1200){
        dr_time = millis();
        step_dr[0] = false;
        step_dr[3] = true;
        step_dr[2] = false;
      }
    }
    if(step_dr[3]){
      //Serial.println("dong co ha");
      drb_stt = false;
      DC[2].goReverse(30);
      if(millis()-dr_time>=1000){
        //Serial.println("dong co dung");
        step_dr[3] = false;
        DC[2].goReverse(0);
        dr_time = millis();
      }
      v2.thu();
      v3.day();
      //drb = 0;
    }
  }else if(drb == 0){
    drb_stt = true;
    v2.thu();
    v3.thu();
    step_dr[0] = true;
  }
}

uint8_t c_wheel = 0;
bool vom_stt = true;
bool ok = true;
void vom_bong(uint8_t speed){
  if(button_R3 && vom_stt){
    vom_stt = false;
    c_wheel += 1;
    if(c_wheel == 3) c_wheel = 0;
  }else if(!button_R3){
    vom_stt = true;
  }
  if(c_wheel == 1 && ok){
    DC[2].goForward(60);
    if(digitalRead(ctp_tren)){
      ok = false;
      DC[2].goReverse(0);
    }
  }else if(c_wheel == 2){
    ok = true;
    DC[2].goReverse(60);// xuong
    if(digitalRead(ctp_duoi)){
      c_wheel = 0;
    }
  }else{
    DC[2].goReverse(0);
  }
}

