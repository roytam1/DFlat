/* ------------- dflat.h ----------- */
#ifndef DFLAT_H
#define DFLAT_H

#ifdef BUILD_FULL_DFLAT
#define INCLUDE_MULTI_WINDOWS
#define INCLUDE_LOGGING
#define INCLUDE_SHELLDOS
#define INCLUDE_WINDOWOPTIONS
#define INCLUDE_PICTUREBOX
#define INCLUDE_MINIMIZE
#define INCLUDE_MAXIMIZE
#define INCLUDE_RESTORE
#define INCLUDE_EXTENDEDSELECTIONS
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <process.h>
#include <conio.h>
#include <bios.h>
#include <ctype.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <time.h>

#define VERSION "Version 10"

#define TRUE 1
#define FALSE 0

#define MAXMESSAGES 50
#define DELAYTICKS 1
#define FIRSTDELAY 7
#define DOUBLETICKS 5

#define MAXTEXTLEN 65000U /* maximum text buffer            */
#define EDITLEN     1024  /* starting length for multiliner */
#define ENTRYLEN     256  /* starting length for one-liner  */
#define GROWLENGTH    64  /* buffers grow by this much      */

#include "system.h"
#include "config.h"
#include "rect.h"
#include "menu.h"
#include "keys.h"
#include "commands.h"
#include "dialbox.h"

/* ------ integer type for message parameters ----- */
typedef long PARAM;

typedef struct window {
    CLASS class;           /* window class                  */
    char *title;           /* window title                  */
    struct window *parent; /* parent window                 */
    int (*wndproc)
        (struct window *, enum messages, PARAM, PARAM);
	/* ----------------- window colors -------------------- */
	char WindowColors[4][2];
    /* ---------------- window dimensions ----------------- */
    RECT rc;               /* window coordinates
                                            (0/0 to 79/24)  */
    int ht, wd;            /* window height and width       */
    RECT RestoredRC;       /* restored condition rect       */
    /* -------------- linked list pointers ---------------- */
    struct window *nextfocus;   /* next window on screen    */
    struct window *prevfocus;   /* previous window on screen*/
    struct window *nextbuilt;   /* next window built        */
    struct window *prevbuilt;   /* previous window built    */

    int attrib;                 /* Window attributes        */
    char *videosave;            /* video save buffer        */
    int condition;              /* Restored, Maximized,
                                   Minimized, Closing       */
    int oldcondition;           /* previous condition       */
    int restored_attrib;        /* attributes when restored */
    void *extension;      /* menus, dialogs, documents, etc */
    struct window *PrevMouse;
    struct window *PrevKeyboard;
	struct window *MenuBarWnd;/* menu bar                   */
	struct window *StatusBar; /* status bar                 */
    /* ----------------- text box fields ------------------ */
    int wlines;     /* number of lines of text              */
    int wtop;       /* text line that is on the top display */
    unsigned char *text; /* window text                     */
    unsigned int textlen;  /* text length                   */
    int wleft;      /* left position in window viewport     */
    int textwidth;  /* width of longest line in textbox     */
    int BlkBegLine; /* beginning line of marked block       */
    int BlkBegCol;  /* beginning column of marked block     */
    int BlkEndLine; /* ending line of marked block          */
    int BlkEndCol;  /* ending column of marked block        */
    int HScrollBox; /* position of horizontal scroll box    */
    int VScrollBox; /* position of vertical scroll box      */
	unsigned int *TextPointers; /* -> list of line offsets	*/
    /* ----------------- list box fields ------------------ */
    int selection;  /* current selection                    */
    int AddMode;    /* adding extended selections mode      */
    int AnchorPoint;/* anchor point for extended selections */
    int SelectCount;/* count of selected items              */
    /* ----------------- edit box fields ------------------ */
    int CurrCol;    /* Current column                       */
    int CurrLine;   /* Current line                         */
    int WndRow;     /* Current window row                   */
    int TextChanged; /* TRUE if text has changed            */
    unsigned char *DeletedText; /* for undo                 */
    unsigned DeletedLength; /* Length of deleted field      */
	int InsertMode;    /* TRUE or FALSE for text insert     */
	int WordWrapMode;  /* TRUE or FALSE for word wrap       */
	unsigned int MaxTextLength; /* maximum text length      */
    /* ---------------- dialog box fields ----------------- */
    void *dFocus;          /* control that has the focus    */
    int ReturnCode;        /* return code from a dialog box */
	int Modal;		       /* True if a modeless dialog box */
	CTLWINDOW *ct;		   /* control structure             */
	/* -------------- popdownmenu fields ------------------ */
	MENU *mnu;		/* points to menu structure             */
	MBAR *holdmenu; /* previous active menu                 */
	/* --------------- help box fields -------------------- */
	void *firstword; /* -> first in list of key words       */
	void *lastword;  /* -> last in list of key words        */
	void *thisword;  /* -> current in list of key words     */
	/* -------------- status bar fields ------------------- */
	int TimePosted;  /* True if time has been posted        */
#ifdef INCLUDE_PICTUREBOX
	/* ------------- picture box fields ------------------- */
	int VectorCount;  /* number of vectors in vector list   */
	void *VectorList; /* list of picture box vectors        */
#endif
} * WINDOW;

#include "classdef.h"
#include "video.h"

enum Condition     {
    ISRESTORED, ISMINIMIZED, ISMAXIMIZED, ISCLOSING
};

void LogMessages (WINDOW, MESSAGE, PARAM, PARAM);
void MessageLog(WINDOW);
/* ------- window methods ----------- */
#define ICONHEIGHT 3
#define ICONWIDTH  10
#define WindowHeight(w)      ((w)->ht)
#define WindowWidth(w)       ((w)->wd)
#define BorderAdj(w)         (TestAttribute(w,HASBORDER)?1:0)
#define BottomBorderAdj(w)   (TestAttribute(w,HASSTATUSBAR)?1:BorderAdj(w))
#define TopBorderAdj(w)      ((TestAttribute(w,HASTITLEBAR) &&   \
                              TestAttribute(w,HASMENUBAR)) ?  \
                              2 : (TestAttribute(w,HASTITLEBAR | \
                              HASMENUBAR | HASBORDER) ? 1 : 0))
#define ClientWidth(w)       (WindowWidth(w)-BorderAdj(w)*2)
#define ClientHeight(w)      (WindowHeight(w)-TopBorderAdj(w)-\
                              BottomBorderAdj(w))
#define WindowRect(w)        ((w)->rc)
#define GetTop(w)            (RectTop(WindowRect(w)))
#define GetBottom(w)         (RectBottom(WindowRect(w)))
#define GetLeft(w)           (RectLeft(WindowRect(w)))
#define GetRight(w)          (RectRight(WindowRect(w)))
#define GetClientTop(w)      (GetTop(w)+TopBorderAdj(w))
#define GetClientBottom(w)   (GetBottom(w)-BottomBorderAdj(w))
#define GetClientLeft(w)     (GetLeft(w)+BorderAdj(w))
#define GetClientRight(w)    (GetRight(w)-BorderAdj(w))
#define GetParent(w)         ((w)->parent)
#define GetTitle(w)          ((w)->title)
#define NextWindow(w)        ((w)->nextfocus)
#define PrevWindow(w)        ((w)->prevfocus)
#define NextWindowBuilt(w)   ((w)->nextbuilt)
#define PrevWindowBuilt(w)   ((w)->prevbuilt)
#define GetClass(w)          ((w)->class)
#define GetAttribute(w)      ((w)->attrib)
#define AddAttribute(w,a)    (GetAttribute(w) |= a)
#define ClearAttribute(w,a)  (GetAttribute(w) &= ~(a))
#define TestAttribute(w,a)   (GetAttribute(w) & (a))
#define isWndVisible(w)      (GetAttribute(w) & VISIBLE)
#define SetVisible(w)        (GetAttribute(w) |= VISIBLE)
#define ClearVisible(w)      (GetAttribute(w) &= ~VISIBLE)
#define gotoxy(w,x,y) cursor(w->rc.lf+(x)+1,w->rc.tp+(y)+1)
int isVisible(WINDOW);
WINDOW CreateWindow(CLASS,char *,int,int,int,int,void*,WINDOW,
       int (*)(struct window *,enum messages,PARAM,PARAM),int);
void AddTitle(WINDOW, char *);
void InsertTitle(WINDOW, char *);
void DisplayTitle(WINDOW, RECT *);
void RepaintBorder(WINDOW, RECT *);
void ClearWindow(WINDOW, RECT *, int);
void writeline(WINDOW, char *, int, int, int);
void InitWindowColors(WINDOW);

void SetNextFocus(WINDOW);
void SetPrevFocus(WINDOW);
void SkipSystemWindows(int);
void RemoveFocusWindow(WINDOW);
void AppendFocusWindow(WINDOW);
void PrependFocusWindow(WINDOW);
void RemoveBuiltWindow(WINDOW);
void AppendBuiltWindow(WINDOW);
WINDOW GetFirstChild(WINDOW);
WINDOW GetNextChild(WINDOW, WINDOW);
WINDOW GetLastChild(WINDOW);
WINDOW GetPrevChild(WINDOW, WINDOW);
WINDOW GetFirstFocusChild(WINDOW);
WINDOW GetNextFocusChild(WINDOW, WINDOW);

int CharInView(WINDOW, int, int);
void GetVideoBuffer(WINDOW);
void RestoreVideoBuffer(WINDOW);
void CreatePath(char *, char *, int, int);
int LineLength(char *);
RECT AdjustRectangle(WINDOW, RECT);
int isDerivedFrom(WINDOW, CLASS);
WINDOW GetAncestor(WINDOW);
void PutWindowChar(WINDOW,int,int,int);
void PutWindowLine(WINDOW, void *,int,int);
#define BaseWndProc(class,wnd,msg,p1,p2)    \
    (*classdefs[(classdefs[class].base)].wndproc)(wnd,msg,p1,p2)
#define DefaultWndProc(wnd,msg,p1,p2)         \
	(classdefs[wnd->class].wndproc == NULL) ? \
	BaseWndProc(wnd->class,wnd,msg,p1,p2) :	  \
    (*classdefs[wnd->class].wndproc)(wnd,msg,p1,p2)
struct LinkedList    {
    WINDOW FirstWindow;
    WINDOW LastWindow;
};
extern struct LinkedList Focus;
extern struct LinkedList Built;
extern WINDOW inFocus;
extern WINDOW CaptureMouse;
extern WINDOW CaptureKeyboard;
extern int foreground, background;
extern int WindowMoving;
extern int WindowSizing;
extern int TextMarking;
extern int VSliding;
extern int HSliding;
extern char *Clipboard;
extern unsigned ClipboardLength;
extern int VSliding;
extern int HSliding;
extern char DFlatApplication[];
extern int ClipString;
/* --------- space between menubar labels --------- */
#define MSPACE 2
/* --------------- border characters ------------- */
#define FOCUS_NW      (unsigned char) '\xc9'
#define FOCUS_NE      (unsigned char) '\xbb'
#define FOCUS_SE      (unsigned char) '\xbc'
#define FOCUS_SW      (unsigned char) '\xc8'
#define FOCUS_SIDE    (unsigned char) '\xba'
#define FOCUS_LINE    (unsigned char) '\xcd'
#define NW            (unsigned char) '\xda'
#define NE            (unsigned char) '\xbf'
#define SE            (unsigned char) '\xd9'
#define SW            (unsigned char) '\xc0'
#define SIDE          (unsigned char) '\xb3'
#define LINE          (unsigned char) '\xc4'
#define LEDGE         (unsigned char) '\xc3'
#define REDGE         (unsigned char) '\xb4'
/* ------------- scroll bar characters ------------ */
#define UPSCROLLBOX    (unsigned char) '\x1e'
#define DOWNSCROLLBOX  (unsigned char) '\x1f'
#define LEFTSCROLLBOX  (unsigned char) '\x11'
#define RIGHTSCROLLBOX (unsigned char) '\x10'
#define SCROLLBARCHAR  (unsigned char) 176 
#define SCROLLBOXCHAR  (unsigned char) 178
/* ------------------ menu characters --------------------- */
#define CHECKMARK      (unsigned char) (SCREENHEIGHT==25?251:4)
#define CASCADEPOINTER (unsigned char) '\x10'
/* ----------------- title bar characters ----------------- */
#define CONTROLBOXCHAR (unsigned char) '\xf0'
#define MAXPOINTER     24      /* maximize token            */
#define MINPOINTER     25      /* minimize token            */
#define RESTOREPOINTER 18      /* restore token             */
/* --------------- text control characters ---------------- */
#define APPLCHAR     (unsigned char) 176 /* fills application window */
#define SHORTCUTCHAR '~'    /* prefix: shortcut key display */
#define CHANGECOLOR  (unsigned char) 174 /* prefix to change colors  */
#define RESETCOLOR   (unsigned char) 175 /* reset colors to default  */
#define LISTSELECTOR   4    /* selected list box entry      */
/* --------- message prototypes ----------- */
void init_messages(void);
void PostMessage(WINDOW, MESSAGE, PARAM, PARAM);
int SendMessage(WINDOW, MESSAGE, PARAM, PARAM);
int dispatch_message(void);
int TestCriticalError(void);
/* ---- standard window message processing prototypes ----- */
int ApplicationProc(WINDOW, MESSAGE, PARAM, PARAM);
int NormalProc(WINDOW, MESSAGE, PARAM, PARAM);
int TextBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int ListBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int EditBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int PictureProc(WINDOW, MESSAGE, PARAM, PARAM);
int MenuBarProc(WINDOW, MESSAGE, PARAM, PARAM);
int PopDownProc(WINDOW, MESSAGE, PARAM, PARAM);
int ButtonProc(WINDOW, MESSAGE, PARAM, PARAM);
int ComboProc(WINDOW, MESSAGE, PARAM, PARAM);
int TextProc(WINDOW, MESSAGE, PARAM, PARAM);
int RadioButtonProc(WINDOW, MESSAGE, PARAM, PARAM);
int CheckBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int SpinButtonProc(WINDOW, MESSAGE, PARAM, PARAM);
int BoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int DialogProc(WINDOW, MESSAGE, PARAM, PARAM);
int SystemMenuProc(WINDOW, MESSAGE, PARAM, PARAM);
int HelpBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int MessageBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int CancelBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int ErrorBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int YesNoBoxProc(WINDOW, MESSAGE, PARAM, PARAM);
int StatusBarProc(WINDOW, MESSAGE, PARAM, PARAM);
/* ------------- normal box prototypes ------------- */
int isWindow(WINDOW);
WINDOW inWindow(int, int);
void SetStandardColor(WINDOW);
void SetReverseColor(WINDOW);
#define HitControlBox(wnd, p1, p2)     \
     (TestAttribute(wnd, CONTROLBOX) && \
     p1 == 2 && p2 == 0)
#define WndForeground(wnd) 		\
	(wnd->WindowColors [STD_COLOR] [FG])
#define WndBackground(wnd) 		\
	(wnd->WindowColors [STD_COLOR] [BG])
#define FrameForeground(wnd) 	\
	(wnd->WindowColors [FRAME_COLOR] [FG])
#define FrameBackground(wnd) 	\
	(wnd->WindowColors [FRAME_COLOR] [BG])
#define SelectForeground(wnd) 	\
	(wnd->WindowColors [SELECT_COLOR] [FG])
#define SelectBackground(wnd) 	\
	(wnd->WindowColors [SELECT_COLOR] [BG])
#define HighlightForeground(wnd) 	\
	(wnd->WindowColors [HILITE_COLOR] [FG])
#define HighlightBackground(wnd) 	\
	(wnd->WindowColors [HILITE_COLOR] [BG])
#define WindowClientColor(wnd, fg, bg) 	\
		WndForeground(wnd) = fg, WndBackground(wnd) = bg
#define WindowReverseColor(wnd, fg, bg) \
		SelectForeground(wnd) = fg, SelectBackground(wnd) = bg
#define WindowFrameColor(wnd, fg, bg) \
		FrameForeground(wnd) = fg, FrameBackground(wnd) = bg
#define WindowHighlightColor(wnd, fg, bg) \
		HighlightForeground(wnd) = fg, HighlightBackground(wnd) = bg
/* -------- text box prototypes ---------- */
#define TextLine(wnd, sel) \
      (wnd->text + *((wnd->TextPointers) + sel))
void WriteTextLine(WINDOW, RECT *, int, int);
void SetAnchor(WINDOW, int, int);
#define TextBlockMarked(wnd) (  wnd->BlkBegLine ||    \
                                wnd->BlkEndLine ||    \
                                wnd->BlkBegCol  ||    \
                                wnd->BlkEndCol)
void MarkTextBlock(WINDOW, int, int, int, int);
#define ClearTextBlock(wnd) wnd->BlkBegLine = wnd->BlkEndLine =  \
                        wnd->BlkBegCol  = wnd->BlkEndCol = 0;
#define GetText(w)        ((w)->text)
#define GetTextLines(w)   ((w)->wlines)
void ClearTextPointers(WINDOW);
void BuildTextPointers(WINDOW);
int TextLineNumber(WINDOW, char *);
/* ------------ Clipboard prototypes ------------- */
void CopyToClipboard(WINDOW);
#define PasteFromClipboard(wnd) PasteText(wnd,Clipboard,ClipboardLength)
int PasteText(WINDOW, char *, unsigned);
/* --------- menu prototypes ---------- */
int CopyCommand(unsigned char *, unsigned char *, int, int);
void PrepFileMenu(void *, struct Menu *);
void PrepEditMenu(void *, struct Menu *);
void PrepSearchMenu(void *, struct Menu *);
void PrepWindowMenu(void *, struct Menu *);
void BuildSystemMenu(WINDOW);
int isActive(MBAR *, int);
char *GetCommandText(MBAR *, int);
int isCascadedCommand(MBAR *,int);
void ActivateCommand(MBAR *,int);
void DeactivateCommand(MBAR *,int);
int GetCommandToggle(MBAR *,int);
void SetCommandToggle(MBAR *,int);
void ClearCommandToggle(MBAR *,int);
void InvertCommandToggle(MBAR *,int);
int BarSelection(int);
/* ------------- list box prototypes -------------- */
int ItemSelected(WINDOW, int);
/* ------------- edit box prototypes ----------- */
#define CurrChar (TextLine(wnd, wnd->CurrLine)+wnd->CurrCol)
#define WndCol   (wnd->CurrCol-wnd->wleft)
#define isMultiLine(wnd)     TestAttribute(wnd, MULTILINE)
void SearchText(WINDOW);
void ReplaceText(WINDOW);
void SearchNext(WINDOW);
void ResetEditBox(WINDOW);
/* --------- message box prototypes -------- */
int GenericMessage(WINDOW, char *, char *, int,
	int (*)(struct window *, enum messages, PARAM, PARAM),
	char *, char *, int, int, int);
#define TestErrorMessage(msg)	\
	GenericMessage(NULL, "Error", msg, 2, ErrorBoxProc,	  \
		Ok, Cancel, ID_OK, ID_CANCEL, TRUE)
#define ErrorMessage(msg) \
	GenericMessage(NULL, "Error", msg, 1, ErrorBoxProc,   \
		Ok, NULL, ID_OK, 0, TRUE)
#define MessageBox(ttl, msg) \
	GenericMessage(NULL, ttl, msg, 1, MessageBoxProc, \
		Ok, NULL, ID_OK, 0, TRUE)
#define YesNoBox(msg)	\
	GenericMessage(NULL, NULL, msg, 2, YesNoBoxProc,   \
		Yes, No, ID_OK, ID_CANCEL, TRUE)
#define CancelBox(wnd, msg) \
	GenericMessage(wnd, "Wait...", msg, 1, CancelBoxProc, \
		Cancel, NULL, ID_CANCEL, 0, FALSE)
void CloseCancelBox(void);
WINDOW MomentaryMessage(char *);
int MsgHeight(char *);
int MsgWidth(char *);

/* ------------- dialog box prototypes -------------- */
int DialogBox(WINDOW, DBOX *, int,
       int (*)(struct window *, enum messages, PARAM, PARAM));
int OpenFileDialogBox(char *, char *);
int SaveAsDialogBox(char *);
void GetDlgListText(WINDOW, char *, enum commands);
int DlgDirList(WINDOW, char *, enum commands,
                            enum commands, unsigned);
int RadioButtonSetting(DBOX *, enum commands);
void PushRadioButton(DBOX *, enum commands);
void PutItemText(WINDOW, enum commands, char *);
void PutComboListText(WINDOW, enum commands, char *);
void GetItemText(WINDOW, enum commands, char *, int);
char *GetDlgTextString(DBOX *, enum commands, CLASS);
void SetDlgTextString(DBOX *, enum commands, char *, CLASS);
void SetCheckBox(DBOX *, enum commands);
void ClearCheckBox(DBOX *, enum commands);
int CheckBoxSetting(DBOX *, enum commands);
CTLWINDOW *FindCommand(DBOX *, enum commands, int);
WINDOW ControlWindow(DBOX *, enum commands);
void EnableButton(DBOX *, enum commands);
void DisableButton(DBOX *, enum commands);
#define GetControl(wnd) (wnd->ct)

#define GetDlgText(db, cmd) GetDlgTextString(db, cmd, TEXT)
#define GetDlgTextBox(db, cmd) GetDlgTextString(db, cmd, TEXTBOX)
#define GetEditBoxText(db, cmd) GetDlgTextString(db, cmd, EDITBOX)
#define GetComboBoxText(db, cmd) GetDlgTextString(db, cmd, COMBOBOX)
#define SetDlgText(db, cmd, s) SetDlgTextString(db, cmd, s, TEXT)
#define SetDlgTextBox(db, cmd, s) SetDlgTextString(db, cmd, s, TEXTBOX)
#define SetEditBoxText(db, cmd, s) SetDlgTextString(db, cmd, s, EDITBOX)
#define SetComboBoxText(db, cmd, s) SetDlgTextString(db, cmd, s, COMBOBOX)

/* ---- types of vectors that can be in a picture box ------- */
enum VectTypes {VECTOR, SOLIDBAR, HEAVYBAR, CROSSBAR, LIGHTBAR};

/* ------------- picture box prototypes ------------- */
void DrawVector(WINDOW, int, int, int, int);
void DrawBox(WINDOW, int, int, int, int);
void DrawBar(WINDOW, enum VectTypes, int, int, int, int);

/* ------------- help box prototypes ------------- */
void LoadHelpFile(void);
void UnLoadHelpFile(void);
int DisplayHelp(WINDOW, char *);

extern char *ClassNames[];

#endif
