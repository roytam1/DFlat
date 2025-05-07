/* --------------- lists.c -------------- */

#include "dflat.h"

struct LinkedList Focus;
struct LinkedList Built;

/* --- set focus to the window beneath the one specified --- */
void SetPrevFocus(WINDOW wnd)
{
    if (wnd != NULL && wnd == inFocus)    {
        WINDOW wnd1 = wnd;
        while (TRUE)    {
            if ((wnd1 = PrevWindow(wnd1)) == NULL)
                wnd1 = Focus.LastWindow;
            if (wnd1 == wnd)
                return;
            if (wnd1 != NULL)
                break;
        }
        if (wnd1 != NULL)
            SendMessage(wnd1, SETFOCUS, TRUE, 0);
    }
}

/* this function assumes that wnd is in the Focus linked list */
static WINDOW SearchFocusNext(WINDOW wnd, WINDOW pwnd)
{
    WINDOW wnd1 = wnd;

    if (wnd != NULL)    {
        while (TRUE)    {
            if ((wnd1 = NextWindow(wnd1)) == NULL)
                wnd1 = Focus.FirstWindow;
            if (wnd1 == wnd)
                return NULL;
            if (wnd1 != NULL)
                if (pwnd == NULL || pwnd == GetParent(wnd1))
                    break;
        }
    }
    return wnd1;
}

/* ----- set focus to the next sibling ----- */
void SetNextFocus(WINDOW wnd)
{
    WINDOW wnd1;

    if (wnd != inFocus)
        return;
    if ((wnd1 = SearchFocusNext(wnd, GetParent(wnd)))==NULL)
        wnd1 = SearchFocusNext(wnd, NULL);
    if (wnd1 != NULL)
        SendMessage(wnd1, SETFOCUS, TRUE, 0);
}

/* ---- remove a window from the Built linked list ---- */
void RemoveBuiltWindow(WINDOW wnd)
{
    if (wnd != NULL)    {
        if (PrevWindowBuilt(wnd) != NULL)
            NextWindowBuilt(PrevWindowBuilt(wnd)) =
                NextWindowBuilt(wnd);
        if (NextWindowBuilt(wnd) != NULL)
            PrevWindowBuilt(NextWindowBuilt(wnd)) =
                PrevWindowBuilt(wnd);
        if (wnd == Built.FirstWindow)
            Built.FirstWindow = NextWindowBuilt(wnd);
        if (wnd == Built.LastWindow)
            Built.LastWindow = PrevWindowBuilt(wnd);
    }
}

/* ---- remove a window from the Focus linked list ---- */
void RemoveFocusWindow(WINDOW wnd)
{
    if (wnd != NULL)    {
        if (PrevWindow(wnd) != NULL)
            NextWindow(PrevWindow(wnd)) = NextWindow(wnd);
        if (NextWindow(wnd) != NULL)
            PrevWindow(NextWindow(wnd)) = PrevWindow(wnd);
        if (wnd == Focus.FirstWindow)
            Focus.FirstWindow = NextWindow(wnd);
        if (wnd == Focus.LastWindow)
            Focus.LastWindow = PrevWindow(wnd);
    }
}

/* ---- append a window to the Built linked list ---- */
void AppendBuiltWindow(WINDOW wnd)
{
    if (wnd != NULL)    {
        if (Built.FirstWindow == NULL)
            Built.FirstWindow = wnd;
        if (Built.LastWindow != NULL)
            NextWindowBuilt(Built.LastWindow) = wnd;
        PrevWindowBuilt(wnd) = Built.LastWindow;
        NextWindowBuilt(wnd) = NULL;
        Built.LastWindow = wnd;
    }
}

/* ---- append a window to the Focus linked list ---- */
void AppendFocusWindow(WINDOW wnd)
{
    if (wnd != NULL)    {
        if (Focus.FirstWindow == NULL)
            Focus.FirstWindow = wnd;
        if (Focus.LastWindow != NULL)
            NextWindow(Focus.LastWindow) = wnd;
        PrevWindow(wnd) = Focus.LastWindow;
        NextWindow(wnd) = NULL;
        Focus.LastWindow = wnd;
    }
}

/* ---- add a window to the beginning of the Focus linked list ---- */
void PrependFocusWindow(WINDOW wnd)
{
    if (wnd != NULL)    {
        if (Focus.LastWindow == NULL)
            Focus.LastWindow = wnd;
        if (Focus.FirstWindow != NULL)
            PrevWindow(Focus.FirstWindow) = wnd;
        NextWindow(wnd) = Focus.FirstWindow;
        PrevWindow(wnd) = NULL;
        Focus.FirstWindow = wnd;
    }
}

/* -------- get the first child of a parent window ------- */
WINDOW GetFirstChild(WINDOW wnd)
{
    WINDOW ThisWindow = Built.FirstWindow;
    while (ThisWindow != NULL)    {
        if (GetParent(ThisWindow) == wnd)
            break;
        ThisWindow = NextWindowBuilt(ThisWindow);
    }
    return ThisWindow;
}

/* -------- get the next child of a parent window ------- */
WINDOW GetNextChild(WINDOW wnd, WINDOW ThisWindow)
{
    if (ThisWindow != NULL)    {
        do    {
            if ((ThisWindow = NextWindowBuilt(ThisWindow)) !=
                    NULL)
                if (GetParent(ThisWindow) == wnd)
                    break;
        }    while (ThisWindow != NULL);
    }
    return ThisWindow;
}

/* -- get first child of parent window from the Focus list -- */
WINDOW GetFirstFocusChild(WINDOW wnd)
{
    WINDOW ThisWindow = Focus.FirstWindow;
    while (ThisWindow != NULL)    {
        if (GetParent(ThisWindow) == wnd)
            break;
        ThisWindow = NextWindow(ThisWindow);
    }
    return ThisWindow;
}

/* -- get next child of parent window from the Focus list -- */
WINDOW GetNextFocusChild(WINDOW wnd, WINDOW ThisWindow)
{
    while (ThisWindow != NULL)    {
        ThisWindow = NextWindow(ThisWindow);
        if (ThisWindow != NULL)
            if (GetParent(ThisWindow) == wnd)
                break;
    }
    return ThisWindow;
}

/* -------- get the last child of a parent window ------- */
WINDOW GetLastChild(WINDOW wnd)
{
    WINDOW ThisWindow = Built.LastWindow;
    while (ThisWindow != NULL)    {
        if (GetParent(ThisWindow) == wnd)
            break;
        ThisWindow = PrevWindowBuilt(ThisWindow);
    }
    return ThisWindow;
}

/* ------- get the previous child of a parent window ------- */
WINDOW GetPrevChild(WINDOW wnd, WINDOW ThisWindow)
{
    if (ThisWindow != NULL)    {
        do    {
            if ((ThisWindow = PrevWindowBuilt(ThisWindow)) !=
                    NULL)
                if (GetParent(ThisWindow) == wnd)
                    break;
        }    while (ThisWindow != NULL);
    }
    return ThisWindow;
}

/* --- bypass system windows when stepping through focus --- */
void SkipSystemWindows(int Prev)
{
    int cl, ct = 0;
    while ((cl = GetClass(inFocus)) == MENUBAR ||
            cl == APPLICATION || cl == STATUSBAR)    {
        if (Prev)
            SetPrevFocus(inFocus);
        else 
            SetNextFocus(inFocus);
        if (++ct == 3)
            break;
    }
}


