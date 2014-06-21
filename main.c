/* servomotion
 *
 * main.c
 *
 *  Created on: Jun 15, 2014
 *      (c) 2014 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "asciiparser.h"
#include "gpio.h"
#include "servo.h"
#include "systick.h"
#include "timer.h"
#include "uart.h"

void yeah(char* arg, uint8_t arglen);
void set_servo0_position(char* arg, uint8_t arglen);

static struct gpio_pin yellow_led;

struct aparser_item uart_parser_item[] = {
	{"y", yeah},
	{"s0", set_servo0_position},
	{NULL, NULL}
};

void yeah(char* arg, uint8_t arglen)
{
	gpio_toggle(&yellow_led);
}

void set_servo0_position(char* arg, uint8_t arglen)
{
	int16_t pos = atoi(arg);
	servo_set_position(0, pos);
}

void set_servo1_position(char* arg, uint8_t arglen)
{
	int16_t pos = atoi(arg);
	servo_set_position(1, pos);
}

int main(void)
{
	struct gpio_pin heartbeat_led;
	struct timer heartbeat_timer;

	struct aparser_ctx uart_parser;

	cli();

	gpio_init_output(&yellow_led, &PORTC, 0, false);
	gpio_init_output(&heartbeat_led, &PORTB, 5, false);
	timer_set_period_ms(&heartbeat_timer, 500);

	servo_init();
	uart_init();
	systick_init();

	aparser_init(&uart_parser, 2);
	aparser_register_commands(&uart_parser, uart_parser_item);

	wdt_enable(WDTO_500MS);

	sei();

	for (;;) {
		uint8_t data;

		if (uart_getchar(&data) == UART_RETCODE_SUCCESS) {
			uart_putchar(data);
			aparser_update_and_execute(&uart_parser, data);
		}

		if (timer_has_expired(&heartbeat_timer)) {
			timer_restart(&heartbeat_timer);
			gpio_toggle(&heartbeat_led);
		}

		wdt_reset();
	}

	return 0;
}
