/* -------------- combobox.c -------------- */

#include "dflat.h"

int ListProc(WINDOW, MESSAGE, PARAM, PARAM);

int ComboProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	switch (msg)	{
		case CREATE_WINDOW:
			wnd->extension = CreateWindow(LISTBOX,
											NULL,
											wnd->rc.lf,wnd->rc.tp+1,
											wnd->ht-1, wnd->wd+1,
											NULL,
											GetParent(wnd),
											ListProc,
											HASBORDER | NOCLIP | SAVESELF);
			((WINDOW)(wnd->extension))->ct->command = wnd->ct->command;
			wnd->ht = 1;
			wnd->rc.bt = wnd->rc.tp;
			break;
		case PAINT:
			foreground = FrameForeground(wnd);
			background = FrameBackground(wnd);
			wputch(wnd, DOWNSCROLLBOX, WindowWidth(wnd), 0);
			break;
		case KEYBOARD:
			if ((int)p1 == DN)	{
				SendMessage(wnd->extension, SETFOCUS, TRUE, 0);
				return TRUE;
			}
			break;
		case LEFT_BUTTON:
			if ((int)p1 == GetRight(wnd) + 1)
				SendMessage(wnd->extension, SETFOCUS, TRUE, 0);
			break;
		default:
			break;
	}
	return BaseWndProc(COMBOBOX, wnd, msg, p1, p2);
}

int ListProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	DBOX *db = GetParent(wnd)->extension;
	WINDOW cwnd = ControlWindow(db, wnd->ct->command);
	char text[130];
	int rtn;
	WINDOW currFocus;

	switch (msg)	{
		case CREATE_WINDOW:
			wnd->ct = malloc(sizeof(CTLWINDOW));
			wnd->ct->setting = OFF;
			break;
		case SETFOCUS:
			if ((int)p1 == FALSE)	{
				SendMessage(wnd, HIDE_WINDOW, 0, 0);
				wnd->ct->setting = OFF;
			}
			else 
				wnd->ct->setting = ON;
			break;
		case SHOW_WINDOW:
			if (wnd->ct->setting == OFF)
				return TRUE;
			break;
		case BORDER:
			currFocus = inFocus;
			inFocus = NULL;
			rtn = DefaultWndProc(wnd, msg, p1, p2);
			inFocus = currFocus;
			return rtn;
		case LB_SELECTION:
			rtn = DefaultWndProc(wnd, msg, p1, p2);
			SendMessage(wnd, LB_GETTEXT, (PARAM) text, wnd->selection);
			PutItemText(GetParent(wnd), wnd->ct->command, text);
			SendMessage(cwnd, PAINT, 0, 0);
			cwnd->TextChanged = TRUE;
			return rtn;
		case KEYBOARD:
			switch ((int) p1)	{
				case ESC:
				case FWD:
				case BS:
					SendMessage(cwnd, SETFOCUS, TRUE, 0);
					return TRUE;
				default:
					break;
			}
			break;
		case LB_CHOOSE:
			SendMessage(cwnd, SETFOCUS, TRUE, 0);
			return TRUE;
		case CLOSE_WINDOW:
			if (wnd->ct != NULL)
				free(wnd->ct);
			break;
		default:
			break;
	}
	return DefaultWndProc(wnd, msg, p1, p2);
}


