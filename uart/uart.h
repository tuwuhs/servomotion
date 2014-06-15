/*
 * uart.h
 *
 *  Created on: Jun 15, 2014
 *      (c) 2014 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#ifndef _UART_H
#	define _UART_H

#define UART_BAUD              (115200)
#define UART_DOUBLE_SPEED
#define UART_INTERRUPT

#define UART_RX_BUFFER_SIZE    (64)
#define UART_TX_BUFFER_SIZE    (64)

#define UART_RETCODE_SUCCESS             (0x00)
#define UART_RETCODE_TX_BUFFER_FULL      (0x01)
#define UART_RETCODE_RX_BUFFER_EMPTY     (0x02)
#define UART_RETCODE_RX_BUFFER_OVERFLOW  (0x03)
#define UART_RETCODE_DISABLED            (0x04)

inline uint8_t uart_is_tx_enable(void);
inline uint8_t uart_is_rx_enable(void);
inline void uart_tx_enable(void);
inline void uart_tx_disable(void);
inline void uart_rx_enable(void);
inline void uart_rx_disable(void);

void uart_init(void);
uint8_t uart_putchar(uint8_t data);
uint8_t uart_getchar(uint8_t* data);
uint8_t uart_puts(char* data);
uint8_t uart_putint(uint16_t value);

inline uint8_t uart_is_tx_enable(void)
{
	return (UCSRB & _BV(TXEN));
}

inline uint8_t uart_is_rx_enable(void)
{
	return (UCSRB & _BV(RXEN));
}

inline void uart_tx_enable(void)
{
	UCSRB |= _BV(TXEN);
}

inline void uart_tx_disable(void)
{
	UCSRB &= ~_BV(TXEN);
}

inline void uart_rx_enable(void)
{
	UCSRB |= _BV(RXEN);
}

inline void uart_rx_disable(void)
{
	UCSRB &= ~_BV(RXEN);
}

#endif //_UART_H
