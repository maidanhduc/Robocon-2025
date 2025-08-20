/*
 * Driver_Robot.h
 *
 *  Created on: Mar 4, 2025
 *      Author: Admin
 */

#ifndef INC_DRIVER_ROBOT_H_
#define INC_DRIVER_ROBOT_H_

#include "main.h"
#include "PWM_Master_f103.h"
#include "GPIO_Master.h"
#include "math.h"
#include "stdlib.h"
#include "UART_Master.h"

int32_t constrains(int32_t value, int32_t down, int32_t up );

double fConstrain(double value, double down, double up);


class Motor_PID{
protected:

    static constexpr uint8_t PWM = 1;
    static constexpr uint8_t UART = 0;
	// PWM
	GPIO_TypeDef* Port_Dir = nullptr;
	uint32_t Pin_Dir = 0;

	GPIO_TypeDef* Port_Pwm = nullptr;
	uint32_t Pin_Pwm = 0;

	// UART
	UART_HandleTypeDef* huart;
	uint8_t mode = true;
	uint8_t address_DC = 0;


public:

	int16_t angle[2];
	Motor_PID(GPIO_TypeDef* Port_Dir, uint32_t Pin_Dir , GPIO_TypeDef* Port_Pwm, uint32_t Pin_Pwm,int16_t angle_val):
		Port_Dir(Port_Dir),Pin_Dir(Pin_Dir),Port_Pwm(Port_Pwm),Pin_Pwm(Pin_Pwm){
		static bool st_add_map = true;
	    if(st_add_map){
	    	Add_Map_STM32F103xx();
	    	st_add_map = false;
	    }

		angle[0] = angle_val;
	    mode = 1;
		GPIO_OUT_Init(Port_Dir, Pin_Dir, GPIO_SPEED_FREQ_HIGH);
		Register_PWM_GPIO(Port_Pwm, Pin_Pwm, 55, 299, 0);
		HAL_GPIO_WritePin(Port_Dir, Pin_Dir, GPIO_PIN_SET);
		if(angle[0] > 0) angle[1] = angle[0] - 900;
		if(angle[0] < 0) angle[1] = angle[0] + 900;
	}

	Motor_PID(UART_HandleTypeDef* huart,uint8_t address_DC) : huart(huart), address_DC(address_DC){
		mode = false;
	}

	void quaythuan(int16_t speed){
		switch(mode){
			case PWM: HAL_GPIO_WritePin(Port_Dir, Pin_Dir, GPIO_PIN_RESET);
				   	   PWM_Write_GPIO(Port_Pwm, Pin_Pwm,299 -  speed);
				   	   break;
			case UART: Controll_DC_UART(huart, address_DC, LOW, (uint8_t)(speed));
						break;
		}

	}

	void quaynghich(int16_t speed){
		switch(mode){
			case PWM: HAL_GPIO_WritePin(Port_Dir, Pin_Dir, GPIO_PIN_SET);
				   	   PWM_Write_GPIO(Port_Pwm, Pin_Pwm,299 -  speed);
				   	   break;
			case UART: Controll_DC_UART(huart, address_DC, HIGH, (uint8_t)(speed));
						break;
		}
	}

	void stop(){
		switch(mode){

		case PWM: quaythuan(2);
				  break;
		case UART: Controll_DC_UART(huart, address_DC, LOW, 0x02);
					break;
		}


	}
};

void run_basic();
int16_t move(int16_t angle_set, int16_t speed_rot,int16_t angle_tt, int16_t speed_tt,uint32_t time_soft); // ver 2
int16_t move_direction(int16_t angle_set, int16_t speed_rot,int16_t angle_tt, int16_t speed_tt,uint32_t time_soft); // ver 1
void rot_left(int16_t speed);
void rot_right(int16_t speed);
void stop();

#endif /* INC_DRIVER_ROBOT_H_ */
