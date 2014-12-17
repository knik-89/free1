/*
 * FSM.c
 *
 *  Created on: 23.10.2012
 *      Author: user
 */

#include "FSM.h"

// FSM LED init
void FSM_LED_Init(void) {
	FSM_LED_state = LED_OFF;
	ResetTimer(TIMER_LED);
}

// FSM LED code
void FSM_LED(void) {
	u08 tmp = 0xff;
	if (GetMessage(MSG_LED)) {
		tmp = *(GetMessageParam(MSG_LED));
	}

	switch (FSM_LED_state) {
	case LED_OFF:
		if (tmp == LED_ON) {
			FSM_LED_state = LED_ON;
			LED_Pin_ON;
		} else {
			LED_Pin_OFF;
		}
		break;

	case LED_ON:
		if (tmp == LED_OFF) {
			FSM_LED_state = LED_OFF;
			LED_Pin_OFF;
		} else if (GetTimer(TIMER_LED)>=LED_BLINK_DELAY) {
			LED_Pin_TOGGLE;
			ResetTimer(TIMER_LED);
		}
		break;

	default:
		break;
	}
}

// FSM LCD init
void FSM_LCD_Init(void) {
	FSM_LCD_state = LCD_OFF;
	ResetTimer(TIMER_LCD);
}

// FSM LCD code
void FSM_LCD(void) {
	u08 tmp = 0xff;
	if (GetMessage(MSG_LCD)) {
		tmp = *(GetMessageParam(MSG_LCD));
	}

	switch (FSM_LCD_state) {
	case LCD_OFF:
		if (tmp == LCD_ON) {
			LCD_Cmd(LCD_DISPLAY_ON);
			FSM_LCD_state = LCD_REFRESH;
		}
		break;

	case LCD_ON:
			switch (tmp) {
			case LCD_OFF:
				LCD_Cmd(LCD_DISPLAY_OFF);
				FSM_LCD_state = LCD_OFF;
				break;

			case LCD_REFRESH:
				FSM_LCD_state = LCD_REFRESH;
				break;

			case LCD_CLEAR:
				FSM_LCD_state = LCD_CLEAR;
				break;

			default:
				break;
			}
			if (GetTimer(TIMER_LCD)>=LCD_REFRESH_DELAY) {
				FSM_LCD_state = LCD_REFRESH;
				ResetTimer(TIMER_LCD);
			}
		break;

	case LCD_REFRESH:
		LCD_Refresh(1);
		FSM_LCD_state = LCD_ON;
		ResetTimer(TIMER_LCD);
		break;

	case LCD_CLEAR:
//		for (u08 cnt = 0; (LCD_MAX_POS-1); cnt++) {
//			frame_buff.row1[cnt] = ' ';
//			frame_buff.row2[cnt] = ' ';
//		}
//		FSM_LCD_state = LCD_REFRESH;
		LCD_Refresh(0);
		FSM_LCD_state = LCD_ON;
		ResetTimer(TIMER_LCD);
		break;

	default:
		break;
	}
}

// FSM UART init
void FSM_UART_Init(void) {
	FSM_UART_state = UART_POLLING;
	ResetTimer(TIMER_UART);
}

// FSM UART code
void FSM_UART(void) {
	u08 tmp = 0xff;
	static	u08 selector = 0;

	if (GetMessage(MSG_UART)) {
		tmp = *(GetMessageParam(MSG_UART));
	}

	switch (FSM_UART_state) {
	case UART_POLLING:
		switch (tmp) {
		case UART_RX_COMPLETE:
			FSM_UART_state = UART_RX_COMPLETE;
			break;

		case UART_TX_COMPLETE:
			FSM_UART_state = UART_TX_COMPLETE;
			break;

		case UART_TX_START:
			FSM_UART_state = UART_TX_START;
			break;

		default:
			break;
		}
		break;

	case UART_RX_COMPLETE:
		selector = RX_buff.buff[RX_buff.head];
		RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
		switch (selector) {					// if string sent to LCD ('s' char is received first)
		case CHAR_CMD_PUT_STRING:
			CMD_message = CMD_PUT_STRING;
			SendMessageWParam(MSG_CMD, &CMD_message);
			break ;

		case CHAR_CMD_COMMAND_PREFIX:		// if command sent to LCD
			CMD_message = CMD_RECEIVED;
			SendMessageWParam(MSG_CMD, &CMD_message);
			break;

		case CHAR_CMD_ECHO:					// if echo command received
			CMD_message = CMD_ECHO;
			SendMessageWParam(MSG_CMD, &CMD_message);
			break;

		default:
			UART_FlushRXBuffer();
			break;
		}

#ifdef PRINT_RESPONSE_TO_CONSOLE
		UART_SendChar(CHAR_OK_RESPONSE);
#endif
		FSM_UART_state = UART_POLLING;
		break;

	case UART_TX_COMPLETE:
		UCSRB &= ~(1<<TXCIE); // disable TXC interrupt
		FSM_UART_state = UART_POLLING;
		break;

	case UART_TX_START:
		if (TX_buff.tail == TX_buff.head) {
			sys_error = SYS_ERR_TX_BUFF_EMPTY;
		} else {
			u08 sreg = SREG;
			cli();
			UCSRB |= (1<<TXCIE); // enable TXC interrupt
			while ( !( UCSRA & (1<<UDRE)) );
			UDR = TX_buff.buff[TX_buff.head];
			TX_buff.head = (TX_buff.head+1)&(TX_BUFFER_SIZE-1);
			SREG = sreg;
		}
		FSM_UART_state = UART_POLLING;
		break;

	default:
		break;
	}
}

// FSM UART init
void FSM_CMD_Init(void) {
	FSM_CMD_state = CMD_POLLING;
	ResetTimer(TIMER_CMD);
}

// FSM UART code
void FSM_CMD(void) {
	u08 tmp = 0xff;
	u08 ii = 0;
	static u08 selector = 0;
	static u08 selector2 = 0;

	if (GetMessage(MSG_CMD)) {
		tmp = *(GetMessageParam(MSG_CMD));
	}

	switch (FSM_CMD_state) {
	case CMD_POLLING:
		switch(tmp) {
		case CMD_PUT_STRING:
			FSM_CMD_state = CMD_PUT_STRING;
			break;

		case CMD_ECHO:
			FSM_CMD_state = CMD_ECHO;
			break;

		case CMD_RECEIVED:
			selector = RX_buff.buff[RX_buff.head];
			switch (selector){
			case CHAR_CMD_CLEAR_SCREEN:
				FSM_CMD_state = CMD_CLEAR_SCREEN;
				break;

			case CHAR_CMD_CLEAR_STRING:
				RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
				if (((RX_buff.tail - RX_buff.head + 256) & (RX_BUFFER_SIZE-1))==1) {
					FSM_CMD_state = CMD_CLEAR_STRING;
				} else {
					UART_FlushRXBuffer();
				}
				break;

			case CHAR_CMD_DISPLAY_ON:
				RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
				if (((RX_buff.tail - RX_buff.head + 256) & (RX_BUFFER_SIZE-1))==1) {
					FSM_CMD_state = CMD_DISPLAY_ONOFF;
				} else {
					UART_FlushRXBuffer();
				}
				break;

			case CHAR_CMD_SET_POSITION:
				RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
				if (((RX_buff.tail - RX_buff.head + 256) & (RX_BUFFER_SIZE-1))==3) {
					FSM_CMD_state = CMD_SET_POSITION;
				} else {
					UART_FlushRXBuffer();
				}
				break;

			case CHAR_CMD_CURSOR:
				RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
				FSM_CMD_state = CMD_CURSOR;
				break;

			case CHAR_CMD_BACKLIGHT:
				RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
				FSM_CMD_state = CMD_BACKLIGHT;
				break;

			case CHAR_CMD_CONTRAST:
				RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
				FSM_CMD_state = CMD_CONTRAST;
				break;

			default:
				UART_FlushRXBuffer();
				break;
			}
			break;

		default:
			break;
		}
		break;

	case CMD_CLEAR_SCREEN:
//		LCD_Refresh(0);							// clear screen
		LCD_message = LCD_CLEAR;
		SendMessageWParam(MSG_LCD, &LCD_message);
		LCD_state.state = LCD_DISPLAY_ON;		// turn cursor off
		LCD_state.cur_pos = 0;					// reset print position
		LCD_state.cur_str = LCD_1ST_ROW;		// to 0,0
		UART_FlushRXBuffer();
		FSM_CMD_state = CMD_POLLING;
		break;

	case CMD_CLEAR_STRING:
		if (RX_buff.buff[RX_buff.head] == '0') {
			LCD_Refresh(2);
		} else if (RX_buff.buff[RX_buff.head] == '1') {
			LCD_Refresh(3);
		}
		UART_FlushRXBuffer();
		FSM_CMD_state = CMD_POLLING;
		break;

	case CMD_SET_POSITION:
		ii = RX_buff.buff[RX_buff.head]-0x30;
		if (ii < 2) {
			LCD_state.cur_str = ii;
		}
		RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
		ii = (RX_buff.buff[RX_buff.head]-0x30)*10;
		RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
		ii += (RX_buff.buff[RX_buff.head]-0x30);
		if ((ii>=0) && (ii<LCD_MAX_POS)) {
			LCD_state.cur_pos = ii;
		}
		UART_FlushRXBuffer();
		FSM_CMD_state = CMD_POLLING;
		break;

	case CMD_PUT_STRING:
		ii = LCD_state.cur_pos;
		while ((ii<LCD_MAX_POS) & (RX_buff.head != RX_buff.tail)){
			LCD_PutChar(LCD_state.cur_str, ii, RX_buff.buff[RX_buff.head], 1);
			ii++;
			RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
		}
		LCD_message = LCD_REFRESH;
		SendMessageWParam(MSG_LCD, &LCD_message);
		UART_FlushRXBuffer();
		FSM_CMD_state = CMD_POLLING;
		break;

	case CMD_CURSOR:
		selector2 = RX_buff.buff[RX_buff.head];
		switch (selector2) {
		case CHAR_CMD_CURSOR_HOME:
			LCD_state.cursor_pos = 0;
			break;

		case CHAR_CMD_CURSOR_ON:
			RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
			if (RX_buff.buff[RX_buff.head] == '1') {
				LCD_state.state |= LCD_CURSOR_ON;
			} else if (RX_buff.buff[RX_buff.head] == '0') {
				LCD_state.state = LCD_DISPLAY_ON;
			}
			LCD_Cmd(LCD_state.state);
			break;

		case CHAR_CMD_CURSOR_BLINK:
			RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
			if (RX_buff.buff[RX_buff.head] == '1') {
				LCD_state.state |= LCD_CURSOR_BLINK;
			} else if (RX_buff.buff[RX_buff.head] == '0') {
				LCD_state.state &= ~LCD_CURSOR_BLINK;
			}
			LCD_Cmd(LCD_state.state);
			break;

		case CHAR_CMD_CURSOR_MOVE:
			RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
			if (RX_buff.buff[RX_buff.head] == '1') {
				if (LCD_state.cursor_pos++ > (2*LCD_MAX_POS-1)) {
					LCD_state.cursor_pos = 0;
				}
			} else if (RX_buff.buff[RX_buff.head] == '0') {
				if (LCD_state.cursor_pos==0) {
					LCD_state.cursor_pos = (2*LCD_MAX_POS-1);
				} else {
					LCD_state.cursor_pos--;
				}
			}
			break;

		case CHAR_CMD_CURSOR_POSI:
			RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
			if (((RX_buff.tail - RX_buff.head + 256) & (RX_BUFFER_SIZE-1))==2) {
				ii = (RX_buff.buff[RX_buff.head]-0x30)*10;
				RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
				ii += (RX_buff.buff[RX_buff.head]-0x30);
				if ((ii>=0) && (ii<(2*LCD_MAX_POS))) {
					LCD_state.cursor_pos = ii;
				}
			}
			break;

		default:
			break;
		}
		LCD_message = LCD_REFRESH;
		SendMessageWParam(MSG_LCD, &LCD_message);
		UART_FlushRXBuffer();
		FSM_CMD_state = CMD_POLLING;
		break;

	case CMD_ECHO:
		while (RX_buff.head != RX_buff.tail){
			UART_SendChar(RX_buff.buff[RX_buff.head]);
			RX_buff.head = (RX_buff.head+1)&(RX_BUFFER_SIZE-1);
		}
		UART_SendChar('\r');
		UART_SendChar('\n');
		FSM_CMD_state = CMD_POLLING;
		break;

	case CMD_BACKLIGHT:
		UART_FlushRXBuffer();
		FSM_CMD_state = CMD_POLLING;
		break;

	case CMD_CONTRAST:
		UART_FlushRXBuffer();
		FSM_CMD_state = CMD_POLLING;
		break;

	case CMD_DISPLAY_ONOFF:
		if (RX_buff.buff[RX_buff.head] == '1') {
			LCD_state.state |= LCD_DISPLAY_ON;
		} else if (RX_buff.buff[RX_buff.head] == '0') {
			LCD_state.state = LCD_DISPLAY_OFF;
		}
		LCD_Cmd(LCD_state.state);
		UART_FlushRXBuffer();
		FSM_CMD_state = CMD_POLLING;
		break;

	default:
		break;
	}
}
