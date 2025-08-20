/*
 * UART_Data.cpp
 *
 *  Created on: Nov 24, 2024
 *      Author: Admin
 */

#include "UART_Data.h"


UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

/*void Debug_stm32(const char *format, ...) {
    char buffer[200]; // Kích thước buffer có thể tùy chỉnh để phù hợp với nhu cầu
    char end = '\n';

    va_list args;
    va_start(args, format);

    // Format chuỗi đầu vào với các tham số
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Gửi chuỗi qua UART
    HAL_UART_Transmit(&huart5, (uint8_t *)buffer, strlen(buffer),10);
    HAL_UART_Transmit(&huart5,(uint8_t*)&end ,1,10);
}*/


extern uint8_t Buffer[2];
extern bool status_readAngle;
int16_t value;
int16_t Compass(){

	static uint32_t time = 0;
	if(status_readAngle){
		char data = 'z';
		HAL_UART_Receive_IT(&huart5, (uint8_t *) &Buffer[0], 1);
		HAL_UART_Transmit(&huart5, (uint8_t *)&data ,1,10);
		time = HAL_GetTick();

		status_readAngle = false;
	}else{
		if(HAL_GetTick() - time >= 1000){
			Debug_stm32("hehe");
			status_readAngle = true;
		}
	}


	return value;
}

void update_Angle_Robot(){
	char data = 'z';
	HAL_UART_Transmit(&huart5, (uint8_t *)&data, 1, 10);

	HAL_UART_Receive_IT(&huart5,(uint8_t *)&Buffer[0] ,1);
}

//// 			RESET			////

void Reset_Angle(){
	char data = 'a';
	HAL_UART_Transmit_IT(&huart5, (uint8_t *)&data, 1);
}


//----------------- ĐỌC GIÁ TRỊ TAY CẦM PS4 -----------------///
/*
 * gửi kí tự 'd' đến esp32 để esp32 gửi data đến board chính như sau
 * uint16_t đầu là những nút bấm basic sau đó int16_t : joystick Lx Ly Rx Ry
 *
 * */

bool status_Read = true;
//extern uint16_t button;
//extern uint8_t buffer_ps4[10];
//extern int16_t joy_LX, joy_LY, joy_RX, joy_RY;
void Ps4(){
	static uint32_t time = 0;
	if(status_Read){
		char data = 'd';
		HAL_UART_Transmit_IT(&huart4, (uint8_t * )&data, 1);
		time = HAL_GetTick();
		status_Read = false;
	}else{

		if(HAL_GetTick() - time >= 1000){
			status_Read = true;
		}
	}

//	Debug_stm32("button : %u  LX  :  %d   LY  :  %d    RX  :  %d  RY   :  %d \r\n",button,joy_LX,joy_LY,joy_RX,joy_RY);
}
