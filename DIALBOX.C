/* ----------------- dialbox.c -------------- */

#include "dflat.h"

static char path[MAXPATH];
static char drive[MAXDRIVE] = " :";
static char dir[MAXDIR];
static char name[MAXFILE];
static char ext[MAXEXT];

static int inFocusCommand(DBOX *);
static void dbShortcutKeys(DBOX *, int);
static int ControlProc(WINDOW, MESSAGE, PARAM, PARAM);
static void ChangeFocus(WINDOW, int);
static CTLWINDOW *AssociatedControl(DBOX *, enum commands);
static void SetRadioButton(DBOX *, CTLWINDOW *);

static int SysMenuOpen;

int DialogProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	DBOX *db = wnd->extension;
	CTLWINDOW *ct;
	WINDOW cwnd;
	int rtn;

	switch (msg)	{
		case CREATE_WINDOW:
			rtn = BaseWndProc(DIALOG, wnd, msg, p1, p2);
			ct = db->ctl;
			while (ct->class)	{
				int attrib = VISIBLE;
				if (TestAttribute(wnd, NOCLIP))
					attrib |= NOCLIP;
				if (wnd->Modal)
					attrib |= SAVESELF;
				ct->vtext = ct->itext;
				ct->setting = ct->isetting;
				if (ct->class == EDITBOX && ct->dwnd.h > 1)
					attrib |= (MULTILINE | HASBORDER);
				else if (ct->class == LISTBOX || ct->class == TEXTBOX)
					attrib |= HASBORDER;
				cwnd = CreateWindow(ct->class,
					 		ct->dwnd.title,
					 		ct->dwnd.x+GetClientLeft(wnd),
					 		ct->dwnd.y+GetClientTop(wnd),
					 		ct->dwnd.h,
					 		ct->dwnd.w,
					 		ct,
					 		wnd,
					 		ControlProc,
					 		attrib);
				if ((ct->class == EDITBOX || ct->class == COMBOBOX) &&
						ct->itext != NULL)
					SendMessage(cwnd, EB_PUTTEXT, (PARAM) ct->itext, 0);
				if (ct->class != BOX &&
					ct->class != TEXT &&
						wnd->dFocus == NULL)
					wnd->dFocus = ct;
				ct++;
			}
			return rtn;
		case SETFOCUS:
			if (wnd->Modal)	{
				if (p1)
					SendMessage(inFocus, SETFOCUS, FALSE, 0);
				inFocus = p1 ? wnd : NULL;
				return TRUE;
			}
			break;
		case SHIFT_CHANGED:
			if (wnd->Modal)
				return TRUE;
			break;
		case LEFT_BUTTON:
			if (WindowSizing || WindowMoving)
				return FALSE;
			if (HitControlBox(wnd, p1-GetLeft(wnd), p2-GetTop(wnd)))	{
				PostMessage(wnd, KEYBOARD, ' ', ALTKEY);
				return TRUE;
			}
			ct = db->ctl;
			while (ct->class)	{
				WINDOW cwnd = ct->wnd;
				if (ct->class == COMBOBOX)	{
					if (p2 == GetTop(cwnd))	{
						if (p1 == GetRight(cwnd)+1)	{
							SendMessage(cwnd, msg, p1, p2);
							return TRUE;
						}
					}
					if (GetClass(inFocus) == LISTBOX)
						SendMessage(wnd, SETFOCUS, TRUE, 0);
				}
				else if (ct->class == SPINBUTTON)	{
					if (p2 == GetTop(cwnd))	{
						if (p1 == GetRight(cwnd)+1 ||
								p1 == GetRight(cwnd)+2)	{
							SendMessage(cwnd, msg, p1, p2);
							return TRUE;
						}
					}
				}
				ct++;
			}
			break;
		case KEYBOARD:
			if (WindowMoving || WindowSizing)
				break;
			switch ((int)p1)	{
				case F1:
					ct = wnd->dFocus;
					if (ct != NULL)
						if (DisplayHelp(wnd, ct->help))
							return TRUE;
					break;
				case CTRL_FIVE:		/* same as SHIFT-TAB */
					if ((int)p2 & (LEFTSHIFT | RIGHTSHIFT))
						ChangeFocus(wnd, FALSE);
					break;
				case BS:
				case UP:
					ChangeFocus(wnd, FALSE);
					break;
				case ALT_F6:
				case '\t':
				case FWD:
				case DN:
					ChangeFocus(wnd, TRUE);
					break;
				case ' ':
					if (((int)p2 & ALTKEY) && TestAttribute(wnd, CONTROLBOX))	{
						SysMenuOpen = TRUE;
						BuildSystemMenu(wnd);
					}
					break;
				case CTRL_F4:
				case ESC:
					SendMessage(wnd, COMMAND, ID_CANCEL, 0);
					break;
				default:
					/* ------ search all the shortcut keys ----- */
					dbShortcutKeys(db, (int) p1);
					break;
			}
			if (wnd->Modal)
				return TRUE;
			break;
		case CLOSE_POPDOWN:
			SysMenuOpen = FALSE;
			break;
		case LB_SELECTION:
		case LB_CHOOSE:
			if (SysMenuOpen)
				return TRUE;
			SendMessage(wnd, COMMAND, inFocusCommand(db), msg);
			break;
		case COMMAND:
			switch ((int) p1)	{
				case ID_OK:
				case ID_CANCEL:
					if ((int)p2 != 0)
						return TRUE;
					wnd->ReturnCode = (int) p1;
					if (wnd->Modal)
						PostMessage(wnd, ENDDIALOG, 0, 0);
					else
						SendMessage(wnd, CLOSE_WINDOW, TRUE, 0);
					return TRUE;
				case ID_HELP:
					if ((int)p2 != 0)
						return TRUE;
					return DisplayHelp(wnd, db->HelpName);
				default:
					break;
			}
			break;
		case CLOSE_WINDOW:
			if (!p1)	{
				SendMessage(wnd, COMMAND, ID_CANCEL, 0);
				return TRUE;
			}
			break;
		default:
			break;
	}
	return BaseWndProc(DIALOG, wnd, msg, p1, p2);
}

int DialogBox(WINDOW wnd, DBOX *db, int Modal,
	int (*wndproc)(struct window *, enum messages, PARAM, PARAM))
{
	int rtn;
	CTLWINDOW *ct;
	WINDOW oldFocus = inFocus;
	WINDOW DialogWnd = CreateWindow(DIALOG,
						db->dwnd.title,
						db->dwnd.x,
						db->dwnd.y,
						db->dwnd.h,
						db->dwnd.w,
						db,
						wnd,
						wndproc,
						Modal ? SAVESELF : 0);
	DialogWnd->Modal = Modal;
	SendMessage(((CTLWINDOW *)(DialogWnd->dFocus))->wnd, SETFOCUS, TRUE, 0);
	SendMessage(DialogWnd, SHOW_WINDOW, 0, 0);
	SendMessage(DialogWnd, INITIATE_DIALOG, 0, 0);
	if (Modal)	{
		SendMessage(DialogWnd, CAPTURE_MOUSE, 0, 0);
		SendMessage(DialogWnd, CAPTURE_KEYBOARD, 0, 0);
		while (dispatch_message())
			;
		rtn = DialogWnd->ReturnCode == ID_OK;
		SendMessage(DialogWnd, RELEASE_MOUSE, 0, 0);
		SendMessage(DialogWnd, RELEASE_KEYBOARD, 0, 0);
		SendMessage(inFocus, SETFOCUS, FALSE, 0);
		SendMessage(DialogWnd, CLOSE_WINDOW, TRUE, 0);
		SendMessage(oldFocus, SETFOCUS, TRUE, 0);
		if (rtn)	{
			ct = db->ctl;
			while (ct->class)	{
				ct->wnd = NULL;
				if (ct->class == RADIOBUTTON || ct->class == CHECKBOX)
					ct->isetting = ct->setting;
				ct++;
			}
		}
		return rtn;
	}
	return FALSE;
}

static int inFocusCommand(DBOX *db)
{
	CTLWINDOW *ct = db->ctl;
	while (ct->class)	{
		if (ct->wnd == inFocus)
			return ct->command;
		ct++;
	}
	return -1;
}

CTLWINDOW *FindCommand(DBOX *db, enum commands cmd, int class)
{
	CTLWINDOW *ct = db->ctl;
	while (ct->class)	{
		if (ct->class == class)
			if (cmd == ct->command)
				return ct;
		ct++;
	}
	return NULL;
}

WINDOW ControlWindow(DBOX *db, enum commands cmd)
{
	CTLWINDOW *ct = db->ctl;
	while (ct->class)	{
		if (ct->class != TEXT && cmd == ct->command)
			return ct->wnd;
		ct++;
	}
	return NULL;
}

void PushRadioButton(DBOX *db, enum commands cmd)
{
	CTLWINDOW *ct = FindCommand(db, cmd, RADIOBUTTON);
	if (ct != NULL)	{
		SetRadioButton(db, ct);
		ct->isetting = ON;
	}
}

static struct {
	CTLWINDOW *rct;
} rbs[MAXRADIOS];

static void SetRadioButton(DBOX *db, CTLWINDOW *ct)
{
	CTLWINDOW *ctt = db->ctl;
	int i;

	/* --- clear all the radio buttons
				in this group on the dialog box --- */

	/* -------- build a table of all radio buttons at the
			same x vector ---------- */
	for (i = 0; i < MAXRADIOS; i++)
		rbs[i].rct = NULL;
	while (ctt->class)	{
		if (ctt->class == RADIOBUTTON)
			if (ct->dwnd.x == ctt->dwnd.x)
				rbs[ctt->dwnd.y].rct = ctt;
		ctt++;
	}

	/* ----- find the start of the radiobutton group ---- */
	i = ct->dwnd.y;
	while (i >= 0 && rbs[i].rct != NULL)
		--i;
	/* ---- ignore everthing before the group ------ */
	while (i >= 0)
		rbs[i--].rct = NULL;

	/* ----- find the end of the radiobutton group ---- */
	i = ct->dwnd.y;
	while (i < MAXRADIOS && rbs[i].rct != NULL)
		i++;
	/* ---- ignore everthing past the group ------ */
	while (i < MAXRADIOS)
		rbs[i++].rct = NULL;

	for (i = 0; i < MAXRADIOS; i++)	{
		if (rbs[i].rct != NULL)	{
			int wason = rbs[i].rct->setting;
			rbs[i].rct->setting = OFF;
			if (wason)
				SendMessage(rbs[i].rct->wnd, PAINT, 0, 0);
		}
	}
	ct->setting = ON;
	SendMessage(ct->wnd, PAINT, 0, 0);
}

int RadioButtonSetting(DBOX *db, enum commands cmd)
{
	CTLWINDOW *ct = FindCommand(db, cmd, RADIOBUTTON);
	if (ct != NULL)
		return (ct->setting == ON);
	return FALSE;
}

static void ControlSetting(DBOX *db, enum commands cmd,
									int class, int setting)
{
	CTLWINDOW *ct = FindCommand(db, cmd, class);
	if (ct != NULL)
		ct->isetting = setting;
}

void SetCheckBox(DBOX *db, enum commands cmd)
{
	ControlSetting(db, cmd, CHECKBOX, ON);
}

void ClearCheckBox(DBOX *db, enum commands cmd)
{
	ControlSetting(db, cmd, CHECKBOX, OFF);
}

void EnableButton(DBOX *db, enum commands cmd)
{
	ControlSetting(db, cmd, BUTTON, ON);
}

void DisableButton(DBOX *db, enum commands cmd)
{
	ControlSetting(db, cmd, BUTTON, OFF);
}

int CheckBoxSetting(DBOX *db, enum commands cmd)
{
	CTLWINDOW *ct = FindCommand(db, cmd, CHECKBOX);
	if (ct != NULL)
		return (ct->isetting == ON);
	return FALSE;
}

char *GetDlgTextString(DBOX *db, enum commands cmd, CLASS class)
{
	CTLWINDOW *ct = FindCommand(db, cmd, class);
	if (ct != NULL)
		return ct->itext;
	else
		return NULL;
}

void SetDlgTextString(DBOX *db, enum commands cmd, char *text, CLASS class)
{
	CTLWINDOW *ct = FindCommand(db, cmd, class);
	if (ct != NULL)	{
		ct->itext = realloc(ct->itext, strlen(text)+1);
		if (ct->itext != NULL)
			strcpy(ct->itext, text);
	}
}

static void Scrollers(WINDOW wnd)
{
	if (wnd->wlines > ClientHeight(wnd) &&
			!TestAttribute(wnd, VSCROLLBAR))	{
		AddAttribute(wnd, VSCROLLBAR);
		SendMessage(wnd, BORDER, 0, 0);
	}
}

void PutComboListText(WINDOW wnd, enum commands cmd, char *text)
{
	CTLWINDOW *ct = FindCommand(wnd->extension, cmd, COMBOBOX);
	if (ct != NULL)		{
		WINDOW lwnd = ((WINDOW)(ct->wnd))->extension;
		SendMessage(lwnd, ADDTEXT, (PARAM) text, 0);
		Scrollers(lwnd);
	}
}

void PutItemText(WINDOW wnd, enum commands cmd, char *text)
{
	CTLWINDOW *ct = FindCommand(wnd->extension, cmd, EDITBOX);

	if (ct == NULL)
		ct = FindCommand(wnd->extension, cmd, TEXTBOX);
	if (ct == NULL)
		ct = FindCommand(wnd->extension, cmd, COMBOBOX);
	if (ct == NULL)
		ct = FindCommand(wnd->extension, cmd, LISTBOX);
	if (ct == NULL)
		ct = FindCommand(wnd->extension, cmd, SPINBUTTON);
	if (ct == NULL)
		ct = FindCommand(wnd->extension, cmd, TEXT);
	if (ct != NULL)		{
		WINDOW cwnd = (WINDOW) (ct->wnd);
		switch (ct->class)	{
			case COMBOBOX:
			case EDITBOX:
				SendMessage(cwnd, EB_PUTTEXT, (PARAM) text, 0);
				if (!isMultiLine(cwnd))
					SendMessage(cwnd, PAINT, 0, 0);
				Scrollers(cwnd);
				break;
			case LISTBOX:
			case TEXTBOX:
			case SPINBUTTON:
				SendMessage(cwnd, ADDTEXT, (PARAM) text, 0);
				Scrollers(cwnd);
				break;
			case TEXT:	{
				SendMessage(cwnd, CLEARTEXT, 0, 0);
				SendMessage(cwnd, ADDTEXT, (PARAM) text, 0);
				SendMessage(cwnd, PAINT, 0, 0);
				break;
			}
			default:
				break;
		}
	}
}

void GetItemText(WINDOW wnd, enum commands cmd, char *text, int len)
{
	CTLWINDOW *ct = FindCommand(wnd->extension, cmd, EDITBOX);

	if (ct == NULL)
		ct = FindCommand(wnd->extension, cmd, COMBOBOX);
	if (ct == NULL)
		ct = FindCommand(wnd->extension, cmd, TEXTBOX);
	if (ct == NULL)
		ct = FindCommand(wnd->extension, cmd, TEXT);
	if (ct != NULL)	{
		WINDOW cwnd = (WINDOW) (ct->wnd);
		if (cwnd != NULL)	{
			switch (ct->class)	{
				case TEXT:
					if (GetText(cwnd) != NULL)	{
						unsigned char *cp = strchr(GetText(cwnd), '\n');
						if (cp != NULL)
							len = (int) (cp - GetText(cwnd));
						strncpy(text, GetText(cwnd), len);
						*(text+len) = '\0';
					}
					break;
				case TEXTBOX:
					if (GetText(cwnd) != NULL)
						strncpy(text, GetText(cwnd), len);
					break;
				case COMBOBOX:
				case EDITBOX:
					SendMessage(cwnd, EB_GETTEXT, (PARAM) text, len);
					break;
				default:
					break;
			}
		}
	}
}

void GetDlgListText(WINDOW wnd, char *text, enum commands cmd)
{
	CTLWINDOW *ct = FindCommand(wnd->extension, cmd, LISTBOX);
	int sel = SendMessage(ct->wnd, LB_CURRENTSELECTION, 0, 0);
	SendMessage(ct->wnd, LB_GETTEXT, (PARAM) text, sel);
}

static int dircmp(const void *c1, const void *c2)
{
	return stricmp(*(char **)c1, *(char **)c2);
}

int DlgDirList(WINDOW wnd, char *fspec,
				enum commands nameid, enum commands pathid,
				unsigned attrib)
{
	int ax, i = 0, criterr = 1;
	struct ffblk ff;
	CTLWINDOW *ct = FindCommand(wnd->extension, nameid, LISTBOX);
	WINDOW lwnd;
	char **dirlist = NULL;

	CreatePath(path, fspec, TRUE, TRUE);

	if (ct != NULL)	{
		lwnd = ct->wnd;
		SendMessage(ct->wnd, CLEARTEXT, 0, 0);

		if (attrib & 0x8000)	{
			union REGS regs;
			char drname[15];
			unsigned int cd, dr;

			cd = getdisk();
			for (dr = 0; dr < 26; dr++)	{
				unsigned ndr;
				setdisk(dr);
				ndr = getdisk();
				if (ndr == dr)	{
					/* ------- test for remapped B drive ------- */
					if (dr == 1)	{
						regs.x.ax = 0x440e;		/* IOCTL function 14 */
						regs.h.bl = dr+1;
						int86(DOS, &regs, &regs);
						if (regs.h.al != 0)
							continue;
					}

					sprintf(drname, "[%c:]", dr+'A');

					/* ------ test for network or RAM disk ---- */
					regs.x.ax = 0x4409;		/* IOCTL function 9 */
					regs.h.bl = dr+1;
					int86(DOS, &regs, &regs);
					if (!regs.x.cflag)	{
						if (regs.x.dx & 0x1000)
							strcat(drname, " (Network)");
						else if (regs.x.dx == 0x0800)
							strcat(drname, " (RAMdisk)");
					}
					SendMessage(lwnd, ADDTEXT, (PARAM) drname, 0);
				}
			}
			setdisk(cd);
		}

		while (criterr == 1)	{
			ax = findfirst(path, &ff, attrib & 0x3f);
			criterr = TestCriticalError();
		}
		if (criterr)
			return FALSE;
		while (ax == 0)	{
			if (!((attrib & 0x4000) &&
					(ff.ff_attrib & (attrib & 0x3f)) == 0) &&
						strcmp(ff.ff_name, "."))	{
				char fname[15];
				sprintf(fname, (ff.ff_attrib & 0x10) ?
								"[%s]" : "%s" , ff.ff_name);
				dirlist = realloc(dirlist, sizeof(char *)*(i+1));
				if (dirlist != NULL)	{
					dirlist[i] = malloc(strlen(fname)+1);
					if (dirlist[i] != NULL)
						strcpy(dirlist[i], fname);
					i++;
				}
			}
			ax = findnext(&ff);
		}
		if (dirlist != NULL)	{
			int j;
			/* -- sort file/drive/directory list box data -- */
			qsort(dirlist, i, sizeof(void *), dircmp);

			/* ---- send sorted list to list box ---- */
			for (j = 0; j < i; j++)	{
				SendMessage(lwnd, ADDTEXT, (PARAM) dirlist[j], 0);
				free(dirlist[j]);
			}
			free(dirlist);
		}
		if (lwnd->wlines > ClientHeight(lwnd))
			AddAttribute(lwnd, VSCROLLBAR);
		else
			ClearAttribute(lwnd, VSCROLLBAR);
		SendMessage(lwnd, SHOW_WINDOW, 0, 0);
	}

	if (pathid)	{
		fnmerge(path, drive, dir, NULL, NULL);
		PutItemText(wnd, pathid, path);
	}

	return TRUE;
}

static CTLWINDOW *AssociatedControl(DBOX *db, enum commands Tcmd)
{
	CTLWINDOW *ct = db->ctl;
	while (ct->class)	{
		if (ct->class != TEXT)
			if (ct->command == Tcmd)
				break;
		ct++;
	}
	return ct;
}

static void dbShortcutKeys(DBOX *db, int ky)
{
	CTLWINDOW *ct;
	int ch = AltConvert(ky);

	if (ch != 0)	{
		ct = db->ctl;
		while (ct->class)	{
			char *cp = ct->vtext;
			while (cp && *cp)	{
				if (*cp == SHORTCUTCHAR && tolower(*(cp+1)) == ch)	{
					if (ct->class == TEXT)
						ct = AssociatedControl(db, ct->command);
					if (ct->class == RADIOBUTTON)
						SetRadioButton(db, ct);
					else if (ct->class == CHECKBOX)	{
						ct->setting ^= ON;
						SendMessage(ct->wnd, PAINT, 0, 0);
					}
					else if (ct->class)	{
						SendMessage(ct->wnd, SETFOCUS, TRUE, 0);
						if (ct->class == BUTTON)
							SendMessage(ct->wnd, KEYBOARD, '\r', 0);
					}
					return;
				}
				cp++;
			}
			ct++;
		}
	}
}

/* generic window processor used by all dialog box control windows */
static int ControlProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	DBOX *db;
	CTLWINDOW *ct;

	if (wnd == NULL)
		return FALSE;
	db = GetParent(wnd) ? GetParent(wnd)->extension : NULL;
	ct = GetControl(wnd);

	switch (msg)	{
		case CREATE_WINDOW:
			ct = wnd->ct = wnd->extension;
			wnd->extension = NULL;
			if (ct != NULL)
				ct->wnd = wnd;
			break;
		case KEYBOARD:
			switch ((int) p1)	{
				case F1:
					if (WindowMoving || WindowSizing)
						break;
					if (!DisplayHelp(wnd, ct->help))
						SendMessage(GetParent(wnd), COMMAND, ID_HELP, 0);
					return TRUE;
				case ' ':
					if (!((int)p2 & ALTKEY))
						break;
				case ALT_F6:
				case CTRL_F4:
				case ALT_F4:
					PostMessage(GetParent(wnd), msg, p1, p2);
					return TRUE;
				default:
					break;
			}
			if (GetClass(wnd) == EDITBOX)
				if (isMultiLine(wnd))
					break;
			switch ((int) p1)	{
				case UP:
					if (!isDerivedFrom(wnd, LISTBOX))	{
						p1 = CTRL_FIVE;
						p2 = LEFTSHIFT;
					}
					break;
				case BS:
					if (!isDerivedFrom(wnd, EDITBOX))	{
						p1 = CTRL_FIVE;
						p2 = LEFTSHIFT;
					}
					break;
				case DN:
					if (!isDerivedFrom(wnd, LISTBOX) &&
							!isDerivedFrom(wnd, COMBOBOX))
						p1 = '\t';
					break;
				case FWD:
					if (!isDerivedFrom(wnd, EDITBOX))
						p1 = '\t';
					break;
				case '\r':
					if (isDerivedFrom(wnd, EDITBOX))
						if (isMultiLine(wnd))
							break;
					if (isDerivedFrom(wnd, BUTTON))
						break;
					SendMessage(GetParent(wnd), COMMAND, ID_OK, 0);
					return TRUE;
				default:
					break;
			}
			break;
		case SETFOCUS:
			if (p1)	{
				DefaultWndProc(wnd, msg, p1, p2);
				GetParent(wnd)->dFocus = ct;
				SendMessage(GetParent(wnd), COMMAND,
					inFocusCommand(db), ENTERFOCUS);
				return TRUE;
			}
			else 
				SendMessage(GetParent(wnd), COMMAND,
					inFocusCommand(db), LEAVEFOCUS);
			break;
		case CLOSE_WINDOW:
			if (ct != NULL)
				if (GetParent(wnd)->ReturnCode == ID_OK &&
						(ct->class == EDITBOX || ct->class == COMBOBOX))
					if (wnd->TextChanged)
						if ((ct->itext = realloc(ct->itext,	strlen(wnd->text)+1)) != NULL)	{
							strcpy(ct->itext, wnd->text);
							if (!isMultiLine(wnd))	{
								char *cp = ct->itext+strlen(ct->itext)-1;
								if (*cp == '\n')
									*cp = '\0';
							}
						}
			break;
		default:
			break;
	}
	return DefaultWndProc(wnd, msg, p1, p2);
}

/* ----- Create unambiguous path from file spec, filling in the
     drive and directory if incomplete. Optionally change to
	 the new drive and subdirectory ------ */
void CreatePath(char *path, char *fspec, int InclName, int Change)
{
	int cm = 0;
	unsigned currdrive;
	char currdir[64];
	char *cp;

	if (!Change)	{
		/* ----- save the current drive and subdirectory ----- */
		currdrive = getdisk();
		getcwd(currdir, sizeof currdir);
		memmove(currdir, currdir+2, strlen(currdir+1));
		cp = currdir+strlen(currdir)-1;
		if (*cp == '\\')
			*cp = '\0';
	}

	*drive = *dir = *name = *ext = '\0';
	fnsplit(fspec, drive, dir, name, ext);
	if (!InclName)
		*name = *ext = '\0';
	*drive = toupper(*drive);

	if (*ext)
		cm |= EXTENSION;
	if (InclName && *name)
		cm |= FILENAME;
	if (*dir)
		cm |= DIRECTORY;
	if (*drive)
		cm |= DRIVE;

	if (cm & DRIVE)
		setdisk(*drive - 'A');
	else 	{
		*drive = getdisk();
		*drive += 'A';
	}

	if (cm & DIRECTORY)	{
		cp = dir+strlen(dir)-1;
		if (*cp == '\\')
			*cp = '\0';
		chdir(dir);
	}
	getcwd(dir, sizeof dir);
	memmove(dir, dir+2, strlen(dir+1));

	if (InclName)	{
		if (!(cm & FILENAME))
			strcpy(name, "*");
		if (!(cm & EXTENSION))
			strcpy(ext, ".*");
	}
	else
		*name = *ext = '\0';
	if (dir[strlen(dir)-1] != '\\')
		strcat(dir, "\\");
	memset(path, 0, sizeof path);
	fnmerge(path, drive, dir, name, ext);

	if (!Change)	{
		setdisk(currdrive);
		chdir(currdir);
	}
}

static void ChangeFocus(WINDOW wnd, int direc)
{
	DBOX *db = wnd->extension;
 	CTLWINDOW *ct = db->ctl;
 	CTLWINDOW *ctt;

	/* --- find the control that has the focus --- */
	while (ct->class)	{
		if (ct == wnd->dFocus)
			break;
		ct++;
	}
	if (ct->class)	{
		ctt = ct;
		do	{
			/* ----- point to next or previous control ----- */
			if (direc)	{
				ct++;
				if (ct->class == 0)
					ct = db->ctl;
			}
			else	{
				if (ct == db->ctl)
					while (ct->class)
						ct++;
				--ct;
			}

			if (ct->class != BOX && ct->class != TEXT)	{
				SendMessage(ct->wnd, SETFOCUS, TRUE, 0);
				SendMessage(ctt->wnd, PAINT, 0, 0);
				SendMessage(ct->wnd, PAINT, 0, 0);
				break;
			}
		} while (ct != ctt);
	}
}
