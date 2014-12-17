/*
 * UART.c
 *
 *  Created on: 23.10.2012
 *      Author: user
 */

#include "UART.h"

inline void UART_Init(u16 MyUBRR) {
	UBRRH = (u08)(MyUBRR>>8);										// set baud rate high byte
	UBRRL = (u08)(MyUBRR);											// set baud rate high byte
	UCSRB = (1<<RXCIE)|(0<<TXCIE)|(0<<UDRIE)|(1<<RXEN)|(1<<TXEN);	// enable interrupts, receiving & transmitting
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);						// set format 8-n-1
	// Flush circle buffers
	TX_buff.head = 0;
	TX_buff.tail = 0;
	RX_buff.head = 0;
	RX_buff.tail = 0;
}

void UART_SendChar(u08 symb) {
	while(!(UCSRA & (1<<UDRE)));
	UDR = symb;
}

void UART_SendString(u08 * send_str) {
	while (*send_str!='\0') {
		TX_buff.buff[TX_buff.tail] = *send_str;
		TX_buff.tail = (TX_buff.tail+1)&(TX_BUFFER_SIZE-1);
		send_str++;
	}
	UART_message = UART_TX_START;
	SendMessageWParam(MSG_UART, &UART_message);
}

ISR (USART_RXC_vect) {
	u08 tmp = 0;
	tmp = UDR;
	if (((RX_buff.tail - RX_buff.head + 256) & (RX_BUFFER_SIZE-1)) < (RX_BUFFER_SIZE-1)) {
		RX_buff.buff[RX_buff.tail] = tmp;
		RX_buff.tail = (RX_buff.tail+1)&(RX_BUFFER_SIZE-1);
		if (tmp == 0x0D) {										// found end string classifier
			RX_buff.tail = (RX_buff.tail-1)&(RX_BUFFER_SIZE-1); // remove string end classifier from buffer
			///RX_buff.buff[RX_buff.tail] = '\0';				// replace string end delimiter with C standard string end
			UART_message = UART_RX_COMPLETE;
			SendMessageWParam(MSG_UART, &UART_message);
		}
	} else {
		sys_error = SYS_ERR_RX_BUF_FULL;
	}
}

ISR (USART_UDRE_vect) {
	;
}

ISR (USART_TXC_vect) {
	if (TX_buff.head!=TX_buff.tail) {
		UDR = TX_buff.buff[TX_buff.head];
		TX_buff.head = (TX_buff.head+1)&(TX_BUFFER_SIZE-1);
	} else {
		UART_message = UART_TX_COMPLETE;
		SendMessageWParam(MSG_UART, &UART_message);
	}
}


void UART_FlushTXBuffer() {
	TX_buff.head = TX_buff.tail;
}

void UART_FlushRXBuffer() {
	u08 sreg = SREG;
	cli();
	RX_buff.head = RX_buff.tail;
	SREG = sreg;
}

