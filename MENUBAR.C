/* ---------------- menubar.c -------------- */

#include "dflat.h"

static void reset_menubar(WINDOW);

static struct {
	int x1, x2;		/* position in menu bar */
	char sc;		/* shortcut key value   */
} menu[10];
static int mctr;

MBAR *ActiveMenuBar;
static MENU *ActiveMenu;

static WINDOW mwnd;
static int Selecting;

static WINDOW Cascaders[MAXCASCADES];
static int casc;
static WINDOW GetDocFocus(WINDOW);

static void SetFocusMsg(WINDOW wnd, PARAM p1)
{
	if ((int) p1)	{
		if (ActiveMenuBar->ActiveSelection == -1)
			ActiveMenuBar->ActiveSelection = 0;
		if (inFocus == wnd)
			SendMessage(wnd, PAINT, 0, 0);
	}
	else	{
		SendMessage(wnd, PAINT, 0, 0);
		SendMessage(GetParent(wnd), ADDSTATUS, 0, 0);
	}
}

static void CommandMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	if (isCascadedCommand(ActiveMenuBar, (int)p1))	{
		/* find the cascaded menu based on command id in p1 */
		MENU *mnu = ActiveMenu+mctr;
		while (mnu->Title != (void *)-1)	{
			if (mnu->CascadeId == (int) p1)	{
				if (casc < MAXCASCADES)	{
					Cascaders[casc++] = mwnd;
					SendMessage(wnd, SELECTION,
						(PARAM)(mnu-ActiveMenu), TRUE);
				}
				break;
			}
			mnu++;
		}
	}
	else 	{
		if (mwnd != NULL)
			SendMessage(mwnd, CLOSE_WINDOW, 0, 0);
		SendMessage(GetDocFocus(wnd), SETFOCUS, TRUE, 0);
		PostMessage(GetParent(wnd), COMMAND, p1, p2);
	}
}

static void BuildMenuMsg(WINDOW wnd, PARAM p1)
{
	int offset = 3;
	reset_menubar(wnd);
	mctr = 0;
	ActiveMenuBar = (MBAR *) p1;
	ActiveMenu = ActiveMenuBar->PullDown;
	while (ActiveMenu->Title != NULL && ActiveMenu->Title != (void*)-1)	{
		char *cp;
		if (strlen(GetText(wnd)+offset) < strlen(ActiveMenu->Title)+3)
			break;
		GetText(wnd) = realloc(GetText(wnd), strlen(GetText(wnd))+5);
		memmove(GetText(wnd) + offset+4, GetText(wnd) + offset,
				strlen(GetText(wnd))-offset+1);
		CopyCommand(GetText(wnd)+offset, ActiveMenu->Title, FALSE,
				wnd->WindowColors [STD_COLOR] [BG]);
		menu[mctr].x1 = offset;
		offset += strlen(ActiveMenu->Title) + (3+MSPACE);
		menu[mctr].x2 = offset-MSPACE;
		cp = strchr(ActiveMenu->Title, SHORTCUTCHAR);
		if (cp)
			menu[mctr].sc = tolower(*(cp+1));
		mctr++;
		ActiveMenu++;
	}
	ActiveMenu = ActiveMenuBar->PullDown;
}

static void PaintMsg(WINDOW wnd)
{
	if (wnd == inFocus)
		SendMessage(GetParent(wnd), ADDSTATUS, 0, 0);
	SetStandardColor(wnd);
    wputs(wnd, GetText(wnd), 0, 0);
	if (ActiveMenuBar->ActiveSelection != -1 &&
			(wnd == inFocus || mwnd != NULL))	{
		char *sel;
		char *cp;
		if ((sel = malloc(200)) != NULL)	{
			int offset = menu[ActiveMenuBar->ActiveSelection].x1;
			int offset1 = menu[ActiveMenuBar->ActiveSelection].x2;
			GetText(wnd)[offset1] = '\0';
			SetReverseColor(wnd);
			memset(sel, '\0', 200);
			strcpy(sel, GetText(wnd)+offset);
			cp = strchr(sel, CHANGECOLOR);
			if (cp != NULL)
				*(cp + 2) = background | 0x80;
			wputs(wnd, sel, offset-ActiveMenuBar->ActiveSelection*4, 0);
			GetText(wnd)[offset1] = ' ';
			if (!Selecting && mwnd == NULL && wnd == inFocus)	{
				char *st = ActiveMenu[ActiveMenuBar->ActiveSelection].StatusText;
				if (st != NULL)
					SendMessage(GetParent(wnd), ADDSTATUS, (PARAM)st, 0);
			}
			free(sel);
		}
	}
}

static void KeyboardMsg(WINDOW wnd, PARAM p1)
{
	MENU *mnu;
	if (mwnd == NULL)	{
		/* ----- search for menu bar shortcut keys ---- */
		int c = tolower((int)p1);
		int a = AltConvert((int)p1);
		int j;
		for (j = 0; j < mctr; j++)	{
			if ((inFocus == wnd && menu[j].sc == c) ||
					(a && menu[j].sc == a))	{
				SendMessage(wnd, SELECTION, j, 0);
				return;
			}
		}
	}
	/* -------- search for accelerator keys -------- */
	mnu = ActiveMenu;
	while (mnu->Title != NULL)	{
		struct PopDown *pd = mnu->Selections;
		if (mnu->PrepMenu)
			(*(mnu->PrepMenu))(GetDocFocus(wnd), mnu);
		while (pd->SelectionTitle != NULL)	{
			if (pd->Accelerator == (int) p1)	{
				if (pd->Attrib & INACTIVE)
					beep();
				else	{
					if (pd->Attrib & TOGGLE)
						pd->Attrib ^= CHECKED;
					SendMessage(GetDocFocus(wnd), SETFOCUS, TRUE, 0);
					PostMessage(GetParent(wnd),
						COMMAND, pd->ActionId, 0);
				}
				return;
			}
			pd++;
		}
		mnu++;
	}
	switch ((int)p1)	{
		case F1:
			if (ActiveMenu != NULL &&
				(mwnd == NULL ||
				(ActiveMenu+ActiveMenuBar->ActiveSelection)->
					Selections[0].SelectionTitle == NULL)) {
				DisplayHelp(wnd, (ActiveMenu+ActiveMenuBar->ActiveSelection)->Title+1);
				return;
			}
			break;
		case '\r':
			if (mwnd == NULL && ActiveMenuBar->ActiveSelection != -1)
				SendMessage(wnd, SELECTION, ActiveMenuBar->ActiveSelection, 0);
			break;
		case F10:
			if (wnd != inFocus && mwnd == NULL)	{
				SendMessage(wnd, SETFOCUS, TRUE, 0);
				break;
			}
			/* ------- fall through ------- */
		case ESC:
			if (inFocus == wnd && mwnd == NULL)	{
				ActiveMenuBar->ActiveSelection = -1;
				SendMessage(GetDocFocus(wnd), SETFOCUS, TRUE, 0);
				SendMessage(wnd, PAINT, 0, 0);
			}
			break;
		case FWD:
			ActiveMenuBar->ActiveSelection++;
			if (ActiveMenuBar->ActiveSelection == mctr)
				ActiveMenuBar->ActiveSelection = 0;
			if (mwnd != NULL)
				SendMessage(wnd, SELECTION, ActiveMenuBar->ActiveSelection, 0);
			else 
				SendMessage(wnd, PAINT, 0, 0);
			break;
		case BS:
			if (ActiveMenuBar->ActiveSelection == 0)
				ActiveMenuBar->ActiveSelection = mctr;
			--ActiveMenuBar->ActiveSelection;
			if (mwnd != NULL)
				SendMessage(wnd, SELECTION, ActiveMenuBar->ActiveSelection, 0);
			else 
				SendMessage(wnd, PAINT, 0, 0);
			break;
		default:
			break;
	}
}

static void LeftButtonMsg(WINDOW wnd, PARAM p1)
{
	int i = BarSelection((int) p1 - GetLeft(wnd));
	if (i < mctr)
		if (i != ActiveMenuBar->ActiveSelection || mwnd == NULL)
			SendMessage(wnd, SELECTION, i, 0);
}

static void SelectionMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	int offset = 3, wd, mx, my;
	MENU *mnu;

	Selecting = TRUE;
	mnu = ActiveMenu+(int)p1;
	if (mnu->PrepMenu != NULL)
		(*(mnu->PrepMenu))(GetDocFocus(wnd), mnu);
	wd = MenuWidth(mnu->Selections);
	if (p2)	{
		mx = GetLeft(inFocus) +	WindowWidth(inFocus) - 1;
		my = GetTop(inFocus) + inFocus->selection;
	}
	else	{
		if (mwnd != NULL)	{
			SendMessage(wnd, SETFOCUS, TRUE, 0);
			SendMessage(mwnd, CLOSE_WINDOW, 0, 0);
		}
		ActiveMenuBar->ActiveSelection = (int) p1;
		offset = menu[ActiveMenuBar->ActiveSelection].x1 -
					4 * ActiveMenuBar->ActiveSelection;
		if (offset > WindowWidth(wnd)-wd)
			offset = WindowWidth(wnd)-wd;
		mx = GetLeft(wnd)+offset;
		my = GetTop(wnd)+1;
	}
	mwnd = CreateWindow(POPDOWNMENU, NULL,
				mx, my,
				MenuHeight(mnu->Selections),
				wd,
				NULL,
				wnd,
				NULL,
				0);
	AddAttribute(mwnd, SHADOW);
	if (mnu->Selections[0].SelectionTitle != NULL)	{
		SendMessage(mwnd, BUILD_SELECTIONS, (PARAM) mnu, 0);
		SendMessage(mwnd, SETFOCUS, TRUE, 0);
	}
	else
		SendMessage(wnd, PAINT, 0, 0);
	Selecting = FALSE;
}

static void ClosePopdownMsg(WINDOW wnd, PARAM p1)
{
	if (casc > 0)
		SendMessage(Cascaders[--casc], CLOSE_WINDOW, p1, 0);
	else 	{
		mwnd = NULL;
		ActiveMenuBar->ActiveSelection = -1;
		if (!Selecting)
			SendMessage(GetDocFocus(wnd), SETFOCUS, TRUE, 0);
		SendMessage(wnd, PAINT, 0, 0);
	}
}

static void CloseWindowMsg(WINDOW wnd)
{
	if (GetText(wnd) != NULL)	{
		free(GetText(wnd));
		GetText(wnd) = NULL;
	}
	mctr = 0;
	ActiveMenuBar->ActiveSelection = -1;
	ActiveMenu = NULL;
	ActiveMenuBar = NULL;
}
	
int MenuBarProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	int rtn;

	switch (msg)	{
		case CREATE_WINDOW:
			reset_menubar(wnd);
			break;
		case SETFOCUS:
			rtn = BaseWndProc(MENUBAR, wnd, msg, p1, p2);
			SetFocusMsg(wnd, p1);
			return rtn;
		case COMMAND:
			CommandMsg(wnd, p1, p2);
			return TRUE;
		case BUILDMENU:
			BuildMenuMsg(wnd, p1);
			break;
		case PAINT:	
			if (!isVisible(wnd) || GetText(wnd) == NULL)
				break;
			PaintMsg(wnd);
			return FALSE;
		case KEYBOARD:
			KeyboardMsg(wnd, p1);
			return TRUE;
		case LEFT_BUTTON:
			LeftButtonMsg(wnd, p1);
			return TRUE;
		case SELECTION:
			SelectionMsg(wnd, p1, p2);
			break;
		case BORDER:
			return TRUE;
		case INSIDE_WINDOW:
			return InsideRect(p1, p2, WindowRect(wnd));
		case CLOSE_POPDOWN:
			ClosePopdownMsg(wnd, p1);
			return TRUE;
		case CLOSE_WINDOW:
			rtn = BaseWndProc(MENUBAR, wnd, msg, p1, p2);
			CloseWindowMsg(wnd);
			return rtn;
		default:
			break;
	}
	return BaseWndProc(MENUBAR, wnd, msg, p1, p2);
}

static WINDOW GetDocFocus(WINDOW wnd)
{
	WINDOW DocFocus = Focus.LastWindow;
	CLASS cl;
	while ((cl = GetClass(DocFocus)) == MENUBAR ||
				cl == POPDOWNMENU ||
					cl == STATUSBAR ||
						cl == APPLICATION)					{
		if ((DocFocus = PrevWindow(DocFocus)) == NULL)	{
			DocFocus = GetParent(wnd);
			break;
		}
	}
	return DocFocus;
}

static void reset_menubar(WINDOW wnd)
{
	if ((GetText(wnd) = realloc(GetText(wnd), SCREENWIDTH+5)) != NULL)	{
		memset(GetText(wnd), ' ', SCREENWIDTH);
		*(GetText(wnd)+WindowWidth(wnd)) = '\0';
	}
}

int BarSelection(int mx)
{
	int i;
	for (i = 0; i < mctr; i++)
		if (mx >= menu[i].x1-4*i &&
				mx <= menu[i].x2-4*i-5)
			break;
	return i;
}


