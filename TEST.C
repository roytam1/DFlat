/* --------------- memopad.c ----------- */

#include "dflat.h"

char DFlatApplication[] = "Tester";

#ifdef BCPP
/* --- to bypass Borland C++ precompiled header problem --- */
extern int far cdecl _setargv__;
static void far *bozo = &_setargv__;
#endif

char **Argv;

static int EditorProc(WINDOW, MESSAGE, PARAM, PARAM);
static int ListboxProc(WINDOW, MESSAGE, PARAM, PARAM);

static WINDOW wnd, cwnd1, cwnd2;

void main(int argc, char *argv[])
{
    Argv = argv;
    init_messages();
    wnd = CreateWindow(APPLICATION,
                        "D-Flat Test " VERSION,
                        0, 0, -1, -1,
                        NULL,
                        NULL,
                        NULL,
                        MOVEABLE  |
                        SIZEABLE  |
                        HASBORDER |
                        HASSTATUSBAR |
						CONTROLBOX
                        );
    cwnd1 = CreateWindow(EDITBOX,
						"Editbox",
						5,5,10,60,
						NULL,wnd,EditorProc,
                        MOVEABLE  |
                        SIZEABLE  |
                        HASBORDER |
						MULTILINE |
						VSCROLLBAR
                        );
    cwnd2 = CreateWindow(LISTBOX,
						"Listbox",
						10,10,10,30,
						NULL,wnd,ListboxProc,
                        MOVEABLE  |
                        SIZEABLE  |
                        HASBORDER
                        );
    SendMessage(cwnd1, SHOW_WINDOW, TRUE, 0);
    SendMessage(cwnd2, SETFOCUS, TRUE, 0);
	PutWindowLine(cwnd1,
	"Now is the time for all good men to come to the aid of their party",
	2,6);
    while (dispatch_message())
        ;
}

static char *text[] = {
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
	"cccccccccccccccccccccccccccccccccccccccccccccccccc",
	"dddddddddddddddddddddddddddddddddddddddddddddddddd",
	"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
	"ffffffffffffffffffffffffffffffffffffffffffffffffff",
	"gggggggggggggggggggggggggggggggggggggggggggggggggg",
	"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh",
	"iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii",
	"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj",
	"kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk",
	"llllllllllllllllllllllllllllllllllllllllllllllllll",
	"mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm",
	"nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn",
	"oooooooooooooooooooooooooooooooooooooooooooooooooo",
	"pppppppppppppppppppppppppppppppppppppppppppppppppp",
	"qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",
	"rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr",
	"ssssssssssssssssssssssssssssssssssssssssssssssssss",
	"tttttttttttttttttttttttttttttttttttttttttttttttttt",
	"uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu",
	"vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv",
	"wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww",
	"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
	"yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy",
	"zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz",
	NULL
};

static int ListboxProc(WINDOW wnd,MESSAGE msg,PARAM p1,PARAM p2)
{
	char **cp = text;
	int rtn;
    switch (msg)    {
        case CREATE_WINDOW:
		    rtn = DefaultWndProc(wnd, msg, p1, p2);
			while (*cp)
				SendMessage(wnd, ADDTEXT, (PARAM) *cp++, 0);
			return rtn;
		case KEYBOARD:
			if ((int)p1 == UP)
				SendMessage(cwnd1, SCROLL, TRUE, 0);
			else if ((int)p1 == DN)
				SendMessage(cwnd1, SCROLL, FALSE, 0);
			break;
        default:
            break;
    }
    return DefaultWndProc(wnd, msg, p1, p2);
}

static int EditorProc(WINDOW wnd,MESSAGE msg,PARAM p1,PARAM p2)
{
    int rtn;
    switch (msg)    {
        case SETFOCUS:
			if ((int)p1)	{
				char **cp = text;
				SendMessage(wnd, CLEARTEXT, 0, 0);
				while (*cp)
					SendMessage(wnd, ADDTEXT, (PARAM) *cp++, 0);
			}
			break;
        default:
            break;
    }
    return DefaultWndProc(wnd, msg, p1, p2);
}

void PrepFileMenu(void *w, struct Menu *mnu)
{
}

void PrepSearchMenu(void *w, struct Menu *mnu)
{
}

void PrepEditMenu(void *w, struct Menu *mnu)
{
}


