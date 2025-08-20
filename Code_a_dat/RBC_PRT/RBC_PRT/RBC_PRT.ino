#include "RB_MAP.h"
#include "RB_func.h"
#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  // for debug
  Serial1.begin(115200); // for screen
  Serial2.begin(115200); // for esp
  Serial3.begin(115200); // for compass
  Serial3.write('a');
  setup_drive();
  dc_ban.goForward(0);
  DC[0].goForward(2);
  DC[1].goForward(2);
  DC[2].goForward(2);
  DC[3].goForward(2);
// test uart
   #if 0
   tx_data.mode_ball = 3;
   tx_data.mode_area = 3;
   
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
void loop() {
  // put your main code here, to run repeatedly:
    UART_get_button();
    
    //send_data_to_debug();
// shoot
    UART_get_angle();
    ball_ctrl(10);
    gun_ctrl();
    ang_ctrl(30);
// move
    ps3_move();
// debug by human
    send_data_to_debug();
    ctrl_mode();
    update_data_screen();
    auto_pwm_shoot();
    //debug_huhu();
    //delay(10);
  
    //move(0, 30, 450, 20, 1, 1);
    // DC[0].goForward(20);
    // DC[1].goForward(20);
    // DC[2].goForward(0);
    // DC[3].goForward(0);
    //Serial.println(but_data);
    // Serial.println(joy_RY);
    // Serial.println(joy_RX);


    

    

}
