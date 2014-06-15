/*
 * systick.h
 *
 *  Created on: Jun 15, 2014
 *      (c) 2014 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define SYSTICK_PERIOD_US (1000)

void systick_init(void);
uint16_t systick_get_current(void);
inline int16_t systick_compare(uint16_t first, uint16_t second);
inline int16_t systick_compare_with_current(uint16_t tick);

inline int16_t systick_compare(uint16_t first, uint16_t second)
{
	return (int16_t) (second - first);
}

inline int16_t systick_compare_with_current(uint16_t tick)
{
	return systick_compare(tick, systick_get_current());
}

#endif /* SYSTICK_H_ */
