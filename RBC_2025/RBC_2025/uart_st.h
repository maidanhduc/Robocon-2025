#include "Arduino.h"
#include <stdint.h>
#include "HardwareSerial.h"

uint8_t data_rx[2];
uint8_t data_rx_angle[2];
uint16_t but_data = 0x00;
int16_t cur_angle;
void UART_get_button(){
  Serial2.write('o');
  if(Serial2.available()>0){
    data_rx[0] = Serial2.read();
    data_rx[1] = Serial2.read();
    but_data = data_rx[0] << 8 | data_rx[1];
    //Serial.println(but_data);
    //delay(10);
  }
}
void UART_get_angle(){
  Serial1.write('z');
  delay(5);
  if(Serial1.available()>0){
    data_rx_angle[0] = Serial1.read();
    data_rx_angle[1] = Serial1.read();
    cur_angle = data_rx_angle[0] << 8 | data_rx_angle[1];
    //Serial.println(cur_angle);
  }
}



