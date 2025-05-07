/* ------------ help.c ----------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dflat.h"

int HelpBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	switch (msg)	{
		case CREATE_WINDOW:
			SendMessage(wnd, CAPTURE_MOUSE, 0, 0);
			SendMessage(wnd, CAPTURE_KEYBOARD, 0, 0);
			break;
		case LEFT_BUTTON:
			if (WindowSizing || WindowMoving)
				return FALSE;
			if (HitControlBox(wnd, p1-GetLeft(wnd), p2-GetTop(wnd)))	{
				PostMessage(wnd, KEYBOARD, ' ', ALTKEY);
				return TRUE;
			}
			break;
		case KEYBOARD:
			switch ((int) p1)	{
				case ALT_F4:
				case ESC:
					PostMessage(wnd, CLOSE_WINDOW, 0, 0);
					break;
				case ' ':
					if (p2 & ALTKEY)
						BuildSystemMenu(wnd);
					break;
				default:
					break;
			}
			break;
		case CLOSE_WINDOW:
			SendMessage(wnd, RELEASE_MOUSE, 0, 0);
			SendMessage(wnd, RELEASE_KEYBOARD, 0, 0);
			break;
		default:
			break;
	}
	return BaseWndProc(HELPBOX, wnd, msg, p1, p2);
}

#define MAXHEIGHT 15

struct helps {
    char *hname;
    long hptr;
	int hwidth;
	int hheight;
	struct helps *NextHelp;
};

static struct helps *FirstHelp = NULL;
static struct helps *LastHelp = NULL;
static struct helps *ThisHelp = NULL;

static FILE *helpfp;
static char hline [80];
static char helpname[65];

/* ----------- load the help text file ------------ */
void LoadHelpFile(char *expath)
{
    char *cp;
	int first;

	UnLoadHelpFile();
	strcpy(helpname, expath);
	cp = strrchr(helpname, '\\');
	if (cp == NULL)
		cp = helpname;
	strcpy(cp, "\\" DFLAT_APPLICATION ".HLP");
    if ((helpfp = fopen(helpname, "rt")) == NULL)
        return;
	*hline = '\0';
	while (*hline != '<')	{
	    if ((fgets(hline, 80, helpfp)) == NULL)	{
			fclose(helpfp);
   	    	return;
		}
	}
    while (*hline == '<')   {
        if (strncmp(hline, "<end>", 5) == 0)
            break;

		if ((ThisHelp = malloc(sizeof(struct helps))) == NULL)
			break;

		if (FirstHelp == NULL)
			FirstHelp = ThisHelp;

		if ((cp = strchr(hline, '>')) == NULL)
			continue;
		*cp = '\0';

		if ((ThisHelp->hname=malloc(strlen(hline+1)+1))==NULL)
			break;

		/* ------- build the help linked list entry --- */
        strcpy(ThisHelp->hname, hline+1);
        ThisHelp->hptr = ftell(helpfp);
		ThisHelp->hwidth = 0;
		ThisHelp->hheight = 0;
		ThisHelp->NextHelp = NULL;

		/* ------ append entry to the linked list ------ */
		if (LastHelp != NULL)
			LastHelp->NextHelp = ThisHelp;
		LastHelp = ThisHelp;

		/* -------- move to the next <> token ------ */
        if (fgets(hline, 80, helpfp) == NULL)
            strcpy(hline, "<end>");
		first = TRUE;
        while (*hline != '<')	{
			if (first)	{
				ThisHelp->hwidth = strlen(hline)+6;
				first = FALSE;
			}
			else 
				ThisHelp->hwidth =
					max(ThisHelp->hwidth, strlen(hline));
			if (ThisHelp->hheight < MAXHEIGHT)
				ThisHelp->hheight++;
	        if (fgets(hline, 80, helpfp) == NULL)
    	        strcpy(hline, "<end>");
		}
    }
	fclose(helpfp);
}

/* ------ free the memory used by the help file table ------ */
void UnLoadHelpFile(void)
{
	while (FirstHelp != NULL)	{
		ThisHelp = FirstHelp;
		free(ThisHelp->hname);
		FirstHelp = ThisHelp->NextHelp;
		free(ThisHelp);
	}
	ThisHelp = LastHelp = NULL;
}

/* ------------ display help text ----------- */
void DisplayHelp(char *Help)
{
	WINDOW wnd;

	if ((helpfp = fopen(helpname, "rt")) == NULL)
       return;

	ThisHelp = FirstHelp;

	while (ThisHelp != NULL)	{
        if (stricmp(Help, ThisHelp->hname) == 0)
            break;
		ThisHelp = ThisHelp->NextHelp;
	}
    if (ThisHelp != NULL)    {
      	fseek(helpfp, ThisHelp->hptr, 0);
       	fgets(hline, 80, helpfp);
		hline[strlen(hline)-1] = '\0';
		/* ----- create the help window ------ */
		wnd = CreateWindow(HELPBOX,
							hline,
							-1, -1,
							ThisHelp->hheight+2,
							ThisHelp->hwidth+2,
							NULL,
							NULL,
							NULL,
							0);
		if (wnd != NULLWND)	{
			/* ----- read the help text ------- */
			hline[strlen(hline)-1] = '\0';
			while (TRUE)	{
            	if (fgets(hline, 80, helpfp) == NULL)
					break;
				hline[strlen(hline)-1] = '\0';
				if (*hline == '<')
					break;
				/* --- add help text to the help window --- */
				SendMessage(wnd, ADDTEXT, LPARAM(hline), 0);
			}
			SendMessage(wnd, SETFOCUS, TRUE, 0);
		}
    }
	fclose(helpfp);
}
