#include "all_thing.h"
#include "math.h"
#include "stm32f1xx_hal.h"
#include "stdint.h"

extern motor_uart m[5];
bool read_stt = true;;
uint8_t rx_data[10];
uint16_t button;
int16_t RX;
int16_t LX;
int16_t RY;
int16_t LY;	
bool angle_stt = true;
uint8_t angle_buff[2];
int16_t angle;
double PI_const = 3.141592654; 
uint16_t speed_robot = 0;
uint16_t MAX_SPEED = 150;
int32_t debug_var;
long X;
long last_X;
long Y;
long last_Y;


/*----------------------------FOR MOVE-----------------------------------*/
void Soft_start(uint16_t target_speed, uint8_t time){
	static uint32_t timest = 0;
	if(speed_robot < target_speed){
		if(HAL_GetTick()-timest >= time){
			speed_robot++;
			timest = HAL_GetTick();
		}
	}
}
void Soft_end(uint16_t target_speed, uint8_t time){
	static uint32_t timeend = 0;
	if(speed_robot > target_speed){
		if(HAL_GetTick()-timeend >= time){
			speed_robot--;
			timeend = HAL_GetTick();
		}
	}
}

float constrain(float value, float min_val, float max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

int pid_cal(int set_point, int input,int constrain_p,  double pP, double pI, double pD){ 
  int output = 0; 
  static long error_p, last_error_p; 
  error_p = set_point - input; 
  output = pP*error_p + pI*(error_p +  last_error_p) + pD*(error_p - last_error_p); 
  last_error_p = error_p; 
  output = constrain(output, - constrain_p, constrain_p); 
  return output; 
}

void rundc1(int v, int vec){ v = abs(v);   if(vec == 1 ){  m[0].Forward(&m[0],v);  }else if(vec == 0){  m[0].Reverse(&m[0],v);}}
void rundc2(int v, int vec){ v = abs(v);   if(vec == 1 ){  m[1].Forward(&m[1],v);  }else if(vec == 0){  m[1].Reverse(&m[1],v);}}
void rundc3(int v, int vec){ v = abs(v);   if(vec == 1 ){  m[2].Forward(&m[2],v);  }else if(vec == 0){  m[2].Reverse(&m[2],v);}}
void rundc4(int v, int vec){ v = abs(v);   if(vec == 1 ){  m[3].Forward(&m[3],v);  }else if(vec == 0){  m[3].Reverse(&m[3],v);}}

void move(int angle_rotare, int v_rotare, int angle_tt, int v_tt){
	
  int angle_compass = angle; 
  int real_v_rotare = pid_cal(angle_rotare/10,angle_compass/10, v_rotare, 0.7, 0.15, 0.08); 
  int pwm1, pwm2, pwm3, pwm4;
  int vec1, vec2, vec3, vec4; 
  pwm1 = real_v_rotare + v_tt*cos((angle_tt - angle_compass - 450)*(PI_const/1800)); 
  pwm2 = real_v_rotare + v_tt*cos((angle_tt - angle_compass - 1350)*(PI_const/1800)); 
  pwm3 = real_v_rotare + v_tt*cos((angle_tt - angle_compass - 2250)*(PI_const/1800)); 
  pwm4 = real_v_rotare + v_tt*cos((angle_tt - angle_compass - 3150)*(PI_const/1800));

  if(pwm1 >  0 ){ vec1 = 1; }else { vec1 = 0 ; }
  if(pwm2 >  0 ){ vec2 = 1; }else { vec2 = 0 ; }
  if(pwm3 >  0 ){ vec3 = 1; }else { vec3 = 0 ; }
  if(pwm4 >  0 ){ vec4 = 1; }else { vec4 = 0 ; }
	
	rundc1(pwm1,vec1);
	rundc2(pwm2,vec2);
	rundc3(pwm3,vec3);
	rundc4(pwm4,vec4);
}

void test(){
	m[0].Forward(&m[0], 200);
	m[1].Forward(&m[1], 200);
	m[2].Forward(&m[2], 200);
	m[3].Forward(&m[3], 200);
	m[4].Forward(&m[4], 20);
}
void rolate_right(uint16_t speed){
	m[0].Forward(&m[0],speed);
	m[1].Forward(&m[1],speed);
	m[2].Forward(&m[2],speed);
	m[3].Forward(&m[3],speed);
}
void rolate_left(uint16_t speed){
	m[0].Reverse(&m[0],speed);
	m[1].Reverse(&m[1],speed);
	m[2].Reverse(&m[2],speed);
	m[3].Reverse(&m[3],speed);
}
	bool okok = true;
	bool state[7] = {false, false, false,false, false, false, true};
	int dem10 = 0;
		static uint32_t time_nhay = 0;
	static uint32_t time_quay = 0;
void test_nhay(){



	okok = HAL_GPIO_ReadPin(GPIO_sensor_port, GPIO_sensor_pin);


	
	 if(button_L3 &&  button_R3){
		
		HAL_GPIO_WritePin(GPIO_day, GPIO_day_pin, GPIO_PIN_RESET);
		
		HAL_GPIO_WritePin(GPIO_kep, GPIO_kep_pin, GPIO_PIN_RESET);		
	}
	//nhu trên
	else if(button_cross && button_up){
			if(!okok)
			{
				time_quay = HAL_GetTick();
				state[6] = false;
				state[0] = true;
			}
			if(state[0]){
				if(HAL_GetTick() - time_quay <= 200){
						m[4].Forward(&m[4], 150);
				}
				else{
					time_quay = HAL_GetTick();
					state[1] = true;
					state[0] = false;
				}
			}
			if(state[1]){
				if(HAL_GetTick() - time_quay <= 70){
					m[4].Reverse(&m[4], 150);
				}
				else{
					time_quay = HAL_GetTick();
					state[2] = true;
					state[1] = false;
				}
			}
			
			if(state[6]){
				m[4].Forward(&m[4], 150);
			}
	}

 else if(button_share &&  button_triangle && button_R1){
		dem10 = 2;
	//	time_nhay = HAL_GetTick();
		HAL_GPIO_WritePin(GPIO_day, GPIO_day_pin, GPIO_PIN_SET);
		if(HAL_GetTick() - time_nhay >= 100){
		HAL_GPIO_WritePin(GPIO_kep, GPIO_kep_pin, GPIO_PIN_SET);
			time_nhay = HAL_GetTick();
		}
}
		else{
		time_nhay = HAL_GetTick();
	}
}


void hand_ctrl(){
	static int16_t ag_rot;
	static int16_t ag_tt;
	static uint16_t time_nhay = 0;
	static uint16_t time_quay = 0;
	static uint8_t da_quay = 0;
	static uint16_t time_quay1 = 0;
	bool state[4] = {true, false, false, false};
  if(button_options){
	  MAX_SPEED = 30;
  }else{
	  MAX_SPEED = 150;
  }
  if(abs(LX) >= 50 || abs(LY) >= 50 || abs(RY) >= 50 || abs(RX) >= 50){
		if( abs(LX) >= 50 || abs(LY) >= 50){
			Soft_start(MAX_SPEED, 10);
			ag_tt = atan2(LX,LY) * RAD_TO_DEG * 10;
			debug_var = ag_tt;
		}else{
			speed_robot = 2;
		}
		if(abs(RX) > 70 || abs(RY) >70){
			Soft_start(MAX_SPEED, 10);
			ag_rot = atan2(RX,RY)*10*RAD_TO_DEG;
		}
		move(ag_rot,50,ag_tt,speed_robot);
  }
  else if(button_up){
		Soft_start(MAX_SPEED, 10);
		ag_tt = ag_rot;
		move(ag_rot,50,ag_tt,speed_robot);

  }else if(button_down){
		Soft_start(MAX_SPEED, 10);
		ag_tt = ag_rot + 1800;
		move(ag_rot,50,ag_tt,speed_robot);

  }else if(button_left){
		Soft_start(MAX_SPEED, 10);
    ag_tt = ag_rot + 900;
    move(ag_rot,50,ag_tt,speed_robot);

  }else if(button_right){

		Soft_start(MAX_SPEED, 10);
    ag_tt = ag_rot - 900;
    move(ag_rot,50,ag_tt,speed_robot);

  }else if(button_R2){
		Soft_start(30, 10);
		ag_rot = angle;
		rolate_right(speed_robot);
	
  }
	else if(button_L2 && !button_share){
		ag_rot = angle;
		Soft_start(30, 10);
		rolate_left(speed_robot);
  }
	else if(button_share && button_triangle){
		
		HAL_GPIO_WritePin(GPIO_day, GPIO_day_pin, GPIO_PIN_SET);
		if(HAL_GetTick() - time_nhay >= 100){
		HAL_GPIO_WritePin(GPIO_kep, GPIO_kep_pin, GPIO_PIN_SET);
			time_nhay = HAL_GetTick();
		}
}
	else if(button_share &&  button_cross){
		
		HAL_GPIO_WritePin(GPIO_day, GPIO_day_pin, GPIO_PIN_RESET);
		
		HAL_GPIO_WritePin(GPIO_kep, GPIO_kep_pin, GPIO_PIN_RESET);		
				
	}
	else if(button_share && button_options){
		if(state[0]){
			m[4].Forward(&m[4], 80);
			if(HAL_GPIO_ReadPin(GPIO_sensor_port, GPIO_sensor_pin) == 0){
					time_quay = HAL_GetTick();
					state[0] = false;
					state[1] = true;
				}
		}
		else if(state[1]){
			m[4].Forward(&m[4], 40);
			if(HAL_GetTick() - time_quay1 >= 250){
				state[1] = false;
				state[2] = true;
			}
		}
		else if(state[2]){
				m[4].Reverse(&m[4], 0);
			state[2] = false;
			state[3] = true;
		}
		else if(state[3]){
			
		}
	}
	else if(button_cross ){
		m[4].Reverse(&m[4], 40);
	}
	else if(button_square){
		m[4].Forward(&m[4], 40);
	}else{
		
		state[0] = true;
		state[1] = false;
		state[2] = false;
		state[3] = false;
		

		if(speed_robot>0){
			Soft_end(2, 1);
			move(ag_rot, speed_robot, ag_tt, speed_robot);
			/*
			if(speed_robot < 4){
				ag_rot = angle;
				if(speed_robot < 3){
					m[0].Forward(&m[0],0);
					m[1].Forward(&m[1],0);
					m[2].Forward(&m[2],0);
					m[3].Forward(&m[3],0);
				}
			}
			*/
		}else{
			m[0].Forward(&m[0],2);
			m[1].Forward(&m[1],2);
			m[2].Forward(&m[2],2);
			m[3].Forward(&m[3],2);
			m[4].Forward(&m[4],2);
		}

  }
}

