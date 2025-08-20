#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Robot.h"
#include <SimpleKalmanFilter.h>
#include "PS3_Index.h"

LiquidCrystal lcd(20, 21, 9, 27, 10, 25);  
SimpleKalmanFilter sick1_kal(2, 2, 0.01);
SimpleKalmanFilter sick2_kal(2, 2, 0.01);
SimpleKalmanFilter simpleKalmanFilter(2, 2, 0.01);

class motor {
protected:
  int m;
  int d;
public:
  motor(int ma, int da)
    : m(ma), d(da) {}

  void quaynghich(int speedd) {
    int speed = 255 - speedd;
    if (speed == 2) {
      speed = 3;
    }
    analogWrite(m, speed);
    digitalWrite(d, 0);
  }
  void quaythuan(int speedd) {
    int speed = 255 - speedd;
    if (speed == 2) {
      speed = 3;
    }
    analogWrite(m, speed);
    digitalWrite(d, 1);
  }
  void phanh() {
    analogWrite(m, 253);
    digitalWrite(d, 0);
  }
};

class basic_motor {
protected:
  int mc;
  int md;
  int dm;
public:
  basic_motor(int mcc, int mdd, int dmm) : mc(mcc), md(mdd), dm(dmm) {}

  void quaynghich(int speed) {
    analogWrite(dm, speed);
    digitalWrite(mc, 1);
    digitalWrite(md, 0);
  }
  void quaythuan(int speed) {
    analogWrite(dm, speed);
    digitalWrite(mc, 0);
    digitalWrite(md, 1);
  }
};

motor dc1(m1, d1);
motor dc2(m2, d2);
motor dc3(m5, d5);
motor dc4(m4, d4);


basic_motor dc11(m11, m12, pwm_m1);
basic_motor dc22(m21, m22, pwm_m2);
basic_motor dc33(m31, m32, pwm_m3);

uint32_t xung = 0; 
uint32_t dist_en; 
void encoder_(){
  xung ++; 
  dist_en = xung*PI_const*0.1; 
}

// ========================  MAIN CODE ========================  //
void setup() {
  
  
  // Set up encoder 
  attachInterrupt(5, encoder_, FALLING);
  //Set controller pin for PID driver
  pinMode(m1, OUTPUT);
  pinMode(d1, OUTPUT);

  pinMode(m2, OUTPUT);
  pinMode(d2, OUTPUT);

  pinMode(m3, OUTPUT);
  pinMode(d3, OUTPUT);

  pinMode(m4, OUTPUT);
  pinMode(d4, OUTPUT);

  pinMode(m5, OUTPUT);
  pinMode(d5, OUTPUT);

  pinMode(40, OUTPUT);
  pinMode(42, OUTPUT);
  pinMode(44, OUTPUT);
  
  // Set encoder input 
  pinMode(enb, INPUT);
  pinMode(ena, INPUT);

  //Basic motor setup
  pinMode(m11, OUTPUT);
  pinMode(m12, OUTPUT);
  pinMode(pwm_m1, OUTPUT);

  pinMode(m21, OUTPUT);
  pinMode(m22, OUTPUT);
  pinMode(pwm_m2, OUTPUT);
  
  pinMode(m31, OUTPUT);
  pinMode(m32, OUTPUT);
  pinMode(pwm_m3, OUTPUT);
  //SICK 
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);

  //SS ball 
  pinMode(33, INPUT); 
  pinMode(35, INPUT); 
  pinMode(39, INPUT); 
  pinMode(A13, INPUT); 
  pinMode(A14, INPUT); 
  pinMode(A12, INPUT); 

  // Serrial
  Serial3.begin(115200);  // For Gyro scop ( compass())
  Serial2.begin(115200);  // For jetson_tx2
  Serial1.begin(115200);  // For debug
  Serial.begin(115200);   // for debug

  dc1.quaythuan(0);
  dc2.quaythuan(0);
  dc3.quaythuan(0);
  dc4.quaythuan(0);
  
  Serial.println("1");
  lcd.begin(20, 4);
  Serial3.print("a"); 
  cam(0);
  
  uart_jet(); 
}
uint32_t time_123 =0, time_234 = 0; 
char xaa;
bool action = true;
int h2h = 0;
int h4h = 1;
int h5h = 1;
int dist_set = 0;
int now_angle = 0;
int color();
int stt_run = 0 ; 


void chose_mode(){
  if(pick_fell){        // ở mức cao khi chọn sân xanh. CHỌN SÂN XANH ( bên Phải R ) 
    if(!pick_retry){    // Chọn ở chế độ không retry START
      auto_3R(); 
    }else{
      auto_3R_retry(); 
    }
  }else{                // ở muc thấp chọn sân Đỏ. Chọn sân Đỏ( bên tría L )
    if(!pick_retry){    // Chọn ở chế độ không retry START
      auto_3L(); 
    }else{
      auto_3L_retry(); 
    }
  }
}

bool rscp = true; 
static uint32_t set_angle1=0;
void loop() {
  Serial.println(button_up);
 // receiver_debug();
  ps3();
  if(bt_in)
  {
    
    soft_start(60,10);
     up_pid(set_angle1, 60, 100, 100);
  }
  else if(button_down)
  {
    
    soft_start(60,10);
     down_pid(set_angle1, 60, 100, 100);
  }
  else 
  { 
    set_angle1=compass();
  }
  //uart_jet();  
  // if(start){ 
  //   chose_mode(); 
  //   //auto_ai_L_2(70, 70);
  //   //goto_silo_L(kr11);
  //   //Serial.println(sick2());
  //   if(!rscp){
  //     rscp = true; 
  //   }
  // }else{
  //   if(rscp){
  //     Serial3.print('a');
  //     rscp = false; 
  //   }
  //   up(0); 
  //   cam(1);  
  //   p_auto_3R();
  //   p_goto_silo_R(); 
  //   p_goto_silo_jetson(); 
  //   p_auto_3L();
  //   p_goto_silo_L(); 
  // }
  debug("ENCODER : " + String(dist_en) + " |||   COMPASS: " + String(compass()) + + " |||   STT : " + String(stt_test) + "||| TIME: " + String(time_real));
  /*if(re_debug == 1){
    p_run_to_3L();
    rs_compass(1);
    run_distance(0, 0, 0, 0, 0, 0, 0,0);

  }else if( re_debug == 2){
   
      //down_pid(0, 100, 100, 100); 
      //run_distance(5200, 0, 150, 25, 10,600, 100);
      run_to_3L();
      rs_compass(0);
    
  }else if(re_debug == 3){
    run_to_3L_retry();
    rs_compass(0);
  }else{
    rs_compass(0);
  }*/
// Serial.print(" mode : ") ;Serial.print(pick_mode); 
// Serial.print(" fell : ") ;Serial.print(pick_ruler); 
//  Serial.print(" stt_run: ") ;Serial.print(stt_run); 
// Serial.print(" Sick : ") ;Serial.println(sick2()); 
//  Serial.print(" color: "); Serial.println(color()); 
  // Serial.print(" ENCODER : ") ;Serial.print(dist_en); 
  // Serial.print(" sick 2 : ") ;Serial.println(re_debug); 

}
