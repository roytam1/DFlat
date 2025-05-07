/* ------------- editbox.c ------------ */

#include "dflat.h"

#define EDITBUFFERLENGTH  1024
#define ENTRYBUFFERLENGTH 256
#define GROWLENGTH        64

#define EditBufLen(wnd) (isMultiLine(wnd) ? EDITBUFFERLENGTH : ENTRYBUFFERLENGTH)

static void SaveDeletedText(WINDOW, char *, int);
static void Forward(WINDOW);
static void Backward(WINDOW);
static void End(WINDOW);
static void Home(WINDOW);
static void Downward(WINDOW);
static void Upward(WINDOW);
static void StickEnd(WINDOW);
static void UpLine(WINDOW);
static void DownLine(WINDOW);
static void NextWord(WINDOW);
static void PrevWord(WINDOW);
static void ResetEditBox(WINDOW);
static void AddTextPointers(WINDOW, int, int);
#define SetLinePointer(wnd, ln)	(wnd->CurrLine = ln)

static int KeyBoardMarking, ButtonDown;
int TextMarking;
static int bx, by;
static int py = -1;

char *Clipboard;
int ClipboardLength;

static int CreateWindowMsg(WINDOW wnd)
{
	int rtn = BaseWndProc(EDITBOX, wnd, CREATE_WINDOW, 0, 0);
	wnd->text = calloc(1, EditBufLen(wnd)+2);
	wnd->textlen = EditBufLen(wnd);
	wnd->InsertMode = TRUE;
	ResetEditBox(wnd);
	return rtn;
}

static int AddTextMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	int rtn = BaseWndProc(EDITBOX, wnd, ADDTEXT, p1, p2);
	if (!isMultiLine(wnd))	{
		wnd->CurrLine = 0;
		wnd->CurrCol = strlen((char *)p1);
		if (wnd->CurrCol >= ClientWidth(wnd))	{
			wnd->wleft = wnd->CurrCol - ClientWidth(wnd);
			wnd->CurrCol -= wnd->wleft;
		}
		wnd->BlkEndCol = wnd->CurrCol;
		SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	}
	BuildTextPointers(wnd);
	return rtn;
}

static int KeyboardCursorMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	int rtn;
	wnd->CurrCol = (int)p1 + wnd->wleft;
	wnd->WndRow = (int)p2;
	wnd->CurrLine = (int)p2 + wnd->wtop;
	rtn = BaseWndProc(EDITBOX, wnd, KEYBOARD_CURSOR, p1, p2);
	if (wnd == inFocus)	{
		if (!CharInView(wnd, (int)p1, (int)p2))
			SendMessage(NULL, HIDE_CURSOR, 0, 0);
		else 
			SendMessage(NULL, SHOW_CURSOR, wnd->InsertMode, 0);
	}
	return rtn;
}

static int SetFocusMsg(WINDOW wnd, PARAM p1)
{
	int rtn = BaseWndProc(EDITBOX, wnd, SETFOCUS, p1, 0);
	if (p1)	{
		SendMessage(NULL, SHOW_CURSOR, wnd->InsertMode, 0);
		SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	}
	else
		SendMessage(NULL, HIDE_CURSOR, 0, 0);
	return rtn;
}

static void ShiftChangedMsg(WINDOW wnd, PARAM p1)
{
	if (!((int)p1 & (LEFTSHIFT | RIGHTSHIFT)) && KeyBoardMarking)	{
		SendMessage(wnd, BUTTON_RELEASED, 0, 0);
		KeyBoardMarking = FALSE;
	}
}

static void GetTextMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	char *cp1 = (char *)p1;
	char *cp2 = wnd->text;
	while (p2-- && *cp2 && *cp2 != '\n')
		*cp1++ = *cp2++;
	*cp1 = '\0';
}

static void LeftButtonMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	int mx = (int) p1 - GetClientLeft(wnd);
	int my = (int) p2 - GetClientTop(wnd);
	char *lp;
	int len;

	if (isMultiLine(wnd))	{

		if (TextBlockMarked(wnd))	{
			ClearTextBlock(wnd);
			SendMessage(wnd, PAINT, 0, 0);
		}

		if (wnd->wlines)	{
			if (my > wnd->wlines-1)
				return;
			lp = TextLine(wnd, my+wnd->wtop);
			len = (int) (strchr(lp, '\n') - lp);
			mx = min(mx, len);
			if (mx < wnd->wleft)	{
				mx = 0;
				SendMessage(wnd, KEYBOARD, HOME, 0);
			}
			ButtonDown = TRUE;
			bx = mx;
			by = my;
		}
		else
			mx = my = 0;

		wnd->WndRow = my;
		SetLinePointer(wnd, my+wnd->wtop);
	}
	if (isMultiLine(wnd) ||
		(!TextBlockMarked(wnd)
			&& mx+wnd->wleft < strlen(wnd->text)))
		wnd->CurrCol = mx+wnd->wleft;
	SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
}

static int MouseMovedMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	int mx = (int) p1 - GetClientLeft(wnd);
	int my = (int) p2 - GetClientTop(wnd);
	if (my > wnd->wlines-1)
		return FALSE;
	if (ButtonDown)	{
		SetAnchor(wnd, bx+wnd->wleft, by+wnd->wtop);
		TextMarking = TRUE;
		SendMessage(wnd, CAPTURE_MOUSE, TRUE, 0);
		ButtonDown = FALSE;
	}
	if (TextMarking && !(WindowMoving || WindowSizing))	{
		int ptop;
		int pbot;
		char *lp;
		int len;
		int y;
		int bbl, bel;
		RECT rc = ClientRect(wnd);

		if (!InsideRect(p1, p2, rc))  	{
			int hs = -1, sc = -1;
			if (p1 < GetClientLeft(wnd))	{
				hs = FALSE;
				p1 = GetClientLeft(wnd);
			}
			if (p1 > GetClientRight(wnd))	{
				hs = TRUE;
				p1 = GetClientRight(wnd);
			}
			if (p2 < GetClientTop(wnd))	{
				sc = FALSE;
				p2 = GetClientTop(wnd);
			}
			if (p2 > GetClientBottom(wnd))	{
				sc = TRUE;
				p2 = GetClientBottom(wnd);
			}
			SendMessage(NULL, MOUSE_CURSOR, p1, p2);
			if (sc != -1)
				SendMessage(wnd, SCROLL, sc, 0);
			if (hs != -1)
				SendMessage(wnd, HORIZSCROLL, hs, 0);
			mx = (int) p1 - GetClientLeft(wnd);
			my = (int) p2 - GetClientTop(wnd);
		}

		ptop = min(wnd->BlkBegLine, wnd->BlkEndLine);
		pbot = max(wnd->BlkBegLine, wnd->BlkEndLine);

		lp = TextLine(wnd, wnd->wtop+my);
		len = (int) (strchr(lp, '\n') - lp);
		mx = min(mx, len-wnd->wleft);

		wnd->BlkEndCol = mx+wnd->wleft;
		wnd->BlkEndLine = my+wnd->wtop;

		bbl = min(wnd->BlkBegLine, wnd->BlkEndLine);
		bel = max(wnd->BlkBegLine, wnd->BlkEndLine);

		while (ptop < bbl)	{
			WriteTextLine(wnd, NULL, ptop, FALSE);
			ptop++;
		}
		for (y = bbl; y <= bel; y++)
			WriteTextLine(wnd, NULL, y, FALSE);
		while (pbot > bel)	{
			WriteTextLine(wnd, NULL, pbot, FALSE);
			--pbot;
		}
		return TRUE;
	}
	return FALSE;
}

static int ButtonReleasedMsg(WINDOW wnd)
{
	if (isMultiLine(wnd))	{
		ButtonDown = FALSE;
		if (TextMarking && !(WindowMoving || WindowSizing))	{
			PostMessage(wnd, RELEASE_MOUSE, 0, 0);
			TextMarking = FALSE;
			if (wnd->BlkBegLine > wnd->BlkEndLine)	{
				swap(wnd->BlkBegLine, wnd->BlkEndLine);
				swap(wnd->BlkBegCol, wnd->BlkEndCol);
			}
			if (wnd->BlkBegLine == wnd->BlkEndLine &&
					wnd->BlkBegCol > wnd->BlkEndCol)
				swap(wnd->BlkBegCol, wnd->BlkEndCol);
			return TRUE;
		}
		else
			py = -1;
	}
	return FALSE;
}

static int ScrollMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	int rtn = FALSE;
	if (isMultiLine(wnd))	{
		if ((rtn = BaseWndProc(EDITBOX, wnd, SCROLL, p1, 0)) != FALSE)	{
			if (p1)	{
				/* -------- scrolling up --------- */
				if (wnd->WndRow == 0)	{
					DownLine(wnd);
					StickEnd(wnd);
				}
				else
					--wnd->WndRow;
			}
			else	{
				/* -------- scrolling down --------- */
				if (wnd->WndRow == ClientHeight(wnd)-1)	{
					UpLine(wnd);
					StickEnd(wnd);
				}
				else
					wnd->WndRow++;
			}
			if (!(int)p2)
				SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
		}
	}
	return rtn;
}

static int HorizScrollMsg(WINDOW wnd, PARAM p1)
{
	int rtn = FALSE;
	char *currchar = CurrChar;
	if (!(p1 && wnd->CurrCol == wnd->wleft && *currchar == '\n'))	{
		if ((rtn = BaseWndProc(EDITBOX, wnd, HORIZSCROLL, p1, 0)) != FALSE)	{
			if (wnd->CurrCol < wnd->wleft)
				wnd->CurrCol++;
			else if (WndCol == ClientWidth(wnd))
				--wnd->CurrCol;
			SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
		}
	}
	return rtn;
}

int SizeMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	int rtn = BaseWndProc(EDITBOX, wnd, SIZE, p1, p2);
	if (WndCol > ClientWidth(wnd)-1)
		wnd->CurrCol = ClientWidth(wnd)-1 + wnd->wleft;
	if (wnd->WndRow > ClientHeight(wnd)-1)	{
		wnd->WndRow = ClientHeight(wnd)-1;
		SetLinePointer(wnd, wnd->WndRow+wnd->wtop);
	}
	SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	return rtn;
}

static int ScrollPageMsg(WINDOW wnd, PARAM p1)
{
	int rtn = FALSE;
	if (isMultiLine(wnd))	{
		rtn = BaseWndProc(EDITBOX, wnd, SCROLLPAGE, p1, 0);
		SetLinePointer(wnd, wnd->wtop+wnd->WndRow);
		StickEnd(wnd);
		SendMessage(wnd, KEYBOARD_CURSOR,WndCol, wnd->WndRow);
	}
	return rtn;
}

static int HorizPageMsg(WINDOW wnd, PARAM p1)
{
	int rtn = BaseWndProc(EDITBOX, wnd, HORIZPAGE, p1, 0);
	if ((int) p1 == FALSE)	{
		if (wnd->CurrCol > wnd->wleft+ClientWidth(wnd)-1)
			wnd->CurrCol = wnd->wleft+ClientWidth(wnd)-1;
	}
	else if (wnd->CurrCol < wnd->wleft)
		wnd->CurrCol = wnd->wleft;
	SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	return rtn;
}

static void DoMultiLines(WINDOW wnd, int c, PARAM p2)
{
	if (isMultiLine(wnd))	{
		if ((int)p2 & (LEFTSHIFT | RIGHTSHIFT))	{
			int kx, ky;
			SendMessage(NULL, CURRENT_KEYBOARD_CURSOR,
				(PARAM) &kx, (PARAM) &ky);
			kx -= GetClientLeft(wnd);
			ky -= GetClientTop(wnd);
			switch (c)	{
				case HOME:
				case END:
				case PGUP:
				case PGDN:
				case CTRL_PGUP:
				case CTRL_PGDN:
				case UP:
				case DN:
				case FWD:
				case BS:
				case CTRL_FWD:
				case CTRL_BS:
					if (!KeyBoardMarking)	{
						if (TextBlockMarked(wnd))	{
							ClearTextBlock(wnd);
							SendMessage(wnd, PAINT, 0, 0);
						}
						KeyBoardMarking = TextMarking = TRUE;
						SetAnchor(wnd, kx+wnd->wleft, ky+wnd->wtop);
					}
					break;
				default:
					break;
			}
		}
		else if (((c != DEL && c != RUBOUT) ||
				!isMultiLine(wnd)) && TextBlockMarked(wnd))	{
			ClearTextBlock(wnd);
			SendMessage(wnd, PAINT, 0, 0);
		}
	}
}

static void DoScrolling(WINDOW wnd, int c, PARAM p2)
{
	switch (c)	{
		case PGUP:
		case PGDN:
			if (isMultiLine(wnd))
				BaseWndProc(EDITBOX, wnd, KEYBOARD, c, p2);
			break;
		case CTRL_PGUP:
		case CTRL_PGDN:
			BaseWndProc(EDITBOX, wnd, KEYBOARD, c, p2);
			break;
		case HOME:
			Home(wnd);
			break;
		case END:
			End(wnd);
			break;
		case CTRL_FWD:
			NextWord(wnd);
			break;
		case CTRL_BS:
			PrevWord(wnd);
			break;
		case CTRL_HOME:
			if (isMultiLine(wnd))	{
				SendMessage(wnd, SCROLLDOC, TRUE, 0);
				wnd->CurrLine = 0;
				wnd->WndRow = 0;
			}
			Home(wnd);
			break;
		case CTRL_END:
			if (isMultiLine(wnd) && wnd->wlines > 0)	{
				SendMessage(wnd, SCROLLDOC, FALSE, 0);
				SetLinePointer(wnd, wnd->wlines-1);
				wnd->WndRow = min(ClientHeight(wnd)-1, wnd->wlines-1);
				Home(wnd);
			}
			End(wnd);
			break;
		case UP:
			if (!isMultiLine(wnd))
				break;
			Upward(wnd);
			break;
		case DN:
			if (!isMultiLine(wnd))
				break;
			Downward(wnd);
			break;
		case FWD:
			Forward(wnd);
			break;
		case BS:
			Backward(wnd);
			break;
		default:
			break;
	}
}

static int MovementKey(WINDOW wnd, int c)
{
	if (!isMultiLine(wnd) && TextBlockMarked(wnd))	{
		ClearTextBlock(wnd);
		SendMessage(wnd, PAINT, 0, 0);
	}
	if (c != RUBOUT || (wnd->CurrLine == 0 && wnd->CurrCol == 0))
		return TRUE;
	Backward(wnd);
	SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	return FALSE;
}

static int DelKey(WINDOW wnd)
{
	char *currchar = CurrChar;
	int repaint = *currchar == '\n';
	if (TextBlockMarked(wnd))	{
		SendMessage(wnd, COMMAND, ID_DELETETEXT, 0);
		SendMessage(wnd, PAINT, 0, 0);
		return TRUE;
	}
	if (*(currchar+1) == '\0')
		return TRUE;
	strcpy(currchar, currchar+1);
	if (repaint)	{
		BuildTextPointers(wnd);
		SendMessage(wnd, PAINT, 0, 0);
	}
	else	{
		AddTextPointers(wnd, wnd->CurrLine+1, -1);
		WriteTextLine(wnd, NULL, wnd->WndRow+wnd->wtop, FALSE);
	}
	wnd->TextChanged = TRUE;
	return FALSE;
}

static int TabKey(WINDOW wnd, PARAM p2)
{
	if (isMultiLine(wnd))	{
		int insmd = wnd->InsertMode;
		do  {
			char *cc = CurrChar+1;
			if (!insmd && *cc == '\0')
				break;
			SendMessage(wnd, KEYBOARD,
				insmd ? ' ' : FWD, 0);
		} while (wnd->CurrCol % cfg.Tabs);
		return TRUE;
	}
	PostMessage(GetParent(wnd), KEYBOARD, '\t', p2);
	return FALSE;
}

static void KeyTyped(WINDOW wnd, int c)
{
	char *currchar = CurrChar;
	if ((c != '\n' && c < ' ') || (c & 0x1000))
		/* ---- not recognized by editor --- */
		return;
	if (!isMultiLine(wnd) && TextBlockMarked(wnd))	{
		ResetEditBox(wnd);
		ClearTextBlock(wnd);
		currchar = CurrChar;
	}
	if (*currchar == '\0')	{
		/* ---- typing at end of text ---- */
		*currchar = '\n';
		*(currchar+1) = '\0';
		BuildTextPointers(wnd);
	}
	/* --- displayable char or newline --- */
	if (c == '\n' || wnd->InsertMode ||	*currchar == '\n')	{
		if (wnd->text[wnd->textlen-1] != '\0')	{
			wnd->textlen += GROWLENGTH;
			wnd->text = realloc(wnd->text, wnd->textlen+2);
			wnd->text[wnd->textlen-1] = '\0';
			currchar = CurrChar;
		}
		/* ------ insert mode ------ */
		memmove(currchar+1, currchar, strlen(currchar)+1);
		AddTextPointers(wnd, wnd->CurrLine+1, 1);
		if (isMultiLine(wnd) && wnd->wlines > 1)
			wnd->textwidth = max(wnd->textwidth,
				(int) (TextLine(wnd, wnd->CurrLine+1)-
				TextLine(wnd, wnd->CurrLine)));
		else
			wnd->textwidth = max(wnd->textwidth,
				strlen(wnd->text));
		WriteTextLine(wnd, NULL,
			wnd->wtop+wnd->WndRow, FALSE);
	}
	/* ----- put the char in the buffer ----- */
	*currchar = c;
	wnd->TextChanged = TRUE;
	if (c == '\n')	{
		wnd->wleft = 0;
		BuildTextPointers(wnd);
		End(wnd);
		Forward(wnd);
		SendMessage(wnd, PAINT, 0, 0);
		return;
	}
	/* ---------- test end of window --------- */
	if (WndCol == ClientWidth(wnd)-1)	{
		int dif;
		char *cp = currchar;
		while (*cp != ' ' && cp != TextLine(wnd, wnd->CurrLine))
			--cp;
		if (!isMultiLine(wnd) || cp == TextLine(wnd, wnd->CurrLine) ||
				!wnd->WordWrapMode)
			SendMessage(wnd, HORIZSCROLL, TRUE, 0);
		else	{
			dif = 0;
			if (c != ' ')	{
				dif = (int) (currchar - cp);
				wnd->CurrCol -= dif;
				SendMessage(wnd, KEYBOARD, DEL, 0);
				--dif;
			}
			SendMessage(wnd, KEYBOARD, '\r', 0);
			currchar = CurrChar;
			wnd->CurrCol = dif;
			if (c == ' ')
				return;
		}
	}
	/* ------ display the character ------ */
	SetStandardColor(wnd);
	PutWindowChar(wnd, c, WndCol, wnd->WndRow);
	/* ----- advance the pointers ------ */
	wnd->CurrCol++;
}

static int DoKeyStroke(WINDOW wnd, int c, PARAM p2)
{
	switch (c)	{
		case HOME:
		case END:
		case PGUP:
		case PGDN:
		case CTRL_PGUP:
		case CTRL_PGDN:
		case FWD:
		case BS:
		case CTRL_FWD:
		case CTRL_BS:
		case CTRL_HOME:
		case CTRL_END:
		case RUBOUT:
			if (MovementKey(wnd, c))
				break;
		case DEL:
			if (DelKey(wnd))
				return TRUE;
			break;
		case CTRL_FIVE:	/* same as Shift+Tab */
			if (!((int)p2 & (LEFTSHIFT | RIGHTSHIFT)))
				break;
		case UP:
		case DN:
			if (!isMultiLine(wnd))
				PostMessage(GetParent(wnd), KEYBOARD, c, p2);
			break;
		case '\t':
			if (TabKey(wnd, p2))
				return TRUE;
			break;
		case '\r':
			if (!isMultiLine(wnd))	{
				PostMessage(GetParent(wnd), KEYBOARD, c, p2);
				break;
			}
			c = '\n';
		default:
			KeyTyped(wnd, c);
			break;
	}
	return FALSE;
}

static int KeyboardMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
	int c;

	/* --- alt keys get processed by lower classes --- */
	if ((int)p2 & ALTKEY)
		return FALSE;
	c = (int) p1;
	switch (c)	{
		/* --- these keys get processed by lower classes --- */
		case ESC:
		case F1:
		case F2:
		case F3:
		case F4:
		case F5:
		case F6:
		case F7:
		case F8:
		case F9:
		case F10:
		case INS:
		case SHIFT_INS:
		case SHIFT_DEL:
			return FALSE;
		/* --- these keys get processed here --- */
		case CTRL_FWD:
		case CTRL_BS:
		case CTRL_HOME:
		case CTRL_END:
		case CTRL_PGUP:
		case CTRL_PGDN:
			break;
		default:
			/* --- other ctrl keys get processed by lower classes --- */
			if ((int)p2 & CTRLKEY)
				return FALSE;
			/* --- all other keys get processed here --- */
			break;
	}
	DoMultiLines(wnd, c, p2);
	DoScrolling(wnd, c, p2);
	if (KeyBoardMarking)	{
		int kx, ky;
		SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
		SendMessage(NULL, CURRENT_KEYBOARD_CURSOR,
			(PARAM) &kx, (PARAM) &ky);
		SendMessage(wnd, MOUSE_MOVED, kx, ky);
		return TRUE;
	}
	if (!TestAttribute(wnd, READONLY))
		if (DoKeyStroke(wnd, c, p2))
			return TRUE;
	SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	return TRUE;
}

static void DeleteTextCmd(WINDOW wnd)
{
	char *bbl, *bel;
	int len;

	if (TextBlockMarked(wnd))	{
		bbl = TextLine(wnd, wnd->BlkBegLine) + wnd->BlkBegCol;
		bel = TextLine(wnd, wnd->BlkEndLine) + wnd->BlkEndCol;
		len = (int) (bel - bbl);
	}
	SaveDeletedText(wnd, bbl, len);
	wnd->TextChanged = TRUE;
	strcpy(bbl, bel);
	wnd->CurrLine = TextLineNumber(wnd, bbl - wnd->BlkBegCol);
	wnd->CurrCol = wnd->BlkBegCol;
	wnd->WndRow = wnd->BlkBegLine - wnd->wtop;
	if (wnd->WndRow < 0)	{
		wnd->wtop = wnd->BlkBegLine;
		wnd->WndRow = 0;
	}
	SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	ClearTextBlock(wnd);
	BuildTextPointers(wnd);
}

static void ClearCmd(WINDOW wnd)
{
	char *bbl, *bel;
	int len;

	if (TextBlockMarked(wnd))	{
		bbl = TextLine(wnd, wnd->BlkBegLine) + wnd->BlkBegCol;
		bel = TextLine(wnd, wnd->BlkEndLine) + wnd->BlkEndCol;
		len = (int) (bel - bbl);
	}
	SaveDeletedText(wnd, bbl, len);
	wnd->CurrLine = TextLineNumber(wnd, bbl);
	wnd->CurrCol = wnd->BlkBegCol;
	wnd->WndRow = wnd->BlkBegLine - wnd->wtop;
	if (wnd->WndRow < 0)	{
		wnd->WndRow = 0;
		wnd->wtop = wnd->BlkBegLine;
	}
	while (bbl < bel)	{
		char *cp = strchr(bbl, '\n');
		if (cp > bel)
			cp = bel;
		strcpy(bbl, cp);
		bel -= (int) (cp - bbl);
		bbl++;
	}
	ClearTextBlock(wnd);
	BuildTextPointers(wnd);
	SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	SendMessage(wnd, PAINT, 0, 0);
	wnd->TextChanged = TRUE;
}

static void UndoCmd(WINDOW wnd)
{
	if (wnd->DeletedText != NULL)	{
		PasteText(wnd, wnd->DeletedText, wnd->DeletedLength);
		free(wnd->DeletedText);
		wnd->DeletedText = NULL;
		wnd->DeletedLength = 0;
		SendMessage(wnd, PAINT, 0, 0);
	}
}

static void ParagraphCmd(WINDOW wnd)
{
	char *bl;
	int bc, ec;
	int fl, el;
	int Blocked;
	char *bbl, *bel, *bb;

	el = wnd->BlkEndLine;
	ec = wnd->BlkEndCol;
	if (!TextBlockMarked(wnd))	{
		Blocked = FALSE;
		/* ---- forming paragraph from cursor position --- */
		fl = wnd->wtop + wnd->WndRow;
		bl = TextLine(wnd, wnd->CurrLine);
		bc = wnd->CurrCol;
		Home(wnd);
		bbl = bel = bl;
		if (bc >= ClientWidth(wnd))
			bc = 0;
		/* ---- locate the end of the paragraph ---- */
		while (*bel)	{
			int blank = TRUE;
			char *bll = bel;
			/* --- blank line marks end of paragraph --- */
			while (*bel && *bel != '\n')	{
				if (*bel != ' ')
					blank = FALSE;
				bel++;
			}
			if (blank)	{
				bel = bll;
				break;
			}
			if (*bel)
				bel++;
		}
		if (bel == bbl)	{
			SendMessage(wnd, KEYBOARD, DN, 0);
			return;
		}
		if (*bel == '\0')
			--bel;
		if (*bel == '\n')
			--bel;
	}
	else	{
		bbl = TextLine(wnd, wnd->BlkBegLine) + wnd->BlkBegCol;
		bel = TextLine(wnd, wnd->BlkEndLine) + wnd->BlkEndCol;

		Blocked = TRUE;
		/* ---- forming paragraph from marked block --- */
		fl = wnd->BlkBegLine;
		bc = wnd->CurrCol = wnd->BlkBegCol;
		wnd->CurrLine = fl;
		if (fl < wnd->wtop)
			wnd->wtop = fl;
		wnd->WndRow = fl - wnd->wtop;
		SendMessage(wnd, KEYBOARD, '\r', 0);
		el++, fl++;
		if (bc != 0)	{
			SendMessage(wnd, KEYBOARD, '\r', 0);
			el++, fl ++;
		}
		bc = 0;
		bl = TextLine(wnd, fl);
		wnd->CurrLine = fl;
		bbl = bl + bc;
		bel = TextLine(wnd, el) + ec;
	}

	/* --- change all newlines in block to spaces --- */
	while (CurrChar < bel)	{
		if (*CurrChar == '\n')	{
			*CurrChar = ' ';
			wnd->CurrLine++;
			wnd->CurrCol = 0;
		}
		else
			wnd->CurrCol++;
	}

	/* ---- insert newlines at new margin boundaries ---- */
	bb = bbl;
	while (bbl < bel)	{
		bbl++;
		if ((int)(bbl - bb) == ClientWidth(wnd)-1)	{
			while (*bbl != ' ' && bbl > bb)
				--bbl;
			if (*bbl != ' ')	{
				bbl = strchr(bbl, ' ');
				if (bbl == NULL || bbl >= bel)
					break;
			}
			*bbl = '\n';
			bb = bbl+1;
		}
	}
	ec = (int)(bel - bb);
	BuildTextPointers(wnd);

	if (Blocked)	{
		/* ---- position cursor at end of new paragraph ---- */
		if (el < wnd->wtop ||
				wnd->wtop + ClientHeight(wnd) < el)
			wnd->wtop = el-ClientHeight(wnd);
		if (wnd->wtop < 0)
			wnd->wtop = 0;
		wnd->WndRow = el - wnd->wtop;
		wnd->CurrLine = el;
		wnd->CurrCol = ec;
		SendMessage(wnd, KEYBOARD, '\r', 0);
		SendMessage(wnd, KEYBOARD, '\r', 0);
	}
	else	{
		/* --- put cursor back at beginning --- */
		wnd->CurrLine = TextLineNumber(wnd, bl);
		wnd->CurrCol = bc;
		if (fl < wnd->wtop)
			wnd->wtop = fl;
		wnd->WndRow = fl - wnd->wtop;
	}
	SendMessage(wnd, PAINT, 0, 0);
	SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	wnd->TextChanged = TRUE;
	BuildTextPointers(wnd);
}

static int CommandMsg(WINDOW wnd, PARAM p1)
{
	switch ((int)p1)	{
		case ID_DELETETEXT:
			DeleteTextCmd(wnd);
			return TRUE;
		case ID_CLEAR:
			ClearCmd(wnd);
			return TRUE;
		case ID_UNDO:
			UndoCmd(wnd);
			return TRUE;
		case ID_PARAGRAPH:
			ParagraphCmd(wnd);
			return TRUE;
		default:
			break;
	}
	return FALSE;
}

int EditBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	int rtn;
	RECT rc;

	rc = ClientRect(wnd);
	switch (msg)	{
		case CREATE_WINDOW:
			return CreateWindowMsg(wnd);
		case ADDTEXT:
			return AddTextMsg(wnd, p1, p2);
		case SETTEXT:
			rtn = BaseWndProc(EDITBOX, wnd, msg, p1, p2);
			wnd->CurrLine = 0;
			return rtn;
		case CLEARTEXT:
			ResetEditBox(wnd);
			ClearTextPointers(wnd);
			break;
		case KEYBOARD_CURSOR:
			return KeyboardCursorMsg(wnd, p1, p2);
		case EB_GETTEXT:
			GetTextMsg(wnd, p1, p2);
			return TRUE;
		case EB_PUTTEXT:
			SendMessage(wnd, CLEARTEXT, 0, 0);
			SendMessage(wnd, ADDTEXT, p1, p2);
			return TRUE;
		case SETFOCUS:
			return SetFocusMsg(wnd, p1);
        case SHIFT_CHANGED:
			ShiftChangedMsg(wnd, p1);
			break;
		case DOUBLE_CLICK:
			if (KeyBoardMarking)
				return TRUE;
			break;
		case LEFT_BUTTON:
			if (KeyBoardMarking || TextMarking)
				return TRUE;
			if (WindowMoving || WindowSizing)
				break;
			if (!InsideRect(p1, p2, rc))
				break;
			LeftButtonMsg(wnd, p1, p2);
			return TRUE;
		case MOUSE_MOVED:
			if (MouseMovedMsg(wnd, p1, p2))
				return TRUE;
			break;
		case BUTTON_RELEASED:
			if (ButtonReleasedMsg(wnd))
				return TRUE;
			break;
		case PAINT:
			rtn = BaseWndProc(EDITBOX, wnd, msg, p1, p2);
			if (isVisible(wnd))
				SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
			return rtn;
		case SCROLL:
			return ScrollMsg(wnd, p1, p2);
		case HORIZSCROLL:
			return HorizScrollMsg(wnd, p1);

		case MOVE:
			rtn = BaseWndProc(EDITBOX, wnd, msg, p1, p2);
			SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
			return rtn;
		case SIZE:
			return SizeMsg(wnd, p1, p2);
		case SCROLLPAGE:
			return ScrollPageMsg(wnd, p1);
		case HORIZPAGE:
			return HorizPageMsg(wnd, p1);
		case KEYBOARD:
			if (WindowMoving || WindowSizing)
				break;
			if (KeyboardMsg(wnd, p1, p2))
				return TRUE;
			break;
		case COMMAND:
			if (CommandMsg(wnd, p1))
				return TRUE;
			break;
		case CLOSE_WINDOW:
			SendMessage(NULL, HIDE_CURSOR, 0, 0);
			if (wnd->DeletedText != NULL)
				free(wnd->DeletedText);
			break;
		default:
			break;
	}
	return BaseWndProc(EDITBOX, wnd, msg, p1, p2);
}

static void SaveDeletedText(WINDOW wnd, char *bbl, int len)
{
	wnd->DeletedLength = len;
	if ((wnd->DeletedText = realloc(wnd->DeletedText, len)) != NULL)
		memmove(wnd->DeletedText, bbl, len);
}

static void Forward(WINDOW wnd)
{
	char *cc = CurrChar+1;
	if (*cc == '\0')
		return;
	if (*CurrChar == '\n')	{
		Home(wnd);
		Downward(wnd);
	}
	else	{
		wnd->CurrCol++;
		if (WndCol == ClientWidth(wnd))
			SendMessage(wnd, HORIZSCROLL, TRUE, 0);
	}
}

static void StickEnd(WINDOW wnd)
{
	char *cp = TextLine(wnd, wnd->CurrLine);
	char *cp1 = strchr(cp, '\n');
	int len = 0;

	if (cp1 != NULL)
		len = (int) (cp1 - cp);
	wnd->CurrCol = min(len, wnd->CurrCol);
	if (wnd->wleft > wnd->CurrCol)	{
		wnd->wleft = max(0, wnd->CurrCol - 4);
		SendMessage(wnd, PAINT, 0, 0);
	}
	else if (wnd->CurrCol-wnd->wleft >= ClientWidth(wnd))	{
		wnd->wleft = wnd->CurrCol - (ClientWidth(wnd)-1);
		SendMessage(wnd, PAINT, 0, 0);
	}
}

static void Downward(WINDOW wnd)
{
	if (isMultiLine(wnd) && wnd->WndRow+wnd->wtop+1 < wnd->wlines)	{
		DownLine(wnd);
		if (wnd->WndRow == ClientHeight(wnd)-1)
			SendMessage(wnd, SCROLL, TRUE, 0);
		wnd->WndRow++;
		StickEnd(wnd);
	}
}

static void DownLine(WINDOW wnd)
{
	wnd->CurrLine++;
}

static void UpLine(WINDOW wnd)
{
	if (wnd->CurrLine > 0)
		--wnd->CurrLine;
}

static void Upward(WINDOW wnd)
{
	if (isMultiLine(wnd) && wnd->CurrLine != 0)	{
		UpLine(wnd);
		if (wnd->WndRow == 0)
			SendMessage(wnd, SCROLL, FALSE, 0);
		--wnd->WndRow;
		StickEnd(wnd);
	}
}

static void Backward(WINDOW wnd)
{
	if (wnd->CurrCol)	{
		if (wnd->CurrCol-- <= wnd->wleft)
			if (wnd->wleft != 0)
				SendMessage(wnd, HORIZSCROLL, FALSE, 0);
	}
	else if (isMultiLine(wnd) && wnd->CurrLine != 0)	{
		Upward(wnd);
		End(wnd);
	}
}

static void End(WINDOW wnd)
{
	while (*CurrChar && *CurrChar != '\n')
		++wnd->CurrCol;
	if (WndCol >= ClientWidth(wnd))	{
		wnd->wleft = wnd->CurrCol - (ClientWidth(wnd)-1);
		SendMessage(wnd, PAINT, 0, 0);
	}
}

static void Home(WINDOW wnd)
{
	wnd->CurrCol = 0;
	if (wnd->wleft != 0)	{
		wnd->wleft = 0;
		SendMessage(wnd, PAINT, 0, 0);
	}
}

#define isWhite(c) 	((c) == ' ' || (c) == '\n')

static void NextWord(WINDOW wnd)
{
	int savetop = wnd->wtop;
	int saveleft = wnd->wleft;
	ClearVisible(wnd);
	while (!isWhite(*CurrChar))	{
		char *cc = CurrChar+1;
		if (*cc == '\0')
			break;
		Forward(wnd);
	}
	while (isWhite(*CurrChar))	{
		char *cc = CurrChar+1;
		if (*cc == '\0')
			break;
		Forward(wnd);
	}
	SetVisible(wnd);
	SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	if (wnd->wtop != savetop || wnd->wleft != saveleft)
		SendMessage(wnd, PAINT, 0, 0);
}

static void PrevWord(WINDOW wnd)
{
	int savetop = wnd->wtop;
	int saveleft = wnd->wleft;
	ClearVisible(wnd);
	Backward(wnd);
	while (isWhite(*CurrChar))	{
		if (wnd->CurrLine == 0 && wnd->CurrCol == 0)
			break;
		Backward(wnd);
	}
	while (!isWhite(*CurrChar))	{
		if (wnd->CurrLine == 0 && wnd->CurrCol == 0)
			break;
		Backward(wnd);
	}
	if (isWhite(*CurrChar))
		Forward(wnd);
	SetVisible(wnd);
	if (wnd->wleft != saveleft)
		if (wnd->CurrCol >= saveleft)
			if (wnd->CurrCol - saveleft < ClientWidth(wnd))
				wnd->wleft = saveleft;
	SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
	if (wnd->wtop != savetop || wnd->wleft != saveleft)
		SendMessage(wnd, PAINT, 0, 0);
}

static void ResetEditBox(WINDOW wnd)
{
	*wnd->text = '\0';
	wnd->wlines = 0;
	wnd->CurrLine = 0;
	wnd->CurrCol = 0;
	wnd->WndRow = 0;
	wnd->TextChanged = FALSE;
	wnd->wleft = 0;
	wnd->textwidth = 0;
}

static void AddTextPointers(WINDOW wnd, int lineno, int ct)
{
	while (lineno < wnd->wlines)
		*((wnd->TextPointers) + lineno++) += ct;
}

