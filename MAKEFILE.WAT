#-------------------------------------------------------------------
#      D - F L A T   M A K E F I L E   -   W A T C O M   C
#-------------------------------------------------------------------

all : memopad.exe huffc.exe
	echo all done

#-------------------------------------------------------------------
#  Delete the FULL macro for a minimal D-Flat application. You can
#  selectively remove features by deleting #define statements at
#  the beginning of DFLAT.H
#-------------------------------------------------------------------
FULL = BUILD_FULL_DFLAT
#-------------------------------------------------------------------
#  Delete the TESTING macro to eliminate the D-Flat Log and the Reload
#  Help file selection on the Help menu
#-------------------------------------------------------------------
TESTING = TESTING_DFLAT
#-------------------------------------------------------------------
MODEL = l
#------------------------------------------------
COMPILE = wcl /c /dWATCOM /d$(FULL) /d$(TESTING) /j /c /w4 /s /m$(MODEL)
#------------------------------------------------

.c.obj:
    $(COMPILE) $*

memopad.exe : memopad.obj window.obj video.obj message.obj             &
              mouse.obj console.obj textbox.obj listbox.obj            &
              normal.obj config.obj menu.obj menubar.obj popdown.obj   &
              rect.obj applicat.obj keys.obj sysmenu.obj menus.obj     &
              editbox.obj dialogs.obj dialbox.obj button.obj           &
              fileopen.obj msgbox.obj helpbox.obj log.obj lists.obj    &
              statbar.obj decomp.obj htree.obj calendar.obj 	       &
	      barchart.obj pictbox.obj combobox.obj clipbord.obj       &
              search.obj
     wcl *.obj /k8192 /fe=memopad.exe

huffc.exe : huffc.c htree.c
     wcl /ml huffc.c htree.c
