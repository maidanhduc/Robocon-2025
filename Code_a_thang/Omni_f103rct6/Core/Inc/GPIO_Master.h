/*
 * GPIO_Master.h
 *
 *  Created on: Nov 22, 2024
 *      Author: Admin
 */

#ifndef INC_GPIO_MASTER_H_
#define INC_GPIO_MASTER_H_

#include "main.h"

/*
#define  GPIO_SPEED_FREQ_LOW         0x00000000U  //!< IO works at 2 MHz, please refer to the product datasheet
#define  GPIO_SPEED_FREQ_MEDIUM      0x00000001U  //!< range 12,5 MHz to 50 MHz, please refer to the product datasheet
#define  GPIO_SPEED_FREQ_HIGH        0x00000002U  //!< range 25 MHz to 100 MHz, please refer to the product datasheet
#define  GPIO_SPEED_FREQ_VERY_HIGH   0x00000003U

#define  GPIO_NOPULL        0x00000000U   //!< No Pull-up or Pull-down activation
#define  GPIO_PULLUP        0x00000001U   //!< Pull-up activation
#define  GPIO_PULLDOWN      0x00000002U

 */

void ENABLE_GPIO_CLK(GPIO_TypeDef* GPIO);

void GPIO_OUT_Init(GPIO_TypeDef* GPIO,uint32_t GPIO_PIN,uint32_t SPEED_FREQ);

void GPIO_IN_Init(GPIO_TypeDef* GPIO,uint32_t GPIO_PIN,uint32_t ModePull);

IRQn_Type Get_IRQn_Type_EXTI(uint32_t GPIO_PIN);

void GPIO_ATTACH_Init(GPIO_TypeDef* GPIO,uint32_t GPIO_PIN,uint32_t ModePull,uint32_t ModeRISING_ORAND_FALLING,uint32_t PreemptPriority, uint32_t SubPriority);

#endif /* INC_GPIO_MASTER_H_ */
