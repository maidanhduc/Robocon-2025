#include <Arduino.h>
#include "mapping.h"
#include "move_de.h"
#include "Uart.h"
void setup(){
  motorPidSetup();
  motor_rise_ball_setup();
  motor_shot_setup();
  pinMode(CTup,INPUT);
  pinMode(CTdown,INPUT);
  //pinMode(A0,INPUT_PULLUP);
  //pinMode(A1,INPUT_PULLUP);
  Serial3.begin(115200); // for compass
  Serial2.begin(115200); // for ESP32
  Serial1.begin(115200); // for send to screen
  Serial.begin(9600); // for debug
  Serial1.write('o');
  // pinMode(m0,OUTPUT);
  // pinMode(d0,OUTPUT);
   // test uart
   #if 1
   tx_data.mode_ball = 1;
   tx_data.mode_area = 2;
   
   tx_data.default_pwm[0] = 0;
   tx_data.default_pwm[1] = 10;
   tx_data.default_pwm[2] = 20;
   tx_data.default_pwm[3] = 30;

   tx_data.default_speed[0] = 0;
   tx_data.default_speed[1] = 1000;
   tx_data.default_speed[2] = 2000;
   tx_data.default_speed[3] = 3000;


   #endif  
}
void loop(){
  send_data_to_debug();
  ctrl_mode();
  ctrl_gun();
  ctrl_ball();
  //basic_move();
  //ps3_ctrl_move();
  //uart_get_speed();
  send_data_to_debug();
  //controll_mode_and_update();
 //uart_get_angle();
}