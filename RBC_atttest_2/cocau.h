#include <Arduino.h>
#include <stdint.h>
#include "RB_MAP.h"
#include "ps3_but.h"


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
    digitalWrite(dir1,0);
  }
  void goReverse(uint16_t pwm){
    analogWrite(pwm_pin,pwm);
    digitalWrite(dir1,0);
    digitalWrite(dir1,1);
  }
  void stop(){
    analogWrite(pwm_pin,0);
    digitalWrite(dir1,0);
    digitalWrite(dir1,0);
  }
};

class khi{
protected: uint8_t sig;
public:
  bool stt;
  khi(uint8_t sig):sig(sig){}
  void thu(){
    stt = true;
    digitalWrite(sig, 1);
  }
  void day(){
    stt = false;
    digitalWrite(sig, 0);
  }
};

khi v1 = khi(dobong); khi v2 = khi(lenbong); khi v3 = khi(don);

void dk_dobong(){
    if(but_data==4096){
      v1.day();
    }else if(but_data != 4096){
      v1.thu();
    }
}
void dk_lenbong(){
  if(but_data == 32768){
    v3.day();
  }else if(but_data != 32768){
    v3.thu();
  }
}

void bat_bong(){
  if(button_share && button_triangle){
    v1.day();
  }else if(!(button_share && button_triangle)){
    v1.thu();
  }

}

uint8_t base_c = 0;
bool base_stt = true;

void do_bong(){
  if(button_share && base_stt && button_circle){
    base_stt = false;
    base_c += 1;
    if(base_c == 3) base_c = 0;
  }else if(!(button_share && base_stt && button_circle)){
    base_stt = true;
  }
  if(base_c == 1){
    v3.day();
  }else if(base_c == 2){
    base_stt = true;
  }else if(base_c == 0){
    v3.thu();
  }
}

