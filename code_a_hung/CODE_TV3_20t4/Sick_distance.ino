
// ========================  Sick ========================  //
int sick1(){   //
  return sick1_kal.updateEstimate(analogRead(A7))*one_dist + 200;
}
int sick2(){   // san phai gan den xa: 355 , 1150, 1910, 2670, 3410  
  return sick2_kal.updateEstimate(analogRead(A6))*one_dist + 200;
}

//double  sick_kp = 3.0, sick_ki = 0.6, sick_kd = 0.01;
double  sick_kp = 0.08, sick_ki = 0.019, sick_kd = 0.0002;   
int error_sick = 0; 
int last_error_sick  = 0; 
int output1 = 0; 
int vaa, vbb; 

void pid_sick1(int set_distance,int set_angle, int v_speed_sick, int speed_rool, int false_set){
  if(millis() - last_time > 10 ){
    angle = compass()/10; 
    error = set_angle - angle;

    int distance_realtime = sick1(); 
    error_sick = set_distance - distance_realtime; 

    if(error_sick < false_set && error_sick > -false_set){
       error_sick = 0;
    }
    if( -100 < error_sick < 100){
      sick_kp = 0.01 ; sick_ki = 0.001; sick_kd = 0.0005; 
    }
    else {
      sick_kp = 0.02 ; sick_ki = 0.005; sick_kd = 0.008; 
    }
    
    output1 = sick_kp * error_sick  + sick_kd*(error_sick  - last_error_sick) + ki*(error_sick + last_error_sick ); 
    last_error_sick = error_sick;
    output1 = constrain(output1, -v_speed_sick, v_speed_sick);  

    if( -4 < error < 4){
      kp = 2.3 ; ki = 0.17; kd = 0.05; 
    }
    else {
      kp = 2; ki = 0.02; kd = 0.01;
    }
    output = (kp) * error + kd * (error - last_error) + ki * (error + last_error);
    last_error = error;
    output = constrain(output, -speed_rool, speed_rool);

    if(output1 < 10 && output1 > 0){
      output1 = 10; 
    }
    if(output1 > -10 && output1 < 0){
      output1 = -10; 
    }
    if(output1 > 1){
      left( output1,output); 
    }else if(output1 < 1){
      right(abs(output1),output); 
    }
    last_time = millis(); 
  }
}
void pid_sick2(int set_distance,int set_angle, int v_speed_sick, int speed_rool, int false_set){
  if(millis() - last_time > 10 ){
    angle = compass()/10; 
    error = set_angle - angle;

    int distance_realtime = sick2(); 
    error_sick = set_distance - distance_realtime; 

    if(error_sick < false_set && error_sick > -false_set){
       error_sick = 0;
    }
    if( -150 < error_sick < 150){
      sick_kp = 0.012 ; sick_ki = 0.003; sick_kd = 0.005; 
    }
    else {
      sick_kp = 0.02 ; sick_ki = 0.005; sick_kd = 0.008; 
    }
    
    output1 = sick_kp * error_sick  + sick_kd*(error_sick  - last_error_sick) + ki*(error_sick + last_error_sick ); 
    last_error_sick = error_sick;
    output1 = constrain(output1, -v_speed_sick, v_speed_sick);  

    if( -4 < error < 4){
      kp = 2.3 ; ki = 0.17; kd = 0.05; 
    }
    else {
      kp = 2; ki = 0.02; kd = 0.01;
    }
    output = (kp) * error + kd * (error - last_error) + ki * (error + last_error);
    last_error = error;
    output = constrain(output, -speed_rool, speed_rool);

    if(output1 < 10 && output1 > 0){
      output1 = 10; 
    }
    if(output1 > -10 && output1 < 0){
      output1 = -10; 
    }
    if(output1 > 1){
      right( output1,output); 
    }else if(output1 < 1){
      left(abs(output1),output); 
    }
    last_time = millis(); 
  }
}

// void pid_sick2(int set_distance, int v_speed_sick){
//   if(millis() - last_time > 10 ){
//     int distance_realtime = sick2(); 
//     error_sick = set_distance - distance_realtime; 
    
//     output = sick_kd * error_sick  + sick_kd*(error_sick  - last_error_sick) + ki*(error_sick + last_error_sick ); 
//     last_error_sick = error_sick;
//     output = constrain(output, -v_speed_sick, v_speed_sick);  
//     if(output < 10 && output > 0){
//       output = 10; 
//     }
//     if(output > -10 && output < 0){
//       output = -10; 
//     }
//     if(output > 1){
//       down_pid(0, output, 60, 60); 
//     }else if(output < 1){
//       up_pid(0, abs(output), 60, 60); 
//     }
//     last_time = millis(); 
//   }
// }

bool truoc = true;
int stt_sick2(){
   if(sick2() < 1200  && truoc){
    if( millis() - timeb >= 15){
      solan_sick2 ++; 
      truoc = false;
    }
    timec = millis(); 
  }else if(sick2() >= 1200){
     if( millis() - timec >= 15){ 
      truoc = true;
    }
    timeb = millis(); 
  }
  return solan_sick2; 
}


bool truoc2 = true; 
void stt_sick22(){
   if(sick2() < 1800 && truoc){
    if( millis() - timeb >= 20){
      solan_sick22 ++; 
      truoc = false;
    }
    timec = millis(); 
  }else if(sick2() >= 1800){
     if( millis() - timec >= 10){ 
      truoc = true;
    }
    timeb = millis(); 
  }
}

// bên dưới chỉnh cho tốc độ v_bot trở về end_speed là tốc độ đặt cuối cùng trước khi dừng lại  
// Hàm 
void sick_down_speed(int distance_reduce,int ac_stop,  int end_speed){
  if(distance_reduce > end_speed){
    soft_state = true; 
  }
  if(sick2() < distance_reduce && v_bot > end_speed){
    soft_state = false; 
    if( millis() - timea > ac_stop){
      v_bot ++; 
      timea = millis(); 
    }
  }
}

