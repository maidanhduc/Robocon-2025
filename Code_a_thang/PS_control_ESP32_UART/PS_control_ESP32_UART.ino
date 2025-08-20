#include <Ps3Controller.h>
#include "Ps3.h"

int16_t joystickLX , joystickLY ,joystickRX, joystickRY;
uint16_t button;

/// xử lý ngắt uart
void IRAM_ATTR onReceive(){
    while(Serial2.available() > 0){
    char receive = Serial2.read();
    Serial.println(receive);
    if(receive == 'd'){
      Serial.println(receive);
      Serial2.write((uint8_t*)&button, 2);
      Serial2.write((uint8_t*)&joystickLX, 2);
      Serial2.write((uint8_t*)&joystickLY, 2);
      Serial2.write((uint8_t*)&joystickRX, 2);
      Serial2.write((uint8_t*)&joystickRY, 2);
    }

  }
}

void notify(){
 // up
    if(Ps3.data.button.up){
      button |= 0x01;
    }else{
      button &=~ 0x01;
    } 
  // down
    if(Ps3.data.button.down){
      button |= 0x02;
    }else{
      button &=~ 0x02;
    } 
  // right
    if(Ps3.data.button.right){
      button |= 0x04;

    }else {
      button &=~ 0x04;

    }
  // left
    if(Ps3.data.button.left){
      button |= 0x08;

    }else{
      button &=~ 0x08;
    }
  // L1
    if(Ps3.data.button.l1){
      button |= 0x10;
    }else{
      button &=~ 0x10;
    }
  // R1
    if(Ps3.data.button.r1 ){
      button |= 0x20;
    }else{
      button &=~ 0x20;
    }
  // L2
    if(Ps3.data.button.l2){
        button |= 0x40;
    }else{
      button &=~ 0x40;
    }
  // R2
  if(Ps3.data.button.r2){
      button |= 0x80;
    }else{
      button &=~ 0x80;
    }
  // cross
  if(Ps3.data.button.cross){
      button |= 0x100;
    }else{
      button &=~ 0x100;
    }
  // cricle
  if(Ps3.data.button.circle){
      button |= 0x200;
    }else{
      button &=~ 0x200;
    }
  // square
  if(Ps3.data.button.square){
      button |= 0x400;
    }else{
      button &=~ 0x400;
    }
  // triangle
  if(Ps3.data.button.triangle){
      button |= 0x800;
    }else{
      button &=~ 0x800;
    }
  // select
  if(Ps3.data.button.select){
      button |= 0x2000;
    }else{
      button &=~ 0x2000;
    }
  // start
  if(Ps3.data.button.start){

      button |= 0x4000;

    }else{

      button &=~ 0x4000;

    }
  // L3
  if(Ps3.data.button.l3){
      button |= 0x1000;

    }else{
      button &=~ 0x1000;
    }
  // R3
  if(Ps3.data.button.r3){
      button |= 0x8000;

    }else{
      button &=~ 0x8000;
 
  }

  //// joystick 
  joystickLX = Ps3.data.analog.stick.lx;
  joystickRX = Ps3.data.analog.stick.rx;
  joystickLY = Ps3.data.analog.stick.ly;
  joystickRY = Ps3.data.analog.stick.ry;

  joystickLX = -constrain(joystickLX,-508,508);
  joystickLY = -constrain(joystickLY,-508,508);
  joystickRX = -constrain(joystickRX,-508,508);
  joystickRY = -constrain(joystickRY,-508,508);
}

void onConnect(){
  Serial.println("Connected!.");
}

void setup(){
  Serial.begin(115200);
  Serial2.begin(115200,SERIAL_8N1,17,16);  // RX ------- TX
  Serial2.onReceive(onReceive);
  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin("f8:b3:b7:45:40:16"); 
  pinMode(2,OUTPUT);
  
}

bool led_2 = false;
unsigned long time_led;
int delta = 300;

void loop(){  

  //Serial.printf("button : 0x%04x  , LX : %d  ,  LY : %d , RX : %d , RY : %d \n",button,joystickLX,joystickLY,joystickRX,joystickRY);

  if(Ps3.isConnected()){
     if(button != 0 || joystickLX != 0|| joystickLY != 0 || joystickRX != 0|| joystickRY != 0)   delta = 100;
      else delta = 300;
      
  }else{
    delta = 1000;
  }

  if(millis() - time_led >= delta){
    digitalWrite(2,led_2);
    if(led_2) led_2 = false;
    else led_2 = true;
    time_led = millis();
  }
 

} 





     /* uint8_t Send_Data[10];
      // gửi giá trị nút bấm
      Send_Data[0] = (button >> 8) & 0xFF;
      Send_Data[1] = button & 0xFF;


      Serial.print("button : "); Serial.println((uint16_t)((Send_Data[0] << 8) | Send_Data[1]));
      // gửi giá trị joy LX
      Send_Data[2] = (joystickLX >> 8) & 0xFF;
      Send_Data[3] = (joystickLX) & 0xFF;

      // Gửi giá trị joy LY
      Send_Data[4] = (joystickLY >> 8) & 0xFF;
      Send_Data[5] = (joystickLY) & 0xFF;

      // Gửi giá trị joy RX
      Send_Data[6] = (joystickRX >> 8) & 0xFF;
      Send_Data[7] = (joystickRX) & 0xFF;

      // Gửi giá trị joy RY
      Send_Data[8] = (joystickRY >> 8) & 0xFF;
      Send_Data[9] = (joystickRY) & 0xFF;

      for(uint8_t i = 0; i < 10 ;i++){
        Serial2.write(Send_Data[i]);
      }
    }*/