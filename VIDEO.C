/* --------------------- video.c -------------------- */

#include "dflat.h"

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

static RECT PrepRect(WINDOW wnd, RECT rc)
{
	if (!TestAttribute(wnd, NOCLIP))	{
		while (GetParent(wnd))	{
			wnd = GetParent(wnd);
			rc = subRectangle(rc, ClientRect(wnd));
		}
	}
	return rc;
}

int CharInView(WINDOW wnd, int x, int y)
{
    int x1 = GetLeft(wnd)+x;
    int y1 = GetTop(wnd)+y;

    if (!TestAttribute(wnd, NOCLIP))    {
        WINDOW wnd1 = GetParent(wnd);
        while (wnd1 != NULL)    {
            /* --- clip character to parent's borders -- */
			RECT rc = ClientRect(wnd1);
			if (!InsideRect(x1, y1, rc))
                return FALSE;
            wnd1 = GetParent(wnd1);
        }
    }
    return (x1 < SCREENWIDTH && y1 < SCREENHEIGHT);
}

/* -------- write a character to a window ------- */
void wputch(WINDOW wnd, int c, int x, int y)
{
	if (isVisible(wnd) && CharInView(wnd, x, y))	{
    	int x1 = GetLeft(wnd)+x;
    	int y1 = GetTop(wnd)+y;
		WINDOW nwnd = NextWindow(wnd);
		while (nwnd != NULL)	{
			if (isVisible(nwnd) && !isAncestor(wnd, nwnd))	{
				RECT rc = PrepRect(nwnd, WindowRect(nwnd));
				if (InsideRect(x1,y1,rc))
					return;
			}
			nwnd = NextWindow(nwnd);
		}
        hide_mousecursor();
        poke(video_address,
            vad(x1,y1),(c & 255) |
                (clr(foreground, background) << 8));
        show_mousecursor();
	}
}

/* ------- write a string to a window ---------- */
void wputs(WINDOW wnd, void *s, int x, int y)
{
    int x1 = GetLeft(wnd)+x;
    int y1 = GetTop(wnd)+y;
    if (x1 < SCREENWIDTH && y1 < SCREENHEIGHT)    {
        int fg = foreground;
        int bg = background;
        unsigned char *str, *ss;
        int *ln, *cp1, *cp2;
        int len;

		if ((ss = malloc(400)) != NULL)	{
			if ((ln = malloc(400)) != NULL)	{
				WINDOW nwnd = NextWindow(wnd);
				cp1 = cp2 = ln;
        		strncpy(ss, s, 399);
				str = ss;
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
            		*cp1++ = (*str & 255) |
                		(clr(foreground, background) << 8);
            		str++;
        		}
        		foreground = fg;
        		background = bg;
        		len = (int)(cp1-ln);
        		if (x1+len > SCREENWIDTH)
            		len = SCREENWIDTH-x1;

				/* --- clip  the line for overlapping windows --- */
				while (len > 0 && nwnd != NULL)	{
					if (isVisible(nwnd) && !isAncestor(wnd, nwnd))	{
						int x2;
						RECT rc = PrepRect(nwnd, WindowRect(nwnd));
						while (len && InsideRect(x1,y1,rc))	{
							x1++;
							ln++;
							--len;
						}
						if (len)	{
							x2 = x1+len-1;
							while (len && InsideRect(x2,y1,rc))	{
								--x2;
								--len;
							}
						}
					}
					nwnd = NextWindow(nwnd);
				}

				if (!TestAttribute(wnd, NOCLIP))	{
					/* -- clip the line to within ancestor windows -- */
					nwnd = GetParent(wnd);
					while (len > 0 && nwnd != NULL)	{
						if (isVisible(nwnd))	{
							int x2;
							RECT rc = PrepRect(nwnd, ClientRect(nwnd));
							while (len && !InsideRect(x1,y1,rc))	{
								x1++;
								ln++;
								--len;
							}
							if (len)	{
								x2 = x1+len-1;
								while (len && !InsideRect(x2,y1,rc))	{
									--x2;
									--len;
								}
							}
						}
						nwnd = GetParent(nwnd);
					}
				}

				if (len > 0)	{
        			hide_mousecursor();
					movedata(FP_SEG(ln), FP_OFF(ln),
						video_address, vad(x1,y1), len*2);
        			show_mousecursor();
				}
				free(cp2);
			}
			free(ss);
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