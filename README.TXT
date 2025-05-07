June, 1992

D-Flat Version 13

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

============== Microsoft C 7.0

To build with Microsoft C 7.0, type this command

    C>nmake /f makefile.msc

========== Borland C++ 3.0 ============

To build with the Borland C++ 3.0 make utility:

Set the DRIVE macro in the makefile to the drive where your compiler
is installed.

Type this command:

    C>make /fmakefile.bcc

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

Note that the makefile now automatically rebuilds the help database.
If you are making source code changes and updating the help database
during the same development cycle, disable the automatic building of
the help database so that you may continue to change memopad.txt from
the memopad program and reload it immediately.

===============================================

This is version 13. It includes these changes:

1. The WatchIcon function blew up if there was no mouse. Fixed.

2. The feature that formed a paragraph from a marked block has never
worked correctly. No one ever complained, so I assumed that no one
ever used it. I took it out. The paragraph command now simply reforms
a paragraph from the cursor location to the end of the current
paragraph.

3. A couple of versions back I put in logic that caused SAVESELF
windows (the APPLICATION window, POPDOWN menus, etc.) to swap their
own video images between video memory and the save buffer. This was
for performance. Hidden windows did not need to repaint if their
images were kept in the same buffer that saved what they covered. 
It got to be difficult to maintain when the window changed size or
went off-screen, and so I took the feature out in this version.

4. The help database compression program was unnecessarily compressing
comments in the help database.

5. Added support for CGA snowy displays. The Watcom compiler does not
support inline assembly language, so that version does not include
the suppression of video snow.

6. Overhauled the focus linked-list and parent-child relationships.
They were flaky at best, and there were conditions where they did not
work and could not be made to work.

7. These functions now work when the dialog box is active.
       SetCheckBox()
       ClearCheckBox()
       CheckBoxSetting()
       EnableButton()
       DisableButton()

----------------------------------------------------------------------
Calendar of D-Flat Source Code Published in the C Programming Column
 We are here: -------------------------------------------------------\/-----
             1991                            1992                     |    
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul Sep Oct
 APPLICAT.C                                                   x            
 BARCHART.C                                       x                   |    
 BOX.C                                                                x 
 BUTTON.C                                                             x    
 CALENDAR.C                                       x                   |
 CHECKBOX.C                                                           x     
 CLASSDEF.C       x                                                   |
 CLASSDEF.H       x                                                   |
 CLASSES.H                    x                                       |
 CLIPBORD.C                                       x                   |
 COMBOBOX.C                                                           x    
 COMMANDS.H                       x                                   |
 CONFIG.C         x           x                                       |
 CONFIG.H         x       x                                           |
 CONSOLE.C    x                                                       |
 DECOMP.C                                                             |   x
 DFLAT.H          x       x                                           |
 DFLATMSG.H                       x                                   |
 DIALBOX.C                                                        x   |     
 DIALBOX.H                    x                                       |
             1991                            1992                     |
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul Sep Oct
 DIALOGS.C                    x                                       |
 DIRECT.C                                                             |       x
 EDITBOX.C                                    x                       |
 FILEOPEN.C                                                           |       x
 HELPBOX.C                                                            |   x 
 HTREE.C                                                              |       x
 HTREE.H                                                              |       x
 HUFFC.C                                                              |       x
 KEYS.C                       x                                       |    
 KEYS.H       x                                                       |    
 LISTBOX.C                                            x               |    
 LISTS.C                          x                                   |    
 LOG.C                                                x               |    
 MEMOPAD.C                        x                                   |    
 MENU.C                                                   x           |    
 MENU.H                       x                                       |    
 MENUBAR.C                                                x           |    
 MENUS.C                      x                                       |    
 MESSAGE.C            x                                               |    
 MESSAGE.H            x                                               |    
             1991                            1992                     |    
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul Sep Oct
 MOUSE.C      x                                                       |    
 MSGBOX.C                                                             x    
 NORMAL.C                             x                               |    
 PICTBOX.C                                        x                   |    
 POPDOWN.C                                                x           |    
 RADIO.C                                                              x     
 RECT.C                           x                                   |    
 RECT.H       x                                                       |    
 SEARCH.C                                         x                   |    
 SLIDEBOX.C                                                           x     
 SPINBUTT.C                                                           x     
 STATBAR.C                                                            |       x 
 SYSMENU.C                                                x           |    
 SYSTEM.H     x                                                       |    
 TEXT.C                                                               x     
 TEXTBOX.C                                x                           |    
 VIDEO.C      x                                                       |    
 VIDEO.H      x                                                       |    
 WATCH.C                                                              x     
 WINDOW.C         x       x                                           |    
                                                                      |    
 We are here: -------------------------------------------------------/\-----

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
July:      The ERRORBOX, and MESSAGEBOX window classes,
           BUTTON, RADIOBUTTON, CHECKBOX, COMBOBOX, SPINBUTTON controls
*August:   Annual C issue. Discusses a Pen-based version of D-Flat
September: The HELPBOX window class
October:   Help Database compression, File Open and File Save dialog boxes.
November:  Official launch of D-Flat++

Buy back issues or Bound editions from M&T to get the complete D-Flat
narrative from May '91 to October '92. These columns describe the
development of D-Flat and serve as a tutorial on its use. The D-Flat
reference manual is DFLAT.DOC.


