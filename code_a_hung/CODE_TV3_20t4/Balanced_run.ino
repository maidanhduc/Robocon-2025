 
// ========================  PID chạy thẳng ========================  //
void balance(int set_angle, int v, int ac, int out)
{
  //v = soft_start(v);
  // v11 = v; 
  // v22 = v; 
  if (millis() - last_time > T){
    angle = compass() / 10;
    error = set_angle - angle;
    output = kp1 * error + kd1 * (error - last_error) + ki1 * (error + last_error1);
    last_error = error;
    output = constrain(output, -out, out);
    if (output != 0)
    {
      v11 = v + output;
      v22 = v - output;
    }else{
      v11 = v22 = v;
    }
    last_time = millis(); 
    v11 = constrain(v11, 0, v+ac);
    v22 = constrain(v22, 0, v+ac);
    if(v11 == 2) v11 = 3;
    if(v22 == 2) v22 = 3;    Serial.println(output);
  }
}

// mình sẽ tạo một biến toàn cục điều khiển tốc độ của robot, biến này tên là v_bot 
// sau đó, mình sẽ gọi các hàm làm thay đổi giá trị của biến toàn cụ này, đầu tiên mình cần làm là v_bot cần phải được reset về 0  sau mỗi lần nhả nút hoặc bị gặp phải điều kiện nào đó, có thể là kết thúc quá trình 

void up_pid(int set_angle, int v_bott, int ac_balance, int out_set){
  // int speed_pid = soft_start(va);
  // Serial.println(speed_pid);

  balance(set_angle, v_bott, ac_balance, out_set);
  dc2.quaynghich(v22); 
  dc4.quaynghich(v22); 
  dc1.quaythuan(v11); 
  dc3.quaythuan(v11); 
}

void down_pid(int set_angle, int v_bott, int ac_balance , int out_set){
  // int speed_pid = soft_start(va);
  // Serial.println(speed_pid);

  balance(set_angle, v_bott, ac_balance, out_set);
  dc2.quaythuan(v11); 
  dc4.quaythuan(v11); 
  dc1.quaynghich(v22); 
  dc3.quaynghich(v22);
}


// void left(int va, int vb){
//   dc2.quaynghich( abs(va)); 
//   dc3.quaythuan(abs(va-vb)); 
//   dc1.quaynghich(abs(va)); 
//   dc4.quaythuan(abs(va+vb)); 
// }
// left pid 
void left_pid(int set_angle, int v_bott, int ac_balance , int out_set){
  // int speed_pid = soft_start(va);
  // Serial.println(speed_pid);

  balance(set_angle, v_bott, ac_balance, out_set);
  dc2.quaynghich(v22); 
  dc4.quaythuan(v11); 
  dc1.quaynghich(v22); 
  dc3.quaythuan(v11);
}

void right_pid(int set_angle, int v_bott, int ac_balance , int out_set){
  // int speed_pid = soft_start(va);
  // Serial.println(speed_pid);

  balance(set_angle, v_bott, ac_balance, out_set);
  dc2.quaythuan(v11); 
  dc4.quaynghich(v22); 
  dc1.quaythuan(v11); 
  dc3.quaynghich(v22);
}



// right pid 

// void down_pid(int set_angle, int v_bott, int ac_balance , int out_set){
//   // int speed_pid = soft_start(va);
//   // Serial.println(speed_pid);

//   balance(set_angle, v_bott, ac_balance, out_set);
//   dc2.quaythuan(v11); 
//   dc4.quaythuan(v11); 
//   dc1.quaynghich(v22); 
//   dc3.quaynghich(v22);
// }

