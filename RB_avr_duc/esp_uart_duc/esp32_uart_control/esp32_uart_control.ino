#include <Ps3Controller.h>
#include "Ps3.h"

uint32_t button_send = 0x00; 
unsigned long timeJump = 0;
bool connect = false;
bool ledState = false;
void  right_stick(){
    short rightX = Ps3.data.analog.stick.rx;
    short rightY = Ps3.data.analog.stick.ry;
    int X = ((float)rightX/128*254);
    int Y = ((float)rightY/128*254);
    uint8_t SX = abs(X);
    uint8_t SY = abs(Y);
    //Serial.println(Y); 

    if(X  <- 155){
      button_send |= rl; 
      button_send |= SX; 
      stt[23] = true; 
    }else if(stt[23]){
      button_send ^= rl;
      button_send ^= SX; 
      stt[23] = false; 
    }

    if(X  > 155){
      button_send |= rr; 
      button_send |= SX; 
      stt[24] = true; 
    }else if(stt[24]){
      button_send ^= rr;
      button_send ^= SX; 
      stt[24] = false; 
    }

    if(Y  > 155){
      button_send |= rd; 
      button_send |= SY; 
      stt[22] = true; 
    }else if(stt[22]){
      button_send ^= rd;
      button_send ^= SY; 
      stt[22] = false; 
    }

    if(Y  < -155){
      button_send |= ru; 
      button_send |= SY; 
      stt[21] = true; 
    }else if(stt[21]){
      button_send ^= ru;
      button_send ^= SY; 
      stt[21] = false; 
    }
}
  
void  left_stick(){
    short leftX = Ps3.data.analog.stick.lx;
    short leftY = Ps3.data.analog.stick.ly;
    int X = ((float)leftX/128*254);
    int Y = ((float)leftY/128*254);
    uint8_t SX = abs(X);
    uint8_t SY = abs(Y);
    uint8_t rss = 0; 
    //Serial.println(Y); 

    if(X  <- 150){
      button_send |= ll; 
      button_send |= SX; 
      stt[19] = true; 
    }else if(stt[19]){
      button_send ^= ll;
      button_send ^= SX; 
      stt[19] = false; 
    }

    if(X  > 150){
      button_send |= lr; 
      button_send |= SX; 
      stt[20] = true; 
    }else if(stt[20]){
      button_send ^= lr;
      button_send ^= SX; 
      stt[20] = false; 
    }

    if(Y  > 150){
      button_send |= ld; 
      button_send |= SY; 
      stt[18] = true; 
    }else if(stt[18]){
      button_send ^= ld;
      button_send ^= SY; 
      stt[18] = false; 
    }

    if(Y  < -150){
      button_send |= lu; 
      button_send |= SY; 
      stt[17] = true; 
    }else if(stt[17]){
      button_send ^= lu;
      button_send ^= SY; 
      stt[17] = false; 
    }
}

void notify(){
  if(Ps3.data.button.l2){
    button_send |= button_l2; 
    stt[1] = true; 
  }else if(stt[1]){
    button_send ^= button_l2; 
    stt[1] = false; 
  }

  if(Ps3.data.button.l1 ){
    button_send |= button_l1; 
    stt[2] = true; 
  }else if(stt[2]){
    button_send ^= button_l1; 
    stt[2] = false; 
  }

  if(Ps3.data.button.up ){
    button_send |= button_up; 
    stt[3] = true; 
  }else if(stt[3]){
    button_send ^= button_up; 
    stt[3] = false; 
  }

  if(Ps3.data.button.left ){
    button_send |= button_left; 
    stt[4] = true; 
  }else if(stt[4]){
    button_send ^= button_left; 
    stt[4] = false; 
  }
  
  if(Ps3.data.button.down ){
    button_send |= button_down; 
    stt[5] = true; 
  }else if(stt[5]){
    button_send ^= button_down; 
    stt[5] = false; 
  }
  
  if(Ps3.data.button.right ){
    button_send |= button_right; 
    stt[6] = true; 
  }else if(stt[6]){
    button_send ^= button_right; 
    stt[6] = false; 
  }
  
  if(Ps3.data.button.select ){
    button_send |= button_select; 
    stt[7] = true; 
  }else if(stt[7]){
    button_send ^= button_select; 
    stt[7] = false; 
  }
  
  if(Ps3.data.button.start ){
    button_send |= button_start; 
    stt[8] = true; 
  }else if(stt[8]){
    button_send ^= button_start; 
    stt[8] = false; 
  }
  
  if(Ps3.data.button.square ){
    button_send |= button_square; 
    stt[9] = true; 
  }else if(stt[9]){
    button_send ^= button_square; 
    stt[9] = false; 
  }
  
  if(Ps3.data.button.cross){
    button_send |= button_x; 
    stt[10] = true; 
  }else if(stt[10]){
    button_send ^= button_x; 
    stt[10] = false; 
  }
  
  if(Ps3.data.button.circle ){
    button_send |= button_circle; 
    stt[11] = true; 
  }else if(stt[11]){
    button_send ^= button_circle; 
    stt[11] = false; 
  }
  
  if(Ps3.data.button.triangle ){
    button_send |= button_triangle; 
    stt[12] = true; 
  }else if(stt[12]){
    button_send ^= button_triangle; 
    stt[12] = false; 
  }

  if(Ps3.data.button.r1 ){
    button_send |= button_r1; 
    stt[13] = true; 
  }else if(stt[13]){
    button_send ^= button_r1; 
    stt[13] = false; 
  }
  
  if(Ps3.data.button.r2 ){
    button_send |= button_r2; 
    stt[14] = true; 
  }else if(stt[14]){
    button_send ^= button_r2; 
    stt[14] = false; 
  }
  
  if(Ps3.data.button.l3 ){
    button_send |= button_l3; 
    stt[15] = true; 
  }else if(stt[15]){
    button_send ^= button_l3; 
    stt[15] = false; 
  }
  
  if(Ps3.data.button.r3 ){
    button_send |= button_r3; 
    stt[16] = true; 
  }else if(stt[16]){
    button_send ^= button_r3; 
    stt[16] = false; 
  }


  right_stick(); 
  left_stick(); 
}
void onDisconnect(){
  connect = false;
  digitalWrite(18, LOW);
}
void onConnect(){
  Serial.println("Connected!.");
  connect = true;
}

void setup(){
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Ps3.attach(notify);
  Ps3.attach(right_stick);
  Ps3.attach(left_stick);
  Ps3.attachOnConnect(onConnect);
  //Ps3.begin("8c:7c:b5:77:ff:bd"); 
  //Ps3.begin("12:4b:2c:0b:00:00"); 
  

  Ps3.begin("00:00:00:00:00:00");  // tay của đức
  //Ps3.begin("00:05:01:04:00:00");
  // Ps3.begin("d4:d4::da:e3:a3:c4");
  pinMode(2, OUTPUT);
  //Ps3.begin("1a:2b:3c:01:01:01");
  //Ps3.begin("8c:7c:b5:77:ff:bd");
  Serial.println("Ready.");
  
   for(int a = 0; a < 26; a++){
    stt[a] = false; 
  }

}

void loop(){ 
  if(connect){
    if(micros() - timeJump > 450000){
      ledState = !ledState;
      digitalWrite(2, ledState);
      timeJump = micros();
    }
  } 
  button_send |= 0x01; 
  Serial.println(button_send);
  uint32_t input = button_send;
  uint8_t byte_a[5];  
  byte_a[0] = '}'; 
  byte_a[1] = (input >> 24) & 0xFF; // Lấy byte cao nhất 
  byte_a[2] = (input >> 16) & 0xFF; // Lấy byte thứ hai từ trái qua
  byte_a[3] = (input >> 8) & 0xFF;  // Lấy byte thứ ba từ trái qua
  byte_a[4] = input & 0xFF;         // Lấy byte thấp nhất

  if(Serial2.available() > 0){
    if(Serial2.read() == 'p'){
      for(int ax = 0; ax < 5; ax++){
        Serial2.write(byte_a[ax]); 
        Serial.println(button_send);
      }
      //Serial2.write(button_send); 
    }
  }

} 