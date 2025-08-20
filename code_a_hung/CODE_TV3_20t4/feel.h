#define silo1 355
#define silo2 1115
#define silo3 1900
#define silo4 2650
#define silo5 3400

/// sl1 355, sl2 1115,sl3 1900,sl4 2650  , sl5 3400

/*  HIGH SPEED with sick DT50
void run_to_area_3(){
    static int v1 = 130;  
 if(button_up){
 // Serial.println("ok ok ok "); 
    if(millis() - timeb < 2000 || sick2() > 2250 ){
      if(h2h == 1 ){
        soft_state = true; 
        soft_start(7 , v1);
        h2h = 1; 
        up_pid(0,v_bot + 30, 50, 80); 
        Serial.println("up1"); 
      }if(h2h == 0 && h3h == 1){
        up_pid(-90,v_bot, 50, 40);
      }
    }else if(sick2() < 2400 && timeb > 1500 && h3h == 1){
        h2h = 0; 
        up_pid(-87,v_bot, 20, 40);
        Serial.println("up2"); 
        h3h = 1; 
        soft_state = true; 
    }
    if(sick2() < 2500 && compass() < -840){
        h3h = 0; 
        timec = millis();
     }
    if(h3h == 0 && h4h == 1){
      soft_state = true; 
      up_pid(0,v_bot, 50, 80);
      Serial.println("up3"); 
      if(millis() - timec > 1500 && compass() > -200){
        if( sick2() < 2600){
          h4h = 0; 
        }
      }
    }

    if(h4h == 0 && h5h == 1){
       //up_pid(90, v_bot, 80, 80);
      rotare(90);
      Serial.println("Dung_"); 
      h4h == 0; 
      if(sick2() < 1500){
        h5h = 0;   
      }
    }
    if(h5h == 0){
      //pid_sick1(2000, 90,50, 40, 100); 
      //stop();
     // up_pid(90, 0, 50, 80);
    // auto_ai_1(40, 40);
    }
 }else {
    up(0);
    soft_state = false; 
    h2h = 1; 
    timeb = millis(); 
    timec = millis(); 
  }
}

*/

