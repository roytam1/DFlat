December, 1991

D-Flat Version 10

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

This is version 10. It includes these changes:

1. Fixed several problems reported by readers and found by me when I
used D-Flat for an application.

2. Began putting back in the conditional compile-time statements that
build different D-Flat configurations based on the features an
application will use. See DFLAT.H for the INCLUDE_ symbols. There
will be more.

3. You cannot miminize the APPLICATIONS window now. It was causing
problems with the menu and status bars.

4. Moved ID_LOGGING, ID_INSERT, and ID_WRAP processing out of
applicat.c and into the log.c and memopad.c modules.

5. Paste did not check for MaxTextLength. It does now.

6. Calendar works OK with BC++ 3.0. BC++ 2.0 had a bug with their
mktime function.

7. A TEXT control now assumes the colors of its parent window.

8. The PutWindowLine function works OK now when other windows overlap
the target window.

9. TEXTBOX-derived controls automatically gain and lose scrollbars
when the height and width of text exceeds those of the control
windows. The windows adjust even after a SIZE message changes the
size of the window.

10. BC++ 2.0 had a problem with precompiled headers, and memopad.c
and huffc.c both had code to work around that problem. The problem is
gone with BC++ 3.0, and the work-around code is gone from D-Flat
v10.

11. Cascaded menu selections may now have accelerator keys.

12. ALT_BS and CTRL_INS now work correctly in older machines. I don't
know whether the problem is in the 8088 family or the old-style
keyboards, but you can't just do inportb(0x60) to read the keyboard
port from outside of an interrupt service routine. The port has valid
data only when read following an interrupt. The message.c module now
reads all keyboard input from within a hooking keyboard ISR.

13. There were some problems with search and replace and with forming
paragraphs from marked blocks. These problems are fixed now.

14. The left/top coordinates that you specify for modeless dialog
boxes are now relative to the dialog box's parent window rather than
the screen, unless the dialog box has no parent.

----------------------------------------------------------------------
Calendar of D-Flat Source Code Published in the C Programming Column
 We are here: ---------------------------\/---------------------------------
             1991                         |  1992                          
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul Aug
 APPLICAT.C                               |                   x            
 BARCHART.C                               |       x                        
 BUTTON.C                                 |                           x    
 CALENDAR.C                               |       x                        
 CLASSDEF.C       x                       |                                
 CLASSDEF.H       x                       |                                
 CLASSES.H                    x           |                                
 CLIPBORD.C                               |       x                        
 COMBOBOX.C                               |                           x    
 COMMANDS.H                       x       |                                
 CONFIG.C         x           x           |                                
 CONFIG.H         x       x               |                                
 CONSOLE.C    x                           |                                
 DFLAT.H          x       x               |                                
 DFLATMSG.H                       x       |                                
 DIALBOX.C                                |                       x        
 DIALBOX.H                    x           |                                
 DIALOGS.C                    x           |                                
 EDITBOX.C                                |   x                            
             1991                         |  1992                          
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul Aug
 FILEOPEN.C                               |                           x    
 HELPBOX.C                                |                               x 
 KEYS.C                       x           |                                
 KEYS.H       x                           |                                
 LISTBOX.C                                |           x                    
 LISTS.C                          x       |                                
 LOG.C                                    |           x                    
 MEMOPAD.C                        x       |                                
 MENU.C                                   |               x                
 MENU.H                       x           |                                
 MENUBAR.C                                |               x                
 MENUS.C                      x           |                                
 MESSAGE.C            x                   |                                
 MESSAGE.H            x                   |                                
 MOUSE.C      x                           |                                
 MSGBOX.C                                 |                       x        
 NORMAL.C                             x   |                                
 PICTBOX.C                                |       x                        
 POPDOWN.C                                |               x                
             1991                         |  1992
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul Aug
 RECT.C                           x       |                                
 RECT.H       x                           |                                
 SEARCH.C                                 |       x                        
 STATBAR.C                                |                               x 
 SYSMENU.C                                |               x                
 SYSTEM.H     x                           |                                
 TEXTBOX.C                                x                                
 VIDEO.C      x                           |                                
 VIDEO.H      x                           |                                
 WINDOW.C         x       x               |                                
                                          |                                
 We are here: ---------------------------/\---------------------------------

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
June:      The DIALBOX window class
July:      BUTTON, RADIOBUTTON, CHECKBOX, COMBOBOX, SPINBUTTON, 
           MESSAGEBOX, ERRORBOX, File Open and File Save dialog boxes.
August:    The HELPBOX window class


