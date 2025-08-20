#include <Arduino.h>
#include <stdint.h>
#include "RB_MAP.h"
#include "uart_st.h"

class Mcocau{
protected:
  uint8_t pwm_pin;
  uint8_t dir1;
  uint8_t dir2;
public:
  Mcocau(uint8_t pwm_pin, uint8_t dir1, uint8_t dir2) : pwm_pin(pwm_pin), dir1(dir1), dir2(dir2){
  }
  void goForward(uint16_t pwm){
    analogWrite(pwm_pin,pwm);
    digitalWrite(dir1,1);
    digitalWrite(dir2,0);
  }
  void goReverse(uint16_t pwm){
    analogWrite(pwm_pin,pwm);
    digitalWrite(dir1,0);
    digitalWrite(dir2,1);
  }
  void stop(){
    analogWrite(pwm_pin,0);
    digitalWrite(dir1,0);
    digitalWrite(dir1,0);
  }
};

Mcocau M_ball = Mcocau(ball_pwm,ball_dir1,ball_dir2);

bool stt_ball = true;
uint8_t c_ball = 0;
bool oii = true;
bool ok = true;
void ball_ctrl(uint8_t speed){
  if(but_data == 512 && oii){
    oii = false;
    c_ball += 1;
    if(c_ball == 3) c_ball = 0;
  }else if(but_data != 512){
    oii = true;
  }
  if(c_ball == 1 && ok){
    M_ball.goReverse(245);
    if(digitalRead(ct_tren)){
      ok = false;
      M_ball.stop();
    }
  }else if(c_ball == 2){
    ok = true;
    M_ball.goForward(150);// xuong
    if(digitalRead(ct_duoi)){
      c_ball = 0;
    }
  }else{
    M_ball.stop();
  }
}


