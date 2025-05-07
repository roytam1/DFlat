/* ------------- listbox.c ------------ */

#include "dflat.h"

static int ExtendSelections(WINDOW, int, int);
static void TestExtended(WINDOW, PARAM);
static void ClearAllSelections(WINDOW);
static void SetSelection(WINDOW, int);
static void FlipSelection(WINDOW, int);
static void ClearSelection(WINDOW, int);
static void near writeselection(WINDOW, int, int, RECT *);
static void near change_selection(WINDOW, int, int);
static int near selection_in_window(WINDOW, int);

static int py = -1;

static int LeftButtonMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	int my = (int) p2 - GetTop(wnd);
	if (my >= wnd->wlines-wnd->wtop)
		my = wnd->wlines - wnd->wtop;

	if (WindowMoving || WindowSizing)
		return FALSE;
	if (!InsideRect(p1, p2, ClientRect(wnd)))
		return FALSE;
	if (wnd->wlines && my != py)	{
		int sel = wnd->wtop+my-1;
		int sh = getshift();
		if (!(sh & (LEFTSHIFT | RIGHTSHIFT)))	{
			if (!(sh & CTRLKEY))
				ClearAllSelections(wnd);
			wnd->AnchorPoint = sel;
			SendMessage(wnd, PAINT, 0, 0);
		}
		SendMessage(wnd, LB_SELECTION, sel, TRUE);
		py = my;
	}
	return TRUE;
}

static int DoubleClickMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	if (WindowMoving || WindowSizing)
		return FALSE;
	if (wnd->wlines)	{
		RECT rc = ClientRect(wnd);
		BaseWndProc(LISTBOX, wnd, DOUBLE_CLICK, p1, p2);
		if (InsideRect(p1, p2, rc))
			SendMessage(wnd, LB_CHOOSE,
				wnd->selection, 0);
	}
	return TRUE;
}

static void BorderMsg(WINDOW wnd)
{
	if (isMultiLine(wnd))	{
		char ttl[80] = "";
		WINDOW wnd1 = wnd;
		char *cp;
		if (!TestAttribute(wnd1, HASTITLEBAR))	{
			if ((wnd1 = GetParent(wnd)) == NULL)
				return;
			if (!TestAttribute(wnd1, HASTITLEBAR))
				return;
		}
		if (wnd1->title)
			strcpy(ttl, wnd1->title);
		if ((cp = strstr(ttl, " (Add)")) != NULL)
			*cp = '\0';
		if (wnd->AddMode)
			/* ---- in Add mode ---- */
			strcat(ttl, " (Add)");
		InsertTitle(wnd1, ttl);
		if (wnd != wnd1)
			SendMessage(wnd1, BORDER, 0, 0);
	}
}

static int AddTextMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	int rtn = BaseWndProc(LISTBOX, wnd, ADDTEXT, p1, p2);
	if (wnd->selection == -1)
		SendMessage(wnd, LB_SETSELECTION, 0, 0);
	if (*(char *)p1 == LISTSELECTOR)
		wnd->SelectCount++;
	return rtn;
}

static void AddModeKey(WINDOW wnd)
{
	if (isMultiLine(wnd))	{
		wnd->AddMode ^= TRUE;
		SendMessage(GetParent(wnd), ADDSTATUS,
			wnd->AddMode ? ((PARAM) "Add Mode") : 0, 0);
	}
}

static void UpKey(WINDOW wnd, PARAM p2)
{
	if (wnd->selection > 0)	{
		if (wnd->selection == wnd->wtop)	{
			writeselection(wnd, wnd->selection, FALSE, NULL);
			BaseWndProc(LISTBOX, wnd, KEYBOARD, UP, p2);
			PostMessage(wnd, LB_SELECTION, wnd->selection-1,
				isMultiLine(wnd) ? p2 : FALSE);
		}
		else	{
			int newsel = wnd->selection-1;
			if (wnd->wlines == ClientHeight(wnd))
				while (*TextLine(wnd, newsel) == LINE)
					--newsel;
			PostMessage(wnd, LB_SELECTION, newsel,
				isMultiLine(wnd) ? p2 : FALSE);
		}
	}
}

static void DnKey(WINDOW wnd, PARAM p2)
{
	if (wnd->selection < wnd->wlines-1)	{
		if (wnd->selection == wnd->wtop+ClientHeight(wnd)-1)	{
			writeselection(wnd, wnd->selection, FALSE, NULL);
			BaseWndProc(LISTBOX, wnd, KEYBOARD, DN, p2);
			PostMessage(wnd, LB_SELECTION, wnd->selection+1,
				isMultiLine(wnd) ? p2 : FALSE);
		}
		else	{
			int newsel = wnd->selection+1;
			if (wnd->wlines == ClientHeight(wnd))
				while (*TextLine(wnd, newsel) == LINE)
					newsel++;
			PostMessage(wnd, LB_SELECTION, newsel,
				isMultiLine(wnd) ? p2 : FALSE);
		}
	}
}

static void HomePgUpKey(WINDOW wnd, PARAM p1, PARAM p2)
{
	BaseWndProc(LISTBOX, wnd, KEYBOARD, p1, p2);
	PostMessage(wnd, LB_SELECTION, wnd->wtop,
		isMultiLine(wnd) ? p2 : FALSE);
}

static void EndPgDnKey(WINDOW wnd, PARAM p1, PARAM p2)
{
	int bot;
	BaseWndProc(LISTBOX, wnd, KEYBOARD, p1, p2);
	bot = wnd->wtop+ClientHeight(wnd)-1;
	if (bot > wnd->wlines-1)
		bot = wnd->wlines-1;
	PostMessage(wnd, LB_SELECTION, bot,
		isMultiLine(wnd) ? p2 : FALSE);
}

static void SpacebarKey(WINDOW wnd, PARAM p2)
{
	if (isMultiLine(wnd))	{
		int sel = SendMessage(wnd, LB_CURRENTSELECTION, 0, 0);
		if (sel != -1)	{
			if (wnd->AddMode)
				FlipSelection(wnd, sel);
			else 	{
				ClearAllSelections(wnd);
				SetSelection(wnd, sel);
			}
			if (!((int) p2 & (LEFTSHIFT | RIGHTSHIFT)))
				wnd->AnchorPoint = sel;
			if (ItemSelected(wnd, sel))
				ExtendSelections(wnd, sel, (int) p2);
			SendMessage(wnd, PAINT, 0, 0);
		}
	}
}

static void EnterKey(WINDOW wnd)
{
	if (wnd->selection != -1)	{
		SendMessage(wnd, LB_SELECTION, wnd->selection, TRUE);
		SendMessage(wnd, LB_CHOOSE, wnd->selection, 0);
	}
}

static void KeyPress(WINDOW wnd, PARAM p1, PARAM p2)
{
	int sel = wnd->selection+1;
	while (sel < wnd->wlines)	{
		char *cp = TextLine(wnd, sel);
		if (cp == NULL)
			break;
		if (isMultiLine(wnd))
			cp++;
		/* --- special for directory list box --- */
		if (*cp == '[')
			cp++;
		if (tolower(*cp) == (int)p1)	{
			SendMessage(wnd, LB_SELECTION, sel,
				isMultiLine(wnd) ? p2 : FALSE);
			if (!selection_in_window(wnd, sel))	{
				wnd->wtop = sel-ClientHeight(wnd)+1;
				SendMessage(wnd, PAINT, 0, 0);
			}
			break;
		}
		sel++;
	}
}

static int KeyboardMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	switch ((int) p1)	{
		case SHIFT_F8:
			AddModeKey(wnd);
			return TRUE;
		case UP:
			TestExtended(wnd, p2);
			UpKey(wnd, p2);
			return TRUE;
		case DN:
			TestExtended(wnd, p2);
			DnKey(wnd, p2);
			return TRUE;
		case PGUP:
		case HOME:
			TestExtended(wnd, p2);
			HomePgUpKey(wnd, p1, p2);
			return TRUE;
		case PGDN:
		case END:
			TestExtended(wnd, p2);
			EndPgDnKey(wnd, p1, p2);
			return TRUE;
		case ' ':
			SpacebarKey(wnd, p2);
			break;
		case '\r':
			EnterKey(wnd);
			return TRUE;
		default:
			KeyPress(wnd, p1, p2);
			break;
	}
	return FALSE;
}

static void GetTextMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	if ((int)p2 != -1)	{
		char *cp1 = (char *)p1;
		char *cp2 = TextLine(wnd, (int)p2);
		while (cp2 && *cp2 && *cp2 != '\n')
			*cp1++ = *cp2++;
		*cp1 = '\0';
	}
}

static int PaintMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	if (isVisible(wnd) && wnd->wlines)	{
		BaseWndProc(LISTBOX, wnd, PAINT, p1, p2);
		if (isMultiLine(wnd))	{
			int sel = 0;
			for (sel = 0; sel < wnd->wlines; sel++)	{
				if (ItemSelected(wnd, sel))
					writeselection(wnd, sel, TRUE, (RECT *)p1);
			}
		}
		writeselection(wnd, wnd->selection, TRUE, (RECT *)p1);
		return TRUE;
	}
	return FALSE;
}

int ListBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	switch (msg)	{
		case CREATE_WINDOW:
			BaseWndProc(LISTBOX, wnd, msg, p1, p2);
			wnd->selection = -1;
			wnd->AnchorPoint = -1;
			return TRUE;
		case KEYBOARD:
			if (WindowMoving || WindowSizing)
				break;
			if (KeyboardMsg(wnd, p1, p2))
				return TRUE;
			break;
		case BUTTON_RELEASED:
			if (WindowMoving || WindowSizing || HSliding || VSliding)
				break;
			py = -1;
			return TRUE;
		case LEFT_BUTTON:
			if (LeftButtonMsg(wnd, p1, p2) == TRUE)
				return TRUE;
			break;
		case DOUBLE_CLICK:
			if (DoubleClickMsg(wnd, p1, p2))
				return TRUE;
			break;
		case BORDER:
			BorderMsg(wnd);
			break;
		case ADDTEXT:
			return AddTextMsg(wnd, p1, p2);
		case CLEARTEXT:
			wnd->selection = -1;
			wnd->AnchorPoint = -1;
			wnd->SelectCount = 0;
			break;
		case SCROLL:
			BaseWndProc(LISTBOX, wnd, msg, p1, p2);
			if ((int)p2 == FALSE)
				writeselection(wnd, wnd->selection, TRUE, NULL);
			return TRUE;
		case LB_CHOOSE:
			SendMessage(GetParent(wnd), msg, p1, p2);
			break;
		case LB_SETSELECTION:
			change_selection(wnd, (int) p1, 0);
			return TRUE;
		case LB_SELECTION:
			change_selection(wnd, (int) p1, (int) p2);
			SendMessage(GetParent(wnd), LB_SELECTION, wnd->selection, 0);
			return TRUE;
		case LB_CURRENTSELECTION:
			return wnd->selection;
		case LB_GETTEXT:
			GetTextMsg(wnd, p1, p2);
			return TRUE;
		case PAINT:
			if (PaintMsg(wnd, p1, p2))
				return TRUE;
			break;
		case HORIZSCROLL:
			return TRUE;
		case CLOSE_WINDOW:
			if (isMultiLine(wnd) && wnd->AddMode)	{
				wnd->AddMode = FALSE;
				SendMessage(GetParent(wnd), ADDSTATUS, 0, 0);
			}
			break;
		default:
			break;
	}
	return BaseWndProc(LISTBOX, wnd, msg, p1, p2);
}


static void TestExtended(WINDOW wnd, PARAM p2)
{
	if (isMultiLine(wnd) && !wnd->AddMode &&
			!((int) p2 & (LEFTSHIFT | RIGHTSHIFT)))	{
		if (wnd->SelectCount > 1)	{
			ClearAllSelections(wnd);
			SendMessage(wnd, PAINT, 0, 0);
		}
	}
}

static void ClearAllSelections(WINDOW wnd)
{
	if (isMultiLine(wnd) && wnd->SelectCount > 0)	{
		int sel;
		for (sel = 0; sel < wnd->wlines; sel++)
			ClearSelection(wnd, sel);
	}
}

static void FlipSelection(WINDOW wnd, int sel)
{
	if (isMultiLine(wnd))	{
		if (ItemSelected(wnd, sel))
			ClearSelection(wnd, sel);
		else
			SetSelection(wnd, sel);
	}
}

static int ExtendSelections(WINDOW wnd, int sel, int shift)
{	
	if (shift & (LEFTSHIFT | RIGHTSHIFT) &&
						wnd->AnchorPoint != -1)	{
		int i = sel;
		int j = wnd->AnchorPoint;
		int rtn;
		if (j > i)
			swap(i,j);
		rtn = i - j;
		while (j <= i)
			SetSelection(wnd, j++);
		return rtn;
	}
	return 0;
}

static void SetSelection(WINDOW wnd, int sel)
{
	if (isMultiLine(wnd) && !ItemSelected(wnd, sel))	{
		char *lp = TextLine(wnd, sel);
		*lp = LISTSELECTOR;
		wnd->SelectCount++;
	}
}

static void ClearSelection(WINDOW wnd, int sel)
{
	if (isMultiLine(wnd) && ItemSelected(wnd, sel))	{
		char *lp = TextLine(wnd, sel);
		*lp = ' ';
		--wnd->SelectCount;
	}
}

int ItemSelected(WINDOW wnd, int sel)
{
	if (isMultiLine(wnd) && sel < wnd->wlines)	{
		char *cp = TextLine(wnd, sel);
		return (int)((*cp) & 255) == LISTSELECTOR;
	}
	return FALSE;
}

static int near selection_in_window(WINDOW wnd, int sel)
{
	return (wnd->wlines && sel >= wnd->wtop &&
			sel < wnd->wtop+ClientHeight(wnd));
}

static void near writeselection(WINDOW wnd, int sel, int reverse, RECT *rc)
{
	if (isVisible(wnd))
		if (selection_in_window(wnd, sel))
			WriteTextLine(wnd, rc, sel, reverse);
}

static void near change_selection(WINDOW wnd, int sel, int shift)
{
	if (sel != wnd->selection)	{
		if (isMultiLine(wnd))		{
			int sels;
			if (!wnd->AddMode)
				ClearAllSelections(wnd);
			sels = ExtendSelections(wnd, sel, shift);
			if (sels > 1)
				SendMessage(wnd, PAINT, 0, 0);
			if (sels == 0 && !wnd->AddMode)	{
				ClearSelection(wnd, wnd->selection);
				SetSelection(wnd, sel);
				wnd->AnchorPoint = sel;
			}
		}
		writeselection(wnd, wnd->selection,
				isMultiLine(wnd) ?
				ItemSelected(wnd, wnd->selection) :
				FALSE,
				NULL);
		wnd->selection = sel;
		writeselection(wnd, sel,
				(isMultiLine(wnd) && wnd->AddMode) ?
				!ItemSelected(wnd, sel) :
				TRUE,
				NULL);
		
	}
}
