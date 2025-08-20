bool b1 = true, b2 = true , b3 = false , b4 = false , b5 = false ; 
int color();  
bool sp1 = 0, sp2 = 0, sp3 = 0; 
bool find_b =  true, find_silo = true, check_b= true; 
bool ball;
uint32_t time_ball1 = 0, time_ball2 = 0, time_ball3 = 0, time_delay_2 = 0; 
int compass_y; 
bool fs1 = false, fs2 = false, fs3 = false; 
bool fs11 = false, fs12 = false, fs13 = false, fs111 = false; 
bool stt_fix_ball = false; 

void lay_bong_thang(int y_set){
  static uint32_t time_delay = millis(); 
  if(y_set >= geo_y && fs1){
    if(millis() - time_delay >= 300){
      while(fs1 && !fs11){
        up_pid(compass_y , 30, 80, 80);
        bong_nhat(); 
        Serial.println("lay_bong_thang");
        if(ss1){ fs1 = false; fs2 = true; } 
        if(millis() - time_delay > 1700){ stt_fix_ball = true;  break; } 
      }
    }
  }else{
    time_delay = millis(); 
  }
}

//================================== Kiểm tra màu ===============================//
int color(){
  static bool color; 
  static bool last_color = true;
  static uint32_t time_delay = 0;  
  if(color_ball){
    color = 0; 
    time_delay = millis();
  }else{
    if(millis() - time_delay < 1600) { color = 0; } else {color = 1;}
  }
  return color; 
}

//================================== Kiểm bóng, lọc bóng ===============================//
// fs11 va fs12
void check_ball(){
  static uint32_t time_delay = 0; 
  static uint32_t time_delay_2 = 0; 
  if(!ss2 && !fs11 && !fs12 && !fs13){    
    bong_nhat();
    bong_check();
    time_delay = millis(); 
    //Serial.println("check_bong");
    time_delay_2 = millis(); 
  }
  if(color() && ss2 && !fs11){   // Bóng đúng  // Da co reset// minh can note lai cac khoa cho no khong quuen cmnm 
    b1 = false;   // khoa tim bong hoac silo khong dieu khien truc tiep cam
    stt_cam = 0; // Dung chuyen trang thai cam 
    fs11 = true; // khoa hau dung 
    fs12 = false;  // Khoa hau sai 
    fs1 = false;  // khoa dieu khien tim bong 
    
  }else if(!color() && ss2){     // Bóng sai 
    fs12 = true;  // Khoa hau sai 
    fs11 = false;  // khoa hau dung 
    b1 = true; // khoa dieu khien tim bong dung la tim sai la tim silo 
    b2 = true; 
    fs2 = false; // Quay lai trang thai tim bong
  }
  if(fs12 == true){
    vut_bong();
    bong_nhat();
    //Serial.println("vut_bong"); 
  }
  if(fs11 == true){
    bong_chuan_bi();
    if(millis() - time_delay > 700){
      dc11.quaynghich(250);
    }
    Serial.println("chon_bong"); 
  }
}

//================================== Kịch bản 1 cho sân bên trái (Red) ===============================//
// void auto_ai_L(int set_dist_ball, int set_dist_silo){
//   static uint32_t time_delay_2  = 0;
//   cam(stt_cam);
//   if(set_dist_ball < geo_y && b1 && !fs2){
//     stt_cam = 1; 
//     compass_y = compass()/10;
//     flow_ball(set_dist_ball , 0, 50, 70, 10); 
//     fs1 = true;
//     fs3 = false;  
//   }else {
//     lay_bong_thang(set_dist_ball);
//   }
//   if(!fs3){
//     check_ball();
//   }
//   if( !b1 && !silo){
//     stt_cam = 0; 
//     goto_silo_L(1115);
//     // flow_silo(set_dist_silo, 0, 20, 30, 10); 
//   }else{
//     if(silo && !b1){
//       fs3 = true; 
//       goto_silo_L(1115);
//       //up(0); 
//       //nha_bong(); 
//     }
//   }
// }


//================================== Kịch bản 1 Cho sân bên phải (Blue dùng sick) ===============================//
void auto_ai_R_1(int set_dist_ball, int set_dist_silo){
  static uint32_t time_delay_2  = 0;
  cam(stt_cam);
  if(set_dist_ball < geo_y && b1 && !fs2){
    check_silo = true;   // Doan nay cho phep chuyen silo o lan tiep theo 
    stt_cam = 1; 
    compass_y = compass()/10;
    flow_ball(set_dist_ball - 20, 0, 80, 80, 10); 
    fs1 = true;
    fs3 = false;  
    p_goto_silo_R();
  }else {
    lay_bong_thang(set_dist_ball);   // đoạn này đang gây ra lỗi 
  }
  if(!fs3){
    check_ball();
  }
  if(!b1){
    stt_cam = 0; 
    goto_silo_R(w_silor[stt_silo]);  //   360 , 1155; 1930; 2690; 3450
    // flow_silo(set_dist_silo, 0, 20, 30, 10); 
    if(silo && !b1){
      fs3 = true;
      //up(0); 
      //nha_bong(); 
    }
  }
}


//================================== Kịch bản 2 Cho sân bên phải (Blue dùng xử lý ảnh ) ===============================//
void auto_ai_R_2(int set_dist_ball, int set_dist_silo){
  static uint32_t time_delay_2  = 0;
  cam(stt_cam);
  if(set_dist_ball < geo_y && b1 && !fs2){
    check_silo = true;   // Doan nay cho phep chuyen silo o lan tiep theo 
    stt_cam = 1; 
    compass_y = compass()/10;
    flow_ball(set_dist_ball - 20, 0, 50, 70, 10); 
    fs1 = true;
    fs3 = false;  
  }else {
    lay_bong_thang(set_dist_ball);
  }
  if(!fs3){
    check_ball();
  }
  if( !b1){
    stt_cam = 0; 
    //goto_silo_R(w_silo[stt_silo]);  //   360 , 1155; 1930; 2690; 3450   (dung cho kich ban 1 )
    //run_to_line();
    goto_siloR_jetson_sick();
    //goto_silo_jetson(); 
    if(silo && !b1){
      fs3 = true;
      //up(0); 
      //nha_bong(); 
    }
  }
}


//================================== Kịch bản 1 Cho sân bên trái (RED dùng sick) ===============================//
void auto_ai_L_1(int set_dist_ball, int set_dist_silo){
  static uint32_t time_delay_2  = 0;
  cam(stt_cam);
  if(set_dist_ball < geo_y && b1 && !fs2){
    check_silo = true;   // Doan nay cho phep chuyen silo o lan tiep theo 
    stt_cam = 1; 
    compass_y = compass()/10;
    flow_ball(set_dist_ball - 20, 0, 80, 80, 10); 
    fs1 = true;
    fs3 = false;  
    p_goto_silo_L();
  }else {
    lay_bong_thang(set_dist_ball);   // đoạn này đang gây ra lỗi 
  }
  if(!fs3){
    check_ball();
  }
  if( !b1){
    stt_cam = 0; 
    goto_silo_L(w_silol[stt_silo]);  //   360 , 1155; 1930; 2690; 3450
    // flow_silo(set_dist_silo, 0, 20, 30, 10); 
    if(silo && !b1){
      fs3 = true;
      //up(0); 
      //nha_bong(); 
    }
  }
}

//================================== Kịch bản 2 Cho sân bên trái (RED dùng xử lý ảnh ) ===============================//
void auto_ai_L_2(int set_dist_ball, int set_dist_silo){
  static uint32_t time_delay_2  = 0;
  cam(stt_cam);
  if(set_dist_ball < geo_y && b1 && !fs2){
    check_silo = true;   // Doan nay cho phep chuyen silo o lan tiep theo 
    stt_cam = 1; 
    compass_y = compass()/10;
    flow_ball(set_dist_ball - 20, 0, 50, 70, 10); 
    fs1 = true;
    fs3 = false;  
  }else {
    lay_bong_thang(set_dist_ball);
  }
  if(!fs3){
    check_ball();
  }
  if( !b1){
    stt_cam = 0; 
    //goto_silo_R(w_silo[stt_silo]);  //   360 , 1155; 1930; 2690; 3450   (dung cho kich ban 1 )
    //run_to_line();
    goto_siloL_jetson_sick();
    //goto_silo_jetson(); 
    if(silo && !b1){
      fs3 = true;
      //up(0); 
      //nha_bong(); 
    }
  }
}

// void check_ball(){
//   // Xem màu 
//   if(ss2 && last_color){
//     if(color_ball){
//       color = false; 
//       last_color = false ; 
//     }
//     else{
//       color = true; 
//     }
//   }else if(!ss2){
//     last_color = true; 
//   }
//  // Loc mau : 
//   if(check_b == true){
//     if(!ss2 && !b3 && !b2){
//       dc11.quaynghich(250);
//     }
//     if(ss2 && !color){
//       dc11.quaynghich(250);
//       dc22.quaythuan(250);
//       b2 = false;
//       ball = false; 
//     }
//     if(ss2 && color){
//       dc11.quaynghich(0);
//       dc22.quaythuan(0);
//       stt_cam = 0;
//       ball = true; 
//     }else {
//       stt_cam = 1;
//     }
//   }
//   up(0); 
// }


// void auto_find_ball(int set_dist){
//   cam(stt_cam); 
//   // Nhặt bóng 
//   static int compass_y; 
//   if(geo_y > set_dist && ss1 == 0){
//     flow_ball(set_dist - 40, 0, 50, 50, 10); 
//     compass_y = compass()/10; 
//     dc11.quaynghich(0);
//     check_b = false; 
//     b1 = true; 
//     b2 = false;
//   }else if(geo_y <= set_dist && b1 && geo_y > 0 ){
//     while(!ss1 && !b2){
//       up_pid(compass_y, 50, 80, 80);
//       dc11.quaynghich(250);
//       check_b = true; 
//       if(ss1)  b2 = true;  // rs cho b1 = false; 
//     }
//     check_ball();
//   if(ball){
//     if(millis() - time_ball1 >= 500){
//       check_b = false; 
//       flow_silo(set_dist - 40, 0, 50, 50, 10); 
//       b1 = false;
//       }
//     } 
//   }
// }


// uint32_t time_b1 = 0; 

// void rotare_with_button(){
//   if(button_l2){
//     if(millis() - time_b1 >= 50 ){
//       set_angle_button++; 
//       rotare(set_angle_button);
//     }
//   }else if(button_r2){
//     if(millis() - time_b1 >= 50 ){
//       set_angle_button--; 
//       rotare(set_angle_button);
//     }
//   }else{
//     time_b1 = millis(); 
//   }
// }












//===========================================================================================================================
void bong_nhat(){
  if(!ss1){
    dc11.quaythuan(250);    
  }else{
    dc11.quaythuan(0);
  }
}

void p_bong_nhat(){
  static uint32_t time_delay = 0; 
  if(ss1){
    dc11.quaynghich(250);
    dc22.quaythuan(0); 
    dc33.quaynghich(0); 
    time_delay = millis(); 
  }else{
    if(millis() - time_delay > 1500 ){
      dc11.quaynghich(0);
    }
  }
}


//==========================
void bong_check(){
  if(!ss2){
    dc11.quaythuan(255);
    dc22.quaythuan(0); 
    dc33.quaynghich(250); 
  }
}

void nhat_bong_check(){
  dc11.quaythuan(250);
  dc33.quaynghich(250); 
}

//========================================
void bong_chuan_bi(){
  if(ss2){
    dc11.quaythuan(0);
    dc22.quaythuan(0); 
    dc33.quaynghich(250); 
  }else{
    dc33.quaynghich(0);
  }
  if(!ss3){
    dc11.quaythuan(0);  
    dc22.quaythuan(250); 
    dc33.quaynghich(250); 
  }else{
    dc33.quaynghich(0); 
    dc22.quaythuan(0); 
  }
}

void nha_bong(){
  static uint32_t time_delay = 0; 
  if(ss3){
    up(0);
    dc11.quaythuan(0);
    dc22.quaythuan(250); 
    dc33.quaynghich(250); 
    time_delay = millis(); 
    //Serial.println("Nha Bong _____");
  }else{
    if(millis() - time_delay > 900 ){
      dc22.quaythuan(0); 
      dc33.quaynghich(0);
      fs11 = false; 
      fs2 = false; 
      fs13 = false; 
      //Serial.println("Da chuyen ");
      stt_cam = 1; cam(stt_cam);
      b1 = true; 
      p_goto_silo_R();
      p_goto_silo_L();
      // p_run_to_line();
      p_goto_silo_jetson();
      if(check_silo){
        stt_silo ++; 
        if(stt_silo == 5) stt_silo = 0; 
        check_silo = false; 
      }
      //p_goto_silo_L();
    }
  }
}

int vut_bong(){
  static uint32_t time_delay = 0;
  static uint32_t time_delay2 = 0;
  static bool x = 0; 
  int t; 
  if(!ss3 && x == 0){
    dc22.quaythuan(255); 
    dc33.quaynghich(255); 
    fs12 = true;
  }else if(ss3){
    x = 1; 
    fs12 = true; 
    dc22.quaythuan(255); 
    dc33.quaynghich(255); 
    time_delay = millis();
  }
  if(!ss3 && x == 1){
    if(millis() - time_delay > 500){
      dc22.quaythuan(0); 
      dc33.quaynghich(0); 
      x = 0; 
      fs12 = false; 
    }
  }
}
