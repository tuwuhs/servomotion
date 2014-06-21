/* servomotion
 *
 * servo.c
 *
 *  Created on: Jun 22, 2014
 *      (c) 2014 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "servo.h"

#define MAX_SERVO_CLK  (F_CPU / 1000000 * MAX_SERVO_US)
#define MIN_SERVO_CLK  (F_CPU / 1000000 * MIN_SERVO_US)

static struct {
	uint16_t next_ocr1a;
	uint16_t next_ocr1b;
	uint8_t counter;
} timer1_ctx;

static void set_next_ocr1a(uint16_t val);
static void set_next_ocr1b(uint16_t val);

ISR(TIMER1_OVF_vect)
{
	timer1_ctx.counter = (timer1_ctx.counter + 1) % 5;
	if (timer1_ctx.counter == 0) {
		// Update OCR values
		OCR1A = (uint16_t)((
				(((int32_t) OCR1A)<<SERVO_LOG2_SMOOTHING_FACTOR) - OCR1A
				+ (int32_t) timer1_ctx.next_ocr1a)
				>>SERVO_LOG2_SMOOTHING_FACTOR);
		OCR1B = (uint16_t)((
				(((int32_t) OCR1A)<<SERVO_LOG2_SMOOTHING_FACTOR) - OCR1B
				+ (int32_t) timer1_ctx.next_ocr1b)
				>>SERVO_LOG2_SMOOTHING_FACTOR);

		// Set the pins to high
		TCCR1A |= (1<<COM1B0) | (1<<COM1A0);
		TCCR1A |= (1<<FOC1B) | (1<<FOC1A);

		// When OCR reached the corresponding pin will be set to low
		TCCR1A &= ~((1<<COM1B0) | (1<<COM1A0));
	}
}

static void set_next_ocr1a(uint16_t val)
{
	uint8_t sreg = SREG;
	cli();
	timer1_ctx.next_ocr1a = val;
	SREG = sreg;
}

static void set_next_ocr1b(uint16_t val)
{
	uint8_t sreg = SREG;
	cli();
	timer1_ctx.next_ocr1b = val;
	SREG = sreg;
}

void servo_init(void)
{
	DDRB |= (1<<PB1) | (1<<PB2);
	TCCR1A = (1<<COM1A1) | (1<<COM1B1);
	TCCR1B = (1<<CS10);
	TIMSK |= (1<<TOIE1);
	OCR1A = (MAX_SERVO_CLK+MIN_SERVO_CLK)/2;
	set_next_ocr1a((MAX_SERVO_CLK+MIN_SERVO_CLK)/2);
	set_next_ocr1b((MAX_SERVO_CLK+MIN_SERVO_CLK)/2);
}

void servo_set_position(uint8_t id, int16_t pos)
{
	if (pos < MIN_SERVO || pos > MAX_SERVO) {
		return;
	}
	uint16_t centre_clk = (MAX_SERVO_CLK + MIN_SERVO_CLK) / 2;
	uint16_t diff_clk = (MAX_SERVO_CLK - MIN_SERVO_CLK) / 2;
	uint16_t value_clk = (int32_t)pos * diff_clk/MAX_SERVO + centre_clk;
	if (id == 0) {
		set_next_ocr1a(value_clk);
	} else {
		set_next_ocr1b(value_clk);
	}
}

