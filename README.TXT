March, 1992

D-Flat Version 11

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

To build with the Turbo C 2.0 make utility:

Set the DRIVE macro in makefile.tc2 to the drive where your compiler
is installed.

Type this command:

    C>make -fmakefile.tc2

============== Microsoft C 6.0

To build with the Microsoft C nmake utility, make the first
lines of the makefile look like this:

    MSC = 1
    BORLAND_CPP = 0

Set the DRIVE macro in the makefile to the drive where your compiler
is installed.

Type this command:

    C>nmake

========== Borland C++ 2.0 ============

To build with the Borland C++ 2.0 make utility, make the first 
lines of the makefile look like this:

    MSC = 0
    BORLAND_CPP = 1

Set the DRIVE macro in the makefile to the drive where your compiler
is installed.

Type this command:

    C>make

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

This is version 11. It includes these changes:

1. The WATCOM version needed to flush stdout before shelling out to
DOS.

2. Added the InputBox function to implement a generic input box
dialog box.

3. You may no longer use the CONTROL macro to initialize the contents
of an EDITBOX or COMBOBOX control. The macro prevents it. It should
have all along. The itext variable in the dialog box for these
controls is allocated from the heap when the user enters text. The
pointer is retained for subsequent uses of the dialog box and then
deallocated when the program processes the STOP message.

4. Added the ClearClipboard function.

5. D-Flat now watches for calls to malloc, calloc, and realloc that
fail. When any of these calls returns NULL, Dflat closes all windows,
displays a small error window that says "Out of Memory!", and returns
FALSE to the calling application's call to init_messages. This is a
longjmp process, so you might be well past that call when it returns.
See memopad.c for an example. The interception redefines the stdlib
function calls as macros that call the corresponding functions in
dfalloc.c.

6. Added the BOOL typedef to make Boolean variables and functions
easier to recognize.

7. Added DELETETEXT and INSERTTEXT messages to delete and insert
lines of text in the textbox.

8. Added SetDlgTitle function. See DFLAT.DOC.

9. Added CopyTextToClipboard function. See DFLAT.DOC.

10. Added the WatchIcon and handshake functions. See DFLAT.DOC.

11. Added the SliderBox function. See DFLAT.DOC and the example in
the PrintPad function of MEMOPAD.C

----------------------------------------------------------------------
Calendar of D-Flat Source Code Published in the C Programming Column
 We are here: ---------------------------------------\/---------------------
             1991                            1992                          
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul Aug
 APPLICAT.C                                           |       x            
 BARCHART.C                                       x   |                    
 BUTTON.C                                             |               x    
 CALENDAR.C                                       x   |                    
 CLASSDEF.C       x                                   |                    
 CLASSDEF.H       x                                   |                    
 CLASSES.H                    x                       |                    
 CLIPBORD.C                                       x   |                    
 COMBOBOX.C                                           |               x    
 COMMANDS.H                       x                   |                    
 CONFIG.C         x           x                       |                    
 CONFIG.H         x       x                           |                    
 CONSOLE.C    x                                       |                    
 DFLAT.H          x       x                           |                    
 DFLATMSG.H                       x                   |                    
 DIALBOX.C                                            |           x        
 DIALBOX.H                    x                       |                    
 DIALOGS.C                    x                       |                    
 EDITBOX.C                                    x       |                    
             1991                            1992     |                    
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul Aug
 FILEOPEN.C                                           |               x    
 HELPBOX.C                                            |                   x 
 KEYS.C                       x                       |                    
 KEYS.H       x                                       |                    
 LISTBOX.C                                            x                    
 LISTS.C                          x                   |                    
 LOG.C                                                x                    
 MEMOPAD.C                        x                   |                    
 MENU.C                                               |   x                
 MENU.H                       x                       |                    
 MENUBAR.C                                            |   x                
 MENUS.C                      x                       |                    
 MESSAGE.C            x                               |                    
 MESSAGE.H            x                               |                    
 MOUSE.C      x                                       |                    
 MSGBOX.C                                             |           x        
 NORMAL.C                             x               |                    
 PICTBOX.C                                        x   |                    
 POPDOWN.C                                            |   x                
             1991                            1992     |                    
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul Aug
 RECT.C                           x                   |                    
 RECT.H       x                                       |                    
 SEARCH.C                                         x   |                    
 STATBAR.C                                            |                   x 
 SYSMENU.C                                            |   x                
 SYSTEM.H     x                                       |                    
 TEXTBOX.C                                x           |                    
 VIDEO.C      x                                       |                    
 VIDEO.H      x                                       |                    
 WINDOW.C         x       x                           |                    
                                                      |                    
 We are here: ---------------------------------------/\---------------------

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
February:  The PICTUREBOX window class, the Clipboard, text searching
March:     The LISTBOX window class and message logging
April:     The MENUBAR and POPDOWN window classes, the system menu
May:       The APPLICATION window class
June:      The DIALBOX, ERRORBOX, and MESSAGEBOX window classes
July:      BUTTON, RADIOBUTTON, CHECKBOX, COMBOBOX, SPINBUTTON controls,
           File Open and File Save dialog boxes.
August:    The HELPBOX window class


