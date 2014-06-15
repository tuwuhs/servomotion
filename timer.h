/*
 * timer.h
 *
 *  Created on: Jun 15, 2014
 *      (c) 2014 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include <stdbool.h>

#include "systick.h"

struct timer
{
	uint16_t period_tick;
	uint16_t next_tick;
};

inline void timer_restart(struct timer* _this);
inline void timer_set_period(struct timer* _this, uint16_t period_tick);
inline void timer_set_period_ms(struct timer* _this, uint16_t period_ms);
inline bool timer_has_expired(struct timer* _this);

inline void timer_restart(struct timer* _this)
{
	_this->next_tick = systick_get_current() + _this->period_tick;
}

inline void timer_set_period(struct timer* _this, uint16_t period_tick)
{
	_this->period_tick = period_tick;
	timer_restart(_this);
}

inline void timer_set_period_ms(struct timer* _this, uint16_t period_ms)
{
	timer_set_period(_this, period_ms * 1000UL / SYSTICK_PERIOD_US);
}

inline bool timer_has_expired(struct timer* _this)
{
	return (systick_compare_with_current(_this->next_tick) >= 0);
}

#endif /* TIMER_H_ */
