/*
 * Driver_Robot.cpp
 *
 *  Created on: Apr 13, 2025
 *      Author: Admin
 */

#include "Driver_Robot.h"
#include "math.h"



long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double fConstrain(double value, double down, double up){
	if(value >= up) return up;
	if(value < down) return down;
	return value;
}


void soft_speed(Variable variable,uint32_t * last_time, int16_t speed,uint32_t time_out,uint8_t delta_speed){ // tốc độ và thời gian khởi động
  if(HAL_GetTick() - (*last_time) >= time_out) {
    if((*variable) < speed){
    	(*variable) += delta_speed;
    	if((*variable) > speed) *variable = speed;
    }else if((*variable) > speed){
    	(*variable) -= delta_speed;
		if((*variable) < speed) *variable = speed;
    }else{
    	(*variable) = speed;
    }

    (*last_time) = HAL_GetTick();
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

    error = get_angle(angle_set) - get_angle(compass());
    //error = angle_set - compass();

    if(abs(error) >= 1000){
        kp = 0.25, ki = 0.1, kd = 0.15;
      //kp = 0.025, ki = 0.01,  kd = 0.02;
    } else
    if(abs(error) >= 400){
      kp = 0.18, ki = 0.1, kd = 0.08;
    } else{
        kp = 0.12, ki = 0.05,  kd = 0.04;

    }

    output = kp * error + ki * (error + last_error) + kd * (error - last_error);
    output = constrain(output, -speed_rot, speed_rot);
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

void run_dc(uint8_t vec,uint8_t num_dc,uint8_t speed){
	if(vec == 1) DC[num_dc].quaythuan(speed);
	if(vec == 0) DC[num_dc].quaynghich(abs(speed));
}

int16_t move(int16_t angle_set, int16_t speed_rot,int16_t angle_tt, int16_t speed_tt,uint32_t time_soft,uint8_t delta_speed){
  // góc muốn đầu robot hướng tới , tốc độ quay, góc tịnh tiến, tốc độ tịnh tiến, thời gian để tăng giảm 1 giá trị speed

  static int16_t speed_run = 0;
  static uint32_t time_soft_speed = 0;
  soft_speed(&speed_run,&time_soft_speed,speed_tt,time_soft,delta_speed);
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
 // Debug("pwm[0] = %d pwm[1] = %d  pwm[2] = %d  pwm[3] = %d",pwm[0],pwm[1],pwm[2],pwm[3] );


  return speed_run;

}



int16_t move_direction(int16_t angle_set, int16_t speed_rot,int16_t angle_tt, int16_t speed_tt,uint32_t time_soft,uint8_t delta_speed){

  static int16_t pwm[4];
  static int16_t speed_run = 0;
  static uint32_t time_soft_speed = 0;
  soft_speed(&speed_run,&time_soft_speed,speed_tt,time_soft,delta_speed);

  static int16_t speed_rotate = 0;
  speed_rotate = PID_rotate(angle_set,speed_rot);
  int16_t angle = compass();

  float alpha[4];

  for(uint8_t i = 0; i < 4; i++)
    alpha[i] = cos((DC[i].angle[0] - angle_tt + angle) * DEG_TO_RAD / 10);

  const double theta[2] = {cos(abs(DC[0].angle[0] - DC[3].angle[0]) * DEG_TO_RAD /10), cos(abs(DC[1].angle[0] - DC[2].angle[0]) * DEG_TO_RAD /10)};

  double gama[2] = {(alpha[0] * alpha[0] + alpha[3] * alpha[3] - 2 * alpha[0] * alpha[3] * theta[0]),
                        (alpha[1] * alpha[1] + alpha[2] * alpha[2] - 2 * alpha[1] * alpha[2] * theta[1])};
  //right robot : DC[0], DC[3]

  pwm[0] = speed_rotate + (speed_run * sqrt(2)) * (alpha[0] - alpha[3] * theta[0]) / gama[0];
  pwm[3] = speed_rotate + (speed_run * sqrt(2)) * (alpha[3] - alpha[0] * theta[0]) / gama[0];


  // left robot : DC[1] DC[2]
  pwm[1] =  speed_rotate - (speed_run * sqrt(2)) * (alpha[1] - alpha[2] * theta[1]) / gama[1];
  pwm[2] =  speed_rotate - (speed_run * sqrt(2)) * (alpha[2] - alpha[1] * theta[1]) / gama[1];

//  uint8_t vecto[4];
//  for(uint8_t i = 0; i < 4; i++){
//	  if(pwm[i] > 0){
//		  vecto[i] = 1;
//		  pwm[i] = (uint8_t)(pwm[i]);
//	  }else{
//		  vecto[i] = 0;
//		  pwm[i] = (uint8_t)(abs(pwm[i]));
//	  }
//	  run_dc(vecto[i],i, pwm[i]);
//  }

  double error = 0;
  for(uint8_t i = 0; i < 4; i++){
	  if(abs(pwm[i]) > 255){
		  error = 255 / pwm[i];
		  break;
	  }
  }

  if(error == 0){
	  for(uint8_t i = 0; i < 4;i++){
		  run_dc(i, pwm[i]);
	  }
  }else{
	  for(uint8_t i = 0; i < 4;i++){
		  pwm[i] = error * pwm[i];
		  run_dc(i, pwm[i]);
	  }
  }

//  run_dc(0,pwm[0]);
//  run_dc(1,pwm[1]);
//  run_dc(2,pwm[2]);
//  run_dc(3,pwm[3]);

  //run_dc(0,pwm[0]);
  //Debug("pwm[0] = %d pwm[1] = %d  pwm[2] = %d  pwm[3] = %d",pwm[0],pwm[1],pwm[2],pwm[3] );

//  Send_speed_UART(&huart3, DC[0].address_DC, pwm[0], DC[1].address_DC, pwm[1], DC[2].address_DC, pwm[2], DC[3].address_DC, pwm[3]);

  return speed_run;

}

int16_t move_hehe(int16_t angle_set, int16_t speed_rot,int16_t angle_tt, int16_t speed_tt,uint32_t time_soft,uint8_t delta_speed){
	  static int16_t pwm[4];
	  static int16_t speed_run = 0;
	  static uint32_t time_soft_speed = 0;
	  soft_speed(&speed_run,&time_soft_speed,speed_tt,time_soft,delta_speed);

	  static int16_t speed_rotate = 0;
	  speed_rotate = PID_rotate(angle_set,speed_rot);
	  int16_t angle = compass();

	  pwm[0] = speed_rotate + (speed_run * sqrt(2) * cos((angle_tt - angle - 450) * DEG_TO_RAD / 10));
	  pwm[1] = speed_rotate + (speed_run * sqrt(2) * cos((angle_tt - angle - 1350) * DEG_TO_RAD / 10));
	  pwm[2] = speed_rotate + (speed_run * sqrt(2) * cos((angle_tt - angle - 2250) * DEG_TO_RAD / 10));
	  pwm[3] = speed_rotate + (speed_run * sqrt(2) * cos((angle_tt - angle - 3150) * DEG_TO_RAD / 10));

	  run_dc(0,pwm[0]);
	  run_dc(1,pwm[1]);
	  run_dc(2,pwm[2]);
	  run_dc(3,pwm[3]);

	  //Debug("pwm[0] = %d pwm[1] = %d  pwm[2] = %d  pwm[3] = %d",pwm[0],pwm[1],pwm[2],pwm[3] );

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
	//Debug("dang phanh");
}

void on_xilanh_ban(){
  HAL_GPIO_WritePin(gpio_xilanh_ban, pin_xilanh_ban, GPIO_PIN_SET);
}
void off_xilanh_ban(){
  HAL_GPIO_WritePin(gpio_xilanh_ban, pin_xilanh_ban, GPIO_PIN_RESET);
}
void on_canh_tay(){
  HAL_GPIO_WritePin(gpio_xilanh_dong, pin_xilanh_dong, GPIO_PIN_SET);
}
void off_canh_tay(){
  HAL_GPIO_WritePin(gpio_xilanh_dong, pin_xilanh_dong, GPIO_PIN_RESET);
}
void on_xilanh_day(){
  HAL_GPIO_WritePin(gpio_xilanh_day, pin_xilanh_day, GPIO_PIN_RESET);
}
void off_xilanh_day(){
  HAL_GPIO_WritePin(gpio_xilanh_day, pin_xilanh_day, GPIO_PIN_SET);
}




bool state_dap_bong = false;
bool state_ban_bong = false;
void ban_bong(){
	static bool triangle;
//	static bool st_tru, st_cong;
	static bool st_ban = false;
	static int16_t v_ban = 130;
	static int16_t speed_ban = 0;
	static uint32_t time_soft_ban = 0;
	static uint32_t time_wait = 0;

	// KHOI DONG BAN VA TANG TOC BAN //
	if(button_triangle && !button_l2){
		if(triangle){
			if(!st_ban) {
				st_ban = true;
				state_dap_bong = false;
				state_ban_bong = true;
				off_canh_tay();
			}
			else{
				if(!button_l2){
//					  if(speed_ban < 50){
//						  v_ban += 2;
//					  }else if(speed_ban < 100){
//						  v_ban += 3;
//					  }else{
//						  v_ban += 5;
//					  }
					  v_ban += 10;
					  if(v_ban > 220) v_ban = 220;
				    }
				}

			triangle = false;
		}

	}else{
		triangle = true;
	}

	/// GIAM TOC BAN //
	static bool cross = true;
	if(button_cross){
		if(!button_l2 && cross){
			  v_ban -= 5;
			  if(v_ban < 80) v_ban = 80;
			  cross = false;

			}
	}else{
		cross = true;
	}

	//Debug("v_ban = %d   speed_ban = %d",v_ban,speed_ban);

	/// DIEU KHIEN TOC BAN //
	if(st_ban){
		//DC_SHOT[1].PID_position_robot(0, 30);
		 if(speed_ban < 20){
			  soft_speed(&speed_ban,&time_soft_ban, v_ban, 10, 2);
		  }else if(speed_ban < 100){
			  soft_speed(&speed_ban,&time_soft_ban, v_ban, 5, 1);
		  }else{
			  soft_speed(&speed_ban,&time_soft_ban, v_ban, 2, 1);
		  }


		  DC[4].quaynghich(speed_ban);
	}else{
		if(!state_dap_bong){
			soft_speed(&speed_ban, &time_soft_ban, 0 ,10, 2);
			DC[4].quaynghich(speed_ban);
		}
		time_wait = HAL_GetTick();

	}

	////  CHO PHEP BAN VA DUNG BAN ////
	static bool circle;
	static uint32_t timer;
	static bool protected_ban;
	if(button_circle && circle && !button_l2){

		protected_ban = true;
		timer = HAL_GetTick();
		circle = false;

	}else if(button_circle && circle && button_l2){
		state_ban_bong = false;
		st_ban = false;

	}else{
		circle = true;
	}

	if(protected_ban && speed_ban >= 100 && HAL_GetTick() - time_wait >= 4000){
		if(HAL_GetTick() - time_wait >= 4500 && HAL_GetTick() - timer >= 1000){
			off_xilanh_ban();
			//state_ban_bong = false;
			//st_ban = false;
			protected_ban = false;
		}else{
			on_xilanh_ban();

		}
	}
}



const int16_t v_dap = 100;
void dap_bong(){
	static bool state[4] = {false,false,false,false};
	static bool square;
	static int16_t speed_dap;
	static uint32_t time_soft;
	static uint32_t time_wait;

	// BAT DAU CHINH DAP BONG // mo canh tay va keo xi lanh xuong
	if(button_square && square && !button_l2){
		state[0] = true;
		off_canh_tay();
		off_xilanh_day();
		state_dap_bong = true;
		state_ban_bong = false;
		square = false;
	}else{
		square = true;
	}

	// KHOI DONG DONG CO //
	if(state[0]){
		//int16_t error_angle = DC_SHOT[1].PID_position_robot(100, 10);
		if(speed_dap < v_dap){
			soft_speed(&speed_dap,&time_soft, v_dap,1, 20);
			time_wait = HAL_GetTick();
		}else{
//			if(HAL_GetTick() - time_wait >= 300 && abs(error_angle) <= 5 ){
			if(HAL_GetTick() - time_wait >= 2500  ){

				state[0] = false;
				state[1] = true;
				time_wait = HAL_GetTick();
			}
		}
	}

	// DAP BONG //
	if(state[1]){
		if(HAL_GetTick() - time_wait >= 500){
			off_xilanh_ban();
			speed_dap = 0;
			state[1] = false;
			state[2] = true;
			time_wait = HAL_GetTick();
		}else{
			on_xilanh_ban();
		}
	}

	// DO BONG //
	if(state[2]){
		if(HAL_GetTick() - time_wait >= 500){
			on_xilanh_day();
			if(HAL_GetTick() - time_wait >= 1000 || HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) ){
				on_canh_tay();
				state_dap_bong = false;
				state[2] = false;
			}
		}
	}

	if(!state_ban_bong){
		DC[4].quaynghich(speed_dap);

	}

}




const uint8_t err_num = 255;
void run_basic(){
	static int16_t v_speed;
	static uint32_t time_soft_speed;
	static uint8_t delta_speed;

	if(button_r2){
		  v_speed = 135;
	  }else{
		  v_speed = 30;
	 }

	if(v_speed < 50){
		time_soft_speed = 8;
		delta_speed = 1;
	}else if(v_speed < 80){
		time_soft_speed = 9;
		delta_speed = 2;
	}else{
		time_soft_speed = 10;
		delta_speed = 2;
	}


	static int16_t ag_rot = 0;
	static int16_t ag_tt = 0;
	//static uint32_t time_update = 0;
	  if((abs(LX) >= err_num || abs(LY) >= err_num || abs(RY) >= err_num || abs(RX) >= err_num) && !button_l2){
		int16_t speed;
		if( abs(LX) >= err_num || abs(LY) >= err_num){
			ag_tt = atan2(LY,LX) * RAD_TO_DEG * 10;
			speed = sqrt(LX * LX + LY * LY);
			speed = map(speed, 0, 508, 20, v_speed);

		}else{
			speed = 0;
		}

		if(abs(RY) >= err_num || abs(RX) >= err_num){
		  ag_rot = atan2(RY,RX) * RAD_TO_DEG * 10;
		}
		move_direction(ag_rot,70,ag_tt,speed,time_soft_speed,delta_speed);

	  }else

	  if(button_up){
		ag_tt = ag_rot;
		move(ag_rot,70,ag_tt,v_speed,time_soft_speed,delta_speed); // dùng như hàm move

	  }else if(button_down){
		ag_tt = ag_rot + 1800;
		move_direction(ag_rot,70,ag_tt,v_speed,time_soft_speed,delta_speed);

	  }else if(button_left){
		ag_tt = ag_rot + 900;
		move_direction(ag_rot,70,ag_tt,v_speed,time_soft_speed,delta_speed);

	  }else if(button_right){
		ag_tt = ag_rot - 900;
		move_direction(ag_rot,70,ag_tt,v_speed,time_soft_speed,delta_speed);

	  }else if(button_r1 && !button_l2){
		rot_right(20);
		ag_rot = compass();
		//time_update = HAL_GetTick();

	  }else if(button_l1 && !button_l2){
		rot_left(20);
		ag_rot = compass();
		//time_update = HAL_GetTick();


	  }else{
		  static uint32_t time_end;
		  static uint8_t delta_end;
		  if(!state_ban_bong && !state_dap_bong){
			  int16_t speed_soft = move_direction(ag_rot,20 , ag_tt, 0 , time_end,delta_end);
				  if(speed_soft < 30){
					  time_end = 10;
					  delta_end = 1;
				  }else if(speed_soft < 180){
					  time_end = 10;
					  delta_end = 2;
				  }else if(speed_soft == 0 ){
					  stop();
				  }
				  //Debug("khong phanh");
		  }else{
			  //stop();
			  //Debug("dang phanh");
				DC[0].quaythuan(2);
				DC[1].quaythuan(2);
				DC[2].quaythuan(2);
				DC[3].quaythuan(2);

		  }

	  }
/////// RESET ANGLE ////////////
	  static bool st_reset = true;
	  if(button_l3){
		  if(st_reset){
			  reset_compass();
			  DC_SHOT[1].set_encoder(0);
			  DC_SHOT[0].set_encoder(0);
			  ag_rot = compass();
			  st_reset	= false;
		  }
	  }else st_reset = true;

	  static bool state_r3;
	  static bool st_xilanh_canhtay = true;
	  if(button_r3 && button_l2){
		  if(state_r3){
			  st_xilanh_canhtay = !st_xilanh_canhtay;
			  if(st_xilanh_canhtay) on_canh_tay(); else off_canh_tay();
			  state_r3 = false;

		  }
	  }else{
		  state_r3 = true;
	  }

//// DIEU KHIEN CO CAU QUAY   ////////
	  if(button_r1 && button_l2){
	  	  //test = DC_SHOT[0].PID_position_robot(-900, 20);
	  	  DC_SHOT[0].quaynghich(6);
	    }else if(button_l1 && button_l2){
	  	  DC_SHOT[0].quaythuan(6);
	    }else{
	  	  DC_SHOT[0].quaythuan(2);
	    }

	    if(button_triangle && button_l2){
	  	  DC_SHOT[1].quaythuan(40);

	    }else if(button_cross && button_l2){
	  	  DC_SHOT[1].quaynghich(40);

	    }else{
	  	  DC_SHOT[1].quaythuan(0);

	    }




}







//	    // BAN BONG //
//	    static int16_t speed_ban = 0;
//	    static int16_t v_ban = 130;
//	    static uint32_t time_soft_ban = 0;
//	    static bool st_r3 = true;
//	    static bool st_ban = false;
//	    if(button_circle && !button_l2){
//	  	  if(st_r3){
//	  		  st_ban = !st_ban;
//	  		  st_r3 = false;
//	  	  }
//	    }else{
//	  	  st_r3 = true;
//	    }
//
//	    if(st_ban){
//	      if(speed_ban < 20){
//		  	  soft_speed(&speed_ban,&time_soft_ban, v_ban, 10, 2);
//	      }else if(speed_ban < 100){
//	    	  soft_speed(&speed_ban,&time_soft_ban, v_ban, 5, 1);
//	      }else{
//	    	  soft_speed(&speed_ban,&time_soft_ban, v_ban, 2, 1);
//	      }
//	  	  DC[4].quaynghich(speed_ban);
//	  	  st_phanh = true;
//
//
//	    }else{
//	  	  soft_speed(&speed_ban,&time_soft_ban, 0, 10, 2);
//	  	  DC[4].quaynghich(speed_ban);
//	  	  st_phanh	= false;
//
//
//	    }
//	    Debug("speed_ban = %d ",speed_ban);
//
//
//	    // XU LI TOC DO BAN //
//	    static bool st_tru, st_cong;
//
//	    if(button_triangle && !button_l2){
//	  	  if(st_cong){
//		      if(speed_ban < 50){
//		    	  v_ban += 2;
//		      }else if(speed_ban < 100){
//		    	  v_ban += 3;
//		      }else{
//		    	  v_ban += 5;
//		      }
//	  		  if(v_ban > 220) v_ban = 220;
//	  		  st_cong = false;
//	  	  }
//	    }else{
//	  	  st_cong = true;
//	    }
//
//	    if(button_cross && !button_l2){
//	  	  if(st_tru){
//		      if(speed_ban < 50){
//		    	  v_ban -= 2;
//		      }else if(speed_ban < 100){
//		    	  v_ban -= 3;
//		      }else{
//		    	  v_ban -= 5;
//		      }
//	  		  if(v_ban < 80) v_ban = 80;
//	  		  st_tru = false;
//	  	  }
//
//	    }else{
//	  	  st_tru = true;
//	    }
//
//	    //Debug("v_ban = %d",v_ban);
//
//	    /// CANH TAY VAF DON BAN //
//	    static bool st_square_1 = true;
//	    static bool st_xilanh_ban = false;
//	    if(button_r3 && !button_l2){
//	    	if(st_square_1){
//	    		st_xilanh_ban = !st_xilanh_ban;
//	    		st_square_1 = false;
//	    	}
//
//	    }else{
//	    	st_square_1 = true;
//	    }
//    	if(st_xilanh_ban){
//	    	on_xilanh_ban();
//    	}else{
//    		off_xilanh_ban();
//    	}
//
//	    static bool st_square_2 = true;
//	    static bool st_canhtay = true;
//	    if(button_r3 && button_l2){
//	    	if(st_square_2){
//	    		st_canhtay = !st_canhtay;
//	    		st_square_2 = false;
//	    	}
//
//	    }else{
//	    	st_square_2 = true;
//	    }
//    	if(st_canhtay){
//	    	on_canh_tay();
//    	}else{
//    		off_canh_tay();
//    	}




