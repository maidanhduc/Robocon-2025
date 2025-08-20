/*by_pidat*/
#ifndef ROBOT_H
#define ROBOT_H

#include "main.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_gpio.h"
#include "stdio.h"
#include "stdlib.h" 

extern uint8_t motor_frame[3];



typedef struct{
	TIM_HandleTypeDef* htim;
	uint32_t channel;
	
	GPIO_TypeDef* dir_port;
	uint16_t dir_pin;
	
	
	void (*forward)(void* self, uint16_t speed);
	void (*reverse)(void* self, uint16_t speed);
	void (*stop)(void* self);
	
}motor;

typedef struct{
	UART_HandleTypeDef* huart;
	uint16_t add;
	
	void(*Forward)(void* self, int speed);
	void(*Reverse)(void* self, int speed);
	
}motor_uart;

/*==pid motor=*/
void Motor_init(motor *m, GPIO_TypeDef* dir_port, uint16_t dir_pin,
              	TIM_HandleTypeDef* htim, uint32_t tim_channel);

void forward(void* self, uint16_t speed);
void reverse(void* self, uint16_t speed);
void stop(void* self);

/*==pid motor control by uart=*/
void Motor_uart_init(motor_uart *m, UART_HandleTypeDef* huart, uint16_t add);

void Forward(void* self, int speed);
void Reverse(void* self, int speed);


#endif