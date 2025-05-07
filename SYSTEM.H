/* --------------- system.h -------------- */
#ifndef SYSTEM_H
#define SYSTEM_H

#if MSC | WATCOM
#include <direct.h>
#else
#include <dir.h>
#endif

#define swap(a,b){short x=a;a=b;b=x;}
/* ----- interrupt vectors ----- */
#define TIMER  8
#define VIDEO  0x10
#define KEYBRD 0x16
#define DOS    0x21
#define CRIT   0x24
#define MOUSE  0x33
#define KEYBOARDVECT 9
/* ------- platform-dependent values ------ */
#define KEYBOARDPORT 0x60
#define FREQUENCY 100
#define COUNT (1193280L / FREQUENCY)
#define ZEROFLAG 0x40
#define MAXSAVES 50
#define SCREENWIDTH  (peekb(0x40,0x4a) & 255)
#define SCREENHEIGHT (isVGA() || isEGA() ? peekb(0x40,0x84)+1 : 25)
#ifdef __FLAT__
#define clearBIOSbuffer() *(short *)(MK_FP(0x40,0x1a)) = \
		        	      *(short *)(MK_FP(0x40,0x1c));
#else
#define clearBIOSbuffer() *(short far *)(MK_FP(0x40,0x1a)) = \
		        	      *(short far *)(MK_FP(0x40,0x1c));
#endif


#define waitforkeyboard() while ((keyportvalue & 0x80) == 0) \
						  clearBIOSbuffer()
/* ----- keyboard BIOS (0x16) functions -------- */
#define READKB 0
#define KBSTAT 1
/* ------- video BIOS (0x10) functions --------- */
#define SETCURSORTYPE 1
#define SETCURSOR     2
#define READCURSOR    3
#define READATTRCHAR  8
#define WRITEATTRCHAR 9
#define HIDECURSOR 0x20
/* ------- the interrupt function registers -------- */
typedef struct {
    short bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,fl;
} IREGS;
/* ---------- keyboard prototypes -------- */
short AltConvert(short);
short getkey(void);
short getshift(void);
BOOL keyhit(void);
void beep(void);
/* ---------- cursor prototypes -------- */
void curr_cursor(short *x, short *y);
void cursor(short x, short y);
void hidecursor(void);
void unhidecursor(void);
void savecursor(void);
void restorecursor(void);
void normalcursor(void);
void set_cursor_type(unsigned short t);
void videomode(void);
void SwapCursorStack(void);
/* ---------- mouse prototypes ---------- */
BOOL mouse_installed(void);
short mousebuttons(void);
void get_mouseposition(short *x, short *y);
void set_mouseposition(short x, short y);
void show_mousecursor(void);
void hide_mousecursor(void);
short button_releases(void);
void resetmouse(void);
void set_mousetravel(short, short, short, short);
#define leftbutton()     (mousebuttons()&1)
#define rightbutton()     (mousebuttons()&2)
#define waitformouse()     while(mousebuttons());
/* ------------ timer macros -------------- */
#define timed_out(timer)         (timer==0)
#define set_timer(timer, secs)     timer=(secs)*182/10+1
#define disable_timer(timer)     timer = -1
#define timer_running(timer)     (timer > 0)
#define countdown(timer)         --timer
#define timer_disabled(timer)     (timer == -1)
/* ----------- video adaptor prototypes ----------- */
BOOL isEGA(void);
BOOL isVGA(void);
void Set25(void);
void Set43(void);
void Set50(void);

#ifndef TURBOC
#ifndef BCPP
/* ============= Color Macros ============ */
#define BLACK         0
#define BLUE          1
#define GREEN         2
#define CYAN          3
#define RED           4
#define MAGENTA       5
#define BROWN         6
#define LIGHTGRAY     7
#define DARKGRAY      8
#define LIGHTBLUE     9
#define LIGHTGREEN   10
#define LIGHTCYAN    11
#define LIGHTRED     12
#define LIGHTMAGENTA 13
#define YELLOW       14
#define WHITE        15
#define keyhit       kbhit
#endif
#endif

#if MSC | WATCOM
/* ============= Compatibility Macros ============ */
#define asm __asm
#undef FP_OFF
#undef FP_SEG
#undef MK_FP
#ifdef __FLAT__
#define FP_OFF(p)    ((unsigned short)(p) & 0x0f)
#define FP_SEG(p)    ((unsigned short)((unsigned long)(p) >> 4))
#define MK_FP(s,o)   ((void *) \
               (((unsigned long)(s) << 4) + (unsigned short)(o)))
#else
#define FP_OFF(p)    ((unsigned short)(p))
#define FP_SEG(p)    ((unsigned short)((unsigned long)(p) >> 16))
#define MK_FP(s,o)   ((void far *) \
               (((unsigned long)(s) << 16) | (unsigned short)(o)))
#endif
#define findfirst(p,f,a) _dos_findfirst(p,a,f)
#define findnext(f)      _dos_findnext(f)
#define ffblk            find_t
#define ff_name          name
#define ff_fsize         size
#define ff_attrib        attrib
#ifdef	__FLAT__
#define poke(a,b,c)  (*((short  *)MK_FP((a),(b))) = (short)(c))
#define pokeb(a,b,c) (*((char *)MK_FP((a),(b))) = (char)(c))
#define peek(a,b)    (*((short  *)MK_FP((a),(b))))
#define peekb(a,b)   (*((char *)MK_FP((a),(b))))
#else
#define poke(a,b,c)  (*((short  far*)MK_FP((a),(b))) = (short)(c))
#define pokeb(a,b,c) (*((char far*)MK_FP((a),(b))) = (char)(c))
#define peek(a,b)    (*((short  far*)MK_FP((a),(b))))
#define peekb(a,b)   (*((char far*)MK_FP((a),(b))))
#endif

#define getvect(v)   _dos_getvect(v)
#define setvect(v,f) _dos_setvect(v,f)
#define fnsplit          _splitpath
#define fnmerge          _makepath
#define EXTENSION         2
#define FILENAME          4
#define DIRECTORY         8
#define DRIVE            16
short getdisk(void);
#define setdisk(d)       _dos_setdrive((d)+1, NULL)
#define bioskey          _bios_keybrd
#define MAXPATH          80
#define MAXDRIVE          3
#define MAXDIR           66
#define MAXFILE           9
#define MAXEXT            5
#endif

typedef enum messages {
#ifdef WATCOM
	WATCOMFIX1 = -1,
#endif
	#undef DFlatMsg
	#define DFlatMsg(m) m,
	#include "dflatmsg.h"
	MESSAGECOUNT
} MESSAGE;

typedef enum window_class    {
#ifdef WATCOM
	WATCOMFIX2 = -1,
#endif
	#define ClassDef(c,b,p,a) c,
	#include "classes.h"
	CLASSCOUNT
} CLASS;

#endif

