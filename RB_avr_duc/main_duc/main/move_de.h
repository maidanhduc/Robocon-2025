#include "class.h"
#include "mapping.h"
#include "indexPs3.h"
//#include "Ps3_update.h"
#include "Uart.h"
#define MAX_SPEED 100
#define MIN_SPEED 0
uint8_t speed =0;
uint8_t dt =15;
uint8_t dt_start;
uint8_t v_plus;
uint8_t v_minus;
int16_t adjustment=0;
static int16_t error;
static int16_t last_Error = 0;
int16_t angle_last;
int8_t status =0;
// =================Di chuyển cơ bản====================//
/*

*/

int32_t constrains(int32_t value, int32_t down, int32_t up ){
	if(value >= up) return up;
	if(value < down) return down;
	return value;
}

double fConstrain(double value, double down, double up){
	if(value >= up) return up;
	if(value < down) return down;
	return value;
}
void soft_start(uint8_t target_v, uint8_t cycle){
  static unsigned long time_str = 0;
  if (millis() - time_str >= cycle){
    if(speed < target_v){
      speed++;
      time_str = millis();
    }
    if (speed > MAX_SPEED){
      speed = MAX_SPEED;
    }
  }
}
void soft_end(uint8_t target_v, uint8_t cycle){
  static unsigned long time_sto = 0;
  if (millis() - time_sto >= cycle){
    if(speed > target_v){
      speed--;
      time_sto = millis();
    }
    if (speed < MIN_SPEED){
      speed = MIN_SPEED;
    }
  } 
}
void counter_clockwise(uint8_t speed){
  soft_start(MAX_SPEED,20);
  DC[0].quaythuan(speed);
  DC[1].quaythuan(speed);
  DC[2].quaythuan(speed);
  DC[3].quaythuan(speed);
}
void rotate_clockwise(uint8_t speed){
  soft_start(MAX_SPEED,20);
  DC[0].quaynghich(speed);
  DC[1].quaynghich(speed);
  DC[2].quaynghich(speed);
  DC[3].quaynghich(speed);
}
// ================== PID MOVE ============================//
void PID_TargetAngle(int16_t angle_set, uint16_t value,uint8_t accelerometer){
  static double Kp,Ki,Kd;
  static uint32_t time=0;
  // static int16_t last_Error = 0;
  // static double integral = 0;
  //static uint16_t error;
  if(millis() - time >= 10 ){
    error = angle_set - cur_angle;
    time = millis();
if(abs(error) >= 900){
			Kp = 0.2 , Ki = 0.15, Kd  = 0.08;
			//Kp = 0.80 , Ki = 0.15, Kd  = 0.05;
		}else if(abs(error) >= 500){
			Kp = 0.1 , Ki = 0.07, Kd  = 0.5;
			//Kp = 1.0 , Ki = 0.25, Kd  = 0.85;
		}else if (abs(error) >= 200){
			Kp = 0.05 , Ki = 0.01, Kd  = 0.02;
			//Kp = 1.2 , Ki = 0.35, Kd  = 0.1;
		}else if(abs(error) >= 50){
			Kp = 0.05 , Ki = 0.01, Kd  = 0.01;
			//Kp = 1.5 , Ki = 0.55, Kd  = 0.15;
		}

    adjustment = Kp*error + Kd*(error-last_Error) + Ki*(error + last_Error);
    //
    adjustment = constrains((int32_t)(adjustment),-accelerometer,accelerometer);
    v_plus = constrains(value + adjustment,0,MAX_SPEED);
    v_minus= constrains(value - adjustment,0,MAX_SPEED);
    last_Error = error;
  }
  // for debug
  /*
  Serial.print("cur_angle: ");Serial.println(cur_angle);
  Serial.print("adjustment:");Serial.println(adjustment);
  Serial.print("v_plus: ");Serial.println(v_plus);
  Serial.print("v_minus: ");Serial.println(v_minus);*/
  //Serial.print("error: ");Serial.println(error);
  //Serial.print("last_error: ");Serial.println(last_Error);
  //Serial.print("integral: ");Serial.println(integral);
}

void pid_up(int16_t angle_set,uint16_t value, uint8_t accelerometer){
  soft_start(MAX_SPEED,15);
  PID_TargetAngle(angle_set,value,accelerometer);
  DC[0].quaythuan(v_plus);
  DC[1].quaythuan(v_minus);
  DC[2].quaynghich(v_minus);
  DC[3].quaynghich(v_minus); 
}
void pid_down(int16_t angle_set,uint16_t value,uint8_t accelerometer){
  //soft_start(MAX_SPEED,15);
  //PID_TargetAngle(angle_set,value,accelerometer);
  DC[0].quaynghich(v_minus);
  DC[1].quaynghich(v_plus);
  DC[2].quaythuan(v_plus);
  DC[3].quaythuan(v_minus);
}
void pid_left(int16_t angle_set,uint16_t value,uint8_t accelerometer){
  soft_start(MAX_SPEED,15);
  PID_TargetAngle(angle_set,value,accelerometer);
  DC[0].quaythuan(v_plus);
  DC[1].quaynghich(v_plus);
  DC[2].quaynghich(v_minus);
  DC[3].quaythuan(v_minus);  
}
void pid_right(int16_t angle_set,uint16_t value,uint8_t accelerometer){
  soft_start(MAX_SPEED,15);
  PID_TargetAngle(angle_set,value,accelerometer);
  DC[0].quaynghich(v_plus);
  DC[1].quaythuan(v_plus);
  DC[2].quaythuan(v_minus);
  DC[3].quaynghich(v_minus); 
}
void pid_rotate(int16_t angle_set,uint16_t value, uint8_t accelerometer){
  if (angle_set >= 0){
    PID_TargetAngle(angle_set,value,accelerometer);
    DC[0].quaythuan(speed);
    DC[1].quaythuan(speed);
    DC[2].quaythuan(speed);
    DC[3].quaythuan(speed);
  }
  else{
    //PID_TargetAngle(angle_set,value,accelerometer);
    DC[0].quaynghich(speed);
    DC[1].quaynghich(speed);
    DC[2].quaynghich(speed);
    DC[3].quaynghich(speed);
  }
}
void antony(){
  soft_start(80,8);
  DC[1].quaynghich(speed);
  DC[2].quaynghich(speed);
}
void set_angle(){
  soft_start(50,10);
  PID_TargetAngle(0,speed,10);
  DC[0].quaynghich(v_plus);
  DC[1].quaynghich(v_plus);
  DC[2].quaynghich(v_plus);
  DC[3].quaynghich(v_plus);
}

//================== PID controll ========================//
void ps3_ctrl_move(){
  //static int8_t status = 0;
  //handle_ctht();
  uart_get_angle();
  uart_get_button();
  if (but_data == button_up){
    status = 1;
    soft_start(MAX_SPEED,15);
    pid_up(angle_last,speed,100);
  }
  else if (but_data == button_down){
    soft_start(MAX_SPEED,15);
    status = -1;
    pid_down(angle_last,speed,120);
  }
  else if(but_data == button_right){
    soft_start(MAX_SPEED,15);
    status = 2;
    pid_right(angle_last,speed,120);
  }
  else if(but_data == button_left){
    soft_start(MAX_SPEED,15);
    status = -2;    
    pid_left(angle_last,speed,120);
  }
  else if(but_data == button_L2){
    status = 3;
    counter_clockwise(speed);
        //Serial.println("L2");
  }
  else if(but_data == button_R2){
    status = -3;
    rotate_clockwise(speed);
  }
  else if(but_data == (button_L3)){
    status = 4;
    antony();
    //DC_rotate.quaynghich(pwm);
  }
  else if(but_data == (button_R3)){
    set_angle();
    //DC_rotate.quaythuan(pwm);
  }
  else {
    angle_last = cur_angle; 
    if (speed != 0){
      DC_rotate.stop();
      soft_end(0,7);
      switch(status){
        case 1 : pid_up(angle_last,speed,100);
                 break;
        case -1: pid_down(angle_last,speed,100);
                 break;
        case 2 : pid_right(angle_last,speed,100);
                 break;
        case -2: pid_left(angle_last,speed,100);
                 break;
        case 3 : counter_clockwise(speed);
                 soft_end(0,5);
                 break;
        case -3: rotate_clockwise(speed);
                 soft_end(0,5);
                 break;
        case  4: DC[1].quaynghich(speed);
                 DC[2].quaynghich(speed);
                 soft_end(0,5);
                 break;
      }
    
    }
    else {
        DC_rotate.stop();
      DC[0].phanh();
      DC[1].phanh();
      DC[2].phanh();
      DC[3].phanh();
    }
    // angle_last = cur_angle;
   //stop_ball();
  }
  //Serial.print("angle_last:");Serial.println(angle_last);
}
void test(){
  DC[0].quaynghich(30);
  delay(5000);
  DC[1].quaynghich(30);
  delay(5000);
  DC[2].quaynghich(30);
  delay(5000);
  DC[3].quaynghich(30);
  delay(5000);

}
void move_up() {
  soft_start(MAX_SPEED, 15);
  DC[0].quaythuan(speed);
  DC[1].quaythuan(speed);
  DC[2].quaynghich(speed);
  DC[3].quaynghich(speed); 
}

void move_down() {
  DC[0].quaynghich(speed);
  DC[1].quaynghich(speed);
  DC[2].quaythuan(speed);
  DC[3].quaythuan(speed);
}

void move_left() {
  soft_start(MAX_SPEED, 15);
  DC[0].quaythuan(speed);
  DC[1].quaynghich(speed);
  DC[2].quaynghich(speed);
  DC[3].quaythuan(speed);  
}

void move_right() {
  soft_start(MAX_SPEED, 15);
  DC[0].quaynghich(speed);
  DC[1].quaythuan(speed);
  DC[2].quaythuan(speed);
  DC[3].quaynghich(speed); 
}

void rotate_clockwise() {
  DC[0].quaythuan(speed);
  DC[1].quaythuan(speed);
  DC[2].quaythuan(speed);
  DC[3].quaythuan(speed);
}

void rotate_counter_clockwise() {
  DC[0].quaynghich(speed);
  DC[1].quaynghich(speed);
  DC[2].quaynghich(speed);
  DC[3].quaynghich(speed);
}

void special_move() {
  soft_start(80, 8);
  DC[1].quaynghich(speed);
  DC[2].quaynghich(speed);
}

void set_default_position() {
  soft_start(50, 10);
  DC[0].quaynghich(speed);
  DC[1].quaynghich(speed);
  DC[2].quaynghich(speed);
  DC[3].quaynghich(speed);
}

//================== Main Control Function ========================//
void basic_move() {
  //uart_get_angle();
  uart_get_button();
  
  if (but_data == button_up) {
    status = 1;
    soft_start(MAX_SPEED, 15);
    move_up();
  }
  else if (but_data == button_down) {
    soft_start(MAX_SPEED, 15);
    status = -1;
    move_down();
  }
  else if (but_data == button_right) {
    soft_start(MAX_SPEED, 15);
    status = 2;
    move_right();
  }
  else if (but_data == button_left) {
    soft_start(MAX_SPEED, 15);
    status = -2;    
    move_left();
  }
  else if (but_data == button_L2) {
    status = 3;
    rotate_counter_clockwise();
  }
  else if (but_data == button_R2) {
    status = -3;
    rotate_clockwise();
  }
  else if (but_data == button_L3) {
    status = 4;
    special_move();
  }
  else if (but_data == button_R3) {
    set_default_position();
  }
  else {
    if (speed != 0) {
      DC_rotate.stop();
      soft_end(0, 7);
      switch(status) {
        case 1:  move_up(); break;
        case -1: move_down(); break;
        case 2:  move_right(); break;
        case -2: move_left(); break;
        case 3:  rotate_counter_clockwise(); soft_end(0, 5); break;
        case -3: rotate_clockwise(); soft_end(0, 5); break;
        case 4:  DC[1].quaynghich(speed);
                 DC[2].quaynghich(speed);
                 soft_end(0, 5); break;
      }
    }
    else {
      DC_rotate.stop();
      DC[0].phanh();
      DC[1].phanh();
      DC[2].phanh();
      DC[3].phanh();
    }
  }
}

  



