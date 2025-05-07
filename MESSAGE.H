/* ----------- message.h ------------ */

#ifndef MESSAGES_H
#define MESSGAES_H

#define MAXMESSAGES 50
#define DELAYTICKS 1
#define FIRSTDELAY 7
#define DOUBLETICKS 5

typedef enum messages {
	#undef DFlatMsg
	#define DFlatMsg(m) m,
	#include "dflatmsg.h"
	MESSAGECOUNT
} MESSAGE;

/* --------- message prototypes ----------- */
void init_messages(void);
void PostMessage(WINDOW, MESSAGE, PARAM, PARAM);
int SendMessage(WINDOW, MESSAGE, PARAM, PARAM);
int dispatch_message(void);
int TestCriticalError(void);

#endif
