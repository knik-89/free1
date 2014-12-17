/*
 * FSM.h
 *
 *  Created on: 23.10.2012
 *      Author: user
 */

#ifndef FSM_H_
#define FSM_H_

#include "main.h"

// Short types definition
#define u08 unsigned char
#define	u16 unsigned int
#define u32 unsigned long int

// FSMs' states
#define STATE_DEFAULT		0
// led
#define LED_OFF				0
#define LED_ON				1
// lcd
#define LCD_OFF				0
#define LCD_ON				1
#define LCD_REFRESH			2
#define LCD_CLEAR			3
#define LCD_1ST_ROW			0
#define LCD_2ND_ROW			1
#define LCD_MAX_POS			16
// uart
#define UART_POLLING		0
#define UART_RX_COMPLETE	1
#define UART_TX_COMPLETE	2
#define UART_TX_START		3
// cmd
#define CMD_POLLING			0
#define CMD_CLEAR_SCREEN	1
#define CMD_CLEAR_STRING	2
#define CMD_SET_POSITION	3
#define CMD_PUT_STRING		4
#define CMD_CURSOR			5
#define CMD_ECHO			6
#define CMD_BACKLIGHT		7
#define CMD_CONTRAST		8
#define CMD_DISPLAY_ONOFF	9
#define CMD_RECEIVED		127

#define CHAR_CMD_PUT_STRING		's'
#define CHAR_CMD_ECHO			'e'
#define CHAR_CMD_COMMAND_PREFIX	'c'
#define CHAR_CMD_COMMAND_REBOOT	'R'

#define CHAR_CMD_CLEAR_SCREEN	'c'
#define CHAR_CMD_CLEAR_STRING	's'
#define CHAR_CMD_SET_POSITION	'p'
#define CHAR_CMD_BACKLIGHT		'b'
#define CHAR_CMD_CONTRAST		'k'
#define CHAR_CMD_CURSOR			'x'
#define CHAR_CMD_DISPLAY_ON		'o'

#define CHAR_CMD_CURSOR_ON		'o'
#define CHAR_CMD_CURSOR_BLINK	'b'
#define CHAR_CMD_CURSOR_MOVE	'm'
#define CHAR_CMD_CURSOR_HOME	'h'
#define CHAR_CMD_CURSOR_POSI	'p'

// FSMs' states
volatile u08 FSM_LED_state;
volatile u08 FSM_LCD_state;
volatile u08 FSM_UART_state;
volatile u08 FSM_CMD_state;

// Command processor variables
struct lcd_state_struct{
	u08 cur_str;
	u08 cur_pos;
	u08 cursor_pos;
	u08 state;
	u08 backlight_PWM;
	u08 contrast_PWM;
} LCD_state;

// FSMs functions
/// init
void FSM_LED_Init(void);
void FSM_LCD_Init(void);
void FSM_UART_Init(void);
void FSM_CMD_Init(void);
/// process
void FSM_LED(void);
void FSM_LCD(void);
void FSM_UART(void);
void FSM_CMD(void);

#endif /* FSM_H_ */
