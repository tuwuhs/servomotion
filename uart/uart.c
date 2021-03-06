/**
 * uart.c
 *
 *  Created on: Jun 15, 2014
 *      (c) 2014 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

#ifdef UART_INTERRUPT
// Pointers are post-increment (points to current data, this is the read/write head)
static uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static uint8_t rx_pointer_read, rx_pointer_write;
static volatile uint8_t rx_count;
static volatile bool uart_rx_overflow;

static uint8_t tx_buffer[UART_TX_BUFFER_SIZE];
static uint8_t tx_pointer_read, tx_pointer_write;
static volatile uint8_t tx_count;

ISR(USART_RXC_vect)
{
	uint8_t status;
	status = UCSRA;
	
	// If receive buffer overflow, discard the entire buffer to avoid data corruption
	if (rx_count >= UART_RX_BUFFER_SIZE) {
		rx_count = 0;
		uart_rx_overflow = true;
		return;
	}

	// If there are transmission error, discard received data
	if (status & (_BV(FE) | _BV(PE) | _BV(DOR)))
		return;
		
	// Receive data to the buffer
	rx_buffer[rx_pointer_write] = UDR;
	
	// Increment count, advance write-head
	rx_count++;
	rx_pointer_write = (rx_pointer_write + 1) % UART_RX_BUFFER_SIZE;
}

ISR(USART_UDRE_vect)
{
 	// Send data from the buffer
	UDR = tx_buffer[tx_pointer_read];

	// Decrement count, advance read-head
	tx_count--;
	tx_pointer_read = (tx_pointer_read + 1) % UART_TX_BUFFER_SIZE;
	
	// No more data to send, disable UDRE interrupt
	if (tx_count == 0) {
		UCSRB &= ~_BV(UDRIE);
	}
}
#endif //UART_INTERRUPT


void uart_init(void)
{
	UCSRA = 0;
	
	// Enable receiver and transmitter
	UCSRB = _BV(RXEN) | _BV(TXEN);
	
	// Async, No parity, 8-bit, 1 stop-bit
	UCSRC = _BV(UCSZ1) | _BV(UCSZ0);

#ifdef UART_DOUBLE_SPEED
	// Set the baud rate
	UBRRH = ((uint16_t) (1.0 * F_CPU / 8 / UART_BAUD) - 1) >> 8;
	UBRRL = ((uint16_t) (1.0 * F_CPU / 8 / UART_BAUD) - 1) & 0xFF;
	UCSRA |= _BV(U2X);
#else
	// Set the baud rate
	UBRRH = ((uint16_t) (1.0 * F_CPU / 16 / UART_BAUD) - 1) >> 8;
	UBRRL = ((uint16_t) (1.0 * F_CPU / 16 / UART_BAUD) - 1) & 0xFF;
#endif
	
#ifdef UART_INTERRUPT
	rx_pointer_read = rx_pointer_write = rx_count = 0;
	tx_pointer_read = tx_pointer_write = tx_count = 0;
	// Enable RXC interrupt, UDR interrupt will be enabled upon first Tx
	UCSRB |= _BV(RXCIE);
	// Do sei() in application!
#endif //UART_INTERRUPT
}

uint8_t uart_putchar(uint8_t data)
{
#ifdef UART_INTERRUPT
	if (!uart_is_tx_enable()) {
		return UART_RETCODE_DISABLED;
	}

	// Check if transmit buffer is full
	if (tx_count >= UART_TX_BUFFER_SIZE)
		return UART_RETCODE_TX_BUFFER_FULL;
		
	// Put data to the buffer, advance write-head
	tx_buffer[tx_pointer_write] = data;
	tx_pointer_write = (tx_pointer_write + 1) % UART_TX_BUFFER_SIZE;

	// Increment count (atomic, no locking needed)
	tx_count++;

	// Enable UDR interrupt
	if (tx_count >= 1) {
		UCSRB |= _BV(UDRIE);
	}
	
	return UART_RETCODE_SUCCESS;
#else
	if (!uart_is_tx_enable()) {
		return UART_RETCODE_DISABLED;
	}

	// Wait for transmit buffer to be empty
	while (!(UCSRA & _BV(UDRE)));
	
	// Put data to buffer
	UDR = data;
	
	return UART_RETCODE_SUCCESS;
#endif //UART_INTERRUPT
}

uint8_t uart_getchar(uint8_t* data)
{
#ifdef UART_INTERRUPT
	if (!uart_is_rx_enable()) {
		return UART_RETCODE_DISABLED;
	}

	// Check if overflow, return with an error, reset the flag
	if (uart_rx_overflow) {
		uart_rx_overflow = false;
		return UART_RETCODE_RX_BUFFER_OVERFLOW;
	}

	// Check if buffer is empty
	if (rx_count == 0)
		return UART_RETCODE_RX_BUFFER_EMPTY;
	
	// Get data from the buffer, advance read-head
	*data = rx_buffer[rx_pointer_read];
	rx_pointer_read = (rx_pointer_read + 1) % UART_RX_BUFFER_SIZE;

	// Decrement count (atomic, no locking needed)
	rx_count--;

	return UART_RETCODE_SUCCESS;
#else
	if (!uart_is_rx_enable()) {
		return UART_RETCODE_DISABLED;
	}

	// Wait for data to be received
//	while (!(UCSRA0 & _BV(RXC0)));

	// Return when no data
	if (!(UCSRA & _BV(RXC))) {
		return UART_RETCODE_RX_BUFFER_EMPTY;
	}
	
	// Receive data
	*data = UDR;
	
	return UART_RETCODE_SUCCESS;
#endif //UART_INTERRUPT
}

uint8_t uart_puts(char* data)
{
	uint8_t ret_code;
	
	while (*data) {
		ret_code = uart_putchar((uint8_t)(*data++));
		if (ret_code != UART_RETCODE_SUCCESS)
			return ret_code;
	}
	
	return ret_code;
}

uint8_t uart_putint(uint16_t value)
{
	uint8_t ret_code;
	uint8_t remainder;
	char data[6];
	uint8_t i;
	
	// Fill last data with 0
	data[5] = 0;
	i = 5;
	
	while (value) {
		remainder = value % 10;
		value = value / 10;
		data[--i] = (remainder + '0');
	}

	ret_code = uart_puts(&data[i]);
	if (ret_code != UART_RETCODE_SUCCESS)
		return ret_code;
		
	return ret_code;
}
