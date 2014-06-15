/* servomotion
 *
 * main.c
 *
 *  Created on: Jun 15, 2014
 *      (c) 2014 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#include <stdbool.h>

#include <avr/io.h>
#include <util/delay.h>

#include "gpio.h"
#include "timer.h"
#include "systick.h"
#include "uart.h"

int main(void)
{
	struct gpio_pin green_led;
	struct gpio_pin heartbeat_led;
	struct gpio_pin pushbutton;

	struct timer heartbeat_timer;

	gpio_init(&green_led, &PORTB, 0, true, false);
	gpio_init(&pushbutton, &PORTB, 1, false, true);
	gpio_init(&heartbeat_led, &PORTB, 2, true, false);
	timer_set_period_ms(&heartbeat_timer, 500);

	uart_init();
	systick_init();
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
		}

		if (timer_has_expired(&heartbeat_timer)) {
			timer_restart(&heartbeat_timer);
			gpio_toggle(&heartbeat_led);
		}
	}

	return 0;
}
