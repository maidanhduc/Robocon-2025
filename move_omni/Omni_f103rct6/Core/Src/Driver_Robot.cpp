/*
 * Driver_Robot.cpp
 *
 *  Created on: Mar 25, 2025
 *      Author: Admin
 */


#define RAD_TO_DEG  57.295779513082320876798154814105 // 180 / pi
#define DEG_TO_RAD 0.017453292519943295769236907684886

#include "Driver_Robot.h"
#include "stdlib.h"
#include "math.h"

extern Motor_PID DC[4];


int32_t constrains(int32_t value, int32_t down, int32_t up ){
	if(value >= up) return up;
	if(value < down) return down;
	return value;
}

double fConstrain(double value, double down, double up){
	if(value >= up) return up;
	if(value < down) return down;
	return value;
}

using Variable = int16_t*;

void soft_speed(Variable variable, int16_t speed,uint32_t time_out){ // tốc độ và thời gian khởi động
  static uint32_t timee = 0;
  if(HAL_GetTick() - timee >= time_out) {
    if((*variable) < speed){
      (*variable)++;
    }else if((*variable) > speed){
      (*variable)--;
    }else{
      (*variable) = speed;
    }

    timee = HAL_GetTick();
  }
}


int16_t get_angle(int16_t angle){ // resolution(độ phân giải) : -1800 -> 1800
  angle = angle % 3600;
  if(angle >= 1800) return angle -= 3600;
  if(angle < -1800) return angle += 3600;
  return angle;

}

int16_t PID_rotate(int16_t angle_set,int16_t speed_rot){ // góc muốn đến , giới hạn tốc độ
  float kp, ki, kd;
  static int16_t error  = 0,last_error = 0;
  int16_t output = 0;

    error = angle_set - get_angle(compass());

    if(abs(error) >= 1000){
      kp = 0.25, ki = 0.01,  kd = 0.07;
    } else
    if(abs(error) >= 400){
      kp = 0.15, ki = 0.06, kd = 0.04;
    } else{
      kp = 0.1, ki = 0.03, kd = 0.01;
    }

    output = kp * error + ki * (error + last_error) + kd * (error - last_error);
    output = constrains(output, -speed_rot, speed_rot);
    //Serial.println(output);
    last_error = error;

  return output;
}

void run_dc(uint8_t num_dc,int16_t speed){
  if(speed >= 0){
    DC[num_dc].quaythuan(speed);
  }else{
    DC[num_dc].quaynghich(abs(speed));
  }
}

/*

  1         0



  2         3


*/
/*
  ***hàm di chuyển đa hướng kết hợp PID góc được dựa trên việc tổng hợp lực dựa trên các lực thành phần kết hợp với giá trị PID góc tạo ra momen quay
  ***tổng hợp lực :
      chia robot làm 2 : trái và phải theo trục robot dựa trên mối quan hệ giữa góc tịnh tiến và góc hiện tại của robot đó chính là trục của việc chia
      dựa vào cặp trái phải mỗi cặp sẽ 2 bánh từ đó tổng hợp lực pwm từng cặp bánh đó  :p
  *** PID góc :
    sự lệch góc so với góc robot hướng tới tạo ra sai số -> áp vào thuật toán PID để tính toán ra momen quay từ đó áp vào tổng hợp lực

**** NOTE:
    +để robot vừa xoay vừa tịnh tiến thì điều quan trọng đó chính là hướng tịnh tiến không thay đổi mà vẫn đáp ứng được việc xoay
    bản chất : vì dựa trên việc chia robot làm 2 thành ra robot sẽ được hiểu thành robot có 2 wheel nhưng có thể xoay tùy ý
    điều này đưa ra được 2 vấn đề :
      chỉ tịnh tiến : 2 lực đó bằng nhau và cùng hướng đến ..... góc tịnh tiến mong muốn
      xoay và tịnh tiến : điều này vẫn là 2 lực nhưng giá trị 2 lực này khác nhau nhưng song song và cùng hướng đến góc tịnh tiến

    +việc bù trừ momen quay và góc của từng bánh xe không đều và đối xứng nhau theo góc 45 độ so với trục là vấn đề quan trọng của bài toán này

    với từng góc lệch khác 45 độ của từng bánh sẽ dẫn đến việc tác động lực của bánh đó với robot sẽ khác nhau và phụ thuộc vào việc góc tịnh tiến so với vị trị
    bánh trên robot vd : bánh 0 là 47 độ nếu tịnh tiến với góc 0 thì bánh sẽ tạo ra lực theo góc 47 độ so với robot
                         nếu là góc tịnh tiến 90 độ thì lúc đó bánh sẽ tạo ra lực theo góc 47-90 = -43 so với trục robot
    có thể thấy góc 45 độ là góc lí tưởng nhưng trên thực tế về phần cơ khí chưa thể đáp ứng điều này nên thuật toán này sinh ra để giúp cho việc
    di chuyển đa hướng thực tế hơn với tùy từng vị trí bánh xe  :)

    với việc bù trù momen quay cũng được coi là linh hồn của thuật toán này bởi vì nếu bù trừ sai sẽ đẫn đến robot di chuyển sai ý muốn và càng càng sai hơn

    bù trừ như nào ?
     bù trừ này cũng gọi là bước gần cuối khi tôi nghĩ đến vì khi test mới nhận ra còn lỏng lẻo trong thuật toán
     để hiểu bù trừ thì ta cần hiểu giá trị trả về của PID_rotate ví dụ như hiệu góc mong muốn - góc hiện tại âm -> trả về âm ? :  dương (lười viết haha)
      tịnh tiến này xảy ra 4th ứng với 4th momen quay
      cặp bánh trái và phải có thể xảy ra các th sau  (L,R) = (1;2 , 0;3), (2;3 , 1;0), (3;0 , 2;1), (0;1 , 3;2)
      dựa vào đó cộng trừ cặp cho hợp lí :>>>

      đến đây là hoàn thiện thuật toán rồi nhưng sẽ còn hướng phát triển nữa như là :
        + 2 động cơ chéo nhau không cùng trên 1 trục -> trục khác
        + bán kính từng bánh khác nhau (ảnh hưởng đến momen quay)
        + xoay đến góc mong muốn nhưng có tính chu kì (đã có)
        + xoay với góc xoay nhỏ nhất (đã có)


*/


// di chuyển ver 2
int16_t move(int16_t angle_set, int16_t speed_rot,int16_t angle_tt, int16_t speed_tt,uint32_t time_soft){
  // góc muốn đầu robot hướng tới , tốc độ quay, góc tịnh tiến, tốc độ tịnh tiến, thời gian để tăng giảm 1 giá trị speed

  static int16_t speed_run = 0;
  soft_speed(&speed_run,speed_tt,time_soft);
  int16_t pwm[4];
  float alpha[4];

  int16_t speed_rotate = PID_rotate(angle_set,speed_rot); // xử lí PID góc

  int16_t angle_diff = get_angle(angle_tt - compass());
  static uint8_t left_front, left_back, right_front, right_back, mode;

  // xử lí góc tịnh tiến của robot

  if(angle_diff <= -450 && angle_diff > -1350 ){
    left_front  =   0;  right_front =  3;
    left_back   =   1;  right_back  =  2;
    angle_diff += 900;
    mode = 1;
  }else if( angle_diff <= 450 && angle_diff > -450){
    left_front  =   1;  right_front =  0;
    left_back   =   2;  right_back  =  3;
    mode = 0;
  }else if( angle_diff > 450  && angle_diff <= 1350){
    left_front  =   2;  right_front =  1;
    left_back   =   3;  right_back  =  0;
    angle_diff -= 900;
    mode = 1;
  }else if(angle_diff > 1350 || angle_diff <= -1350 ){
    left_front  =   3;  right_front =  2;
    left_back   =   0;  right_back  =  1;
    if(angle_diff > 1350) angle_diff -= 1800;
    if(angle_diff <= -1350) angle_diff += 1800;
    mode = 0;
  }

  alpha[right_front]  = cos((DC[right_front].angle[mode] - angle_diff) * DEG_TO_RAD / 10);
  alpha[right_back]   = cos((DC[right_back].angle[mode]  - angle_diff) * DEG_TO_RAD / 10);
  alpha[left_front]   = cos((DC[left_front].angle[mode] - angle_diff + 1800) * DEG_TO_RAD / 10);
  alpha[left_back]    = cos((DC[left_back].angle[mode]  - angle_diff + 1800) * DEG_TO_RAD / 10);

  const double theta[2] = {cos(abs(DC[right_front].angle[mode] - DC[right_back].angle[mode]) * DEG_TO_RAD /10),
                          cos(abs(DC[left_front].angle[mode] -  DC[left_back].angle[mode]) *  DEG_TO_RAD /10)};


  const double gama[2] = {(alpha[right_front] * alpha[right_front] + alpha[right_back] * alpha[right_back] - 2 * alpha[right_front] * alpha[right_back] * theta[0]),
                   (alpha[left_front] * alpha[left_front] + alpha[left_back] * alpha[left_back] - 2 * alpha[left_front] * alpha[left_back] * theta[1])};
  //right robot

  // xử lí tốc độ

  pwm[right_front] =  (speed_rotate + speed_run * sqrt(2)) * (alpha[right_front] - alpha[right_back]  * theta[0]) / gama[0];
  pwm[right_back]  =  (speed_rotate + speed_run * sqrt(2)) * (alpha[right_back] -  alpha[right_front] * theta[0]) / gama[0];


  // left robot
  pwm[left_front]  =  (-speed_rotate + speed_run * sqrt(2)) * (alpha[left_front] - alpha[left_back]  * theta[1]) / gama[1];
  pwm[left_back]   =  (-speed_rotate + speed_run * sqrt(2)) * (alpha[left_back]  - alpha[left_front] * theta[1]) / gama[1];

  run_dc(right_front,pwm[right_front]);
  run_dc(right_back ,pwm[right_back] );
  run_dc(left_front ,pwm[left_front] );
  run_dc(left_back  ,pwm[left_back]  );

  return speed_run;

}



// di chuyển ver 1
int16_t move_direction(int16_t angle_set, int16_t speed_rot,int16_t angle_tt, int16_t speed_tt,uint32_t time_soft){

  static int16_t speed_run = 0;
  soft_speed(&speed_run,speed_tt,time_soft);
  static int16_t speed_rotate = 0;
  speed_rotate = PID_rotate(angle_set,speed_rot);
  int16_t pwm[4];
  float alpha[4];


  for(uint8_t i = 0; i < 4; i++)
    alpha[i] = cos((DC[i].angle[0] - angle_tt + compass()) * DEG_TO_RAD / 10);

  const double theta[2] = {cos(abs(DC[0].angle[0] - DC[3].angle[0]) * DEG_TO_RAD /10), cos(abs(DC[1].angle[0] - DC[2].angle[0]) * DEG_TO_RAD /10)};

  double gama[2] = {(alpha[0] * alpha[0] + alpha[3] * alpha[3] - 2 * alpha[0] * alpha[3] * theta[0]),
                        (alpha[1] * alpha[1] + alpha[2] * alpha[2] - 2 * alpha[1] * alpha[2] * theta[1])};
  //right robot : DC[0], DC[3]

  pwm[0] = speed_rotate + (speed_run * sqrt(2)) * (alpha[0] - alpha[3] * theta[0]) / gama[0];
  pwm[3] = speed_rotate + (speed_run * sqrt(2)) * (alpha[3] - alpha[0] * theta[0]) / gama[0];


  // left robot : DC[1] DC[2]
  pwm[1] =  speed_rotate - (speed_run * sqrt(2)) * (alpha[1] - alpha[2] * theta[1]) / gama[1];
  pwm[2] =  speed_rotate - (speed_run * sqrt(2)) * (alpha[2] - alpha[1] * theta[1]) / gama[1];

  run_dc(0,pwm[0]);
  run_dc(1,pwm[1]);
  run_dc(2,pwm[2]);
  run_dc(3,pwm[3]);

  return speed_run;

}

void rot_left(int16_t speed){
	DC[0].quaythuan(speed);
	DC[1].quaythuan(speed);
	DC[2].quaythuan(speed);
	DC[3].quaythuan(speed);
}

void rot_right(int16_t speed){
	DC[0].quaynghich(speed);
	DC[1].quaynghich(speed);
	DC[2].quaynghich(speed);
	DC[3].quaynghich(speed);
}

void stop(){
	DC[0].quaythuan(0);
	DC[1].quaythuan(0);
	DC[2].quaythuan(0);
	DC[3].quaythuan(0);  // muốn phanh điền tham trị 2
}





const uint8_t err_num = 100; // tùy tay cầm có thể hiệu chỉnh
void run_basic(){

  static int16_t ag_rot = 0;
  static int16_t ag_tt = 0;

  if(abs(LX) >= err_num || abs(LY) >= err_num || abs(RY) >= err_num || abs(RX) >= err_num){

    if( abs(LX) >= err_num || abs(LY) >= err_num){
        ag_tt = atan2(LX,LY) * RAD_TO_DEG * 10;
    }

    if(abs(RY) >= err_num || abs(RX) >= err_num){
      ag_rot = atan2(RX,RY) * RAD_TO_DEG * 10;
    }
    move_direction(ag_rot,50,ag_tt,80,8);
  }else

  if(button_up){
    move_direction(get_angle(ag_rot),50,ag_rot,80,8); // dùng như hàm move

  }else if(button_down){
    move_direction(get_angle(ag_rot),50,ag_rot + 1800,80,8);

  }else if(button_left){
    move_direction(get_angle(ag_rot),50,ag_rot + 900,80,8);

  }else if(button_right){
    move_direction(get_angle(ag_rot),50,ag_rot - 900,80,8);

  }else if(button_r1){
	rot_right(50);
  }else if(button_l1){
	rot_left(50);

  }else{

   int16_t st = move_direction(ag_rot, 50, ag_tt, 0, 6);
   if(st <= 20){ // dừng mềm
	   ag_rot = compass();
	   // phanh or dung
	   if(st == 0) stop();
   }
  }
}


