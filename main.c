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

#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "gpio.h"
#include "timer.h"
#include "systick.h"
#include "uart.h"
#include "asciiparser.h"

void yeah(char* arg, uint8_t arglen);

static struct gpio_pin yellow_led;

struct aparser_item uart_parser_item[] = {
	{"y", yeah},
	{NULL, NULL}
};

void yeah(char* arg, uint8_t arglen)
{
	gpio_toggle(&yellow_led);
}

int main(void)
{
	struct gpio_pin green_led;
	struct gpio_pin heartbeat_led;
	struct gpio_pin pushbutton;

	struct timer heartbeat_timer;

	struct aparser_ctx uart_parser;

	gpio_init_output(&green_led, &PORTB, 0, false);
	gpio_init_output(&yellow_led, &PORTC, 0, false);
	gpio_init_input(&pushbutton, &PORTB, 1, true);
	gpio_init_output(&heartbeat_led, &PORTB, 2, false);
	timer_set_period_ms(&heartbeat_timer, 500);

	uart_init();
	systick_init();

	aparser_init(&uart_parser, 2);
	aparser_register_commands(&uart_parser, uart_parser_item);

	wdt_enable(WDTO_500MS);

	sei();

	for (;;) {
		uint8_t data;

		if (gpio_is_low(&pushbutton)) {
			gpio_set_high(&green_led);
		} else {
			gpio_set_low(&green_led);
		}

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
