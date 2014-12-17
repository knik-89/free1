/*
 * UART.h
 *
 *  Created on: 23.10.2012
 *      Author: user
 */

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"

#define u08 unsigned char
#define	u16 unsigned int

#define TX_BUFFER_SIZE		16
#define RX_BUFFER_SIZE		128


struct {
	u08 buff[TX_BUFFER_SIZE];
	u08 head;
	u08 tail;
} TX_buff;

struct {
	u08 buff[RX_BUFFER_SIZE];
	u08 head;
	u08 tail;
} RX_buff;

void UART_Init(u16 MyUBRR);
void UART_SendChar(u08 symb);
void UART_SendString(u08 * send_str);
void UART_FlushTXBuffer(void);
void UART_FlushRXBuffer(void);

#endif /* UART_H_ */
