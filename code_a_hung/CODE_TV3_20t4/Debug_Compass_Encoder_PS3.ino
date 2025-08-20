
// ========================  DEBUG ========================  //
void debug(String data){   // Dùng serial bao nhiêu thì dùng nhé
  //if(Serial2.available()){
    // char t = Serial2.read(); 
    // if(t = '='){
      Serial2.println(data);
  //   }
  // }
}

void receiver_debug(){
  static uint8_t last_re_debug = 0;
  static String data_receiver = "";
  if(Serial2.available()){
      data_receiver = Serial2.readStringUntil('\n');
     // Serial.print(data_receiver); 
      re_debug = data_receiver.toInt();
      if(re_debug == 0){
        re_debug = last_re_debug;
      }   
  }
  last_re_debug = re_debug;
}
// ========================  PS 3 ========================  //
void ps3() {
  static uint32_t  combined_data = 0; 
  Serial2.print('p'); 
    uint8_t byte_[4] = {0x00,0x00,0x00,0x00};
    delay(2); 
   if(Serial2.available()){
    // for(int ps3read = 0; ps3read <= 3; ps3read++){
    //    byte_[ps3read] = Serial.read(); 
    // }
      char haha = Serial2.read(); 
 
      if(haha == '}'){
      uint8_t byte_1 = Serial2.read(); 
      uint8_t byte_2 = Serial2.read(); 
      uint8_t byte_3 = Serial2.read(); 
      uint8_t byte_4 = Serial2.read();
        combined_data = ((uint32_t)byte_1 << 24) | ((uint32_t)byte_2 << 16) | ((uint32_t)byte_3 << 8) | byte_4;
      }      
    }
     
   
   
   bt_in = combined_data; 
}




// ========================  La bàn FEE ========================  //
// void compass() {
//   Serial2.write('z');
//   if(Serial2.available()){
//     delay(5);
//     uint8_t f_byte = Serial2.read(); 
//     uint8_t low_byte = Serial2.read();
//     uint8_t high_byte = Serial2.read();
//     angle = (high_byte << 8) | (low_byte); 
//     angle = angle /10; 
//   }
// }
//========================= Reset_góc ==========================  //
bool last_compass = 0; 
void rs_compass( bool cp){
  if(cp!= last_compass){
    if(cp == 1) Serial3.println("a");  
    last_compass = cp; 
  }
}
// ========================  La bàn TGC ========================  //
int compass() {
  Serial3.print('z');
  static int16_t value = 0;
  if(Serial3.available()){
    delay(5);
    uint8_t high_byte = Serial3.read();
    uint8_t low_byte = Serial3.read();
    value = (high_byte << 8) | (low_byte); 
   }
  return(value); 
}

// ========================  ENCODER ========================  //
// void encoder_(){
//   if( digitalRead(enb) == 0){
//     numof_rotary++; 
//   }else{
//     numof_rotary--; 
//   }
// }