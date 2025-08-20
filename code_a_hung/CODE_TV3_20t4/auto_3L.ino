bool st1l = true, st2l = false , st3l = false, st4l = false, st5l = false;   // sick2
uint32_t delay_st1l = 0, delay_st2l = 0, delay_st3l = 0, delay_st4l = 0 ; 
int time_st1l = 2300, time_st2l , time_st3l; 
int v_autol = 100; 
// thêm một con cảm biến số 1 tên là cb_run
// Các hàm chạy sau khi kết thúc sẽ có một hàm reset các biến trạng thái của nó mở đầu bằng "p"
//============================ Tự động chạy từ điểm xuất phát tới khu vực 3 sân bên trái ( Đã có PID sick) ==========================// 
void auto_3L(){
  if(st1l && millis() - delay_st1l < time_st1l || st1l && !cb_run){
    soft_state = true; 
    soft_start(7 , v_autol);
    down_pid(0,v_bot + 30, 50, 80); 
    st2l = true; 
  }else if(millis() - delay_st1l > time_st1l && cb_run && st2l){
    // left_pid(0,v_bot, 60, 60); 
    pid_sick1(3900,0, v_bot, 100, 20);
    st1l = false ; 
    st3l = true; 
    delay_st2l = millis();
  }
  if(st3l && !cb_run || st3l && sick1() > 3850){
    if(millis() - delay_st2l < 10){
      //left_pid(0,v_bot, 60, 60); 
      st1l = false;
      st2l = false;
      delay_st3l = millis();
    }else{
      if(millis() - delay_st3l <= 2400){   // thong so chuan la 2400
        down_pid(0,v_bot, 30, 60); 
      }else{
        rotare(-90);
        if(compass() < -870){
          st1l = false;
          st2l = false;
          st3l = false;
          st4l = true; 
          delay_st4l = millis(); 
        }
      }
    }
  }
  if(st4l && compass() < -860){
    if(millis() - delay_st4l < 1000){    // chuan la 1000 minh cho la khong up pid
      //up_pid(-87,v_bot, 60, 60);
      up(0);
    }else{
      st1l = false;
      st2l = false;
      st3l = false; 
      st4l = false;
      up(0); 
    }
  }
  if(!st1l && !st2l && !st3l && !st4l){
   // up(0);   // Bước cuối cùng
    if(!pick_mode){   // ở chế độ mode ngẫu nhiên NGẪU NHIÊN
      auto_ai_L_1(50,70);   // kich ban 1 : 
    }else{             // Ngược lại chạy ở chế độ tự động silo chọn ( khi đèn xanh ở mode sáng )
      auto_ai_L_2(50,70);  //kich ban 2 
    } 
  }
}

//============================ Tự động chạy từ điểm RETRY tới khu vực 3 sân bên Trái ( Đã có PID sick) ==========================// 
void auto_3L_retry(){
  if(st1l && millis() - delay_st1l < 600 || st1l && !cb_run){
    v_bot = 100 ;
    down_pid(0,v_bot - 20, 50, 80); 
    st2l = true; 
  }else if(millis() - delay_st1l > 600 && cb_run && st2l){
    // left_pid(0,v_bot, 60, 60); 
    pid_sick1(3900,0, v_bot, 100, 20);
    st1l = false ; 
    st3l = true; 
    delay_st2l = millis();
  }
  if(st3l && !cb_run || st3l && sick1() > 3850){
    if(millis() - delay_st2l < 10){
      //left_pid(0,v_bot, 60, 60); 
      st1l = false;
      st2l = false;
      delay_st3l = millis();
    }else{
      if(millis() - delay_st3l <= 2400){
        down_pid(0,v_bot, 30, 60); 
      }else{
        rotare(-90);
        if(compass() < -870){
          st1l = false;
          st2l = false;
          st3l = false;
          st4l = true; 
          delay_st4l = millis(); 
        }
      }
    }
  }
  if(st4l && compass() < -860){
    // if(millis() - delay_st4l < 1000){
    //   up_pid(-87,v_bot, 60, 60); 
    // }else{
      st1l = false;
      st2l = false;
      st3l = false; 
      st4l = false;
      up(0); 
    // }
  }
  if(!st1l && !st2l && !st3l && !st4l){
    //up(0);   // Bước cuối cùng
    if(!pick_mode){   // ở chế độ mode ngẫu nhiên NGẪU NHIÊN
      auto_ai_L_1(50,70);   // kich ban 1 : 
    }else{             // Ngược lại chạy ở chế độ tự động silo chọn ( khi đèn xanh ở mode sáng )
      auto_ai_L_2(50,70);  //kich ban 2 
    } 
  }
}

//============================================goto_silo_with_sick===========================================// goc quay la -90 
bool st_silo_1l = true, st_silo_2l = false, st_silo_3l = false, st_silo_4l = false;
uint32_t delay_silo_1l =0,delay_silo_2l = 0;
void goto_silo_L(int stt_silo){
  static int dist_silol = 0;
  dist_silol = stt_silo;
  if(!cb_run && st_silo_1l || st_silo_1l && sick2() + sick1() < 4600){
    down_pid(-90,90, 80, 80);
    delay_silo_1l = millis();
  }else if(cb_run && abs(compass() + 900) < 50 && sick2() + sick1() > 4300 && st_silo_1l){
    st_silo_1l = false;
    st_silo_2l = true; 
  }
  if(st_silo_2l){
     pid_sick1(dist_silol,-90, 55, 70, 20);
     if(abs(sick1() - dist_silol) < 20){
      if(millis() - delay_silo_2l >= 200){
        st_silo_1l = false; 
        st_silo_2l = false; 
        st_silo_3l = true; 
      }
     }else{
        delay_silo_2l = millis();
     }
  }
  if(st_silo_3l){
    down_pid(-90, 30, 40, 40);
    if(silo || siloo){
      st_silo_1l = false; 
      st_silo_2l = false; 
      st_silo_3l = false;
      st_silo_4l = true; 
    }
  }
  if(st_silo_4l){
    //up(0);
    nha_bong(); 
  }
}

//==================================================== Ham chay toi silo kich ban 2 =========================================================//
void goto_siloL_jetson(){
  if(gsj1){
    down_pid(0, 100, 50, 70); 
    Serial.print("_______Step1_______ ");
    if(abs(compass() + 90) < 50){
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
    if(abs(compass() + 90) < 50 && abs(geo_x) < 50){
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

//==========================================Jetsom + sick (dung chung cac khoa voi goto jetson nhes anhe) kich ban 2 ver 2 dung ngon  ==========================================//
void goto_siloL_jetson_sick(){
  if(gsj1){
    down_pid(-90, 100, 50, 70); 
    Serial.print("_______Step1_______ ");
    if(abs(compass() + 900) < 80){
      gsj1 = false; 
      gsj2= true; 
    }
  }
  if(gsj2){
    flow_silo(150,0,100, 50, 10); 
    Serial.print("_______Step2_______ ");
    if(cb_run && sick1() + sick2() > 4000){
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
     
    if(silo){
      down_rl(0, 15); 
    }else if(siloo){
      down_rl(15, 0); 
    }else if(!silo && !siloo){
      down_pid(90, 20, 50, 70); 
    }else if(silo && siloo){
      stop(); 
    }
    if(silo && siloo && abs(compass() + 900) < 80){
      gsj1 = false; 
      gsj2 = false; 
      gsj3 = false; 
      gsj4 = false; 
      gsj5 = true; 
      up(0);
    }
  }
  if(gsj5){
     Serial.print("_______Step5_______ ");
    if(stt_pick1){
      up(0); 
      if(millis() - delay_step_5 > 200){
        if(sick2() < (kr1 + kr0)*0.5){
          true_silo = kr0; 
          stt_pick1 = false;  
        }else if(sick2() >= (kr1 + kr0)*0.5  && sick2() < (kr2+kr1)*0.5){
          true_silo = kr1; 
          stt_pick1 = false; 
        }else if(sick2() >= (kr1 + kr2)*0.5  && sick2() < (kr2+kr3)*0.5){
          true_silo = kr2; 
          stt_pick1 = false; 
        }else if(sick2() >= (kr2 + kr3)*0.5  && sick2() < (kr3+kr4)*0.5){
          true_silo = kr3; 
          stt_pick1 = false; 
        }else if(sick2() >= (kr3 + kr4)*0.5  ){
          true_silo = kr4; 
          stt_pick1 = false; 
        }
      }
    }else{
      pid_sick1(true_silo,-90, 30, 50, 6);
      if(abs( sick1() - true_silo) < 30){
        gsj1 = false; 
        gsj2 = false; 
        gsj3 = false; 
        gsj4 = false; 
        gsj5 = false;
        gsj6 = true; 
      } 
      Serial.print(true_silo); 
    }
  }
  if(gsj6){
    down_pid(-90, 18, 50, 70); 
    if(silo && siloo){
      gsj1 = false; 
      gsj2 = false; 
      gsj3 = false; 
      gsj4 = false; 
      gsj5 = false;
      gsj6 = false;
      gsj7 = true; 
    } 
  }
  if(gsj7){
    nha_bong(); 
  }
}

//============================ Hàm RESET ==========================// 
void p_goto_silo_L(){
  st_silo_1l = true; 
  st_silo_2l = false; 
  st_silo_3l = false; 
  st_silo_4l = false;
}

void p_auto_3L(){
  v_bot = 0; 
  soft_state = false; 
  st1l = true; 
  st2l = false;
  st3l = false; 
  st4l = false; 
  delay_st1l = millis(); 
}
