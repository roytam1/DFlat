/* ----------- clipbord.c ------------ */
#include "dflat.h"

char *Clipboard;
unsigned ClipboardLength;

void CopyToClipboard(WINDOW wnd)
{
    if (TextBlockMarked(wnd))    {
        char *bbl=TextLine(wnd,wnd->BlkBegLine)+wnd->BlkBegCol;
        char *bel=TextLine(wnd,wnd->BlkEndLine)+wnd->BlkEndCol;
        ClipboardLength = (int) (bel - bbl);
        Clipboard = realloc(Clipboard, ClipboardLength);
        if (Clipboard != NULL)
            memmove(Clipboard, bbl, ClipboardLength);
    }
}

int PasteText(WINDOW wnd, char *SaveTo, unsigned len)
{
    if (SaveTo != NULL && len > 0)    {
        unsigned plen = strlen(wnd->text) + len;
        char *bl, *el;

		if (plen <= wnd->MaxTextLength)	{
        	if (plen > wnd->textlen)    {
            	wnd->text = realloc(wnd->text, plen+2);
            	wnd->textlen = plen;
        	}
        	if (wnd->text != NULL)    {
            	bl = CurrChar;
            	el = bl+len;
            	memmove(el, bl, strlen(bl)+1);
            	memmove(bl, SaveTo, len);
            	BuildTextPointers(wnd);
            	wnd->TextChanged = TRUE;
				return TRUE;
        	}
			else
				ResetEditBox(wnd);
		}
    }
	return FALSE;
}
