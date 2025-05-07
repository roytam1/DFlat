/* ---------------- classdef.c ---------------- */

#include <stdio.h>
#include "dflat.h"

/*
 * Add class definitions to this table.
 * Add the class symbol to the CLASS list in dflat.h
 */

CLASSDEFS classdefs[] = {
    {   /* ---------- NORMAL Window Class ----------- */
        NORMAL,
        -1,
        NormalProc
    },
    {   /* ---------- APPLICATION Window Class ----------- */
        APPLICATION,
        NORMAL,
        ApplicationProc,
        VISIBLE | SAVESELF | CONTROLBOX
    },
    {   /* ------------ TEXTBOX Window Class -------------- */
        TEXTBOX,
        NORMAL,
        TextBoxProc
    },
    {   /* ------------- LISTBOX Window class ------------- */
        LISTBOX,
        TEXTBOX,
        ListBoxProc
    },
    {   /* ------------- EDITBOX Window Class -------------- */
        EDITBOX,
        TEXTBOX,
        EditBoxProc
    },
    {   /* ------------- MENUBAR Window Class --------------- */
        MENUBAR,
        NORMAL,
        MenuBarProc,
        VISIBLE | NOCLIP
    },
    {   /* ------------- POPDOWNMENU Window Class ----------- */
        POPDOWNMENU,
        LISTBOX,
        PopDownProc,
        SAVESELF | NOCLIP | HASBORDER
    },
    {   /* ----------- BUTTON Window Class --------------- */
        BUTTON,
        TEXTBOX,
        ButtonProc,
        SHADOW | NOCLIP
    },
    {   /* ------------- DIALOG Window Class -------------- */
        DIALOG,
        NORMAL,
        DialogProc,
        SHADOW | MOVEABLE | CONTROLBOX | HASBORDER | NOCLIP
    },
    {   /* ------------ ERRORBOX Window Class ----------- */
        ERRORBOX,
        DIALOG,
        DialogProc,
        SHADOW | HASBORDER
    },
    {   /* --------- MESSAGEBOX Window Class ------------- */
        MESSAGEBOX,
        DIALOG,
        DialogProc,
        SHADOW | HASBORDER
    },
    {   /* ----------- HELPBOX Window Class --------------- */
        HELPBOX,
		DIALOG,
        HelpBoxProc,
        SHADOW | MOVEABLE | SAVESELF | HASBORDER | NOCLIP | CONTROLBOX
    },
    {   /* -------------- DUMMY Window Class ---------------- */
        DUMMY,
        -1,
        NULL,
        HASBORDER
    }
};

/* ------- return the offset of a class into the class
                 definition table ------ */
int FindClass(CLASS class)
{
    int i;
    for (i = 0; i < sizeof(classdefs) / sizeof(CLASSDEFS); i++)
        if (class == classdefs[i].class)
            return i;
    return 0;
}



