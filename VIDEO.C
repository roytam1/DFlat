/* --------------------- video.c -------------------- */

#include "dflat.h"

int ClipString;

static unsigned video_address;
/* -- read a rectangle of video memory into a save buffer -- */
void getvideo(RECT rc, void far *bf)
{
    int ht = RectBottom(rc)-RectTop(rc)+1;
    int bytes_row = (RectRight(rc)-RectLeft(rc)+1) * 2;
    unsigned vadr = vad(RectLeft(rc), RectTop(rc));
    hide_mousecursor();
    while (ht--)    {
        movedata(video_address, vadr, FP_SEG(bf),
                FP_OFF(bf), bytes_row);
        vadr += SCREENWIDTH*2;
        bf = (char far *)bf + bytes_row;
    }
    show_mousecursor();
}

/* -- write a rectangle of video memory from a save buffer -- */
void storevideo(RECT rc, void far *bf)
{
    int ht = RectBottom(rc)-RectTop(rc)+1;
    int bytes_row = (RectRight(rc)-RectLeft(rc)+1) * 2;
    unsigned vadr = vad(RectLeft(rc), RectTop(rc));
    hide_mousecursor();
    while (ht--)    {
        movedata(FP_SEG(bf), FP_OFF(bf), video_address,
                vadr, bytes_row);
        vadr += SCREENWIDTH*2;
        bf = (char far *)bf + bytes_row;
    }
    show_mousecursor();
}

/* -------- read a character of video memory ------- */
unsigned int GetVideoChar(int x, int y)
{
    int c;
    hide_mousecursor();
    c = peek(video_address, vad(x,y));
    show_mousecursor();
    return c;
}

/* -------- write a character of video memory ------- */
void PutVideoChar(int x, int y, int c)
{
    if (x < SCREENWIDTH && y < SCREENHEIGHT)    {
        hide_mousecursor();
        poke(video_address, vad(x,y), c);
        show_mousecursor();
    }
}

static int isAncestor(WINDOW wnd, WINDOW awnd)
{
	while (wnd != NULL)	{
		if (wnd == awnd)
			return TRUE;
		wnd = GetParent(wnd);
	}
	return FALSE;
}

int CharInView(WINDOW wnd, int x, int y)
{
	WINDOW nwnd = NextWindow(wnd);
	WINDOW pwnd;
	RECT rc;
    int x1 = GetLeft(wnd)+x;
    int y1 = GetTop(wnd)+y;

	if (!TestAttribute(wnd, VISIBLE))
		return FALSE;
    if (!TestAttribute(wnd, NOCLIP))    {
        WINDOW wnd1 = GetParent(wnd);
        while (wnd1 != NULL)    {
            /* --- clip character to parent's borders -- */
			if (!TestAttribute(wnd1, VISIBLE))
				return FALSE;
			if (!InsideRect(x1, y1, ClientRect(wnd1)))
                return FALSE;
            wnd1 = GetParent(wnd1);
        }
    }
	while (nwnd != NULL)	{
		if (isVisible(nwnd) && !isAncestor(wnd, nwnd))	{
			rc = WindowRect(nwnd);
			if (!TestAttribute(nwnd, NOCLIP))	{
				pwnd = nwnd;
				while (GetParent(pwnd))	{
					pwnd = GetParent(pwnd);
					rc = subRectangle(rc, ClientRect(pwnd));
				}
			}
			if (InsideRect(x1,y1,rc))
				return FALSE;
		}
		nwnd = NextWindow(nwnd);
	}
    return (x1 < SCREENWIDTH && y1 < SCREENHEIGHT);
}

/* -------- write a character to a window ------- */
void wputch(WINDOW wnd, int c, int x, int y)
{
	if (CharInView(wnd, x, y))	{
        hide_mousecursor();
        poke(video_address,
            vad(GetLeft(wnd)+x,GetTop(wnd)+y),(c & 255) |
                (clr(foreground, background) << 8));
        show_mousecursor();
	}
}

/* ------- write a string to a window ---------- */
void wputs(WINDOW wnd, void *s, int x, int y)
{
	int x1 = GetLeft(wnd)+x;
	int x2 = x1;
	int y1 = GetTop(wnd)+y;
    if (x1 < SCREENWIDTH && y1 < SCREENHEIGHT && isVisible(wnd))	{
		int *ln;
		if ((ln = malloc(400)) != NULL)	{
			int *cp1 = ln;
	        unsigned char *str = s;
	        int fg = foreground;
    	    int bg = background;
	        int len;
			int off = 0;
        	while (*str)    {
            	if (*str == CHANGECOLOR)    {
                	str++;
                	foreground = (*str++) & 0x7f;
                	background = (*str++) & 0x7f;
                	continue;
            	}
            	if (*str == RESETCOLOR)    {
                	foreground = fg & 0x7f;
                	background = bg & 0x7f;
                	str++;
                	continue;
            	}
   	        	*cp1 = (*str & 255) | (clr(foreground, background) << 8);
				if (ClipString)
					if (!CharInView(wnd, x, y))
						*cp1 = peek(video_address, vad(x2,y1));
				cp1++;
				str++;
				x++;
				x2++;
        	}
        	foreground = fg;
        	background = bg;
   			len = (int)(cp1-ln);
   			if (x1+len > SCREENWIDTH)
       			len = SCREENWIDTH-x1;

			if (!ClipString && !TestAttribute(wnd, NOCLIP))	{
				/* -- clip the line to within ancestor windows -- */
				RECT rc = WindowRect(wnd);
				WINDOW nwnd = GetParent(wnd);
				while (len > 0 && nwnd != NULL)	{
					if (!isVisible(nwnd))	{
						len = 0;
						break;
					}
					rc = subRectangle(rc, ClientRect(nwnd));
					nwnd = GetParent(nwnd);
				}
				while (len > 0 && !InsideRect(x1+off,y1,rc))	{
					off++;
					--len;
				}
				if (len > 0)	{
					x2 = x1+len-1;
					while (len && !InsideRect(x2,y1,rc))	{
						--x2;
						--len;
					}
				}
			}
			if (len > 0)	{
        		hide_mousecursor();
				movedata(FP_SEG(ln), FP_OFF(ln+off),
					video_address, vad(x1+off,y1), len*2);
        		show_mousecursor();
			}
			free(ln);
		}
    }
}

/* --------- get the current video mode -------- */
void get_videomode(void)
{
    videomode();
    /* ---- Monochrome Display Adaptor or text mode ---- */
    if (ismono())
        video_address = 0xb000;
    else
        /* ------ Text mode -------- */
        video_address = 0xb800 + video_page;
}

/* --------- scroll the window. d: 1 = up, 0 = dn ---------- */
void scroll_window(WINDOW wnd, RECT rc, int d)
{
	union REGS regs;
    hide_mousecursor();
	regs.h.cl = RectLeft(rc);
	regs.h.ch = RectTop(rc);
	regs.h.dl = RectRight(rc);
	regs.h.dh = RectBottom(rc);
	regs.h.bh = clr(WndForeground(wnd),WndBackground(wnd));
	regs.h.ah = 7 - d;
	regs.h.al = 1;
    int86(VIDEO, &regs, &regs);
    show_mousecursor();
}


