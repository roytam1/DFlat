/* -------------- button.c -------------- */

#include "dflat.h"

int ButtonProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	CTLWINDOW *ct = GetControl(wnd);
	int x;
	if (ct != NULL)	{
		switch (msg)	{
			case SETFOCUS:
				BaseWndProc(BUTTON, wnd, msg, p1, p2);
				/* ------- fall through ------- */
			case PAINT:
				if (isVisible(wnd))	{
					if (TestAttribute(wnd, SHADOW) && cfg.mono == 0)	{
						/* -------- draw the button's shadow ------- */
						background = WndBackground(GetParent(wnd));
						foreground = BLACK;
						for (x = 1; x <= WindowWidth(wnd); x++)
							wputch(wnd, 223, x, 1);
						wputch(wnd, 220, WindowWidth(wnd), 0);
					}
					if (ct->itext != NULL)	{
						unsigned char *txt;
						if ((txt = calloc(1, strlen(ct->itext)+10)) != NULL)	{
							if (ct->setting == OFF)	{
								txt[0] = CHANGECOLOR;
								txt[1] = wnd->WindowColors [HILITE_COLOR] [FG] | 0x80;
								txt[2] = wnd->WindowColors [STD_COLOR] [BG] | 0x80;
							}
							CopyCommand(txt+strlen(txt), ct->itext, !ct->setting,
								WndBackground(wnd));
							SendMessage(wnd, CLEARTEXT, 0, 0);
							SendMessage(wnd, ADDTEXT, (PARAM) txt, 0);
							free(txt);
						}
					}
					/* --------- write the button's text ------- */
					WriteTextLine(wnd, NULL, 0, wnd == inFocus);
				}
				return TRUE;
			case KEYBOARD:
				if (p1 != '\r')
					break;
				/* ---- fall through ---- */
			case LEFT_BUTTON:
				if (cfg.mono == 0)	{
					/* --------- draw a pushed button -------- */
					background = WndBackground(GetParent(wnd));
					foreground = WndBackground(wnd);
					wputch(wnd, ' ', 0, 0);
					for (x = 0; x < WindowWidth(wnd); x++)	{
						wputch(wnd, 220, x+1, 0);
						wputch(wnd, 223, x+1, 1);
					}
				}
				if (msg == LEFT_BUTTON)
					SendMessage(NULL, WAITMOUSE, 0, 0);
				else
					SendMessage(NULL, WAITKEYBOARD, 0, 0);
				SendMessage(wnd, PAINT, 0, 0);
				if (ct->setting == ON)
					PostMessage(GetParent(wnd), COMMAND, ct->command, 0);
				else
					beep();
				return TRUE;
			case HORIZSCROLL:
				return TRUE;
			default:
				break;
		}
	}
	return BaseWndProc(BUTTON, wnd, msg, p1, p2);
}

int TextProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	CTLWINDOW *ct;
	switch (msg)	{
		case PAINT:
			ct = GetControl(wnd);
			if (ct != NULL && ct->itext != NULL && GetText(wnd) == NULL)	{
				int i, len;
				char *cp, *cp2 = ct->itext;

				len = min(ct->dwnd.h, MsgHeight(cp2));
				cp = cp2;
				for (i = 0; i < len; i++)	{
					int mlen;
					char *txt = cp;
					char *cp1 = cp;
					char *np = strchr(cp, '\n');
					if (np != NULL)
						*np = '\0';
					mlen = strlen(cp);
					while ((cp1 = strchr(cp1, SHORTCUTCHAR)) != NULL)	{
						mlen += 3;
						cp1++;
					}
					if (np != NULL)
						*np = '\n';
					if ((txt = malloc(mlen+1)) != NULL)	{
			 			CopyCommand(txt, cp, FALSE, WndBackground(wnd));
						txt[mlen] = '\0';
						SendMessage(wnd, ADDTEXT, (PARAM)txt, 0);
						if ((cp = strchr(cp, '\n')) != NULL)
							cp++;
						free(txt);
					}
				}
			}
			break;
		default:
			break;
	}
	return BaseWndProc(TEXT, wnd, msg, p1, p2);
}

static void SetFocusCursor(WINDOW wnd)
{
	if (wnd == inFocus)	{
		SendMessage(NULL, SHOW_CURSOR, 0, 0);
		SendMessage(wnd, KEYBOARD_CURSOR, 1, 0);
	}
}

int RadioButtonProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	int rtn;
	DBOX *db = GetParent(wnd)->extension;
	CTLWINDOW *ct = GetControl(wnd);
	if (ct != NULL)	{
		switch (msg)	{
			case SETFOCUS:
				if (!(int)p1)
					SendMessage(NULL, HIDE_CURSOR, 0, 0);
			case MOVE:
				rtn = BaseWndProc(RADIOBUTTON, wnd, msg, p1, p2);
				SetFocusCursor(wnd);
				return rtn;
			case PAINT:	{
				char rb[] = "( )";
				if (ct->setting)
					rb[1] = 7;
				SendMessage(wnd, CLEARTEXT, 0, 0);
				SendMessage(wnd, ADDTEXT, (PARAM) rb, 0);
				SetFocusCursor(wnd);
				break;
			}
			case KEYBOARD:
				if ((int)p1 != ' ')
					break;
			case LEFT_BUTTON:
				PushRadioButton(db, ct->command);
				break;
			default:
				break;
		}
	}
	return BaseWndProc(RADIOBUTTON, wnd, msg, p1, p2);
}

int CheckBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	int rtn;
	CTLWINDOW *ct = GetControl(wnd);
	if (ct != NULL)	{
		switch (msg)	{
			case SETFOCUS:
				if (!(int)p1)
					SendMessage(NULL, HIDE_CURSOR, 0, 0);
			case MOVE:
				rtn = BaseWndProc(CHECKBOX, wnd, msg, p1, p2);
				SetFocusCursor(wnd);
				return rtn;
			case PAINT:	{
				char cb[] = "[ ]";
				if (ct->setting)
					cb[1] = 'X';
				SendMessage(wnd, CLEARTEXT, 0, 0);
				SendMessage(wnd, ADDTEXT, (PARAM) cb, 0);
				SetFocusCursor(wnd);
				break;
			}
			case KEYBOARD:
				if ((int)p1 != ' ')
					break;
			case LEFT_BUTTON:
				ct->setting ^= ON;
				SendMessage(wnd, PAINT, 0, 0);
				return TRUE;
			default:
				break;
		}
	}
	return BaseWndProc(CHECKBOX, wnd, msg, p1, p2);
}

int BoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	int rtn;
	CTLWINDOW *ct = GetControl(wnd);
	if (ct != NULL)	{
		switch (msg)	{
			case SETFOCUS:
			case PAINT:
				return FALSE;
			case LEFT_BUTTON:
			case BUTTON_RELEASED:
				return SendMessage(GetParent(wnd), msg, p1, p2);
			case BORDER:
				rtn = BaseWndProc(BOX, wnd, msg, p1, p2);
				if (ct != NULL)
					if (ct->itext != NULL)
						writeline(wnd, ct->itext, 1, 0, FALSE);
				return rtn;
			default:
				break;
		}
	}
	return BaseWndProc(BOX, wnd, msg, p1, p2);
}

int SpinButtonProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	int rtn;
	CTLWINDOW *ct = GetControl(wnd);
	if (ct != NULL)	{
		switch (msg)	{
			case CREATE_WINDOW:
				wnd->wd -= 2;
				wnd->rc.rt -= 2;
				break;
			case SETFOCUS:
				rtn = BaseWndProc(SPINBUTTON, wnd, msg, p1, p2);
				if (!(int)p1)
					SendMessage(NULL, HIDE_CURSOR, 0, 0);
				SetFocusCursor(wnd);
				return rtn;
			case PAINT:
				foreground = FrameForeground(wnd);
				background = FrameBackground(wnd);
				wputch(wnd, UPSCROLLBOX, WindowWidth(wnd), 0);
				wputch(wnd, DOWNSCROLLBOX, WindowWidth(wnd)+1, 0);
				SetFocusCursor(wnd);
				break;
			case LEFT_BUTTON:
				if (p1 == GetRight(wnd) + 1)
					SendMessage(wnd, KEYBOARD, UP, 0);
				else if (p1 == GetRight(wnd) + 2)
					SendMessage(wnd, KEYBOARD, DN, 0);
				if (wnd != inFocus)
					SendMessage(wnd, SETFOCUS, TRUE, 0);
				return TRUE;
			case LB_SETSELECTION:
				rtn = BaseWndProc(SPINBUTTON, wnd, msg, p1, p2);
				wnd->wtop = (int) p1;
				SendMessage(wnd, PAINT, 0, 0);
				return rtn;
			default:
				break;
		}
	}
	return BaseWndProc(SPINBUTTON, wnd, msg, p1, p2);
}

static int WatchIconProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	int rtn;
	switch (msg)	{
		case CREATE_WINDOW:
			rtn = DefaultWndProc(wnd, msg, p1, p2);
			SendMessage(wnd, CAPTURE_MOUSE, 0, 0);
			SendMessage(wnd, HIDE_MOUSE, 0, 0);
			SendMessage(wnd, CAPTURE_KEYBOARD, 0, 0);
			return rtn;
		case PAINT:
			SetStandardColor(wnd);
			writeline(wnd, " À ", 1, 1, FALSE);
			return TRUE;
		case BORDER:
			rtn = DefaultWndProc(wnd, msg, p1, p2);
			writeline(wnd, "Í", 2, 0, FALSE);
			return rtn;
		case MOUSE_MOVED:
			SendMessage(wnd, MOVE, p1, p2);
			return TRUE;
		case CLOSE_WINDOW:
			SendMessage(wnd, RELEASE_MOUSE, 0, 0);
			SendMessage(wnd, RELEASE_KEYBOARD, 0, 0);
			SendMessage(wnd, SHOW_MOUSE, 0, 0);
			break;
		default:
			break;
	}
	return DefaultWndProc(wnd, msg, p1, p2);
}

WINDOW WatchIcon(void)
{
	int mx, my;
	WINDOW wnd;
	SendMessage(NULL, CURRENT_MOUSE_CURSOR, (PARAM) &mx, (PARAM) &my);
	wnd = CreateWindow(
					BOX,
					NULL,
					mx, my, 3, 5,
					NULL,NULL,
					WatchIconProc,
					VISIBLE | HASBORDER | SHADOW | SAVESELF);
	return wnd;
}

static int (*GenericProc)(WINDOW wnd,MESSAGE msg,PARAM p1,PARAM p2);
static BOOL KeepRunning;
static int SliderLen;
static int Percent;
extern DBOX SliderBoxDB;

static void InsertPercent(char *s)
{
	int offset;
	char pcc[5];

	sprintf(s, "%c%c%c",
			CHANGECOLOR,
			color[DIALOG][SELECT_COLOR][FG]+0x80,
			color[DIALOG][SELECT_COLOR][BG]+0x80);
	s += 3;
	memset(s, ' ', SliderLen);
	*(s+SliderLen) = '\0';
	sprintf(pcc, "%d%%", Percent);
	strncpy(s+SliderLen/2-1, pcc, strlen(pcc));
	offset = (SliderLen * Percent) / 100;
	memmove(s+offset+4, s+offset, strlen(s+offset)+1);
	sprintf(pcc, "%c%c%c%c",
			RESETCOLOR,
			CHANGECOLOR,
			color[DIALOG][SELECT_COLOR][BG]+0x80,
			color[DIALOG][SELECT_COLOR][FG]+0x80);
	strncpy(s+offset, pcc, 4);
	*(s + strlen(s) - 1) = RESETCOLOR;
}

static int SliderTextProc(WINDOW wnd,MESSAGE msg,PARAM p1,PARAM p2)
{
	switch (msg)	{
		case PAINT:
			Percent = (int)p2;
			InsertPercent(GetText(wnd) ?
				GetText(wnd) : SliderBoxDB.ctl[1].itext);
			GenericProc(wnd, PAINT, 0, 0);
			if (Percent >= 100)
				SendMessage(GetParent(wnd),COMMAND,ID_CANCEL,0);
			if (!dispatch_message())
				PostMessage(GetParent(wnd), ENDDIALOG, 0, 0);
			return KeepRunning;
		default:
			break;
	}
	return GenericProc(wnd, msg, p1, p2);
}

static int SliderBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	int rtn;
	WINDOW twnd;
	switch (msg)	{
		case CREATE_WINDOW:
			AddAttribute(wnd, SAVESELF);
			rtn = DefaultWndProc(wnd, msg, p1, p2);
			twnd = SliderBoxDB.ctl[1].wnd;
			GenericProc = twnd->wndproc;
			twnd->wndproc = SliderTextProc;
			KeepRunning = TRUE;
			SendMessage(wnd, CAPTURE_MOUSE, 0, 0);
			SendMessage(wnd, CAPTURE_KEYBOARD, 0, 0);
			return rtn;
		case COMMAND:
			if ((int)p2 == 0 && (int)p1 == ID_CANCEL)	{
				if (Percent >= 100 ||
						YesNoBox("Terminate process?"))
					KeepRunning = FALSE;
				else
					return TRUE;
			}
			break;
		case CLOSE_WINDOW:
			SendMessage(wnd, RELEASE_MOUSE, 0, 0);
			SendMessage(wnd, RELEASE_KEYBOARD, 0, 0);
			break;
		default:
			break;
	}
	return DefaultWndProc(wnd, msg, p1, p2);
}

WINDOW SliderBox(int len, char *ttl, char *msg)
{
	SliderLen = len;
	SliderBoxDB.dwnd.title = ttl;
	SliderBoxDB.dwnd.w =
		max(strlen(ttl),max(len, strlen(msg)))+4;
	SliderBoxDB.ctl[0].itext = msg;
	SliderBoxDB.ctl[0].dwnd.w = strlen(msg);
	SliderBoxDB.ctl[0].dwnd.x =
		(SliderBoxDB.dwnd.w - strlen(msg)-1) / 2;
	SliderBoxDB.ctl[1].itext =
		realloc(SliderBoxDB.ctl[1].itext, len+10);
	Percent = 0;
	InsertPercent(SliderBoxDB.ctl[1].itext);
	SliderBoxDB.ctl[1].dwnd.w = len;
	SliderBoxDB.ctl[1].dwnd.x = (SliderBoxDB.dwnd.w-len-1)/2;
	SliderBoxDB.ctl[2].dwnd.x = (SliderBoxDB.dwnd.w-10)/2;
	DialogBox(NULL, &SliderBoxDB, FALSE, SliderBoxProc);
	return SliderBoxDB.ctl[1].wnd;
}

