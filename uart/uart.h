/**
* \file UART function for PC-link, header file
* \author Tuwuh Sarwoprasojo
* \date 1 October 2011
* All rights reserved.
**/

#ifndef _UART_H
#	define _UART_H

#ifndef RETCODE_SUCCESS
#	define RETCODE_SUCCESS         0x00
#endif

#ifndef TRUE
#	define TRUE    (1 == 1)
#endif
#ifndef FALSE
#	define FALSE   (1 == 0)
#endif

#define UART_BAUD              57600
#define UART_DOUBLE_SPEED
#define UART_INTERRUPT

#define UART_RX_BUFFER_SIZE    64
#define UART_TX_BUFFER_SIZE    64

#define UART_RETCODE_SUCCESS             0x00
#define UART_RETCODE_TX_BUFFER_FULL      0x01
#define UART_RETCODE_RX_BUFFER_EMPTY     0x02
#define UART_RETCODE_RX_BUFFER_OVERFLOW  0x03
#define UART_RETCODE_DISABLED            0x04

uint8_t uart_is_tx_enable(void);
uint8_t uart_is_rx_enable(void);
void uart_tx_enable(void);
void uart_tx_disable(void);
void uart_rx_enable(void);
void uart_rx_disable(void);

void uart_init(void);
uint8_t uart_putchar(char data);
uint8_t uart_getchar(char *data);
uint8_t uart_puts(char *data);
uint8_t uart_putint(uint16_t value);

#endif //_UART_H
