/*
 * systick.c
 *
 *  Created on: Jun 15, 2014
 *      (c) 2014 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "systick.h"
#include "timer.h"

#define RELOAD_VALUE      ((uint8_t) (-(F_CPU/64/SYSTICK_PERIOD_US)))

static volatile uint16_t g_tick;

ISR(TIMER0_OVF_vect)
{
	TCNT0 = RELOAD_VALUE;
	g_tick += 1;
}

void systick_init(void)
{
	// Prescaler: clk/64
	TCCR0 = (1<<CS00) | (1<<CS01);

	// Enable overflow interrupt
	TIMSK |= (1<<TOIE0);
}

uint16_t systick_get_current(void)
{
	uint16_t tick;
	uint8_t sreg = SREG;

	// Disable interrupt before reading the tick
	cli();
	tick = g_tick;
	SREG = sreg;
	return tick;
}
