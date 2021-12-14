/*
 * stm32_hal_btn.h
 *
 *  Created on: Apr 13, 2021
 *      Author: cyril
 */

#ifndef INC_STM32_HAL_BTN_H_
#define INC_STM32_HAL_BTN_H_

#include <stdbool.h>
#include "stm32f1xx_hal.h"

#define CM_HAL_BTN_DEBOUNCE 80
#define CM_HAL_BTN_TIMEOUT 300
#define CM_HAL_BTN_CLICK_TIMEOUT 500
#define CM_HAL_BTN_STEP_TIMEOUT 400

enum CM_HAL_BTN_Reason {
	CM_HAL_BTN_CB_PRESSED,
	CM_HAL_BTN_CB_RELEASED,
	CM_HAL_BTN_CB_CLICK_TIMEOUT
};

#pragma pack(push,1)
struct CM_HAL_BTN_Flags {
	bool btn_deb: 1;
	bool hold_flag: 1;
	bool counter_flag: 1;
	bool isHolded_f: 1;
	bool isRelease_f: 1;
	bool isPress_f: 1;
	bool step_flag: 1;
	bool oneClick_f: 1;
	bool isOne_f: 1;
	bool inv_state: 1;
	bool mode: 1;
	bool type: 1;
	bool tickMode: 1;
	bool noPin: 1;
};
#pragma pack(pop)

struct CM_HAL_BTN {
	GPIO_TypeDef *GPIOx;
	uint32_t GPIO_Pin;
	uint16_t debounce;
	uint16_t timeout;
	uint16_t click_timeout;
	uint16_t step_timeout;

	/* Callback */
	void (*callback)(struct CM_HAL_BTN *btn, void *userData, enum CM_HAL_BTN_Reason reason);
	void *userData;

	/* Private area */
	struct CM_HAL_BTN_Flags flags;
	uint32_t btn_counter;
	uint32_t last_counter;
	uint32_t last_hold_counter;
	uint32_t btn_timer;
	bool btn_state;
	bool btn_flag;
};

#define CM_HAL_BTN_StaticInit(_GPIOx, _GPIO_Pin, _Pull, _Dir) \
	{ .GPIOx = _GPIOx, .GPIO_Pin = _GPIO_Pin, \
	.debounce = CM_HAL_BTN_DEBOUNCE, \
	.timeout = CM_HAL_BTN_TIMEOUT, \
	.click_timeout = CM_HAL_BTN_CLICK_TIMEOUT, \
	.step_timeout = CM_HAL_BTN_STEP_TIMEOUT, \
	.flags = {.type = (_Pull  == GPIO_PULLDOWN), \
				.inv_state = _Dir}, \
	.btn_counter = 0, .last_counter = 0, .last_hold_counter = 0, \
	.btn_timer = 0, .btn_state = false, .btn_flag = false }


void CM_HAL_BTN_Init(struct CM_HAL_BTN* btn);
bool CM_HAL_BTN_isPress(struct CM_HAL_BTN* btn);
bool CM_HAL_BTN_isRelease(struct CM_HAL_BTN* btn);
bool CM_HAL_BTN_isClick(struct CM_HAL_BTN* btn);
bool CM_HAL_BTN_isHolded(struct CM_HAL_BTN* btn);
bool CM_HAL_BTN_isHold(struct CM_HAL_BTN* btn);
bool CM_HAL_BTN_state(struct CM_HAL_BTN* btn);
bool CM_HAL_BTN_isSingle(struct CM_HAL_BTN* btn);
bool CM_HAL_BTN_isDouble(struct CM_HAL_BTN* btn);
bool CM_HAL_BTN_isTripple(struct CM_HAL_BTN* btn);
bool CM_HAL_BTN_hasClicks(struct CM_HAL_BTN* btn);
uint8_t CM_HAL_BTN_getClicks(struct CM_HAL_BTN* btn);
uint8_t CM_HAL_BTN_getHoldClicks(struct CM_HAL_BTN* btn);
bool CM_HAL_BTN_isStep(struct CM_HAL_BTN* btn, uint8_t clicks);

// This function should be called in interrupt routines:
// EXTI for click support
// SysTick for hold support
void CM_HAL_BTN_IRQHandle(struct CM_HAL_BTN* btn);



#endif /* INC_STM32_HAL_BTN_H_ */
