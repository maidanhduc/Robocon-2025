/* 
1, Thứ tự điều khiển ở đây: đầu tiên gọi hàm soft_start(int), sau đó gọi hàm điều khiển động cơ rồi đến hàm sick distance. 
2. cac ham deu co cac khoa truoc va khoa sau. ham thao tac se co ham reset thao tac do. 
3. cac khoa trong ham se duoc khai bao ten lien quan den ham 
4. cac thao tac duoc thuc hien tuan tu thong qua cac khoa

//========================= Danh sach cac ham ==============================// 
up_pid(goc, tocdo, toc_do_phan_hoi, toc_do_quay_phanhoi); 
down_pid
right_pid
left_pid 

pid_sick1(khoang cach, goc, vantoc_truc, vantocquay, sai so); 
pid_sick2

flow_ball(set_y, set_x, speed_y, speed_x, false); 
flow_silo(); 

goto_ball(); ;














// Ham minh lam dau tien de du phong 

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
        if( sick2() < 2500){
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


bool b1 = true, b2 = true , b3 = false , b4 = false , b5 = false ; 
int color();  
bool sp1 = 0, sp2 = 0, sp3 = 0; 
bool find_b =  true, find_silo = true, check_b= true; 
bool ball;
uint32_t time_ball1 = 0, time_ball2 = 0, time_ball3 = 0, time_delay_2 = 0; 
int compass_y; 
bool fs1 = false, fs2 = false, fs3 = false; 
bool fs11 = false, fs12 = false, fs13 = false, fs111 = false; 

void lay_bong_thang(int y_set){
  static uint32_t time_delay = millis(); 
  if(y_set >= geo_y && fs1){
    if(millis() - time_delay >= 300){
      while(fs1 && !fs11){
        up_pid(compass_y , 30, 80, 80);
        bong_nhat(); 
        Serial.println("lay_bong_thang");
        if(geo_y >= 0 && ss1){ fs1 = false; fs2 = true; } 
        if(millis() - time_delay > 2000) break; 
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
    if(millis() - time_delay < 1500) { color = 0; } else {color = 1;}
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
    Serial.println("check_bong");
    time_delay_2 = millis(); 
  }
  if(color() && ss2 && !fs11){   // Bóng đúng  // chưa có reset 
    // if(millis() - time_delay_2 < 800){
    //   rotare(90);
    //   fs13 = true;
    //   b2 = false; 
    //   Serial.println("quay");
    // }else{

    //   b1 = false;
    //   fs11 = true;
    // }
    b1 = false;   
    stt_cam = 0;
    fs11 = true;
    fs12 = false;  
    fs1 = false; 
    
  }else if(!color() && ss2){     // Bóng sai 
    fs12 = true; 
    fs11 = false; 
    b1 = true; 
    b2 = true; 
  }
  if(fs12 == true){
    vut_bong();
    bong_nhat();
    Serial.println("vut_bong"); 
  }
  if(fs11 == true){
    bong_chuan_bi();
    if(millis() - time_delay > 700){
      dc11.quaynghich(250);
    }
    Serial.println("chon_bong"); 
  }
}

//================================== Kịch bản 1 ===============================//
void auto_ai_1(int set_dist_ball, int set_dist_silo){
  static uint32_t time_delay_2  = 0;
  cam(stt_cam);
  if(set_dist_ball < geo_y && b1 && !fs2){
    stt_cam = 1; 
    compass_y = compass()/10;
    flow_ball(set_dist_ball , 0, 80, 100, 10); 
    fs1 = true;
    fs3 = false;  
  }else{
    lay_bong_thang(set_dist_ball);
  }
  if(!fs3){
    check_ball();
  }
  if(set_dist_silo < geo_y && !b1 && !silo){
    stt_cam = 0; 
    flow_silo(set_dist_silo, 0, 80, 100, 10); 
  }else{
    if(silo && !b1){
      fs3 = true; 
      up(0); 
      nha_bong(); 
    }
  }
}


//================================== Kịch bản 2 ===============================//
void auto_ai_2(int set_dist_ball, int set_dist_silo){
  cam(stt_cam);
  if(set_dist_ball < geo_y && b1 && !fs2){
    stt_cam = 1; 
    compass_y = compass()/10;
    up_ball(0, 50, 50); 
    fs1 = true;
    fs3 = false;  
  }else{
    lay_bong_thang(set_dist_ball);
  }
  if(!fs3){
    check_ball();
  }
  if(set_dist_silo < geo_y && !b1 && !silo){
    stt_cam = 0; 
    flow_silo(set_dist_ball - 40, 0, 80, 100, 10); 
  }else{
    if(silo && !b1){
      fs3 = true; 
      up(0); 
      nha_bong(); 
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
    dc11.quaythuan(0);
    dc22.quaythuan(250); 
    dc33.quaynghich(250); 
    time_delay = millis(); 
  }else{
    if(millis() - time_delay >500 ){
      dc22.quaythuan(0); 
      dc33.quaynghich(0);
      fs11 = false; 
      fs2 = false; 
      fs13 = false; 
      stt_cam = 1; 
      b1 = 1; 
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
















*/ 