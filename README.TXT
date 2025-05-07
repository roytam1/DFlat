October, 1991

D-Flat Version 8

The source files in the DFLAT archive constitute the D-Flat windowing
system. This is public domain code. You may use it in your
applications without restriction. You may freely distribute source
code. It would be nice if you would give credit to Dr. Dobb's Journal
as the original publisher of the source code.

The software build procedure makes a program named memopad.exe. It is
a multiple-document notepad program. Observe the #define VERSION
statement in dflat.h. The version number should correspond with the n
in the DFLATn.ARC and DFnTXT.ARC filenames that you downloaded. Check
the uploads periodically to see if there is a more recent version
available.

My CompuServe ID is 71101,1262. I monitor the DDJFORUM every day and
prefer that you discuss D-Flat there so that every one can benefit
from your comments.

======== Turbo C 2.0 =========

To build with the Turbo C 2.0 make utility, change the
three lines in the makefile so that they look like this:

    MSC = 0
    BORLAND_CPP = 0
    TURBOC = 1

Set the DRIVE macro in the makefile to the drive where your compiler
is installed.

Type this command:

    C>make

============== Microsoft C 6.0

To build with the Microsoft C nmake utility, make the first three
lines of the makefile look like this:

    MSC = 1
    BORLAND_CPP = 0
    TURBOC = 0

Set the DRIVE macro in the makefile to the drive where your compiler
is installed.

Type this command:

    C>nmake

========== Borland C++ 2.0 ============

To build with the Borland C++ 2.0 make utility, make the first three
lines of the makefile look like this:

    MSC = 0
    BORLAND_CPP = 1
    TURBOC = 0

Set the DRIVE macro in the makefile to the drive where your compiler
is installed.

Type this command:

    C>make

NOTE: I have seen some odd compiled code when you compile with
Borland C++. In one instance, argv and argc were 0000:0000 and 0 upon
entry to main.

========== WATCOM C 8.0 ============

To build with Watcom C 8.0, type this command

    C>wmake /f makefile.wat

=======================================================

D-Flat uses Compressed help files. It uses an adaptation of the
Huffman compression programs from the Dr. Dobb's Journal C
Programming Column of early 1991. If the program finds the compressed
MEMOPAD.HLP, it loads it. Otherwise, it looks for MEMOPAD.TXT, the
ASCII version of the file, and loads that file instead. When you
execute the Reload Help command on the Help menu, the program loads
the same way. To test changes to the help file that you make from the
MEMOPAD program, delete the MEMOPAD.HLP file so that the reload
command loads the MEMOPAD.TXT file. You can compress MEMOPAD.TXT into
MEMOPAD.HLP later. The program starts faster without compressed
help.

To compress the help file type this on the command line:

   C>huffc memopad.txt memopad.hlp

===============================================

This is version 8. It includes these improvements:

1. Many of the big switch statements in window-processing modules are
chopped into function calls from the cases. This permits some
compilers to do a better job with optimizing.

2. There is a new window class, the PICTUREBOX, derived from the
TEXTBOX. You can draw horizontal and vertical lines and bar-charts.
See the files calendar.c and barchart.c for examples. The new
messages are DRAWVECTOR, DRAWBOX, and DRAWBAR. The new functions are
DrawVector, DrawBox, and DrawBar. DLFLAT.DOC describes these
functions and messages.

3. More advancements in modeless dialog boxes.

4. The window clipping logic is now all done in video.c.

5. I removed all the #ifdef INCLUDE_xxxx tests. They were becoming
impossible to manage. My plan is to reinstate the conditional compile
feature after the library is complete.

6. Removed Clipboard and Text Search code from the main EDITBOX class
module. That code is now in CLIPBORD.C and SEARCH.C. Their commands are
now intercepted and processed by MEMOPAD.C rather than EDITBOX.C

----------------------------------------------------------------------
Calendar of D-Flat Source Code Published in the C Programming Column

             1991                            1992
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul
 APPLICAT.C                                               x
 BUTTON.C                                                         x
 CLASSDEF.C       x
 CLASSDEF.H       x
 CLASSES.H                    x
 CLIPBORD.C  (unscheduled)
 COMBOBOX.C                                                       x
 COMMANDS.H                       x
 CONFIG.C         x           x
 CONFIG.H         x       x
 CONSOLE.C    x               
 DFLAT.H          x       x
 DFLATMSG.H                       x
 DIALBOX.C                                                    x
 DIALBOX.H                    x
 DIALOGS.C                    x
 EDITBOX.C                                    x
 FILEOPEN.C                                                       x
 HELPBOX.C                                                            x
             1991                            1992
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul
 KEYS.C                       x
 KEYS.H       x
 LISTBOX.C                                        x
 LISTS.C                          x
 LOG.C                                                                x
 MEMOPAD.C                        x
 MENU.C                                               x
 MENU.H                       x
 MENUBAR.C                                            x
 MENUS.C                      x
 MESSAGE.C            x
 MESSAGE.H            x      
 MOUSE.C      x
 MSGBOX.C                                                         x
 NORMAL.C                             x
 POPDOWN.C                                            x
 RECT.C                           x
 RECT.H       x
 SEARCH.C    (unscheduled)
             1991                            1992
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul
 STATBAR.C                                                          x
 SYSMENU.C                                            x
 SYSTEM.H    x
 TEXTBOX.C                                x
 VIDEO.C     x
 VIDEO.H     x
 WINDOW.C         x       x

---------------------------------------------------------------------
Calendar of D-Flat Topics in the C Programming Column

1991
May:       Low-level keyboard, mouse, and screen functions
June:      Classes, configuration
July:      Event collection, message passing
August:    Window driver
September: Defining dialog boxes, menus, keys
October:   Commands, messages, rectangles, example application
November:  The NORMAL window class
December:  The TEXTBOX window class

1992
January:   The EDITBOX window class
February:  The LISTBOX window class
March:     The MENUBAR and POPDOWN window classes, the system menu
April:     The APPLICATION window class
May:       The DIALBOX window class
June:      BUTTON, RADIOBUTTON, CHECKBOX, COMBOBOX, SPINBUTTON, 
           MESSAGEBOX, ERRORBOX, File Open and File Save dialog boxes.
July:      The HELPBOX window class and message logging


