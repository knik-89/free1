/*
 * messages.h
 *
 *  Created on: 23.05.2012
 *      Author: Masakra
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

//#define USE_BROADCAST_MESSAGES

#define MAX_MESSAGES 			4

/// Messages definition
#define							0
#define MSG_LCD					1
#define MSG_UART				2
#define MSG_CMD					3

#ifdef USE_BROADCAST_MESSAGES
	#define MAX_BROADCAST_MESSAGES 	4

	#define B_MSG_1					0
	#define B_MSG_2					1
	#define B_MSG_3					2
	#define B_MSG_4					3
#endif

#define u08 unsigned char
#define	u16 unsigned int

void InitMessages(void);
void ProcessMessages(void);
void SendMessageWParam(u08 Msg, u08 * ParamPtr);
void SendMessageWOParam(u08 Msg);
u08  GetMessage(u08 Msg);
u08 *GetMessageParam(u08 Msg);
#ifdef USE_BROADCAST_MESSAGES
	void SendBroadcastMessage(u08 Msg);
	u08  GetBroadcastMessage(u08 Msg);
#endif

#endif /* MESSAGES_H_ */
