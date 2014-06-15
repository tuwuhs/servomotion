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

int main(void)
{
	struct gpio_pin green_led;
	struct gpio_pin pushbutton;
	gpio_init(&green_led, &PORTB, 0, true, false);
	gpio_init(&pushbutton, &PORTB, 1, false, true);

	for (;;) {
		if (gpio_is_high(&pushbutton)) {
			gpio_set_high(&green_led);
		} else {
			gpio_set_low(&green_led);
		}
	}

	return 0;
}
