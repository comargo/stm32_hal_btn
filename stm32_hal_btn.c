/**
  ******************************************************************************
  * @file     lib.c
  * @author   Auto-generated by STM32CubeIDE
  * @version  V1.0
  * @date     13/04/2021 19:30:15
  * @brief    Default under dev library file.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stm32_hal_btn.h>
#include "stm32f1xx_hal.h"

/** Functions ----------------------------------------------------------------*/
void CM_HAL_BTN_Init(struct CM_HAL_BTN* btn)
{
	assert_param(IS_GPIO_ALL_INSTANCE(btn->GPIOx));
	assert_param(IS_GPIO_PIN(btn->GPIO_Pin));
	switch((uintptr_t)btn->GPIOx) {
#ifdef GPIOA_BASE
	case GPIOA_BASE:
		__HAL_RCC_GPIOA_CLK_ENABLE();
		break;
#endif
#ifdef GPIOB_BASE
	case GPIOB_BASE:
		__HAL_RCC_GPIOB_CLK_ENABLE();
		break;
#endif
#ifdef GPIOC_BASE
	case GPIOC_BASE:
		__HAL_RCC_GPIOC_CLK_ENABLE();
		break;
#endif
#ifdef GPIOD_BASE
	case GPIOD_BASE:
		__HAL_RCC_GPIOD_CLK_ENABLE();
		break;
#endif
#ifdef GPIOE_BASE
	case GPIOE_BASE:
		__HAL_RCC_GPIOE_CLK_ENABLE();
		break;
#endif
	}

	GPIO_InitTypeDef GPIO_InitStruct = {
			.Pin =  btn->GPIO_Pin,
			.Mode = GPIO_MODE_IT_RISING_FALLING,
			.Pull = btn->flags.type?GPIO_PULLDOWN:GPIO_PULLUP,
			.Speed = 0
	};
	HAL_GPIO_Init(btn->GPIOx, &GPIO_InitStruct);

	IRQn_Type irq = RCC_IRQn;
	switch(btn->GPIO_Pin) {
	case GPIO_PIN_0:
		irq = EXTI0_IRQn;
		break;
	case GPIO_PIN_1:
		irq = EXTI1_IRQn;
		break;
	case GPIO_PIN_2:
		irq = EXTI2_IRQn;
		break;
	case GPIO_PIN_3:
		irq = EXTI3_IRQn;
		break;
	case GPIO_PIN_4:
		irq = EXTI4_IRQn;
		break;
	case GPIO_PIN_5:
	case GPIO_PIN_6:
	case GPIO_PIN_7:
	case GPIO_PIN_8:
	case GPIO_PIN_9:
		irq = EXTI9_5_IRQn;
		break;
	case GPIO_PIN_10:
	case GPIO_PIN_11:
	case GPIO_PIN_12:
	case GPIO_PIN_13:
	case GPIO_PIN_14:
	case GPIO_PIN_15:
		irq = EXTI15_10_IRQn;
		break;
	}
	HAL_NVIC_SetPriority(irq, 0, 0);
	HAL_NVIC_EnableIRQ(irq);
}

uint8_t CM_HAL_BTN_isPress(struct CM_HAL_BTN* btn)
{
	if(btn->flags.isPress_f) {
		btn->flags.isPress_f = 0;
		return 1;
	}
	else return 0;
}

uint8_t CM_HAL_BTN_isRelease(struct CM_HAL_BTN* btn)
{
	if (btn->flags.isRelease_f) {
		btn->flags.isRelease_f = 0;
		return 1;
	} else return 0;

}

uint8_t CM_HAL_BTN_isClick(struct CM_HAL_BTN* btn)
{
	if (btn->flags.isOne_f) {
		btn->flags.isOne_f = 0;
		return 1;
	} else return 0;
}

uint8_t CM_HAL_BTN_isHolded(struct CM_HAL_BTN* btn)
{
	if (btn->flags.isHolded_f) {
		btn->flags.isHolded_f = 0;
		return 1;
	} else return 0;
}

uint8_t CM_HAL_BTN_isHold(struct CM_HAL_BTN* btn)
{
	if (btn->flags.step_flag) return 1;
	else return 0;
}

uint8_t CM_HAL_BTN_state(struct CM_HAL_BTN* btn)
{
	return btn->btn_state;
}

uint8_t CM_HAL_BTN_isSingle(struct CM_HAL_BTN* btn)
{
	return CM_HAL_BTN_hasClicks(btn) && (CM_HAL_BTN_getClicks(btn) == 1);
}

uint8_t CM_HAL_BTN_isDouble(struct CM_HAL_BTN* btn)
{
	return CM_HAL_BTN_hasClicks(btn) && (CM_HAL_BTN_getClicks(btn) == 2);
}

uint8_t CM_HAL_BTN_isTripple(struct CM_HAL_BTN* btn)
{
	return CM_HAL_BTN_hasClicks(btn) && (CM_HAL_BTN_getClicks(btn) == 3);
}

uint8_t CM_HAL_BTN_hasClicks(struct CM_HAL_BTN* btn)
{
	if (btn->flags.counter_flag) {
		btn->flags.counter_flag = 0;
		return 1;
	} else return 0;
}

uint8_t CM_HAL_BTN_getClicks(struct CM_HAL_BTN* btn)
{
	uint8_t thisCount = btn->last_counter;
	btn->last_counter = 0;
	return thisCount;

}

uint8_t CM_HAL_BTN_getHoldClicks(struct CM_HAL_BTN* btn)
{
	return btn->flags.hold_flag ? btn->last_hold_counter : 0;
}

uint8_t CM_HAL_BTN_isStep(struct CM_HAL_BTN* btn, uint8_t clicks)
{
	if (btn->btn_counter == clicks && btn->flags.step_flag && (HAL_GetTick() - btn->btn_timer >= btn->step_timeout)) {
		btn->btn_timer = HAL_GetTick();
		return 1;
	}
	else return 0;
}

void CM_HAL_BTN_IRQHandle(struct CM_HAL_BTN* btn)
{
	btn->btn_state =  !HAL_GPIO_ReadPin(btn->GPIOx, btn->GPIO_Pin) ^ (btn->flags.inv_state ^ btn->flags.type);

	uint32_t thisMls = HAL_GetTick();

	// Pressing
	if (btn->btn_state && !btn->btn_flag) {
		if (!btn->flags.btn_deb) {
			btn->flags.btn_deb = 1;
			btn->btn_timer = thisMls;
		} else {
			if (thisMls - btn->btn_timer >= btn->debounce) {
				btn->btn_flag = 1;
				btn->flags.isPress_f = 1;
				btn->flags.oneClick_f = 1;
				if(btn->callback)
					btn->callback(btn, btn->userData, CM_HAL_BTN_CB_PRESSED);
			}
		}
	} else {
		btn->flags.btn_deb = 0;
	}

	// Releasing
	if (!btn->btn_state && btn->btn_flag) {
		btn->btn_flag = 0;
		if (!btn->flags.hold_flag) btn->btn_counter++;
		btn->flags.hold_flag = 0;
		btn->flags.isRelease_f = 1;
		btn->btn_timer = thisMls;
		if (btn->flags.step_flag) {
			btn->last_counter = 0;
			btn->btn_counter = 0;
			btn->flags.step_flag = 0;
		}
		if (btn->flags.oneClick_f) {
			btn->flags.oneClick_f = 0;
			btn->flags.isOne_f = 1;
		}
		if(btn->callback)
			btn->callback(btn, btn->userData, CM_HAL_BTN_CB_RELEASED);
	}

	// Button holden
	if (btn->btn_flag && btn->btn_state && (thisMls - btn->btn_timer >= btn->timeout) && !btn->flags.hold_flag) {
		btn->flags.hold_flag = 1;
		btn->last_hold_counter = btn->btn_counter;
		//btn_counter = 0;
		//last_counter = 0;
		btn->flags.isHolded_f = 1;
		btn->flags.step_flag = 1;
		btn->flags.oneClick_f = 0;
		btn->btn_timer = thisMls;
	}

	// Multiple clicks handle
	if ((thisMls - btn->btn_timer >= btn->click_timeout) && (btn->btn_counter != 0)) {
		btn->last_counter = btn->btn_counter;
		btn->btn_counter = 0;
		btn->flags.counter_flag = 1;
		btn->callback(btn, btn->userData, CM_HAL_BTN_CB_CLICK_TIMEOUT);
	}
}
