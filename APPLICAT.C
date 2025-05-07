/* ------------- applicat.c ------------- */

#include "dflat.h"

static int ScreenHeight;

extern DBOX Display;
extern DBOX Windows;
extern DBOX Log;

static void ShellDOS(WINDOW);
static void CreateMenu(WINDOW);
static void CreateStatusBar(WINDOW);
static void CloseAll(WINDOW);
static void SelectColors(WINDOW);
static void SetScreenHeight(int);
static void MoreWindows(WINDOW);
static void ChooseWindow(WINDOW, int);
static void SelectTexture(void);
static void SelectBorder(WINDOW);
static void SelectTitle(WINDOW);
static void SelectStatusBar(WINDOW);
static void SelectLines(WINDOW);
static int DisplayModified;

int ApplicationProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	int rtn, WasVisible;
	static int AltDown = FALSE;

	switch (msg)	{
		case CREATE_WINDOW:
			ScreenHeight = SCREENHEIGHT;
			if (!LoadConfig())
				cfg.ScreenLines = ScreenHeight;
			if (!isVGA() && !DisplayModified)	{
				/* ---- modify Display Dialog Box for EGA, CGA ---- */
				CTLWINDOW *ct, *ct1;
				int i;
				ct = FindCommand(&Display, ID_OK, BUTTON);
				if (isEGA())
					ct1 = FindCommand(&Display, ID_50LINES, RADIOBUTTON);
				else	{
					CTLWINDOW *ct2;
					ct2 = FindCommand(&Display, ID_COLOR, RADIOBUTTON) - 1;
					ct2->dwnd.w++;
					for (i = 0; i < 7; i++)
						(ct2+i)->dwnd.x += 8;
					ct1 = FindCommand(&Display, ID_25LINES, RADIOBUTTON) - 1;
				}
				for (i = 0; i < 4; i++)
					*ct1++ = *ct++;
				DisplayModified = TRUE;
			}
			if (cfg.Border)
				SetCheckBox(&Display, ID_BORDER);
			if (cfg.Title)
				SetCheckBox(&Display, ID_TITLE);
			if (cfg.StatusBar)
				SetCheckBox(&Display, ID_STATUSBAR);
			if (cfg.Texture)
				SetCheckBox(&Display, ID_TEXTURE);
			if (cfg.mono == 1)
				PushRadioButton(&Display, ID_MONO);
			else if (cfg.mono == 2)
				PushRadioButton(&Display, ID_REVERSE);
			else
				PushRadioButton(&Display, ID_COLOR);
			if (cfg.ScreenLines == 25)
				PushRadioButton(&Display, ID_25LINES);
			else if (cfg.ScreenLines == 43)
				PushRadioButton(&Display, ID_43LINES);
			else if (cfg.ScreenLines == 50)
				PushRadioButton(&Display, ID_50LINES);
			if (SCREENHEIGHT != cfg.ScreenLines)	{
				SetScreenHeight(cfg.ScreenLines);
				if (WindowHeight(wnd) == ScreenHeight ||
						SCREENHEIGHT-1 < GetBottom(wnd))	{
					WindowHeight(wnd) = SCREENHEIGHT-1;
					GetBottom(wnd) = GetTop(wnd)+WindowHeight(wnd)-1;
					wnd->RestoredRC = WindowRect(wnd);
				}
			}
			SelectColors(wnd);
			SelectBorder(wnd);
			SelectTitle(wnd);
			SelectStatusBar(wnd);
			rtn = BaseWndProc(APPLICATION, wnd, msg, p1, p2);
			if (wnd->extension != NULL)
				CreateMenu(wnd);
			CreateStatusBar(wnd);
			LoadHelpFile();
			SendMessage(NULL, SHOW_MOUSE, 0, 0);
			return rtn;
		case ADDSTATUS:
			if (wnd->StatusBar != NULL)	{
				if (p1 && *(char *)p1)
					SendMessage(wnd->StatusBar, SETTEXT, p1, p2);
				else 
					SendMessage(wnd->StatusBar, CLEARTEXT, 0, 0);
				SendMessage(wnd->StatusBar, PAINT, 0, 0);
			}
			return TRUE;
		case SETFOCUS:
			if ((int)p1 == (inFocus != wnd))	{
				/* ---- setting/clearing focus ------ */
				if ((int)p1)
					SendMessage(inFocus, SETFOCUS, FALSE, 0);
				/* remove window from list */
				RemoveFocusWindow(wnd);
				/* move window to end/beginning of list */
				p1 ? AppendFocusWindow(wnd) : PrependFocusWindow(wnd);
				inFocus = p1 ? wnd : NULL;
				SendMessage(wnd, BORDER, 0, 0);
				return TRUE;
			}
			break;
		case SIZE:
			WasVisible = isVisible(wnd);
			if (WasVisible)
				SendMessage(wnd, HIDE_WINDOW, 0, 0);
			if (p1-GetLeft(wnd) < 30)
				p1 = GetLeft(wnd) + 30;
			BaseWndProc(APPLICATION, wnd, msg, p1, p2);
			CreateMenu(wnd);
			CreateStatusBar(wnd);
			if (WasVisible)
				SendMessage(wnd, SHOW_WINDOW, 0, 0);
			return TRUE;
		case KEYBOARD:
			AltDown = FALSE;
			if (WindowMoving || WindowSizing)
					return BaseWndProc(APPLICATION, wnd, msg, p1, p2);
			switch ((int) p1)	{
				case F1:
					return BaseWndProc(APPLICATION, wnd, msg, p1, p2);
				case ALT_F4:
					PostMessage(wnd, CLOSE_WINDOW, 0, 0);
					return TRUE;
                case ALT_F6:
                    SetNextFocus(inFocus);
                    SkipSystemWindows(FALSE);
                    return TRUE;
				case ALT_HYPHEN:
					BuildSystemMenu(wnd);
					return TRUE;
				default:
					break;
			}
			PostMessage(wnd->MenuBarWnd, msg, p1, p2);
			return TRUE;
		case SHIFT_CHANGED:
			if ((int)p1 & ALTKEY)
				AltDown = TRUE;
			else if (AltDown)	{
				AltDown = FALSE;
				if (((int)p1 & ~ALTKEY) == 0)
					SendMessage(wnd->MenuBarWnd, KEYBOARD,
						wnd->MenuBarWnd == inFocus ? ESC : F10, 0);
			}
			return TRUE;
		case PAINT:
			if (isVisible(wnd))
				ClearWindow(wnd, (RECT *)p1, cfg.Texture ?
					APPLCHAR : ' ');
			return TRUE;
		case COMMAND:
			switch ((int)p1)	{
				case ID_HELP:
					DisplayHelp(wnd, DFlatApplication);
					break;
				case ID_HELPHELP:
					DisplayHelp(wnd, "HelpHelp");
					break;
				case ID_EXTHELP:
					DisplayHelp(wnd, "ExtHelp");
					break;
				case ID_KEYSHELP:
					DisplayHelp(wnd, "KeysHelp");
					break;
				case ID_HELPINDEX:
					DisplayHelp(wnd, "HelpIndex");
					break;
				case ID_LOADHELP:
					LoadHelpFile();
					break;
				case ID_LOG:
					MessageLog(wnd);
					if (CheckBoxSetting(&Log, ID_LOGGING))
						SetCommandToggle(&MainMenu, ID_LOG);
					else
						ClearCommandToggle(&MainMenu, ID_LOG);
					break;
				case ID_DOS:
					ShellDOS(wnd);
					return TRUE;
				case ID_EXIT:
				case ID_SYSCLOSE:
					PostMessage(wnd, CLOSE_WINDOW, 0, 0);
					break;
				case ID_DISPLAY:
					if (DialogBox(wnd, &Display, TRUE, NULL))	{
						SendMessage(wnd, HIDE_WINDOW, 0, 0);
						SelectColors(wnd);
						SelectLines(wnd);
						SelectBorder(wnd);
						SelectTitle(wnd);
						SelectStatusBar(wnd);
						SelectTexture();
						CreateMenu(wnd);
						CreateStatusBar(wnd);
						SendMessage(wnd, SHOW_WINDOW, 0, 0);
					}
					break;
				case ID_SAVEOPTIONS:
					SaveConfig();
					break;
				case ID_WINDOW:
					ChooseWindow(wnd, (int)p2-2);
					break;
				case ID_CLOSEALL:
					CloseAll(wnd);
					break;
				case ID_MOREWINDOWS:
					MoreWindows(wnd);
					break;
				case ID_SYSRESTORE:
				case ID_SYSMOVE:
				case ID_SYSSIZE:
				case ID_SYSMINIMIZE:
				case ID_SYSMAXIMIZE:
					return BaseWndProc(APPLICATION, wnd, msg, p1, p2);
				default:
					if (inFocus != wnd->MenuBarWnd && inFocus != wnd)
						PostMessage(inFocus, msg, p1, p2);
					break;
			}
			return TRUE;
		case CLOSE_WINDOW:	{
			char exmsg[80];
			sprintf(exmsg, "Exit %s?", DFlatApplication);
			if (!YesNoBox(exmsg))
				return FALSE;
			CloseAll(wnd);
			PostMessage(NULL, STOP, 0, 0);
			rtn = BaseWndProc(APPLICATION, wnd, msg, p1, p2);
			if (ScreenHeight != SCREENHEIGHT)
				SetScreenHeight(ScreenHeight);
			UnLoadHelpFile();
			SendMessage(NULL, HIDE_MOUSE, 0, 0);
			return rtn;
		}
		default:
			break;
	}
	return BaseWndProc(APPLICATION, wnd, msg, p1, p2);
}

static void SwitchCursor(void)
{
	SendMessage(NULL, SAVE_CURSOR, 0, 0);
	SwapCursorStack();
	SendMessage(NULL, RESTORE_CURSOR, 0, 0);
}

/* ------- Shell out to DOS ---------- */
static void ShellDOS(WINDOW wnd)
{
	SendMessage(wnd, HIDE_WINDOW, 0, 0);
	SwitchCursor();
	if (ScreenHeight != SCREENHEIGHT)
		SetScreenHeight(ScreenHeight);
	SendMessage(NULL, HIDE_MOUSE, 0, 0);
	printf("To return to %s, execute the DOS exit command.",
					DFlatApplication);
	spawnl(P_WAIT, getenv("COMSPEC"), NULL);
	if (SCREENHEIGHT != cfg.ScreenLines)
		SetScreenHeight(cfg.ScreenLines);
	SwitchCursor();
	SendMessage(wnd, SHOW_WINDOW, 0, 0);
	SendMessage(NULL, SHOW_MOUSE, 0, 0);
}

static void CreateMenu(WINDOW wnd)
{
	AddAttribute(wnd, HASMENUBAR);
	if (wnd->MenuBarWnd != NULL)
		SendMessage(wnd->MenuBarWnd, CLOSE_WINDOW, 0, 0);
	wnd->MenuBarWnd = CreateWindow(MENUBAR,
						NULL,
						GetClientLeft(wnd),
						GetClientTop(wnd)-1,
						1,
						ClientWidth(wnd),
						NULL,
						wnd,
						NULL,
						0);
	SendMessage(wnd->MenuBarWnd, BUILDMENU, (PARAM) wnd->extension, 0);
	AddAttribute(wnd->MenuBarWnd, VISIBLE);
}

static void CreateStatusBar(WINDOW wnd)
{
	if (wnd->StatusBar != NULL)	{
		SendMessage(wnd->StatusBar, CLOSE_WINDOW, 0, 0);
		wnd->StatusBar = NULL;
	}
	if (TestAttribute(wnd, HASSTATUSBAR))	{
		wnd->StatusBar = CreateWindow(STATUSBAR,
							NULL,
							GetClientLeft(wnd),
							GetBottom(wnd),
							1,
							ClientWidth(wnd),
							NULL,
							wnd,
							NULL,
							SAVESELF);
		AddAttribute(wnd->StatusBar, VISIBLE);
	}
}

static char *Menus[9] = {
	"~1.                      ",
	"~2.                      ",
	"~3.                      ",
	"~4.                      ",
	"~5.                      ",
	"~6.                      ",
	"~7.                      ",
	"~8.                      ",
	"~9.                      "
};

static int WindowSel;

static char *WindowName(WINDOW wnd)
{
	if (GetTitle(wnd) == NULL)	{
		if (GetClass(wnd) == DIALOG)
			return ((DBOX *)(wnd->extension))->HelpName;
		else 
			return "Untitled";
	}
	else
		return GetTitle(wnd);
}

static WINDOW oldFocus;

void PrepWindowMenu(void *w, struct Menu *mnu)
{
	WINDOW wnd = w;
	struct PopDown *p0 = mnu->Selections;
	struct PopDown *pd = mnu->Selections + 2;
	struct PopDown *ca = mnu->Selections + 13;
	int MenuNo = 0;
	WINDOW wnd1 = NULL;
	mnu->Selection = 0;
	oldFocus = NULL;
	if (GetClass(wnd) != APPLICATION)	{
		oldFocus = wnd;
		wnd1 = Built.FirstWindow;
		while (wnd1 != NULL && MenuNo < 9)	{
			if (GetClass(wnd1) != MENUBAR &&
					GetClass(wnd1) != STATUSBAR &&
						GetParent(wnd1) == GetParent(wnd))	{
				strncpy(Menus[MenuNo]+4, WindowName(wnd1), 20);
				pd->SelectionTitle = Menus[MenuNo];
				if (wnd1 == oldFocus)	{
					pd->Attrib |= CHECKED;
					mnu->Selection = MenuNo+2;
				}
				else
					pd->Attrib &= ~CHECKED;
				pd++;
				MenuNo++;
			}
			wnd1 = NextWindowBuilt(wnd1);
		}
	}
	if (MenuNo)
		p0->SelectionTitle = "~Close all";
	else
		p0->SelectionTitle = NULL;
	if (wnd1 != NULL)	{
		*pd++ = *ca;
		if (mnu->Selection == 0)
			mnu->Selection = 11;
	}
	pd->SelectionTitle = NULL;
}

static int WindowPrep(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	switch (msg)	{
		case INITIATE_DIALOG:	{
			WINDOW wnd1 = Built.FirstWindow;
			WINDOW cwnd = ControlWindow(&Windows, ID_WINDOWLIST);
			int sel = 0;
			if (cwnd == NULL)
				return FALSE;
			while (wnd1 != NULL)	{
				if (GetClass(wnd1) != MENUBAR &&
						GetClass(wnd1) != STATUSBAR &&
							GetParent(wnd1) == GetParent(wnd) &&
								wnd1 != wnd)	{
					if (wnd1 == oldFocus)
						WindowSel = sel;
					SendMessage(cwnd, ADDTEXT, (PARAM) WindowName(wnd1), 0);
					sel++;
				}
				wnd1 = NextWindowBuilt(wnd1);
			}
			SendMessage(cwnd, LB_SETSELECTION, WindowSel, 0);
			AddAttribute(cwnd, VSCROLLBAR);
			PostMessage(cwnd, SHOW_WINDOW, 0, 0);
			break;
		}
		case COMMAND:
			switch ((int) p1)	{
				case ID_OK:
					if ((int)p2 == 0)
						WindowSel = SendMessage(ControlWindow(&Windows,
									ID_WINDOWLIST),
									LB_CURRENTSELECTION, 0, 0);
					break;
				case ID_WINDOWLIST:
					if ((int) p2 == LB_CHOOSE)
						SendMessage(wnd, COMMAND, ID_OK, 0);
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	return DefaultWndProc(wnd, msg, p1, p2);
}

static void MoreWindows(WINDOW wnd)
{
	if (DialogBox(wnd, &Windows, TRUE, WindowPrep))
		ChooseWindow(wnd, WindowSel);
}

static void ChooseWindow(WINDOW wnd, int WindowNo)
{
	WINDOW wnd1 = GetFirstChild(wnd);
	while (wnd1 != NULL)	{
		if (GetClass(wnd1) != MENUBAR && GetClass(wnd1) != STATUSBAR)
			if (WindowNo-- == 0)
				break;
		wnd1 = GetNextChild(wnd, wnd1);
	}
	if (wnd1 != NULL)	{
		SendMessage(wnd1, SETFOCUS, TRUE, 0);
		if (wnd1->condition == ISMINIMIZED)
			SendMessage(wnd1, RESTORE, 0, 0);
	}
}

static void DoWindowColors(WINDOW wnd)
{
	WINDOW cwnd;
	InitWindowColors(wnd);
	cwnd = GetFirstChild(wnd);
	while (cwnd != NULL)	{
		DoWindowColors(cwnd);
		cwnd = GetNextChild(wnd, cwnd);
	}
}

static void SelectColors(WINDOW wnd)
{
	if (RadioButtonSetting(&Display, ID_MONO))
		cfg.mono = 1;
	else if (RadioButtonSetting(&Display, ID_REVERSE))
		cfg.mono = 2;
	else
		cfg.mono = 0;
	if ((ismono() || video_mode == 2) && cfg.mono == 0)
		cfg.mono = 1;

	if (cfg.mono == 1)
		memcpy(cfg.clr, bw, sizeof bw);
	else if (cfg.mono == 2)
		memcpy(cfg.clr, reverse, sizeof reverse);
	else
		memcpy(cfg.clr, color, sizeof color);
	DoWindowColors(wnd);
}

static void SelectLines(WINDOW wnd)
{
	cfg.ScreenLines = 25;
	if (isEGA() || isVGA())	{
		if (RadioButtonSetting(&Display, ID_43LINES))
			cfg.ScreenLines = 43;
		else if (RadioButtonSetting(&Display, ID_50LINES))
			cfg.ScreenLines = 50;
	}
	if (SCREENHEIGHT != cfg.ScreenLines)	{
		int FullScreen = WindowHeight(wnd) == SCREENHEIGHT;
		SetScreenHeight(cfg.ScreenLines);
		if (FullScreen || SCREENHEIGHT-1 < GetBottom(wnd))
			SendMessage(wnd, SIZE, (PARAM) GetRight(wnd),
				SCREENHEIGHT-1);
	}
}

static void SetScreenHeight(int height)
{
	if (isEGA() || isVGA())	{
		SendMessage(NULL, SAVE_CURSOR, 0, 0);
		switch (height)	{
			case 25:
				Set25();
				break;
			case 43:
				Set43();
				break;
			case 50:
				Set50();
				break;
			default:
				break;
		}
		SendMessage(NULL, RESTORE_CURSOR, 0, 0);
		SendMessage(NULL, RESET_MOUSE, 0, 0);
		SendMessage(NULL, SHOW_MOUSE, 0, 0);
	}
}

static void SelectTexture(void)
{
	cfg.Texture = CheckBoxSetting(&Display, ID_TEXTURE);
}

static void SelectBorder(WINDOW wnd)
{
	cfg.Border = CheckBoxSetting(&Display, ID_BORDER);
	if (cfg.Border)
		AddAttribute(wnd, HASBORDER);
	else
		ClearAttribute(wnd, HASBORDER);
}

static void SelectStatusBar(WINDOW wnd)
{
	cfg.StatusBar = CheckBoxSetting(&Display, ID_STATUSBAR);
	if (cfg.StatusBar)
		AddAttribute(wnd, HASSTATUSBAR);
	else
		ClearAttribute(wnd, HASSTATUSBAR);
}

static void SelectTitle(WINDOW wnd)
{
	cfg.Title = CheckBoxSetting(&Display, ID_TITLE);
	if (cfg.Title)
		AddAttribute(wnd, HASTITLEBAR);
	else
		ClearAttribute(wnd, HASTITLEBAR);
}

static void CloseAll(WINDOW wnd)
{
	WINDOW wnd1 = GetLastChild(wnd);
	SendMessage(wnd, SETFOCUS, TRUE, 0);
	while (wnd1 != NULL)	{
		if (GetClass(wnd1) == MENUBAR || GetClass(wnd1) == STATUSBAR)
			wnd1 = GetPrevChild(wnd, wnd1);
		else	{
			ClearVisible(wnd1);
			SendMessage(wnd1, CLOSE_WINDOW, 0, 0);
			wnd1 = GetLastChild(wnd);
		}
	}
	SendMessage(wnd, PAINT, 0, 0);
}

