/* ---------------- search.c ------------- */
#include "dflat.h"

extern DBOX SearchTextDB;
extern DBOX ReplaceTextDB;
static int CheckCase = TRUE;

static int SearchCmp(int a, int b)
{
	if (b == '\n')
		b = ' ';
	if (CheckCase)
		return a != b;
	return tolower(a) != tolower(b);
}

static void SearchTextBox(WINDOW wnd, int Replacing, int incr)
{
	char *s1, *s2, *cp1 = CurrChar;
	DBOX *db = Replacing ? &ReplaceTextDB : &SearchTextDB;
	char *cp = GetEditBoxText(db, ID_SEARCHFOR);
	int rpl = TRUE, FoundOne = FALSE;
	while (rpl)	{
		rpl = Replacing ?
				CheckBoxSetting(&ReplaceTextDB, ID_REPLACEALL) : FALSE;
		if (TextBlockMarked(wnd))	{
			ClearTextBlock(wnd);
			SendMessage(wnd, PAINT, 0, 0);
		}
		if (cp && cp1 && *cp && *cp1)	{
			if (incr)
				cp1++;
			while (*cp1)	{
				s1 = cp;
				s2 = cp1;
				while (*s1 && *s1 != '\n')	{
					if (SearchCmp(*s1,*s2))
						break;
					s1++, s2++;
				}
				if (*s1 == '\0' || *s1 == '\n')
					break;
				cp1++;
			}
			if (*s1 == 0 || *s1 == '\n')	{
				/* ----- hit at *cp1 ------- */
				FoundOne = TRUE;
				wnd->BlkEndLine = TextLineNumber(wnd, s2);
				wnd->BlkBegLine = TextLineNumber(wnd, cp1);
				if (wnd->BlkEndLine < wnd->BlkBegLine)
					wnd->BlkEndLine = wnd->BlkBegLine;
				wnd->BlkEndCol = (int)(s2 - TextLine(wnd, wnd->BlkEndLine));
				wnd->BlkBegCol = (int)(cp1 - TextLine(wnd, wnd->BlkBegLine));

				wnd->CurrCol = wnd->BlkBegCol;
				wnd->CurrLine = wnd->BlkBegLine;
				wnd->WndRow = wnd->CurrLine - wnd->wtop;

				if (WndCol > ClientWidth(wnd)-1)
					wnd->wleft = wnd->CurrCol;
				if (wnd->WndRow > ClientHeight(wnd)-1)	{
					wnd->wtop = wnd->CurrLine;
					wnd->WndRow = 0;
				}
				SendMessage(wnd, PAINT, 0, 0);
				SendMessage(wnd, KEYBOARD_CURSOR, WndCol, wnd->WndRow);
				if (Replacing)	{
					if (rpl || YesNoBox("Replace the text?"))	{
						char *cr = GetEditBoxText(db, ID_REPLACEWITH);
						int oldlen = strlen(cp);
						int newlen = strlen(cr);
						int dif;
						if (oldlen < newlen)	{
							dif = newlen-oldlen;
							if (wnd->textlen < strlen(wnd->text)+dif)	{
								int offset = (int)(cp1-wnd->text);
								wnd->textlen += dif;
								wnd->text = realloc(wnd->text, wnd->textlen+2);
								if (wnd->text == NULL)
									return;
								cp1 = wnd->text + offset;
							}
							memmove(cp1+dif, cp1, strlen(cp1)+1);
						}
						else if (oldlen > newlen)	{
							dif = oldlen-newlen;
							memmove(cp1, cp1+dif, strlen(cp1)+1);
						}
						strncpy(cp1, cr, newlen);
						wnd->TextChanged = TRUE;
						BuildTextPointers(wnd);
					}
					if (rpl)	{
						incr = TRUE;
						continue;
					}
					ClearTextBlock(wnd);
					SendMessage(wnd, PAINT, 0, 0);
				}
				return;
			}
			break;
		}
	}
	if (cp && *cp && !FoundOne)
		MessageBox("Search/Replace Text", "No match found");
}

void ReplaceText(WINDOW wnd)
{
	if (CheckCase)
		SetCheckBox(&ReplaceTextDB, ID_MATCHCASE);
	if (DialogBox(wnd, &ReplaceTextDB, TRUE, NULL))	{
		CheckCase = CheckBoxSetting(&ReplaceTextDB, ID_MATCHCASE);
		SearchTextBox(wnd, TRUE, FALSE);
	}
}

void SearchText(WINDOW wnd)
{
	if (CheckCase)
		SetCheckBox(&SearchTextDB, ID_MATCHCASE);
	if (DialogBox(wnd, &SearchTextDB, TRUE, NULL))	{
		CheckCase = CheckBoxSetting(&SearchTextDB, ID_MATCHCASE);
		SearchTextBox(wnd, FALSE, FALSE);
	}
}

void SearchNext(WINDOW wnd)
{
	SearchTextBox(wnd, FALSE, TRUE);
}


