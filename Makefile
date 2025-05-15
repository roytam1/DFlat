# Watcom Makefile for FreeDOS EDIT
.erase

CFLAGS=-w4 -e25 -s -zq -otexan -ol -oa -oe20 -d2 -fp3 -fpi87 -5r -bt=dos -mf

!ifdef __UNIX__
HOST_CL=cc
HOST_CL_FLAGS=-DNOGLOBALARGV
REL_PATH=./
!else
HOST_CL=owcc
REL_PATH=
!endif
!ifdef __MSDOS__
HOST_CL_FLAGS=-mcmodel=l
!endif

objs = &
	APPLICAT.obj &
	BARCHART.obj &
	BOX.obj &
	BUTTON.obj &
	CALENDAR.obj &
	CHECKBOX.obj &
	CLIPBORD.obj &
	COMBOBOX.obj &
	CONFIG.obj &
	CONSOLE.obj &
	DECOMP.obj &
	DFALLOC.obj &
	DIALBOX.obj &
	DIALOGS.obj &
	DIRECT.obj &
	EDITBOX.obj &
	EDITOR.obj &
	FILEOPEN.obj &
	HELPBOX.obj &
	HTREE.obj &
	KEYS.obj &
	LISTBOX.obj &
	LISTS.obj &
	LOG.obj &
	memopad.obj &
	MENU.obj &
	MENUBAR.obj &
	menus.obj &
	MESSAGE.obj &
	MOUSE.obj &
	MSGBOX.obj &
	NORMAL.obj &
	PICTBOX.obj &
	POPDOWN.obj &
	RADIO.obj &
	RECT.obj &
	SEARCH.obj &
	SLIDEBOX.obj &
	SPINBUTT.obj &
	STATBAR.obj &
	SYSMENU.obj &
	TEST.obj &
	TEXT.obj &
	TEXTBOX.obj &
	VALUEBOX.obj &
	VIDEO.obj &
	WATCH.obj &
	WINDOW.obj


all: memopad.exe memopad.hlp

memopad.exe: $(objs) memopad.lnk Makefile
	wlink @memopad.lnk

memopad.hlp: huffc.exe fixhelp.exe
	$(REL_PATH)huffc.exe memopad.txt memopad.hlp
	$(REL_PATH)fixhelp.exe memopad

huffc.exe:
	$(HOST_CL) $(HOST_CL_FLAGS) huffc.c htree.c -o huffc.exe

fixhelp.exe:
	$(HOST_CL) $(HOST_CL_FLAGS) fixhelp.c decomp.c -o fixhelp.exe

.c.obj: .autodepend
	wcc386 $(CFLAGS) -Fo=$*.obj $*.c

memopad.lnk : Makefile .always
	@echo NAME $^& >$^@
	@echo SYSTEM CauseWay >>$^@
	@echo OPTION QUIET >>$^@
	@echo OPTION MAP >>$^@
	@echo OPTION STACK=8192 >>$^@
!ifdef DEBUG
	@echo DEBUG ALL >>$^@
!endif
	@for %i in ($(objs)) do @echo FILE %i >>$^@

clean: .symbolic
	rm -f *.obj
	rm -f *.exe
	rm -f *.OBJ
	rm -f *.EXE
	rm -f *.err
	rm -f *.ERR
