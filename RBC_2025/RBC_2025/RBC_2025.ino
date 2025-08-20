#include "RB_MAP.h"
#include "RB_func.h"
#include <Arduino.h>


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial1.write('a');
  
  setup_drive();
  
}
void loop() {
  // put your main code here, to run repeatedly:
   //UART_get_button();
   //Serial.println(speed);
   //UART_get_angle();
   //Speed_up(MAX_SPEED,1);
   DC[0].goForward(10);
   //ps3_ctrl();
   //Serial.print("vplus:");
   //Serial.println(v_plus);
   
   //Serial.print("vminus:");
   //Serial.println(v_minus);
   //delay(100);





  
  

}
