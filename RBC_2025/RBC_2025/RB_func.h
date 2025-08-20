#include <stdint.h>
#include "HardwareSerial.h"
#include "RB_MAP.h"
#include "DRIVER_base.h"
#include "uart_st.h"
#define MAX_SPEED 80
#define MIN_SPEED 2
uint8_t speed = 0;
int8_t status = 0;
Motor DC[4] = { Motor(M0_DIR,M0_PWM),Motor(M1_DIR,M1_PWM),
                Motor(M2_DIR,M2_PWM),Motor(M3_DIR,M3_PWM) };
uint8_t v_plus;
uint8_t v_minus;

void PID_TargetAngle(int16_t angle, uint8_t sample_t){
    static double Kp;
    static double Kd;
    static uint32_t time = 0;
    static int16_t last_Error = 0;
	  static int16_t error;
    if(millis() - time >= sample_t){

      error = angle - cur_angle;

      if(abs(error) >= 700){
        Kp = 0.01;Kd = 0.04;
      }else if(abs(error) >= 500){
        Kp = 0.01;Kd = 0.02;
      }else if(abs(error) >= 200){
        Kp = 0.15;Kd = 0.03;
      }else if(abs(error) >= 100){
        Kp = 0.15;Kd = 0.01;
      }else if(abs(error) >= 50){
        Kp = 0.15;Kd = 0.1;
      }else{
        Kp = 0.2;Kd = 0.1;
      }

      int16_t adjustment = Kp*error + Kd*((error - last_Error)/(double)sample_t);
      
      //Serial.print("error = ");Serial.println(error);
      //Serial.print("Kp = ");Serial.println(Kp);
      //Serial.print("Kd = ");Serial.println(Kd);
      //Serial.println(adjustment);

      v_minus = speed - adjustment;
      if(v_minus < MIN_SPEED) v_minus = MIN_SPEED;// 280 > 290
      v_plus = speed + adjustment;
      if(v_plus < MAX_SPEED) v_plus = MAX_SPEED;
      time = millis();
      last_Error = error;
	  }
}
void Speed_up(uint8_t target_v, uint8_t t){
  static uint32_t time_str = 0;
  if(millis() - time_str >= t){
    if(speed < target_v){
      speed++;
      time_str = millis();
    }
    if(speed > MAX_SPEED){
      speed = MAX_SPEED;
    }
  }
}
void Speed_down(uint8_t target_v, uint8_t t){
  static uint32_t time_sto = 0;
  if(millis() - time_sto >= t){
    if(speed > target_v){
      speed--;
      time_sto = millis();
    }
    if(speed < MIN_SPEED){
      speed = MIN_SPEED;
    }
  }
}
void pid_up(int16_t angle){
  PID_TargetAngle(0, 10);
  Speed_up(MAX_SPEED, 1);
    DC[0].goForward(v_minus);
    DC[1].goReverse(v_plus);
    DC[2].goReverse(v_plus);
    DC[3].goForward(v_minus);
}

void pid_down(int16_t angle){
  PID_TargetAngle(0, 50);
  Speed_up(MAX_SPEED, 1);
  DC[0].goReverse(v_plus);
  DC[1].goForward(v_minus);
  DC[2].goForward(v_minus);
  DC[3].goReverse(v_plus);
}

void pid_right(int16_t angle){
  PID_TargetAngle(angle, 50);
  Speed_up(MAX_SPEED, 1);
    DC[0].goReverse(v_minus);
    DC[1].goReverse(v_minus);
    DC[2].goForward(v_plus);
    DC[3].goForward(v_plus);
}

void pid_left(int16_t angle){
  PID_TargetAngle(angle, 50);
  Speed_up(MAX_SPEED, 1);
    DC[0].goForward(v_plus);
    DC[1].goForward(v_plus);
    DC[2].goReverse(v_minus);
    DC[3].goReverse(v_minus);
}
void ps3_ctrl_1(){ // PID move
  if(but_data == 256){
    status = 1;
    Speed_up(MAX_SPEED, 1);
    PID_TargetAngle(0, 50);
    DC[0].goForward(v_minus);
    DC[1].goReverse(v_plus);
    DC[2].goReverse(v_plus);
    DC[3].goForward(v_minus);
  }else if(but_data == 512){
    status = -1;
    Speed_up(MAX_SPEED, 1);
    PID_TargetAngle(0, 50);
    DC[0].goReverse(v_minus);
    DC[1].goForward(v_plus);
    DC[2].goForward(v_plus);
    DC[3].goReverse(v_minus);
  }else if(but_data == 1024){
    status = 2;
    pid_right(0);
  }else if(but_data){
    status = -2;
    pid_left(0);
  }else{
    Speed_down(1, 3);
    if(status == 1){
      DC[0].goForward(speed);
      DC[1].goReverse(speed);
      DC[2].goReverse(speed);
      DC[3].goForward(speed);  
    }
    else if(status == -1){
      DC[0].goReverse(speed);
      DC[1].goForward(speed);
      DC[2].goForward(speed);
      DC[3].goReverse(speed);
    }
    else if(status == 2){
      DC[0].goReverse(speed);
      DC[1].goReverse(speed);
      DC[2].goForward(speed);
      DC[3].goForward(speed);
    }
    else if(status == -2){
      DC[3].goReverse(speed);
      DC[2].goReverse(speed);
      DC[1].goForward(speed);
      DC[0].goForward(speed);
    }else if(status == 0){
      DC[0].goForward(2);
      DC[1].goForward(2);
      DC[2].goForward(2);
      DC[3].goForward(2);
    }
  }
}
void ps3_ctrl(){ // normal move
  if(but_data == 256){
    status = 1;
    Speed_up(MAX_SPEED, 1);
    DC[0].goForward(speed);
    DC[1].goReverse(speed);
    DC[2].goReverse(speed);
    DC[3].goForward(speed);
  }else if(but_data == 512){
    status = -1;
    Speed_up(MAX_SPEED, 10);
    DC[0].goReverse(speed);
    DC[1].goForward(speed);
    DC[2].goForward(speed);
    DC[3].goReverse(speed);
  }else if(but_data == 1024){
    status = 2;
    Speed_up(MAX_SPEED, 10);
    DC[0].goReverse(speed);
    DC[1].goReverse(speed);
    DC[2].goForward(speed);
    DC[3].goForward(speed);
  }else if(but_data == 2048){
    status = -2;
    Speed_up(MAX_SPEED, 1);
    DC[0].goForward(speed);
    DC[1].goForward(speed);
    DC[2].goReverse(speed);
    DC[3].goReverse(speed);
  }else if(but_data == 4096){
    DC[0].goForward(30);
    DC[1].goForward(30);
    DC[2].goForward(30);
    DC[3].goForward(30);
  }else if(but_data == 8192){
    DC[0].goReverse(30);
    DC[1].goReverse(30);
    DC[2].goReverse(30);
    DC[3].goReverse(30);
  }else if(but_data == 0){
    Speed_down(2, 10);
    if(status == 1){
      //Speed_down(1, 3);
      DC[0].goForward(speed);
      DC[1].goReverse(speed);
      DC[2].goReverse(speed);
      DC[3].goForward(speed);
    }
    else if(status == -1){
      DC[0].goReverse(speed);
      DC[1].goForward(speed);
      DC[2].goForward(speed);
      DC[3].goReverse(speed);
    }
    else if(status == 2){
      DC[0].goReverse(speed);
      DC[1].goReverse(speed);
      DC[2].goForward(speed);
      DC[3].goForward(speed);
    }
    else if(status == -2){
      DC[0].goForward(speed);
      DC[1].goForward(speed);
      DC[2].goReverse(speed);
      DC[3].goReverse(speed);
    }else{
      DC[0].goForward(2);
      DC[1].goForward(2);
      DC[2].goForward(2);
      DC[3].goForward(2);
    }
  }
}