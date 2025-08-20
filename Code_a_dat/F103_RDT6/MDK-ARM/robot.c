#include "robot.h"

uint8_t motor_frame[3] = {0x00, 0x00, 0xff};

void Motor_init(motor *m, GPIO_TypeDef* dir_port, uint16_t dir_pin,TIM_HandleTypeDef* htim, uint32_t tim_channel){
	m->dir_port = dir_port;
	m->dir_pin  = dir_pin;
	m->htim = htim;
	m->channel = tim_channel;
	
	m->forward = forward;
	m->reverse = reverse;
	m->stop = stop;
	
	//HAL_TIM_PWM_Start(htim, tim_channel);
									
}

void forward(void* self, uint16_t speed){
	motor* m = (motor*)self;
		HAL_GPIO_WritePin(m->dir_port, m->dir_pin, GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(m->htim, m->channel, speed);
}

void reverse(void* self, uint16_t speed){
	motor* m = (motor*)self;
		HAL_GPIO_WritePin(m->dir_port, m->dir_pin, GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(m->htim, m->channel, speed);
}

void stop(void* self){
	motor* m = (motor*)self;
	__HAL_TIM_SET_COMPARE(m->htim, m->channel, 0);
}

/*---------------------------Motor control by uart--------------------------------------*/

void Motor_uart_init(motor_uart *m, UART_HandleTypeDef* huart, uint16_t add){
	m->huart = huart;
	m->add = add;
	m->Forward = Forward;
  m->Reverse = Reverse;
} 

void Forward(void* self, int speed){
	motor_uart* m = (motor_uart*)self;
	motor_frame[0] = m->add + 128*1; motor_frame[1] = speed ; 
	HAL_UART_Transmit(m->huart, motor_frame, sizeof(motor_frame), 10);
}

void Reverse(void* self, int speed){
	motor_uart* m = (motor_uart*)self;
	motor_frame[0] = m->add + 128*0; motor_frame[1] = speed ; 
	HAL_UART_Transmit(m->huart, motor_frame, sizeof(motor_frame), 10);
}

