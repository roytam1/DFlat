/* ------------- mouse.c ------------- */

#include "dflat.h"

static union REGS regs;
static struct SREGS sregs;

static void near mouse(short m1,short m2,short m3,short m4)
{
    regs.w.dx = m4;
    regs.w.cx = m3;
    regs.w.bx = m2;
    regs.w.ax = m1;
#ifdef __FLAT__
	 int386x(MOUSE, &regs, &regs, &sregs);
#else
    int86x(MOUSE, &regs, &regs, &sregs);
#endif
}

/* ---------- reset the mouse ---------- */
void resetmouse(void)
{
	segread(&sregs);
    mouse(0,0,0,0);
}

/* ----- test to see if the mouse driver is installed ----- */
BOOL mouse_installed(void)
{
    unsigned char *ms;
    ms = MK_FP(peek(0, MOUSE*4+2), peek(0, MOUSE*4));
    return (SCREENWIDTH <= 80 && ms != NULL && *ms != 0xcf);
}

/* ------ return true if mouse buttons are pressed ------- */
short mousebuttons(void)
{
    if (mouse_installed())	{
		segread(&sregs);
        mouse(3,0,0,0);
	    return regs.w.bx & 3;
	}
	return 0;
}

/* ---------- return mouse coordinates ---------- */
void get_mouseposition(short *x, short *y)
{
	*x = *y = -1;
    if (mouse_installed())    {
		segread(&sregs);
        mouse(3,0,0,0);
        *x = regs.w.cx/8;
        *y = regs.w.dx/8;
		if (SCREENWIDTH == 40)
			*x /= 2;
    }
}

/* -------- position the mouse cursor -------- */
void set_mouseposition(short x, short y)
{
    if (mouse_installed())	{
		segread(&sregs);
		if (SCREENWIDTH == 40)
			x *= 2;
        mouse(4,0,x*8,y*8);
	}
}

/* --------- display the mouse cursor -------- */
void show_mousecursor(void)
{
    if (mouse_installed())	{
		segread(&sregs);
        mouse(1,0,0,0);
	}
}

/* --------- hide the mouse cursor ------- */
void hide_mousecursor(void)
{
    if (mouse_installed())	{
		segread(&sregs);
        mouse(2,0,0,0);
	}
}

/* --- return true if a mouse button has been released --- */
short button_releases(void)
{
    if (mouse_installed())	{
		segread(&sregs);
        mouse(6,0,0,0);
	    return regs.w.bx;
	}
	return 0;
}

/* ----- set mouse travel limits ------- */
void set_mousetravel(short minx, short maxx, short miny, short maxy)
{
    if (mouse_installed())	{
		if (SCREENWIDTH == 40)	{
			minx *= 2;
			maxx *= 2;
		}
		segread(&sregs);
        mouse(7, 0, minx*8, maxx*8);
		mouse(8, 0, miny*8, maxy*8);
	}
}


