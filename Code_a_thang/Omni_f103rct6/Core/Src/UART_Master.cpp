/*
 * UART_Master.cpp
 *
 *  Created on: Feb 21, 2025
 *      Author: Admin
 */


#include "UART_Master.h"
#include "string.h"
#include "TIMER_GPIO_ATTACH.h"


Info_position_robot my_info_pt(10.0,5.0,0,false);
Info_position_robot frd_info_pt(0,0,0,false);



//////////////// ++++++++++++	UART3	+++++++++++++ ///////////////



UART3_stages uart3_stages = STAGE_NO_SEND;

char buffer_dg[250];
//uint8_t buffer_map[12]; // 12 byte

char start_dg = '(';
char end_dg = ')';

char start_map = '!';

bool st_data_dg = true;
bool st_data_recv_map = true;
bool flag_uart3 = false;

void Debug(const char* format,...){ // uart 3

	//static uint32_t time = 0;


	if(uart3_stages == STAGE_NO_SEND && !flag_uart3 && st_data_dg){

		va_list args;
		va_start(args,format); // chuyển các kí tự thành 1 mảng

		vsnprintf(buffer_dg,sizeof(buffer_dg),format,args);
		va_end(args);

		/*size_t len = strlen(buffer_dg);
		char buff[ len + 3];
		buff[0] = '(';
		buff[len + 1] = ')';
		buff[len + 2] = '\0';
		//memcpy((uint8_t*)&buff[1],(uint8_t*)&buffer_dg,strlen(buffer_dg));
		strcpy(&buff[1], buffer_dg);
		if(HAL_UART_Transmit_IT(&huart3, (uint8_t*)buff, strlen(buff)) != HAL_OK){
			Error_Handler();
		}

		*/

		HAL_UART_Transmit_IT(&huart3, (uint8_t*)&start_dg, 1);

		st_data_dg = false;
		uart3_stages = STAGE_DEBUG;


		/*HAL_UART_Transmit(&huart3, (uint8_t*)&start_dg, 1, 10);
		HAL_UART_Transmit(&huart3, (uint8_t*)&buffer_dg, strlen(buffer_dg), 100);
		HAL_UART_Transmit(&huart3, (uint8_t*)&end_dg, 1, 10);*/


	}



}



void Data_map(){ // uart3

	static uint32_t timee = 0;
	if( flag_uart3 && st_data_dg){

			uint8_t buffer[13];
			buffer[0] = '!';
			memcpy((uint8_t*)&buffer[1],(uint8_t*)&my_info_pt,12);

			if(HAL_UART_Transmit(&huart3, (uint8_t*)&buffer, sizeof(buffer),100) != HAL_OK){
				Error_Handler(); // dấu !
			}

			flag_uart3 = false;

	}else{
		if(HAL_GetTick() - timee >= 300){
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
			flag_uart3 = true;
			timee = HAL_GetTick();
	}



		/*HAL_UART_Transmit_IT(&huart3, (uint8_t*)&start_map, 1);

		//st_data_recv_map = false;*/

		//HAL_UART_Transmit(&huart3, (uint8_t*)&start_map, 1, 10);
		//HAL_UART_Transmit(&huart3, (uint8_t*)&my_info_pt, 12, 100);
		//uart3_stages = STAGE_MAP;
		//HAL_UART_Transmit_IT(&huart3, (uint8_t*)&start_map, 1);


		//HAL_UART_Transmit(&huart3, (uint8_t*)&my_info_pt, 12,100);
		//st_data_recv_map = false;

	}

}

///////////////// ************ UART 4	********************//////////////

UART4_stages uart4_stages = STAGE_NO_READ;
uint16_t button;
int16_t LX,LY,RX,RY;

void ps3(){

	static uint32_t time = 0;

	if(uart4_stages == STAGE_NO_READ){
		char start = 'd';
		uart4_stages = STAGE_READ_BUTTON;
		time = HAL_GetTick();
		HAL_UART_Transmit_IT(&huart4, (uint8_t *)&start, 1);
	}else{
		if(HAL_GetTick() - time >= 1500){
			uart4_stages = STAGE_NO_READ;
		}
	}

}

	/*******************************/

///////////**************	UART 5	***************//////////


UART5_stages uart5_stages = STAGE_NO_READ_COMPASS;
uint8_t buffer_angle[2];
int16_t value_compass = 0;

int16_t compass(){
	static uint32_t time = 0;

	if(uart5_stages == STAGE_NO_READ_COMPASS){
		char data = 'z';
		uart5_stages = STAGE_READ_START;
		HAL_UART_Transmit_IT(&huart5,(uint8_t *)&data, 1);
	}else{
		if(HAL_GetTick() - time >= 3000){
			uart5_stages = STAGE_NO_READ_COMPASS;
		}
	}

	return value_compass;
}


void reset_compass(){

	if(uart5_stages == STAGE_NO_READ_COMPASS){
		char reset = 'a';
		uart5_stages = STAGE_RESET_COMPASS;
		HAL_UART_Transmit_IT(&huart5, (uint8_t*)&reset, 1);
	}

}


///////////////////// INTERRUPT UART /////////////////
// 	CALL BACK TX //
bool flag;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){

	/*************** UART 3 *******************/
	if(huart->Instance == huart3.Instance){


		switch(uart3_stages){

		case STAGE_MAP:
			flag = false;
			uart3_stages = STAGE_NO_SEND;
			HAL_UART_Transmit_IT(&huart3,(uint8_t*)&my_info_pt, 12);
			//Error_Handler();
			break;

		case STAGE_DEBUG:
			uart3_stages = STAGE_END_DEBUG;
			//HAL_UART_Receive_IT(huart, (uint8_t*)&st_data_dg, 1);
			HAL_UART_Transmit_IT(&huart3,(uint8_t*)buffer_dg, strlen(buffer_dg));
			break;

		case STAGE_END_DEBUG:
			flag = true;
			uart3_stages = STAGE_NO_SEND;
			HAL_UART_Transmit_IT(&huart3, (uint8_t*)&end_dg, 1);
			break;

		case STAGE_NO_SEND:
			if(flag){
				HAL_UART_Receive_IT(&huart3, (uint8_t*)&st_data_dg, 1);
			}else{
				HAL_UART_Receive_IT(&huart3,(uint8_t*)&frd_info_pt, 12);
			}
			break;
		}

	}

	/**********************************************/



	/******************	UART 4	********************/
	if(huart->Instance == huart4.Instance){

		HAL_UART_Receive_IT(huart, (uint8_t*)&button, 2);

	}

	/**********************************************/



	/************	UART 5	********************/

	if(huart->Instance == huart5.Instance){
		if(uart5_stages == STAGE_READ_START){
			HAL_UART_Receive_IT(&huart5, (uint8_t*)&buffer_angle[0],1);
			uart5_stages = STAGE_BYTE_H;
		}
		if(uart5_stages == STAGE_RESET_COMPASS){
			uart5_stages = STAGE_NO_READ_COMPASS;
		}
	}

	/*++++++++++++++++++++++++++++++++*/




	/************	UART 2	************/

	if(huart->Instance == huart2.Instance){

	}

	/**********************/

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	//***********	UART 3	************//
	if(huart->Instance == huart3.Instance){
		st_data_dg = true;

	}



	//**********	UART4	***************//
	if(huart->Instance == huart4.Instance){
		switch(uart4_stages){

		case STAGE_READ_BUTTON:
			uart4_stages = STAGE_READ_LX;
			HAL_UART_Receive_IT(huart, (uint8_t*)&LX, sizeof(LX));
			break;

		case STAGE_READ_LX:
			uart4_stages = STAGE_READ_LY;
			HAL_UART_Receive_IT(huart, (uint8_t*)&LY, sizeof(LY));
			break;

		case STAGE_READ_LY:
			uart4_stages = STAGE_READ_RX;
			HAL_UART_Receive_IT(huart, (uint8_t*)&RX, sizeof(RX));
			break;

		case STAGE_READ_RX:
			uart4_stages = STAGE_READ_RY;
			HAL_UART_Receive_IT(huart, (uint8_t*)&RY, sizeof(RY));
			break;

		case STAGE_READ_RY:
			uart4_stages = STAGE_NO_READ;
			break;

		case STAGE_NO_READ:
			st_data_dg = true;
			break;

		}

	}

	//************	UART5	************//
	if(huart->Instance == huart5.Instance){
		if(uart5_stages == STAGE_BYTE_H){
			HAL_UART_Receive_IT(&huart5, (uint8_t*)&buffer_angle[1], 1);
			uart5_stages = STAGE_BYTE_L;
		}
		if(uart5_stages == STAGE_BYTE_L){
			value_compass = (int16_t) ((buffer_angle[0] << 8) | (buffer_angle[1]));
			uart5_stages = STAGE_NO_READ_COMPASS;
			}
		}



}



