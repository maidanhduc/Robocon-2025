#ifndef CLASS_H  // Nếu chưa được định nghĩa
#define CLASS_H  // Định nghĩa CLASS_H
#include "Arduino.h"
#include "stdint.h"

// ============== tạo class=======================//
class motor{
  protected:
    uint8_t m;
    uint8_t d;
  public:
    motor(uint8_t ma, uint8_t da): m(ma), d(da){}
    void pinSetup(){
      pinMode(m, OUTPUT);
      pinMode(d, OUTPUT);
      digitalWrite(m,1); // bug anh thang sua
      digitalWrite(d,0);
      
    }
    void quaythuan(uint8_t pwm){
      digitalWrite(d,1);
      analogWrite(m,255-pwm);
    }
    void quaynghich(uint8_t pwm){
      digitalWrite(d,0);
      analogWrite(m,255-pwm);
    }
    void stop(){
      analogWrite(m,255);
    }
    void phanh(){
      analogWrite(m,253);
    }
    
};
class nor_motor {
  protected:
    unsigned char In1;
    unsigned char In2;
    unsigned char Pwm;
  public:
    nor_motor(uint8_t in1, uint8_t in2 ,uint8_t pwm){
      In1 = in1;
      In2 = in2;
      Pwm = pwm;
    }
    void pinSetup(){
      pinMode(In1,OUTPUT);
      pinMode(In2,OUTPUT);
      pinMode(Pwm,OUTPUT);
      // digitalWrite(In1,0);
      // digitalWrite(In2,0);
      // analogWrite(Pwm,0);
    }
    void turnRight(uint8_t pwmm){
      digitalWrite(In1,1);
      digitalWrite(In2,0);
      analogWrite(Pwm,pwmm);
    }
    void turnLeft(uint8_t pwmm){
      digitalWrite(In1,0);
      digitalWrite(In2,1);
      analogWrite(Pwm, pwmm);
    } 
    void stop(){
      analogWrite(Pwm,0);
    }

};
//==========================khai báo class================//
motor DC[4]={motor(m0,d0),motor(m1,d1),
             motor(m2,d2),motor(m3,d3)};
motor DC_rotate(m_r,d_r);
motor DC_shot(m_b,d_b);
nor_motor DC_rise(in_r,in_l,pwm5);
//==========================hàm ??========================//
void motorPidSetup(){
  for (uint8_t i=0;i<4;i++){
    DC[i].pinSetup();
  }
}
void motor_rise_ball_setup(){
  DC_rise.pinSetup();
}
void motor_shot_setup(){
  DC_rotate.pinSetup();
  DC_shot.pinSetup();
}
#endif