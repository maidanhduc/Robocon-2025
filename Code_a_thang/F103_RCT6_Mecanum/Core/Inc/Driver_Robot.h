/*
 * Driver_Robot.h
 *
 *  Created on: Apr 13, 2025
 *      Author: Admin
 */

#ifndef INC_DRIVER_ROBOT_H_
#define INC_DRIVER_ROBOT_H_

#include "main.h"
#include "PWM_Master_f103.h"
#include "GPIO_Master.h"
#include "UART_Master.h"
#include "stdlib.h"

#define RAD_TO_DEG  57.295779513082320876798154814105 // 180 / pi
#define DEG_TO_RAD 0.017453292519943295769236907684886


#define SPEED_MAX 300

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

//int32_t constrains(int32_t value, int32_t down, int32_t up );

double fConstrain(double value, double down, double up);

int16_t get_angle(int16_t angle);


class Motor_PID{
protected:
    static constexpr uint8_t PWM = 1;
    static constexpr uint8_t UART = 0;
	// PWM
	GPIO_TypeDef* Port_Dir = nullptr;
	uint32_t Pin_Dir = 0;

	TIM_HandleTypeDef* htim;
	uint16_t TIM_CHANNEL;

	// UART
	UART_HandleTypeDef* huart = nullptr;
	uint8_t mode = true;



public:
	uint8_t address_DC = 0;
	int16_t angle[2];
	Motor_PID(GPIO_TypeDef* Port_Dir, uint32_t Pin_Dir , TIM_HandleTypeDef* htim,uint16_t TIM_CHANNEL,int16_t angle_val):
		Port_Dir(Port_Dir),Pin_Dir(Pin_Dir),htim(htim),TIM_CHANNEL(TIM_CHANNEL){
		static bool st_add_map = true;
	    if(st_add_map){
	    	Add_Map_STM32F103xx();
	    	st_add_map = false;
	    }

		angle[0] = angle_val;
	    mode = true;
		GPIO_OUT_Init(Port_Dir, Pin_Dir, GPIO_SPEED_FREQ_HIGH);
//		HAL_TIM_PWM_Start(htim, TIM_CHANNEL);

		//Register_PWM_GPIO(Port_Pwm, Pin_Pwm, 55, 299, 0);
		HAL_GPIO_WritePin(Port_Dir, Pin_Dir, GPIO_PIN_SET);
		if(angle[0] > 0) angle[1] = angle[0] - 900;
		if(angle[0] < 0) angle[1] = angle[0] + 900;
	}

	Motor_PID(UART_HandleTypeDef* huart,uint8_t address_DC,int16_t angle_val) : huart(huart), address_DC(address_DC){
		mode = false;
		angle[0] = angle_val;
		if(angle[0] > 0) angle[1] = angle[0] - 900;
		if(angle[0] < 0) angle[1] = angle[0] + 900;
	}

	void quaythuan(int16_t speed){
		if(speed == 2) speed = 3;
		if(speed < 0) speed = 0;
		switch(mode){
			case PWM:  speed = SPEED_MAX -  speed;

					   HAL_GPIO_WritePin(Port_Dir, Pin_Dir, GPIO_PIN_RESET);
				   	   //PWM_Write_GPIO(Port_Pwm, Pin_Pwm,speed);
					   __HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL,speed);
				   	   break;
			case UART:
						if(speed > 255) speed = 255;
						Controll_DC_UART(huart, address_DC, HIGH, (uint8_t)(speed));
						break;
		}

	}

	void quaynghich(int16_t speed){
		if(speed == 2) speed = 3;
		if(speed < 0) speed = 0;
		switch(mode){
			case PWM:  speed = SPEED_MAX - speed;
					   HAL_GPIO_WritePin(Port_Dir, Pin_Dir, GPIO_PIN_SET);
				   	   //PWM_Write_GPIO(Port_Pwm, Pin_Pwm,speed);
					   __HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL,speed);

				   	   break;
			case UART: 	if(speed > 255) speed = 255;
						Controll_DC_UART(huart, address_DC, LOW, (uint8_t)(speed));
						break;
		}
	}

	void stop(){
		switch(mode){

		case PWM: HAL_GPIO_WritePin(Port_Dir, Pin_Dir, GPIO_PIN_RESET);
//	   	   	   	  PWM_Write_GPIO(Port_Pwm, Pin_Pwm,253);
		   __HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL,SPEED_MAX - 2);

				  break;
		case UART: Controll_DC_UART(huart, address_DC, LOW, 0x02);
					break;
		}


	}
};



class Module_shot{
protected:

    static constexpr uint8_t tim = 1;
    static constexpr uint8_t exti = 0;

	// UART
	UART_HandleTypeDef* huart;
	uint8_t address_DC = 0;
	uint8_t mode = exti;

	int32_t encoder_for_round; // encoder trên 1 vòng
	int32_t angle_low; // góc giới hạn trên
	int32_t angle_high; // góc giới hạn dưới


	// ENCODER TIM
	TIM_HandleTypeDef* htim = nullptr;

	int32_t encoder_limit = 0;
	// ENCODER
	GPIO_TypeDef* Port_ChannelA = nullptr;
	uint32_t Pin_ChannelA = 0;
	GPIO_TypeDef* Port_ChannelB = nullptr;
	uint32_t Pin_ChannelB = 0;
	// PID
	uint32_t time = 0;
	int16_t last_error = 0;
	bool state_rotate = true;

public:

	int16_t angle_module = 0; // góc của module
	int32_t encoder_module = 0; //
	int32_t level_count = 0;


	// ENCODER EXTI


	Module_shot(UART_HandleTypeDef* huart,uint8_t address_DC,int32_t encode_for_round,int32_t angle_low,int32_t angle_high,TIM_HandleTypeDef* htim,int32_t encoder_limit)
	: huart(huart),address_DC(address_DC), encoder_for_round(encode_for_round), angle_low(angle_low),angle_high(angle_high),htim(htim),encoder_limit(encoder_limit){
		mode = tim;
	}

	Module_shot(UART_HandleTypeDef* huart,uint8_t address_DC,int32_t encode_for_round,int32_t angle_low,int32_t angle_high,GPIO_TypeDef* Port_ChannelA,uint32_t Pin_ChannelA,GPIO_TypeDef* Port_ChannelB,uint32_t Pin_ChannelB)
	: huart(huart),address_DC(address_DC), encoder_for_round(encode_for_round), angle_low(angle_low),angle_high(angle_high),Port_ChannelA(Port_ChannelA),Pin_ChannelA(Pin_ChannelA),Port_ChannelB(Port_ChannelB),Pin_ChannelB(Pin_ChannelB){
		mode = exti;
	}

	void set_encoder(int32_t value){
		encoder_module = value;
		angle_module = encoder_module * 3600 / encoder_for_round;
		__HAL_TIM_SET_COUNTER(htim,value);
	}


	void stop(){
		Controll_DC_UART(huart, address_DC, LOW, 2);
	}


	void update_position(uint32_t GPIO_Pin ){ // gọi thường không cần ngắt
		if(GPIO_Pin == Pin_ChannelA){

			if(HAL_GPIO_ReadPin(Port_ChannelA, Pin_ChannelA) == HAL_GPIO_ReadPin(Port_ChannelB, Pin_ChannelB)){
				encoder_module++;
			}else{
				encoder_module--;
			}
			//return encoder_module;
		}

		if(GPIO_Pin == Pin_ChannelB){
			if(HAL_GPIO_ReadPin(Port_ChannelB, Pin_ChannelB) == HAL_GPIO_ReadPin(Port_ChannelA, Pin_ChannelA)){
				encoder_module--;
			}else{
				encoder_module++;
			}
		}
	}

			//return encoder_module
		void update_position(){
			int16_t current_count = __HAL_TIM_GET_COUNTER(htim);
			if(current_count > encoder_limit || current_count < -encoder_limit){
				level_count += current_count;
				__HAL_TIM_SET_COUNTER(htim,0);
				current_count = 0;
			}

			encoder_module = level_count + current_count;
			angle_module = encoder_module * 3600 / encoder_for_round;
		}



		void quaythuan(int16_t speed){
			if(speed == 2) speed = 3;
			update_position();
			angle_module = encoder_module * 3600 / encoder_for_round; // Gọi ở loop để giảm tiêu thụ trên ngắt
			if(angle_module <= angle_high && angle_module >= angle_low){
				Controll_DC_UART(huart, address_DC, HIGH, speed);
				state_rotate = true;
			}
			else{
				Controll_DC_UART(huart, address_DC, HIGH, 0);
				state_rotate = false;
			}
		}

		void quaynghich(int16_t speed){
			if(speed == 2) speed = 3;
			update_position();
			angle_module = encoder_module * 3600 / encoder_for_round;
			if(angle_module <= angle_high && angle_module >= angle_low){
				state_rotate = true;
				Controll_DC_UART(huart, address_DC, LOW, speed);
			}
			else{
				state_rotate = false;
				Controll_DC_UART(huart, address_DC, LOW, 0);
			}
		}



	//return angle_module;



	int16_t PID_position_map(int16_t angle_set,int16_t speed){ // theo mapp

		update_position();
		float kp, ki, kd;
		angle_module = encoder_module * 3600 / encoder_for_round;
		int16_t error;

		error = angle_set - get_angle(compass() + angle_module); // quy đổi sang hệ tọa sân đấu

	    if(abs(error) >= 1000){
	      kp = 0.25, ki = 0.01,  kd = 0.07;
	    } else
	    if(abs(error) >= 400){
	      kp = 0.15, ki = 0.06, kd = 0.04;
	    } else{
	      kp = 0.1, ki = 0.03, kd = 0.01;
	    }


		int16_t output = kp * error + ki * (error + last_error) + kd * (error - last_error);
		output = constrain(output,-speed,speed);
		last_error = error;
		if(output >= 0 ){
			quaythuan(output);
		}else{
			quaynghich(abs(output));
		}

//		time = HAL_GetTick();

		if(state_rotate){
			return 0;
		}else{
			return error;
		}

	}

	int16_t PID_position_robot(int16_t angle_set,int16_t speed){ // theo mapp

		update_position();
		angle_module = encoder_module * 3600 / encoder_for_round;
		int16_t error;int16_t output;
		float kp, ki, kd;

		error = angle_set -  angle_module; // quy đổi sang hệ tọa sân đấu

	    if(abs(error) >= 1000){
	      kp = 0.25, ki = 0.01,  kd = 0.07;
	    } else
	    if(abs(error) >= 400){
	      kp = 0.15, ki = 0.06, kd = 0.04;
	    } else{
	      kp = 0.1, ki = 0.03, kd = 0.01;
	    }


		output = kp * error + ki * (error + last_error) + kd * (error - last_error);
		output = constrain(output,-speed,speed);
		last_error = error;
		if(output > 0 ){
			quaythuan(output);
		}else if(output < 0){
			quaynghich(abs(output));
		}else{
			quaythuan(0);
		}

		time = HAL_GetTick();

		if(state_rotate){
			return 0;
		}else{
			return error;
		}
		return output;

	}

};

using Variable = int16_t*; // ?
extern Motor_PID DC[5];
extern Module_shot DC_SHOT[2];


void run_basic();
void Debug(const char* format,...);
void run_dc(uint8_t num_dc,int16_t speed);
void run_dc(uint8_t vec,uint8_t num_dc,uint8_t speed);
int16_t move(int16_t angle_set, int16_t speed_rot,int16_t angle_tt, int16_t speed_tt,uint32_t time_soft,uint8_t delta_speed);
int16_t move_direction(int16_t angle_set, int16_t speed_rot,int16_t angle_tt, int16_t speed_tt,uint32_t time_soft,uint8_t delta_speed);
int16_t move_hehe(int16_t angle_set, int16_t speed_rot,int16_t angle_tt, int16_t speed_tt,uint32_t time_soft,uint8_t delta_speed);
void rot_left(int16_t speed);
void rot_right(int16_t speed);
void stop();
long map(long x, long in_min, long in_max, long out_min, long out_max);
void soft_speed(Variable variable,uint32_t * last_time, int16_t speed,uint32_t time_out,uint8_t delta_speed); // tốc độ và thời gian khởi động
void on_xilanh_ban();
void off_xilanh_ban();
void on_canh_tay();
void off_canh_tay();
void on_xilanh_day();
void off_xilanh_day();
void ban_bong();
void dap_bong();

#endif /* INC_DRIVER_ROBOT_H_ */
