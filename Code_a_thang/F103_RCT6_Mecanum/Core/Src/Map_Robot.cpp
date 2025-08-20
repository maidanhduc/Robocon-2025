/*
 * Map_Robot.cpp
 *
 *  Created on: Apr 16, 2025
 *      Author: Admin
 */


#include "Map_Robot.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

int32_t encoder_x; // giá trị encoder trục x
int32_t encoder_y; // giá trị encoder trục y
int32_t last_encoder_x;
int32_t last_encoder_y;
int32_t level_count_x;
int32_t level_count_y;
int16_t last_compass; // tính toán góc trước và sau để quy đổi

double robot_en_x;
double robot_en_y;



int32_t get_encoder(int32_t * value,int32_t * level_count,TIM_HandleTypeDef * htim,int16_t encoder_limit){
	int16_t count_current = __HAL_TIM_GET_COUNTER(htim);

	if((*value) > encoder_limit || (*value) < - encoder_limit){
		(*level_count) += count_current;
		__HAL_TIM_SET_COUNTER(htim,0);
		count_current = 0;
	}

	(*value) = (*level_count) + count_current;

	return (*value);
}


void update_position_encoder(TIM_HandleTypeDef * htim_en_x,TIM_HandleTypeDef * htim_en_y){
//	last_encoder_x = encoder_x;
//	last_encoder_y = encoder_y;
	encoder_x = get_encoder(&encoder_x, &level_count_x,htim_en_x, 10000);
	encoder_y = get_encoder(&encoder_y, &level_count_y,htim_en_y, 10000);

}


// coi robot chạy tiến và sang phải các giá trị encoder dương nghĩa là en càng tăng góc càng âm
int32_t get_encoder_x(){
	return encoder_x;
}

int32_t get_encoder_y(){
	return encoder_y;
}

void update_popsition_map_from_encoder(TIM_HandleTypeDef * htim_en_x,TIM_HandleTypeDef * htim_en_y){
	update_position_encoder(htim_en_x,htim_en_y);
	int16_t angle = compass();
	double encoder_tick_x = get_encoder_x() - last_encoder_x;
	double encoder_tick_y = get_encoder_y() - last_encoder_y;

	double encoder_x_yaw = encoder_for_round_robot_to_x * (angle - last_compass) / 3600;
	double encoder_y_yaw = encoder_for_round_robot_to_y * (angle - last_compass) / 3600;

	robot_en_x += (encoder_tick_x + encoder_x_yaw) * radius_encoder / tick_encoder_for_round_en;
	robot_en_y += (encoder_tick_y + encoder_y_yaw) * radius_encoder / tick_encoder_for_round_en;

}


int32_t get_robot_x_encoder(){
	int32_t value = (int32_t)(robot_en_x);
	return value;
}

int32_t get_robot_y_encoder(){
	int32_t value = (int32_t)(robot_en_y);
	return value;
}
