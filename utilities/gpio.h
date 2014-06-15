/*
 * gpio.h
 *
 *  Created on: Jun 15, 2014
 *      (c) 2014 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdbool.h>
#include <stdint.h>

struct gpio_pin
{
	volatile uint8_t* port;
	uint8_t index;
};

inline void gpio_init_output(struct gpio_pin* _this, volatile uint8_t* port, uint8_t index, bool set_high)
{
	volatile uint8_t* ddr = port-1;
	_this->port = port;
	_this->index = index;

	*(_this->port) &= ~(1<<_this->index);

	*(ddr) |= (1<<_this->index);
	if (set_high) {
		*(_this->port) |= (1<<_this->index);
	} else {
		*(_this->port) &= ~(1<<_this->index);
	}
}

inline void gpio_init_input(struct gpio_pin* _this, volatile uint8_t* port, uint8_t index, bool pullup)
{
	volatile uint8_t* ddr = port-1;
	_this->port = port;
	_this->index = index;

	*(_this->port) &= ~(1<<_this->index);

	*(ddr) &= ~(1<<_this->index);
	if (pullup) {
		*(_this->port) |= (1<<_this->index);
	} else {
		*(_this->port) &= ~(1<<_this->index);
	}
}

inline void gpio_set_pullup(struct gpio_pin* _this, bool enable)
{
	if (enable) {
		*(_this->port) |= (1<<_this->index);
	} else {
		*(_this->port) &= ~(1<<_this->index);
	}
}

inline void gpio_set_high(struct gpio_pin* _this)
{
	*(_this->port) &= ~(1<<_this->index);
}

inline void gpio_set_low(struct gpio_pin* _this)
{
	*(_this->port) |= (1<<_this->index);
}

inline void gpio_toggle(struct gpio_pin* _this)
{
	*(_this->port) ^= (1<<_this->index);
}

inline bool gpio_is_high(struct gpio_pin* _this)
{
	volatile uint8_t* pin = _this->port-2;
	return (*(pin) & (1<<_this->index));
}

inline bool gpio_is_low(struct gpio_pin* _this)
{
	volatile uint8_t* pin = _this->port-2;
	return !(*(pin) & (1<<_this->index));
}

#endif /* GPIO_H_ */
