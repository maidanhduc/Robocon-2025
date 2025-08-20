bool st1 = true, st2 = false , st3 = false, st4 = false, st5 = false;        // Sick 2 nhe anh em 
uint32_t delay_st1 = 0, delay_st2 = 0, delay_st3 = 0, delay_st4 = 0 ; 
int time_st1 = 2200, time_st2 , time_st3; 
int v_auto = 100; 
// thêm một con cảm biến số 1 tên là cb_run
// Các hàm chạy sau khi kết thúc sẽ có một hàm reset các biến trạng thái của nó mở đầu bằng "p"
//============================ Tự động chạy từ điểm xuất phát tới khu vực 3 sân bên phải ( chưa PID sick) ==========================// 
void auto_3R(){
  if(st1 && millis() - delay_st1 < time_st1 ){
    soft_state = true; 
    soft_start(7 , v_auto);
    down_pid(0,v_bot + 30, 50, 80); 
    st2 = true; 
  }else if(millis() - delay_st1 > time_st1 && cb_run && st2){
    right_pid(0,v_bot, 60, 60); 
    st1 = false ; 
    st3 = true; 
    delay_st2 = millis();
  }
  if(st3 && !cb_run || st3 && sick1() < 1750 && sick2() > 3000){
    if(millis() - delay_st2 < 10){
      right_pid(0,v_bot, 60, 60); 
      st1 = false;
      st2 = false;
      delay_st3 = millis();
    }else{
      if(millis() - delay_st3 <= 2600){
        down_pid(0,v_bot, 30, 60); 
    }else{
        rotare(90);
        if(compass() > 870){
          st1 = false;
          st2 = false;
          st3 = false;
          st4 = true; 
          delay_st4 = millis(); 
        }
      }
    }
  }
  if(st4 && compass() > 860){
    if(millis() - delay_st4 < 300){
      up_pid(87,v_bot - 30, 60, 60); 
    }else{
      st1 = false;
      st2 = false;
      st3 = false; 
      st4 = false;
      up(0); 
    }
  }
  if(!st1 && !st2 && !st3 && !st4){
    // up(0);
    Serial.println("ok Dang run");
    if(!pick_mode){   // ở chế độ mode ngẫu nhiên NGẪU NHIÊN
      auto_ai_R_1(50,70);   // kich ban 1 : 
    }else{             // Ngược lại chạy ở chế độ tự động silo chọn ( khi đèn xanh ở mode sáng )
      auto_ai_R_2(50,70);  //kich ban 2 -
    }    
    
  }
}

//============================ Tự động chạy từ điểm RETRY tới khu vực 3 sân bên phải ( chưa PID sick) ==========================// 
void auto_3R_retry(){
  if(st1 && millis() - delay_st1 < 600 ){
    v_bot = 100;
    down_pid(0,v_bot - 20, 50, 80); 
    st2 = true; 
  }else if(millis() - delay_st1 > 600 && cb_run && st2){
    right_pid(0,v_bot, 60, 60); 
    st1 = false ; 
    st3 = true; 
    delay_st2 = millis();
  }
  if(st3 && !cb_run || st3 && sick1() < 1750){
    if(millis() - delay_st2 < 20){
      right_pid(0,v_bot, 60, 60); 
      st1 = false;
      st2 = false;
      delay_st3 = millis();
    }else{
      if(millis() - delay_st3 <= 2600){
        down_pid(0,v_bot, 30, 60); 
    }else{
        rotare(90);
        if(compass() > 870){
          st1 = false;
          st2 = false;
          st3 = false;
          st4 = true; 
          up(0); 
          delay_st4 = millis(); 
        }
      }
    }
  }
  if(st4 && compass() > 860){
    // if(millis() - delay_st4 < 1000){
    //   up_pid(87,v_bot, 60, 60); 
    // }else{
      st1 = false;
      st2 = false;
      st3 = false; 
      st4 = false;
    //   up(0); 
    // }
  }
  if(!st1 && !st2 && !st3 && !st4){

    //======================================= Chay khu vuc 3 tu don============================== 
    if(!pick_mode){   // ở chế độ mode ngẫu nhiên NGẪU NHIÊN
      auto_ai_R_1(50,70);   // kich ban 1 : 
    }else{             // Ngược lại chạy ở chế độ tự động silo chọn ( khi đèn xanh ở mode sáng )
      auto_ai_R_2(50,70);  //kich ban 2 
    } 
  }
}

//======================================== Ham tu dong chay toi silo R theo sick 2 =========================================//
// /// sl1 355, sl2 1115,sl3 1900,sl4 2650  , sl5 3400
bool st_silo_r1 = true, st_silo_r2 = false, st_silo_r3 = false, st_silo_r4 = false;
uint32_t delay_silo_r1 = 0, delay_silo_r2 = 0;
void goto_silo_R(int stt_silo){
  static int dist_silo = 0;
  dist_silo = stt_silo;
  if(!cb_run && st_silo_r1 || st_silo_r1 && sick1() + sick2() < 4800){
    down_pid(90,80, 80, 80);
    delay_silo_r1 = millis();
  }else if(cb_run && abs(compass() - 900) < 50 && sick1() + sick2() > 5000 && st_silo_r1){
    st_silo_r1 = false;
    st_silo_r2 = true; ////
  }
  if(st_silo_r2){
     pid_sick2(dist_silo,90, 55, 70, 6);
     if(abs(sick2() - dist_silo) < 15){
      if(millis() - delay_silo_r2 > 400){
        st_silo_r1 = false; 
        st_silo_r2 = false; 
        st_silo_r3 = true; 
      }
     }else{
      delay_silo_r2 = millis();
     }
  }
  if(st_silo_r3){
    down_pid(90,50, 40, 40);
    if(silo){
      st_silo_r1 = false; 
      st_silo_r2 = false; 
      st_silo_r3 = false;
      st_silo_r4 = true;
    }
  }
  if(st_silo_r4){
   //up(0);
   nha_bong();    // Thao tác cuối cùng sau khi tới silo 
  }
}

//====================================== Ham dung khi lui lay bong ma khong thay bong nao ==================================//
bool ffb1 = true, ffb2 = false, ffb3 = false, ffb4 = false;
uint32_t delay_ffb1 = 0, delay_ffb2 = 0, delay_ffb3 = 0; 
void up_for_find_ball(){
  if(ffb1){
    if(millis() - delay_ffb1 < 500){
      down_pid(90,70, 80, 80);
    }else{
      ffb1 = false; 
      ffb2 = true; 
    }
  }
  if(ffb2){

  }
}

//============================================goto_silo_with_detech silo===========================================//
double kppoint = 0.08 ,  kipoint = 0.15, kdpoint = 0.001; 
void runto_silo_R(int set_point_silo, int set_angle, int v_check_point, int v_angle, int false_set){
  if(millis() - last_time > 10){
    set_angle = compass()/10; 
    error = set_angle - angle;
    static int error_point  = 0; 
    static int output_point = 0;
    static int last_error_point = 0;  
    int real_point = geo_x; 
    error_point = set_point_silo - real_point; 
    if(set_point >= abs(error_point)){
       error_point = 0;
    }
    output_point = kppoint * error_point + kdpoint*(error_point  - last_error_point) + kipoint*(error_point + last_error_point ); 
    last_error_point = error_point;
    output_point = constrain(output_point, - v_check_point, v_check_point);  
    // Hieu chinh pid angle tuong ung 
    if( -4 < error < 4){
      kp = 2 ; ki = 0.17; kd = 0.02; 
    }
    else {
      kp = 1.6; ki = 0.2; kd = 0.04;
    }
    output = (kp) * error + kd * (error - last_error) + ki * (error + last_error);
    last_error = error;
    output = constrain(output, -v_angle, v_angle);

    if(output_point < 10 && output_point > 0){
      output_point = 10; 
    }
    if(output_point > -10 && output_point < 0){
      output_point = -10; 
    }
    if(output_point < 0){
      left( abs(output_point),-output); 
    }else if(output_point > 0){
      right(output_point,-output); 
    }
    last_time = millis(); 
  }
  // Serial.print(" geo_x  : ") ;Serial.print(geo_x); 
  // Serial.print(" out_point  : ") ;Serial.print(output_point); 
  // Serial.print(" out_angle : "); Serial.println(output);
} 

//=====================================================go to silo wwith jetson ====================================================//

void goto_siloR_jetson(){
  if(gsj1){
    down_pid(0, 100, 50, 70); 
    Serial.print("_______Step1_______ ");
    if(abs(compass()) < 50){
      gsj1 = false; 
      gsj2= true; 
    }
  }
  if(gsj2){
    flow_silo(150,0,100, 50, 10); 
    Serial.print("_______Step2_______ ");
    if(cb_run){
      gsj1 = false; 
      gsj2 = false; 
      gsj3 = true; 
    }
  }
  if(gsj3){
    runto_silo_R(0, 0 , 40, 40, 10); 
    Serial.print("_______Step3_______ ");
    if(abs(compass()) < 50 && abs(geo_x) < 50){
      gsj1 = false; 
      gsj2 = false; 
      gsj3 = false; 
      gsj4 = true; 
    }
  }
  if(gsj4){
    flow_silo(150,0,25, 50, 10);
    Serial.print("_______Step4_______ ");
    if(silo){
      gsj1 = false; 
      gsj2 = false; 
      gsj3 = false; 
      gsj4 = false; 
      gsj5 = true;
    }
  }
  if(gsj5){
    Serial.print("_______Step5_______ ");
    nha_bong(); 
  }
}

//==========================================Jetsom + sick (dung chung cac khoa voi goto jetson nhes anhe) ==========================================//

void goto_siloR_jetson_sick(){
  if(gsj1){
    down_pid(90, 100, 50, 70); 
    Serial.print("_______Step1_______ ");
    if(abs(compass() - 900) < 50){
      gsj1 = false; 
      gsj2= true; 
    }
  }
  if(gsj2){
    flow_silo(150,0,100, 50, 10); 
    Serial.print("_______Step2_______ ");
    if(cb_run && sick1() + sick2() > 4160){
      gsj1 = false; 
      gsj2 = false; 
      gsj3 = true; 
    }
  }
  if(gsj3){
    flow_silo(150,0,25, 50, 10);
    Serial.print("_______Step3_______ ");
    if(silo || siloo){
      gsj1 = false; 
      gsj2 = false; 
      gsj3 = false; 
      gsj4 = true; 
    }
  }
  if(gsj4){
    Serial.print("_______Step4_______ ");
     // Silo bên trái, siloo bên phải
    if(silo){
      down_rl(0, 15); 
    }else if(siloo){
      down_rl(15, 0); 
    }else if(!silo && !siloo){
      down_pid(90, 15, 50, 70); 
    }else if(silo && siloo){
      stop(); 
    }
    if(silo && siloo && abs(compass() - 900) < 80 ){
      gsj1 = false; 
      gsj2 = false; 
      gsj3 = false; 
      gsj4 = false; 
      gsj5 = true; 
      delay_step_5 = millis(); 
      up(0);
    }
  }
  if(gsj5){
    Serial.print("_______Step5_______ ");
    if(stt_pick1){
      up(0); 
      if(millis() - delay_step_5 > 200){
        if(sick2() < (kr11 + kr01)*0.5){
          true_silo = kr01; 
          stt_pick1 = false;  
        }else if(sick1() >= (kr11 + kr01)*0.5  && sick1() < (kr21+kr11)*0.5){
          true_silo = kr11; 
          stt_pick1 = false; 
        }else if(sick1() >= (kr11 + kr21)*0.5  && sick1() < (kr21+kr31)*0.5){
          true_silo = kr21; 
          stt_pick1 = false; 
        }else if(sick1() >= (kr21 + kr31)*0.5  && sick1() < (kr31+kr41)*0.5){
          true_silo = kr31; 
          stt_pick1 = false; 
        }else if(sick1() >= (kr31 + kr41)*0.5  ){
          true_silo = kr41; 
          stt_pick1 = false; 
        }
      }
    }else{
      pid_sick2(true_silo, 90, 25, 50, 6);
      if(abs( sick2() - true_silo) < 30){
        gsj1 = false; 
        gsj2 = false; 
        gsj3 = false; 
        gsj4 = false; 
        gsj5 = false;
        gsj6 = true; 
      }
    }
    Serial.print(true_silo); 

  }
  if(gsj6){
    Serial.print("_______Step6_______ ");
    true_silo = 0; 
    stt_pick1 = true; 
    down_pid(90, 18, 50, 70); 
    if(siloo||silo||silo && siloo){
      gsj1 = false; 
      gsj2 = false; 
      gsj3 = false; 
      gsj4 = false; 
      gsj5 = false;
      gsj6 = false;
      gsj7 = true; 
      up(15); 
    } 
  }
  if(gsj7){
    Serial.print("_______Step7_______ ");
    nha_bong(); 
  }
}

//============================ Hàm RESET ==========================//
void p_auto_3R(){
  v_bot = 0; 
  soft_state = false; 
  st1 = true; 
  st2 = false;
  st3 = false; 
  st4 = false; 
  delay_st1 = millis(); 
}

void p_goto_silo_R(){
  st_silo_r1 = true; 
  st_silo_r2 = false; 
  st_silo_r3 = false; 
  st_silo_r4 = false;
}

void p_up_for_find_ball(){
  ffb1 = true; 
  ffb2 = false; 
  ffb3 = false; 
  delay_ffb1 = millis(); 
}

void p_goto_silo_jetson(){
  gsj1 = true; 
  gsj2 = false; 
  gsj3 = false; 
  gsj4 = false; 
  gsj5 = false; 
  gsj6 = false; 
  gsj7 = false; 
}
// Ham kich ban lan luot 
