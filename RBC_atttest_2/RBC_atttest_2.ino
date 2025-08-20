#include "RB_MAP.h"
#include "RB_func.h"
#include <Arduino.h>


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200); // for screen
  Serial3.begin(115200);
  setup_drive();

  DC[0].goForward(0); // ban 1
  DC[1].goForward(0); // ban 2
  DC[2].goForward(0); //  xoay than
  DC[3].goForward(0); // pheu ban 

  v1.thu(); // dong mo
  v2.thu(); // len bong
  v3.thu(); // do bong
// test uart
   #if 1
   tx_data.mode_ball = 2;
   tx_data.mode_area = 1;
   /*
   tx_data.default_pwm[0] = 0;
   tx_data.default_pwm[1] = 10;
   tx_data.default_pwm[2] = 20;
   tx_data.default_pwm[3] = 30;

   tx_data.default_speed[0] = 0;
   tx_data.default_speed[1] = 1000;
   tx_data.default_speed[2] = 2000;
   tx_data.default_speed[3] = 3000;
   */

   #endif  
}
void loop() {
    uart_get_stm32();
    shoot();
    vom_bong(10);
    bat_bong();
    auto_ball();
    rolate();
    //Serial.println(digitalRead(ctp_duoi));
    //Serial.println(digitalRead(ctp_tren));
    dribbing();
// debug by human
    send_data_to_debug();
    ctrl_mode();
    update_data_screen();
    auto_pwm_shoot();
    //debug_huhu();
    //delay(10);



}
