/*
 * TIMER_ATTACH.cpp
 *
 *  Created on: Mar 2, 2025
 *      Author: Admin
 */

#include <TIMER_GPIO_ATTACH.h>

volatile uint32_t tick = 0;

uint32_t millis(){
	return tick;
}
///////////	+++++++++ CALL BACK	+++++++++++++////////

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if(htim->Instance == TIM6){ /// ngat 1ms
		tick++;
	}

	if(htim->Instance == TIM7){ // ngắt sau 10ms
		///// update map
	}

}



