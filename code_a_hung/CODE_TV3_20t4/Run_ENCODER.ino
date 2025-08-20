void rs_en(){
  xung  = 0;
  dist_en = 0;
}

void run_distance(int dist, int angle, int speed, int end_speed,  int ac_run, int ac_stop, int ac_angle, bool yes_no ){
  static int last_dist = 0; 
  static uint8_t  speed_run = 0; 
  static uint8_t  speed_run_1 = 0; 
  static uint32_t time_seta = 0;
  if(dist_en < dist - ac_stop){
    if(speed_run < speed){
      if(millis() - time_seta >= ac_run){
        speed_run ++; 
        time_seta = millis();
        speed_run_1 = speed_run; 
      }
    }
  }else if(speed_run != 0 && dist_en >= dist - ac_stop ){
    if(speed_run > end_speed ){
      speed_run =  (dist - dist_en) * (speed_run_1 )*1/ac_stop; 
    }else{
      speed_run = end_speed;
    }
    
  }else if(dist_en == dist){
    speed_run = 0; 
  }
  if(yes_no){
    down_pid(angle, speed_run, ac_angle, ac_angle + 20);
  }else{
    up(0);
    speed_run = 0;
  }

  Serial.println(speed_run);
}

//===========================================START_L_ENCODER=================================================//
bool run3l1 = true, run3l2 = false, run3l3 = false, run3l4 = false, run3l5 = false, run3l6 = false, run3l7 = false, run3l8 = false; 
void run_to_3L(){
  if(run3l1){
    run_distance(4200, 0, 180, 160, 10, 200, 100,1);
    stt_test = 1;
    if(dist_en >= 4150){
      run3l1 = false; 
      run3l2 = true; 
      rs_en();
    }
  }

  if(run3l2){
    run_distance(3450, -90, 130, 110, 1 ,200, 30,1); 
     stt_test = 2;
    if( dist_en >= 3440){
      run3l1 = false; 
      run3l2 = false;
      run3l3 = true;
      rs_en();  
    }
  }
  if(run3l3){
    run_distance(4000, 0, 100, 10, 1, 600, 50,1);
    //rotare(0);
    stt_test = 3;
    time_real = millis() - time_run_real;
    if( abs(compass()) < 20 && dist_en >= 4000){
      run3l1 = false; 
      run3l2 = false; 
      run3l3 = false; 
      run3l4 = true; 
      up(0);
     // rs_en();
      stt_test = 0;
    }
  }
}

//===========================================RETRY_L_ENCODER=================================================//
void run_to_3L_retry(){
  if(run3l1){
    run_distance(3200, -90, 130, 110, 2 ,300, 18,1); 
     stt_test = 2;
    if( dist_en >= 3100){
      run3l1 = false; 
      run3l2 = true;
      rs_en();  
      up(0);
    }
  }
  if(run3l2){
    run_distance(4000, 0, 100, 10, 1, 600, 50,1);
    //rotare(0);
    stt_test = 3;
    time_real = millis() - time_run_real;
    if( abs(compass()) < 20 && dist_en >= 4000){
      run3l1 = false; 
      run3l2 = false; 
      run3l3 = true; 
      up(0);
     // rs_en();
      stt_test = 0;
    }
  }
}


void p_run_to_3L(){
  rs_en();
  time_run_real = millis();
  run3l1 = true; 
  run3l2 = false; 
  run3l3 = false; 
  run3l4 = false;
  run3l5 = false; 
  run3l6 = false; 
  run3l7 = false; 
  run3l8 = false;

}