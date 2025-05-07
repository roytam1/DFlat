/* ----------- clipbord.c ------------ */
#include "dflat.h"

char *Clipboard;
int ClipboardLength;

void CopyToClipboard(WINDOW wnd)
{
    if (TextBlockMarked(wnd))    {
        char *bbl=TextLine(wnd,wnd->BlkBegLine)+wnd->BlkBegCol;
        char *bel=TextLine(wnd,wnd->BlkEndLine)+wnd->BlkEndCol;
        int len = (int) (bel - bbl);
        ClipboardLength = len;
        Clipboard = realloc(Clipboard, ClipboardLength);
        if (Clipboard != NULL)
            memmove(Clipboard, bbl, ClipboardLength);
    }
}

void PasteText(WINDOW wnd, char *SaveTo, int len)
{
    if (SaveTo != NULL && len > 0)    {
        int plen = strlen(wnd->text) + len;
        char *bl, *el;

        if (plen > wnd->textlen)    {
            wnd->text = realloc(wnd->text, plen+2);
            wnd->textlen = plen;
        }
        bl = CurrChar;
        el = bl+len;
        memmove(el,    bl,    strlen(bl)+1);
        memmove(bl, SaveTo, len);
        BuildTextPointers(wnd);
        wnd->TextChanged = TRUE;
    }
}
