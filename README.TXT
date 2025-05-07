March, 1994

D-Flat Version 18

The source files in the DFLAT archive constitute the D-Flat windowing
system. This is public domain code. You may use it in your
applications without restriction. You may freely distribute source
code. It would be nice if you would give credit to Dr. Dobb's Journal
as the original publisher of the source code.

The software build procedure makes a program named memopad.exe. It is
a multiple-document notepad program. Observe the #define VERSION
statement in dflat.h. The version number should correspond with the
nn in the DFLTnn.ARC filename that you downloaded. Check the uploads
periodically to see if there is a more recent version available.

My CompuServe ID is 71101,1262. I monitor the DDJFORUM every day and
prefer that you discuss D-Flat there so that every one can benefit
from your comments.

============== Microsoft C 7.0

To build with Microsoft C 7.0, type this command

    C>nmake /f makefile.msc

========== Borland C++ 3.0 ============

To build with the Borland C++ 3.1 or 4.0 make utility:

Set the DRIVE macro in the makefile to the drive and directory 
where your compiler is installed.

Type this command:

    C>make /fmakefile.bcc

========== WATCOM C 8.0 ============

To build with Watcom C 8.0, type this command

    C>wmake /f makefile.wat

=======================================================

D-Flat uses Compressed help files. It uses an adaptation of the
Huffman compression programs from the Dr. Dobb's Journal C
Programming Column of early 1991. 

The makefile compresses the help file with these commands:

   C>huffc memopad.txt memopad.hlp
   C>fixhelp memopad.hlp

Both of the programs must be run in order for the help system to work
properly. Note that .hlp files from previous versions are not
compatible with version. The .txt files are, however, so you can use
the huffc and fixhelp programs to rebuild the .hlp file.

===============================================

This is version 18. It fixes these problems:

1. Changing screen modes did not work with all systems.

2. Selecting text and scrolling with the keyboard sent the cursor off
of the screen.

3. The PAINT message did not repaint (clear) the screen after a
CLEARTEXT message.

4. Overhauled the File Open and Save As dialog boxes. Note that these
DBs now return the file name only and have changed the drive and 
subdirectory to what the user chooses. The application window saves
and restores the drive and subdirectory on open and close, but your
application needs to know that the log-on could be different after
using the dialog boxes.

5. Added an EDITOR class that manages tabs on input and output. This
class does not do paragraph reforming.

6. Added the wd parameter to the InputBox function.

7. Clicking a box frame in a dialog box disabled the radio buttons,
checkboxes, etc. inside the box.

8. Dialog box unnecessarily repainted when changing focus among controls.

9. Padding blank lines when painting a textbox used a line that was one
character too short.

10. Help windows would be overwritten by the borders of the current
in-focus window.

11. Large help files took a long time to load, particularly on slower
processors. This was because the load process read the entire
compressed database to build an internal list of help windows. I added
a program (fixhelp.c) that puts the list at the end of the database.
The load process now seeks to the end and reads the list instead of
computing it. I removed the feature that let D-Flat applications load
the uncompressed text version of the help file.

12. Added a [*] comment token to the help database. The memopad
doesn't use it, but Quincy does. A program can call the HelpComment
function to retrieve a pointer to a comment associated with the
specified help window. The Quincy tutorial uses it to record script
commands associated with specific tutorial help windows.

13. The Search and Replace code did not work properly if you tried to
replace, for example, "void" with "(void)" and chose to change every
occurrence. The program went into a loop replacing first "void" with
"(void)", then "(void)" with "((void))", and so on.

14. Removed much of the heap usage in the helpbox window.
Applications would sometimes crash unexpectedly upon exit after you
used a lot of help window navigation. These changes seem to have
corrected the problem and made the code simpler, too.

----------------------------------------------------------------------
Calendar of D-Flat Source Code Published in the C Programming Column

             1991                            1992                              
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul Aug Sep Oct
 APPLICAT.C                                                   x            
 BARCHART.C                                       x                            
 BOX.C                                                                x    
 BUTTON.C                                                             x     
 CALENDAR.C                                       x                        
 CHECKBOX.C                                                           x      
 CLASSDEF.C       x                                                        
 CLASSDEF.H       x                                                        
 CLASSES.H                    x                                            
 CLIPBORD.C                                       x                        
 COMBOBOX.C                                                           x    
 COMMANDS.H                       x                                        
 CONFIG.C         x           x                                            
 CONFIG.H         x       x                                                
 CONSOLE.C    x                                                            
 DECOMP.C                                                                     x
 DFLAT.H          x       x                                                    
 DFLATMSG.H                       x                                               
 DIALBOX.C                                                        x                
 DIALBOX.H                    x                                                    
             1991                            1992                                  
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul Aug Sep Oct
 DIALOGS.C                    x                                                   
 DIRECT.C                                                                         x
 EDITBOX.C                                    x                                    
 FILEOPEN.C                                                                       x
 HELPBOX.C                                                                    x 
 HTREE.C                                                                          x
 HTREE.H                                                                          x
 HUFFC.C                                                                          x
 KEYS.C                       x                                            
 KEYS.H       x                                                            
 LISTBOX.C                                            x                    
 LISTS.C                          x                                        
 LOG.C                                                x                    
 MEMOPAD.C                        x                                        
 MENU.C                                                   x                
 MENU.H                       x                                            
 MENUBAR.C                                                x                
 MENUS.C                      x                                            
 MESSAGE.C            x                                                    
 MESSAGE.H            x                                                    
             1991                            1992                          
             May Jun Jul Aug Sep Oct Nov Dec Jan Feb Mar Apr May Jun Jul Aug Sep Oct
 MOUSE.C      x                                                            
 MSGBOX.C                                                             x     
 NORMAL.C                             x                                    
 PICTBOX.C                                        x                        
 POPDOWN.C                                                x                
 RADIO.C                                                              x    
 RECT.C                           x                                        
 RECT.H       x                                                            
 SEARCH.C                                         x                        
 SLIDEBOX.C                                                           x    
 SPINBUTT.C                                                           x      
 STATBAR.C                                                                        x 
 SYSMENU.C                                                x                
 SYSTEM.H     x                                                            
 TEXT.C                                                               x      
 TEXTBOX.C                                x                                
 VIDEO.C      x                                                            
 VIDEO.H      x                                                            
 WATCH.C                                                              x      
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
February:  The PICTUREBOX window class, the Clipboard, text searching
March:     The LISTBOX window class and message logging
April:     The MENUBAR and POPDOWN window classes, the system menu
May:       The APPLICATION window class
June:      The DIALBOX window class 
July:      The ERRORBOX, and MESSAGEBOX window classes,
           BUTTON, RADIOBUTTON, CHECKBOX, COMBOBOX, SPINBUTTON controls
August:    Annual C issue. Discusses a Pen-based version of D-Flat
September: The HELPBOX window class
October:   Help Database compression, File Open and File Save dialog boxes.
November:  Official launch of D-Flat++

Buy back issues or the CD-ROM from Miller-Freeman to get the complete D-Flat
narrative from May '91 to October '92. These columns describe the
development of D-Flat and serve as a tutorial on its use. The D-Flat
reference manual is DFLAT.DOC.



