/*
 * Map_Robot.h
 *
 *  Created on: Apr 16, 2025
 *      Author: Admin
 */

#ifndef INC_MAP_ROBOT_H_
#define INC_MAP_ROBOT_H_

#include "main.h"
#include "Driver_Robot.h"

/*
 * 							   +x
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * y-							0						y+
 *
 *
 * */





#define encoder_for_round_robot_to_x	3000 // encoder đếm được khi robot quay 1 vòng
#define encoder_for_round_robot_to_y	3000 //

#define radius_encoder  58 // đơn vị milimet
#define tick_encoder_for_round_en	300
extern int32_t encoder_x; // giá trị encoder trục x
extern int32_t encoder_y; // giá trị encoder trục y
extern int32_t last_encoder_x;
extern int32_t last_encoder_y;
extern int32_t level_count_x;
extern int32_t level_count_y;
extern int16_t last_compass; // tính toán góc trước và sau để quy đổi

int32_t get_encoder(int32_t * value,int32_t * level_count,TIM_HandleTypeDef * htim,int16_t encoder_limit);
int32_t get_encoder_x();
int32_t get_encoder_y();

int32_t get_robot_x_encoder();
int32_t get_robot_y_encoder();

void update_position_encoder();
void update_popsition_map_from_encoder();
void update_popsition_map_from_sick();
void update_popsition_map();
int16_t run_map(int16_t x, int16_t y,int16_t angle_rot,int16_t speed_rot,int16_t speed_tt,uint32_t time_soft);



#endif /* INC_MAP_ROBOT_H_ */
