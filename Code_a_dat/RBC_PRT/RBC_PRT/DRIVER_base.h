#include <Arduino.h>
#include <stdint.h>


class Motor{
protected:
  uint8_t dir_pin;
  uint8_t pwm_pin;
public:
  Motor(uint8_t dir_pin, uint8_t pwm_pin) : dir_pin(dir_pin), pwm_pin(pwm_pin){
  }
  void goForward(uint16_t pwm){
    digitalWrite(dir_pin,1);
    analogWrite(pwm_pin, 255-pwm);
  }
  void goReverse(uint16_t pwm){
    digitalWrite(dir_pin,0);
    analogWrite(pwm_pin, 255-pwm);
  }
};

struct pid_param{
  double Kp;
  double Ki;
  double Kd;
};

void setup_drive(){
  pinMode(M0_PWM, OUTPUT);
  pinMode(M0_DIR, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M1_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M3_PWM, OUTPUT);
  pinMode(M3_DIR, OUTPUT);

  
  pinMode(ball_pwm,OUTPUT);
  pinMode(ball_dir1,OUTPUT);
  pinMode(ball_dir2,OUTPUT);

  pinMode(shoot_pwm, OUTPUT);
  pinMode(shoot_dir, OUTPUT);

  pinMode(dcgoc_dir, OUTPUT);
  pinMode(dcgoc_pwm, OUTPUT);


  pinMode(ct_tren, INPUT);
  pinMode(ct_duoi, INPUT);
}


