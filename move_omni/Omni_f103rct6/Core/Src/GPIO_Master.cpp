/*
 * GPIO_Master.cpp
 *
 *  Created on: Nov 24, 2024
 *      Author: Admin
 */

#include "GPIO_Master.h"

void ENABLE_GPIO_CLK(GPIO_TypeDef* GPIO){
	if(GPIO == GPIOA)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();

	}else if(GPIO == GPIOB)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();

	}else if(GPIO == GPIOC){

		__HAL_RCC_GPIOC_CLK_ENABLE();

	}else if(GPIO == GPIOD)
	{
		__HAL_RCC_GPIOD_CLK_ENABLE();

	}
}

void GPIO_OUT_Init(GPIO_TypeDef* GPIO,uint32_t GPIO_PIN,uint32_t SPEED_FREQ){

	ENABLE_GPIO_CLK(GPIO);

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = SPEED_FREQ;

	HAL_GPIO_Init(GPIO, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIO, GPIO_PIN, GPIO_PIN_RESET);

}

void GPIO_IN_Init(GPIO_TypeDef* GPIO,uint32_t GPIO_PIN,uint32_t ModePull){

	ENABLE_GPIO_CLK(GPIO);

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = ModePull;

	HAL_GPIO_Init(GPIO, &GPIO_InitStruct);

}

/*
IRQn_Type Get_IRQn_Type_EXTI(uint32_t GPIO_PIN){
	if(GPIO_PIN == GPIO_PIN_0){
		return EXTI0_IRQn;
	}
	if(GPIO_PIN == GPIO_PIN_1){
		return EXTI1_IRQn;
	}
	if(GPIO_PIN == GPIO_PIN_2){
		return EXTI2_IRQn;
	}
	if(GPIO_PIN == GPIO_PIN_3){
		return EXTI3_IRQn;
	}
	if(GPIO_PIN == GPIO_PIN_4){
		return EXTI4_IRQn;
	}
	if(GPIO_PIN == GPIO_PIN_5 || GPIO_PIN == GPIO_PIN_6 || GPIO_PIN == GPIO_PIN_7 || GPIO_PIN == GPIO_PIN_8 || GPIO_PIN == GPIO_PIN_9){
		return EXTI9_5_IRQn;
	}
	if(GPIO_PIN == GPIO_PIN_10 || GPIO_PIN == GPIO_PIN_11 || GPIO_PIN == GPIO_PIN_12 || GPIO_PIN == GPIO_PIN_13 || GPIO_PIN == GPIO_PIN_14 || GPIO_PIN == GPIO_PIN_15){
		return EXTI15_10_IRQn;
	}

	return EXTI0_IRQn;
}


void GPIO_ATTACH_Init(GPIO_TypeDef* GPIO,uint32_t GPIO_PIN,uint32_t ModePull,uint32_t ModeRISING_ORAND_FALLING,uint32_t PreemptPriority, uint32_t SubPriority){ /// PreemptPriority, uint32_t SubPriority là ưu tiên ngắt chính và ưu tiên ngắt phụ

	ENABLE_GPIO_CLK(GPIO);
	IRQn_Type IRQn = Get_IRQn_Type_EXTI(GPIO_PIN);
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN;
	GPIO_InitStruct.Mode = ModeRISING_ORAND_FALLING;
	GPIO_InitStruct.Pull = ModePull;
	HAL_GPIO_Init(GPIO, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(IRQn,PreemptPriority, SubPriority);
	HAL_NVIC_EnableIRQ(IRQn);

}*/
