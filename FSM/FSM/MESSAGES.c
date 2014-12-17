/*
 * messages.c
 *
 *  Created on: 23.05.2012
 *      Author: Masakra
 */

#include "MESSAGES.h"

typedef struct {
	u08 Msg;
	void *ParamPtr;
} MSG_DATA;

volatile MSG_DATA Messages[MAX_MESSAGES];

#ifdef USE_BROADCAST_MESSAGES
volatile u08 BroadcastMessages[MAX_BROADCAST_MESSAGES];
#endif

void InitMessages(void) {
	u08 i;
	for (i=0; i< MAX_MESSAGES; i++) {
		Messages[i].Msg = 0;
	}
#ifdef USE_BROADCAST_MESSAGES
	for (i=0; i<MAX_BROADCAST_MESSAGES; i++) {
		BroadcastMessages[i] = 0;
	}
#endif
}

void SendMessageWParam(u08 Msg, u08 * ParamPtr) {
	if (Messages[Msg].Msg == 0) {
		Messages[Msg].Msg = 1;
		Messages[Msg].ParamPtr = ParamPtr;
	}
}

void SendMessageWOParam(u08 Msg) {
	if (Messages[Msg].Msg == 0) {
		Messages[Msg].Msg = 1;
	}
}

u08 GetMessage(u08 Msg) {
	if (Messages[Msg].Msg == 2) {
		Messages[Msg].Msg = 0;
		return 1;
	}
	return 0;
}

u08 *GetMessageParam(u08 Msg) {
	return Messages[Msg].ParamPtr;
}

#ifdef USE_BROADCAST_MESSAGES
void SendBroadcastMessage(u08 Msg) {
	if (BroadcastMessages[Msg] == 0) {
		BroadcastMessages[Msg] = 1;
	}
}

u08 GetBroadcastMessage(u08 Msg) {
	if (BroadcastMessages[Msg] == 2) {
		return 1;
	}
	return 0;
}
#endif

void ProcessMessages(void) {
	u08 i;
	for (i=0; i< MAX_MESSAGES; i++) {
		if (Messages[i].Msg == 2) Messages[i].Msg = 0;
		if (Messages[i].Msg == 1) Messages[i].Msg = 2;
	}
#ifdef USE_BROADCAST_MESSAGES
	for (i=0; i< MAX_BROADCAST_MESSAGES; i++) {
		if (BroadcastMessages[i] == 2) BroadcastMessages[i] = 0;
		if (BroadcastMessages[i] == 1) BroadcastMessages[i] = 2;
	}
#endif
}
