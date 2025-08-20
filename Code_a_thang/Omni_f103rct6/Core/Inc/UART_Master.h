/*
 * UART_Master.h
 *
 *  Created on: Feb 21, 2025
 *      Author: Admin
 */

#ifndef INC_UART_MASTER_H_
#define INC_UART_MASTER_H_

#include "main.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

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


extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

// lưu thông tin map
class Info_position_robot{
public:
    float x;
    float y;
    int16_t angle;
    bool st ;
	// 1 byte cua struct

	Info_position_robot(float x,float y,int16_t angle,bool st): x(x),y(y),angle(angle),st(st){}
};

extern Info_position_robot my_info_pt;
extern Info_position_robot frd_info_pt;


enum  UART3_stages{ // uart5 send map and debug

	// các giai đoạn gửi data gửi của uart 5
	STAGE_NO_SEND,
	STAGE_MAP,
	STAGE_DEBUG,
	STAGE_END_DEBUG

};

extern UART3_stages uart3_stages;


/////////////////////////////////////////////

enum UART4_stages{ // tay cầm

	STAGE_NO_READ,
	STAGE_READ_BUTTON,
	STAGE_READ_LX,
	STAGE_READ_LY,
	STAGE_READ_RX,
	STAGE_READ_RY,


};

extern UART4_stages uart4_stages;
extern uint16_t button;
extern int16_t LX,LY,RX,RY;


/***************************************/

enum UART5_stages{
	STAGE_NO_READ_COMPASS,
	STAGE_READ_START,
	STAGE_BYTE_H,
	STAGE_BYTE_L,
	STAGE_RESET_COMPASS
};










void Debug(const char* format,...); // uart3

int16_t compass();					// uart5

void reset_compass();

void ps3();							// uart4  gui ki tu 'd'

void Data_map();					// uart3

void Read_point_ball();				// uart2


#endif /* INC_UART_MASTER_H_ */
