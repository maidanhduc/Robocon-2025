/*
 * UART_Data.h
 *
 *  Created on: Nov 23, 2024
 *      Author: Admin
 */

#ifndef INC_UART_DATA_H_
#define INC_UART_DATA_H_

#include "main.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

//extern UART_HandleTypeDef huart5; /// để debug qua esp32
//extern UART_HandleTypeDef huart4; /// uart để nhận giá trị góc từ stm32



void Debug_stm32(const char *format, ...);

///// --------------------- ĐỌC GIÁ TRỊ GÓC -------------------- ///
//		ĐỌC 		//

int16_t Compass();

void update_Angle_Robot();

//// 			RESET			////

void Reset_Angle();


//----------------- PS4 ----------------------////

extern uint16_t button;
extern int16_t joy_LX, joy_LY, joy_RX, joy_RY;


#define button_up 			((0x01 	& 	button) != 0)  // 0
#define button_down			((0x02	&	button) != 0)  // 1
#define button_right		((0x04	&	button) != 0)  // 2
#define button_left			((0x08	&	button) != 0)  // 3
#define button_R1			((0x20	&	button) != 0)  // 4
#define button_L1			((0x10	&	button) != 0)  // 5
#define button_L2			((0x40	&	button) != 0)  // 6
#define	button_R2			((0x80	&	button) != 0)  // 7
#define button_square		((0x400	&	button) != 0)  // 8
#define button_triangle		((0x800	&	button) != 0)  // 9
#define button_circle		((0x200	&	button) != 0)  // 10
#define button_cross		((0x100	&	button) != 0)  // 11
#define button_share		((0x2000 &	button) != 0) // 12
#define button_options		((0x4000 &	button) != 0)  // 13
#define button_L3			((0x1000 &	button) != 0)  // 14
#define button_R3			((0x8000 &	button) != 0) // 15


void Ps4();

#endif /* INC_UART_DATA_H_ */
