/*
 * UART_Master.cpp
 *
 *  Created on: Apr 13, 2025
 *      Author: Admin
 */

#include "UART_Master.h"
#include "string.h"




#define UART_BUFFER_SIZE 8
//
uint8_t uart_tx_buffer[UART_BUFFER_SIZE][3];
uint8_t uart_tx_head = 0;
uint8_t uart_tx_tail = 0;
bool flag_send_speed = true;

void Controll_DC_UART(UART_HandleTypeDef* huart, uint8_t add_DC, uint8_t dir, uint8_t pwm_DC) {
	uint8_t next_head = (uart_tx_head + 1) % UART_BUFFER_SIZE;

	if (next_head == uart_tx_tail) return;

	uart_tx_buffer[uart_tx_head][0] = add_DC + dir * 128;
	uart_tx_buffer[uart_tx_head][1] = pwm_DC;
	uart_tx_buffer[uart_tx_head][2] = 0xFF;
	uart_tx_head = next_head;

	// Nếu không đang gửi, bắt đầu truyền
	if (flag_send_speed) {
		flag_send_speed = false;
		HAL_UART_Transmit_DMA(huart, uart_tx_buffer[uart_tx_tail], 3);
	}
}


//	  uint8_t buffer_speed[3] = {0,0,0xFF};
//
//	  buffer_speed[0] = 0x03 + 1 * 128; buffer_speed[1] = 100;
//	  HAL_UART_Transmit(&huart4, buffer_speed, 3, 10);
//	  buffer_speed[0] = 0x04 + 0 * 128; buffer_speed[1] = 100;
//	  HAL_UART_Transmit(&huart4, buffer_speed, 3, 10);
//	  buffer_speed[0] = 0x05 + 0 * 128; buffer_speed[1] = 100;
//	  HAL_UART_Transmit(&huart4, buffer_speed, 3, 10);
//	  buffer_speed[0] = 0x06 + 1 * 128; buffer_speed[1] = 100;
//	  HAL_UART_Transmit(&huart4, buffer_speed, 3, 10);


UART5_stages uart5_stages = STAGE_NO_READ_COMPASS;
int16_t value_compass = 0;
uint8_t Buffer[2]; // Nhận giá trị góc gửi về
bool stage_send = true;
int16_t delta_value_compass;
int16_t compass(){
	static uint32_t time = 0;

	if(uart5_stages == STAGE_NO_READ_COMPASS){
		char data = 'z';
		if(HAL_UART_Transmit_IT(&huart5,(uint8_t *)&data, 1) == HAL_OK){
			time = HAL_GetTick();
			uart5_stages = STAGE_READ_START;
		}
	}else{
		if(HAL_GetTick() - time >= 1000){
			uart5_stages = STAGE_NO_READ_COMPASS;
		}
	}
	return value_compass - delta_value_compass;
}

static bool st_reset_angle = true;
void reset_compass(){
	//char flag_reset = 'a';
//	if(st_reset_angle){
//
//		uart5_stages = STAGE_RESET_COMPASS;
//		HAL_UART_Transmit_IT(&huart5, (uint8_t*)&flag_reset, 1);
//		st_reset_angle = false;
//	}
	delta_value_compass = value_compass;


//	HAL_UART_Transmit_IT(&huart5, (uint8_t*)&flag_reset, 1);
//	uart5_stages = STAGE_RESET_COMPASS;
}



char buffer_dg[255];
char buffer[258];
uint16_t len_uart3;
void Debug(const char* format,...){ // uart 1

	//static uint32_t time = 0;

	va_list args;
	va_start(args,format); // chuyển các kí tự thành 1 mảng

	vsnprintf(buffer_dg,sizeof(buffer_dg),format,args);
	va_end(args);

	size_t len = strlen(buffer_dg);
	if(len > 0 && len < 255){
		buffer[1] = '(';
		buffer[len + 2] = ')';
		//buffer[len + 2] = '\0';
		memcpy((uint8_t*)&buffer[2],buffer_dg,len);
		len_uart3 = len + 3;

//		if(HAL_UART_Transmit_DMA(&huart3, (uint8_t*)&buffer, len + 2) == HAL_OK){
//
//		}
	}


}


UART3_stages uart3_stages = STAGE_NO_READ;
uint16_t button;
char flag_esp32 = '0';
int16_t LX,LY,RX,RY;
bool st_debug = false;
void ps4(){

	static uint32_t time = 0;

	if(uart3_stages == STAGE_NO_READ && flag_esp32 == '0'){
		char start = '!';
		uart3_stages = STAGE_READ_BUTTON;
		time = HAL_GetTick();
		if(len_uart3 == 0){
			HAL_UART_Transmit_DMA(&huart3, (uint8_t *)&start, 1);
		}else{
			buffer[0] = start;
			HAL_UART_Transmit_DMA(&huart3, (uint8_t*)&buffer, len_uart3);
			len_uart3 = 0;
		}
	}else{
		if(HAL_GetTick() - time >= 1500){
			uart3_stages = STAGE_NO_READ;
			flag_esp32 = '0';
		}
	}

}








void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	//////////////*********** UART4  *******************///////
	if(huart->Instance == huart4.Instance){ // 4

		//flag_send_speed = true;
	    uart_tx_tail = (uart_tx_tail + 1) % UART_BUFFER_SIZE;

	    if (uart_tx_tail != uart_tx_head) {
	        HAL_UART_Transmit_DMA(huart, uart_tx_buffer[uart_tx_tail], 3);
	    } else {
	        flag_send_speed = true;
	    }
		//flag_send_speed = true;
	}

	///////************** UART5  ******************//////////
	if(huart->Instance == huart5.Instance){
		if(uart5_stages == STAGE_RESET_COMPASS){
			st_reset_angle = true;
			uart5_stages = STAGE_NO_READ_COMPASS;
		}else{
			HAL_UART_Receive_IT(huart, (uint8_t*)&Buffer, 2);
		}

	}

	if(huart->Instance == huart3.Instance){
		if(uart3_stages == STAGE_READ_BUTTON){
			HAL_UART_Receive_DMA(huart, (uint8_t*)&flag_esp32, 1);
		}else if(uart3_stages == STAGE_DEBUG){
			uart3_stages = STAGE_NO_READ;
		}

	}
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if(huart->Instance == huart5.Instance){

		value_compass = (int16_t)((Buffer[0] << 8) | Buffer[1]);
		uart5_stages = STAGE_NO_READ_COMPASS;
	}


	if(huart->Instance == huart3.Instance){

			if(flag_esp32 != '{'){
				HAL_UART_Receive_DMA(&huart3, (uint8_t*)&flag_esp32, 1);
				return;
			}else{
				switch(uart3_stages){

				case STAGE_READ_BUTTON:
					uart3_stages = STAGE_READ_LX;
					HAL_UART_Receive_DMA(huart, (uint8_t*)&button, 2);
					break;

				case STAGE_READ_LX:
					uart3_stages = STAGE_READ_LY;
					HAL_UART_Receive_DMA(huart, (uint8_t*)&LX, 2);
					break;

				case STAGE_READ_LY:
					uart3_stages = STAGE_READ_RX;
					HAL_UART_Receive_DMA(huart, (uint8_t*)&LY, 2);
					break;

				case STAGE_READ_RX:
					uart3_stages = STAGE_READ_RY;
					HAL_UART_Receive_DMA(huart, (uint8_t*)&RX, 2);
					break;

				case STAGE_READ_RY:
					uart3_stages = STAGE_NO_READ;
					HAL_UART_Receive_DMA(huart, (uint8_t*)&RY, 2);
					break;

				case STAGE_NO_READ:
					//st_data_dg = true;
					flag_esp32 = '0';
					break;
				case STAGE_DEBUG: break;

				}
			}



		}


}
