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
					if (TestAttribute(wnd, SHADOW))	{
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
				/* --------- draw a pushed button -------- */
				background = WndBackground(GetParent(wnd));
				foreground = WndBackground(wnd);
				wputch(wnd, ' ', 0, 0);
				for (x = 0; x < WindowWidth(wnd); x++)	{
					wputch(wnd, 220, x+1, 0);
					wputch(wnd, 223, x+1, 1);
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
			if (ct != NULL && GetText(wnd) == NULL)	{
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
	else
		SendMessage(NULL, HIDE_CURSOR, 0, 0);
}

int RadioButtonProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	int rtn;
	DBOX *db = GetParent(wnd)->extension;
	CTLWINDOW *ct = GetControl(wnd);
	if (ct != NULL)	{
		switch (msg)	{
			case SETFOCUS:
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
				break;
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

