/* ---------------- video.h ----------------- */

#ifndef VIDEO_H
#define VIDEO_H

#include "rect.h"

void getvideo(RECT, void *);
void storevideo(RECT, void *);
extern unsigned short video_mode;
extern unsigned short video_page;
void wputch(WINDOW, short, short, short);
unsigned short GetVideoChar(short, short);
void PutVideoChar(short, short, short);
void get_videomode(void);
void wputs(WINDOW, void *, short, short);
void scroll_window(WINDOW, RECT, short);

#define clr(fg,bg) ((fg)|((bg)<<4))
#define vad(x,y) ((y)*(SCREENWIDTH*2)+(x)*2)
#define ismono() (video_mode == 7)
#define istext() (video_mode < 4)
#define videochar(x,y) (GetVideoChar(x,y) & 255)

#endif

