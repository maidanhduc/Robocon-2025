
bool last_cam = 0; 
String wcam = "0"; 
void cam( bool camm){
  if(camm != last_cam){
    if(camm == 1) Serial2.println("1");  else Serial2.println("0"); 
    last_cam = camm; 
  }
}

bool last_type_ball = 0; 
void type_ball_call(bool type_bal){
  // if(type_bal != last_type_ball){
    if(type_bal == 0) Serial2.println("2"); else Serial2.println("3"); 
  //   last_type_ball = type_bal; 
  // }
}
//===========================================================
String data = "";
void uart_jet(){
  if(Serial2.available()){
     data = Serial2.readStringUntil('\r'); 
  }
  int pos = data.indexOf(',');
  if (pos != -1) {
    // Tách dữ liệu trước và sau ký tự ','
    String data1 = data.substring(0, pos);
    String data2 = data.substring(pos + 1);
    // Chuyển đổi thành số nguyên
    geo_x = data1.toInt();
    geo_y = data2.toInt();
    if(geo_x == 0){ geo_x = 0;} else { geo_x = map(geo_x, 0,640,-320, 320);}
    if(geo_y <= 0){ geo_y = 1;} else { geo_y = map(geo_y, 0,470,470,0);}
    if( geo_y < 0 ){
      geo_y = 1; 
    }
  }
}

//====================================================  Run to ball ===============================================// 
int vx1, vx2; 

void run_ball(int set_x, int speed_x, int ac){
  if(millis() - last_time > 10 ){
    kp = 0.1 ; ki = 0.005; kd = 0.001; 
    error = set_x - geo_x;
    output = (kp) * error + kd * (error - last_error) + ki * (error + last_error);
    last_error = error;
    output = constrain(output, -speed_x, speed_x);

   if (output != 0)
    {
      vx1 = speed_x + output;
      vx2 = speed_x - output;
    }else{
      vx1 = vx2 = speed_x;
    }
    vx1 = constrain(vx1, 0, speed_x + ac);
    vx2 = constrain(vx2, 0, speed_x + ac);
    last_time = millis(); 
  }
}
void up_ball(int set_x, int v_x, int ac){
  run_ball(set_x, v_x, ac);
  dc2.quaynghich(vx2); 
  dc4.quaynghich(vx2); 
  dc1.quaythuan(vx1); 
  dc3.quaythuan(vx1);
}

//=====================================================FLOW_Ball=====================================================//
int error_jet, last_error_jet; 
int output_jet; 

void flow_ball(int set_y,int set_x, int v_speed_y, int speed_x, int false_set){
  if(geo_x == 0 && geo_y == 0){
    up(0); 
  }
  if(millis() - last_time > 10 ){

    // kp = 0.1 ; ki = 0.005; kd = 0.0004; 
     error = set_x- geo_x;
    if( -10 < error < 10){
      kp = 0.1 ; ki = 0.035; kd = 0.01; 
    }
    else {
      kp = 0.15 ; ki = 0.025; kd = 0.004; 
    }

    int distance_realtime = geo_y; 
    error_jet = set_y - distance_realtime; 

    if(error_jet < false_set && error_jet > -false_set){
       error_jet = set_y - simpleKalmanFilter.updateEstimate(geo_y);
    }
    
    output_jet = 0.2 * error_jet  + 0.005*(error_jet - last_error_jet) + 0.01*(error_jet + last_error_jet); 
    last_error_jet = error_jet;
    output_jet = constrain(output_jet, -v_speed_y, v_speed_y);  

    output = (kp) * error + kd * (error - last_error) + ki * (error + last_error);
    last_error = error;
    output = constrain(output, -speed_x, speed_x);

    if(output_jet < 8 && output_jet > 0){
      output_jet = 8; 
    }
    if(output_jet > -8 && output_jet < 0){
      output_jet = -8; 
    }
    if(output_jet > 1){
      down_jet( output_jet,output); 
    }else if(output_jet < 1){
      up_jet(abs(output_jet),output); 
    }else{
      up(0);
    }
    last_time = millis(); 
  }
}

void up_jet(int v1, int del_v1){
  dc2.quaynghich(abs(v1 - del_v1)); 
  dc4.quaynghich(abs(v1 - del_v1)); 
  dc1.quaythuan(abs(v1 + del_v1)); 
  dc3.quaythuan(abs(v1 + del_v1));
}
void down_jet(int v1, int del_v1){
  dc2.quaythuan(abs(v1 + del_v1)); 
  dc4.quaythuan(abs(v1 + del_v1)); 
  dc1.quaynghich(abs(v1 - del_v1)); 
  dc3.quaynghich(abs(v1 - del_v1));
}

//=====================================================FLOW_Silo=====================================================//
void flow_silo(int set_distance,int set_angle, int v_speed_sick, int speed_rool, int false_set){
  
  if(millis() - last_time > 10 ){

    kp = 0.1 ; ki = 0.005; kd = 0.0004; 
    angle = geo_x; 
    error = set_angle - angle;

    int distance_realtime = geo_y; 
    error_jet = set_distance - distance_realtime; 

    if(error_jet < false_set && error_jet > -false_set){
       error_jet = set_distance - simpleKalmanFilter.updateEstimate(geo_y);
    }
    
    output_jet = 0.2 * error_jet  + 0.005*(error_jet - last_error_jet) + 0.01*(error_jet + last_error_jet); 
    last_error_jet = error_jet;
    output_jet = constrain(output_jet, -v_speed_sick, v_speed_sick);  

    output = (kp) * error + kd * (error - last_error) + ki * (error + last_error);
    last_error = error;
    output = constrain(output, -speed_rool, speed_rool);

    if(output_jet < 10 && output_jet > 0){
      output_jet = 10; 
    }
    if(output_jet > -10 && output_jet < 0){
      output_jet = -10; 
    }
    // if(output_jet > 1){
      down_jet( v_speed_sick,output); 
    // }else if(output_jet < 1){
    //   down_jet(abs(v_speed_sick),output); 
    // }
    last_time = millis(); 
  }
  if(geo_x == 0 && geo_y == 0){
    up(0); 
  }
}

