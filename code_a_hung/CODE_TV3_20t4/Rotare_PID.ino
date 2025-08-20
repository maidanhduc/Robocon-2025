
// ========================  Quay góc ========================  //
void left_rotate( int v) {
  dc1.quaynghich(v); 
  dc2.quaynghich(v); 
  dc3.quaynghich(v); 
  dc4.quaynghich(v);
}
void right_rotate( int v) {
  dc1.quaythuan(v); 
  dc2.quaythuan(v); 
  dc3.quaythuan(v); 
  dc4.quaythuan(v); 
}

//=========================================================quay theo bong=====================================================//
void rotare_jet( int set_point){
  if (millis() - last_time > T) { 
    int check_point = geo_x; 
    error = set_point - check_point;
    // if( -4 < error < 4){
    //   kp = 2.3 ; ki = 0.17;= 0.05; 
    // } kd 
    //else {
      kp = 0.2; ki = 0.002; kd = 0.001;
    //}
    output = (kp) * error + kd * (error - last_error) + ki * (error + last_error);
    last_error = error;
    output = constrain(output, -80, 80);
    if (output > 0 && output < 6 ) output = 6;
    if (output < 0 && output > -6) output = -6;
    if ( output < 1) {
      left_rotate(abs(output));
    }
    else if ( output > 1) {
      right_rotate(output);
    }
    else {
      stop();
    }
    last_time = millis();
  }
}

//==========================================Quay===========================================//
void rotare( int set_angle){
  static int v_rota = 0; 
  if (millis() - last_time > T) {
    angle = compass()/10; 
    error = set_angle - angle;
    
    if( -10 < error && error < 10){
      kp = 1.8 ; ki = 0.018; kd = 0.01; 
      v_rota = 90;
    }else {
      kp = 2.5; ki = 0.02; kd = 0.01;
      v_rota = 130;
    }
    output = (kp) * error + kd * (error - last_error) + ki * (error + last_error);
    last_error = error;
    output = constrain(output, -v_rota, v_rota);
    if (output > 0 && output < 8 ) output = 8;
    if (output < 0 && output > -8) output = -8;
    if ( output < 1) {
      left_rotate(abs(output));
    }
    else if ( output > 1) {
      right_rotate(output);
    }
    else {
      stop();
    }
    last_time = millis();
    last_error1 = 0;
  }
}

//===================================================quay luon==================================================//
void quay(int anpha){
  while(abs(compass() - anpha*10) > 10){
    rotare(anpha);
  }
}