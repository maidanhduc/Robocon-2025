/*
 * PWM_Master.h
 *
 *  Created on: Nov 22, 2024
 *      Author: Admin
 */

#ifndef INC_PWM_MASTER_F103_H_
#define INC_PWM_MASTER_F103_H_

/*
#include "stm32f1xx_hal_conf.h"

#define HAL_TIM_MODULE_ENABLED
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_tim_ex.h"*/
#include "main.h"
#include "unordered_map"
#include "vector"




/*
 * PWM_Master.h
 *
 *  Created on: Nov 14, 2024
 *      Author: Admin
 */

class GPIO_TIM_Mapping {

public:

	GPIO_TypeDef *GPIO;
	uint32_t Pin;
    TIM_TypeDef *TIMx;
    uint32_t TIM_CHANNEL;
    TIM_HandleTypeDef htim;
    bool status;

    GPIO_TIM_Mapping() :GPIO(nullptr), Pin(0),TIMx(nullptr), TIM_CHANNEL(0), status(false) {
    	Error_Handler();
    }
    GPIO_TIM_Mapping(GPIO_TypeDef* GPIO, uint32_t Pin, TIM_TypeDef* TIMx, uint32_t TIM_CHANNEL,bool status)
    : GPIO(GPIO), Pin(Pin), TIMx(TIMx), TIM_CHANNEL(TIM_CHANNEL),status(status) {}

    // Để bảng băm làm việc với class, bạn cần cung cấp operator== và một hàm băm (hash function).
    bool operator==(const GPIO_TIM_Mapping & other) const {
        return (GPIO == other.GPIO && Pin == other.Pin);
    }


};



struct GPIOPinHash {
    size_t operator()(const GPIO_TIM_Mapping& mapping) const {
        // Sử dụng địa chỉ GPIO và Pin để tạo ra một giá trị hash
        return (reinterpret_cast<size_t>(mapping.GPIO) ^ std::hash<uint16_t>()(mapping.Pin) ^ std::hash<bool>()(mapping.status));
    }
};



int Timer_IsEnable(TIM_TypeDef *TIMx);

uint16_t Timer_GetPrescaler(TIM_TypeDef *TIMx);

uint16_t Timer_GetPeriod(TIM_TypeDef *TIMx);

void Enable_RCC_TIMx(TIM_TypeDef* TIMx);


void ENABLE_GPIO_CLOCK(GPIO_TypeDef* GPIO);

void TIMER_PWM_Init(GPIO_TypeDef *GPIO, uint16_t Pin, TIM_HandleTypeDef *htim,
        TIM_TypeDef *TIMx, uint32_t timerChannel, uint16_t Prescaler,
        uint16_t Period, uint16_t Pulse); // cấu hình timer


// Khi cần thêm các ánh xạ
void AddMapping(GPIO_TypeDef* GPIO, uint16_t Pin, TIM_TypeDef* TIMx, uint32_t TIM_CHANNEL,bool status);

// tìm tất cả ánh xạ phù hợp
std::vector<GPIO_TIM_Mapping> FindAllMappings(GPIO_TypeDef* GPIO, uint16_t GPIO_PIN, bool status);

// tìm ánh xạ đơn lẻ
GPIO_TIM_Mapping FindMapping(GPIO_TypeDef* GPIO, uint16_t GPIO_PIN, bool status);

void UpdateMapping(const GPIO_TIM_Mapping& oldMapping, const GPIO_TIM_Mapping& newMapping);


void Register_PWM_GPIO(GPIO_TypeDef *GPIO, uint32_t Pin, uint16_t Prescaler,uint16_t Period, uint16_t Pulse);


void PWM_Write_GPIO(GPIO_TypeDef* Port, uint32_t Pin,uint16_t Pulse);

void Add_Map_STM32F103xx();


#endif /* INC_PWM_MASTER_F103_H_ */
