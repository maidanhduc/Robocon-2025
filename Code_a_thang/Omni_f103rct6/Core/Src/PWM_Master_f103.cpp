/*
 * PWM_Master.cpp
 *
 *  Created on: Nov 24, 2024
 *      Author: Admin
 */


#include "PWM_Master_f103.h"




std::unordered_multimap<GPIO_TIM_Mapping, GPIO_TIM_Mapping, GPIOPinHash> gpioMap;

int Timer_IsEnable(TIM_TypeDef *TIMx) {
	return (TIMx->CR1 & TIM_CR1_CEN) ? 1 : 0;
}

uint16_t Timer_GetPrescaler(TIM_TypeDef *TIMx) {

	return TIMx->PSC;
}

uint16_t Timer_GetPeriod(TIM_TypeDef *TIMx) {

	return TIMx->ARR;
}

void Enable_RCC_TIMx(TIM_TypeDef* TIMx){
	if(TIMx == TIM1){
		__HAL_RCC_TIM1_CLK_ENABLE();

	}else if(TIMx == TIM2){
		__HAL_RCC_TIM2_CLK_ENABLE();

	}else if(TIMx == TIM3){
		__HAL_RCC_TIM3_CLK_ENABLE();

	}else if(TIMx == TIM4){
		__HAL_RCC_TIM4_CLK_ENABLE();

	}else if(TIMx == TIM5){
		__HAL_RCC_TIM5_CLK_ENABLE();

	}else if(TIMx == TIM6){
		__HAL_RCC_TIM6_CLK_ENABLE();

	}else if(TIMx == TIM7){
		__HAL_RCC_TIM7_CLK_ENABLE();

	}else if(TIMx == TIM8){
		__HAL_RCC_TIM8_CLK_ENABLE();
	}
}


void ENABLE_GPIO_CLOCK(GPIO_TypeDef* GPIO){
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
void TIMER_PWM_Init(GPIO_TypeDef *GPIO, uint16_t Pin, TIM_HandleTypeDef *htim,
        TIM_TypeDef *TIMx, uint32_t timerChannel, uint16_t Prescaler,
        uint16_t Period, uint16_t Pulse) // cấu hình timer
		{
	// Bật clock
	Enable_RCC_TIMx(TIMx);
	ENABLE_GPIO_CLOCK(GPIO);


	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

	htim->Instance = TIMx;
	htim->Init.Prescaler = Prescaler;
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.Period = Period;
	htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // không chia tần số  DIV2 chia 2 DIV4 chia cho 4


	htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_PWM_Init(htim) != HAL_OK) {

		Error_Handler();
	}// begin


	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig)!= HAL_OK) {

		Error_Handler();
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = Pulse;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

	if(TIMx == TIM1 || TIMx == TIM8){
		sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	}

	if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, timerChannel) != HAL_OK) {

		Error_Handler();
	}


	if(TIMx == TIM1 || TIMx == TIM8){
	    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
		sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
		sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
		sBreakDeadTimeConfig.DeadTime = 0;
		sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
		sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
		sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
		if (HAL_TIMEx_ConfigBreakDeadTime(htim, &sBreakDeadTimeConfig) != HAL_OK)
		  {
			Error_Handler();
		  }
	}


	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIO, &GPIO_InitStruct);

	if(TIMx == TIM3)
    __HAL_AFIO_REMAP_TIM3_ENABLE();

	if (HAL_TIM_PWM_Start(htim, timerChannel) != HAL_OK) {
		Error_Handler();
	}
}

// Khi cần thêm các ánh xạ
void AddMapping(GPIO_TypeDef* GPIO, uint16_t Pin, TIM_TypeDef* TIMx, uint32_t TIM_CHANNEL, bool status) {
    gpioMap.emplace(GPIO_TIM_Mapping(GPIO, Pin, TIMx, TIM_CHANNEL,status),
                    GPIO_TIM_Mapping(GPIO, Pin, TIMx, TIM_CHANNEL,status));
}

// tìm tất cả ánh xạ phù hợp
std::vector<GPIO_TIM_Mapping> FindAllMappings(GPIO_TypeDef* GPIO, uint16_t GPIO_PIN, bool status) {
	std::vector<GPIO_TIM_Mapping> foundMappings;

	auto range = gpioMap.equal_range(GPIO_TIM_Mapping(GPIO, GPIO_PIN, nullptr, 0, status));

	for (auto it = range.first; it != range.second; ++it) {

		foundMappings.push_back(it->second);
	}

	return foundMappings;
}

// tìm ánh xạ đơn lẻ
GPIO_TIM_Mapping FindMapping(GPIO_TypeDef* GPIO, uint16_t GPIO_PIN, bool status) {
    auto range = gpioMap.equal_range(GPIO_TIM_Mapping(GPIO, GPIO_PIN, nullptr, 0, status));
    for (auto it = range.first; it != range.second; ++it) {
            // Nếu tìm thấy một ánh xạ thỏa mãn điều kiện, trả về ánh xạ này
            return it->second;
        }
    return GPIO_TIM_Mapping();
}

void UpdateMapping(const GPIO_TIM_Mapping& oldMapping, const GPIO_TIM_Mapping& newMapping) {
    // Xóa ánh xạ cũ
    auto range = gpioMap.equal_range(oldMapping);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second == oldMapping) {
            gpioMap.erase(it);
            break;
        }
    }

    gpioMap.emplace(newMapping, newMapping);
}


void Register_PWM_GPIO(GPIO_TypeDef *GPIO, uint32_t Pin, uint16_t Prescaler,uint16_t Period, uint16_t Pulse) {


	auto mappings = FindAllMappings(GPIO, Pin, false);

	for (auto& mapping : mappings) {
		if (!Timer_IsEnable(mapping.TIMx) || (Timer_GetPeriod(mapping.TIMx) == Period && Timer_GetPrescaler(mapping.TIMx) == Prescaler)) {

			TIMER_PWM_Init(GPIO, Pin, &mapping.htim, mapping.TIMx, mapping.TIM_CHANNEL,
                    Prescaler, Period, Pulse);

			GPIO_TIM_Mapping newMapping = mapping;
			newMapping.status = true;
			UpdateMapping(mapping, newMapping);

			return;
		}
	}

	Error_Handler();


}


void PWM_Write_GPIO(GPIO_TypeDef* Port, uint32_t Pin,uint16_t Pulse)
{
	GPIO_TIM_Mapping mapping = FindMapping(Port, Pin,true);

	if (mapping.TIMx == nullptr) {
		// Nếu không tìm thấy ánh xạ hợp lệ, xử lý lỗi
		Error_Handler();
	}

	__HAL_TIM_SET_COMPARE(&mapping.htim,mapping.TIM_CHANNEL,Pulse);
//	TIM_HandleTypeDef htim;

//	__HAL_TIM_SET_COMPARE(&htim,TIM_CHANNEL_1,200);

}

void Add_Map_STM32F103xx()
{

/**/	AddMapping(GPIOA, GPIO_PIN_0, TIM2, TIM_CHANNEL_1, false);
	AddMapping(GPIOA, GPIO_PIN_0, TIM5, TIM_CHANNEL_1, false);

/**/	AddMapping(GPIOA, GPIO_PIN_1, TIM2, TIM_CHANNEL_2, false);
	AddMapping(GPIOA, GPIO_PIN_1, TIM5, TIM_CHANNEL_2, false);

/**/	AddMapping(GPIOA, GPIO_PIN_2, TIM2, TIM_CHANNEL_3, false);
	AddMapping(GPIOA, GPIO_PIN_2, TIM5, TIM_CHANNEL_3, false);

/**/	AddMapping(GPIOA, GPIO_PIN_3, TIM2, TIM_CHANNEL_4, false);
	AddMapping(GPIOA, GPIO_PIN_3, TIM5, TIM_CHANNEL_4, false);

/**/	AddMapping(GPIOA, GPIO_PIN_6, TIM3, TIM_CHANNEL_1, false);

/**/	AddMapping(GPIOA, GPIO_PIN_7, TIM3, TIM_CHANNEL_2, false);

/**/	AddMapping(GPIOA, GPIO_PIN_8, TIM1, TIM_CHANNEL_1, false);

/**/	AddMapping(GPIOA, GPIO_PIN_9, TIM1, TIM_CHANNEL_2, false);

/**/	AddMapping(GPIOA, GPIO_PIN_10, TIM1, TIM_CHANNEL_3, false);

/**/	AddMapping(GPIOA, GPIO_PIN_11, TIM1, TIM_CHANNEL_4, false);

/**/	AddMapping(GPIOA, GPIO_PIN_15, TIM2, TIM_CHANNEL_1, false);

/**/	AddMapping(GPIOB, GPIO_PIN_0, TIM3, TIM_CHANNEL_3, false);

/**/	AddMapping(GPIOB, GPIO_PIN_1, TIM3, TIM_CHANNEL_4, false);

/**/	AddMapping(GPIOB, GPIO_PIN_3, TIM2, TIM_CHANNEL_2, false);

/**/	AddMapping(GPIOB, GPIO_PIN_4, TIM3, TIM_CHANNEL_1, false);

/**/	AddMapping(GPIOB, GPIO_PIN_5, TIM3, TIM_CHANNEL_2, false);

/**/	AddMapping(GPIOB, GPIO_PIN_6, TIM4, TIM_CHANNEL_1, false);

/**/	AddMapping(GPIOB, GPIO_PIN_7, TIM4, TIM_CHANNEL_2, false);

/**/	AddMapping(GPIOB, GPIO_PIN_8, TIM4, TIM_CHANNEL_3, false);

/**/	AddMapping(GPIOB, GPIO_PIN_9, TIM4, TIM_CHANNEL_4, false);

/**/	AddMapping(GPIOB, GPIO_PIN_10, TIM2, TIM_CHANNEL_3, false);

/**/	AddMapping(GPIOB, GPIO_PIN_11, TIM2, TIM_CHANNEL_4, false);

/**/	AddMapping(GPIOC, GPIO_PIN_6, TIM3, TIM_CHANNEL_1, false);
	AddMapping(GPIOC, GPIO_PIN_6, TIM8, TIM_CHANNEL_1, false);

/**/	AddMapping(GPIOC, GPIO_PIN_7, TIM3, TIM_CHANNEL_2, false);
	AddMapping(GPIOC, GPIO_PIN_7, TIM8, TIM_CHANNEL_2, false);

/**/	AddMapping(GPIOC, GPIO_PIN_8, TIM3, TIM_CHANNEL_3, false);
	AddMapping(GPIOC, GPIO_PIN_8, TIM8, TIM_CHANNEL_3, false);

/**/	AddMapping(GPIOC, GPIO_PIN_9, TIM3, TIM_CHANNEL_4, false);
	AddMapping(GPIOC, GPIO_PIN_9, TIM8, TIM_CHANNEL_4, false);



}
