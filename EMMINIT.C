/* ------------- emminit.c --------------- */

/*
 * LIM Expanded Memory Manager Driver Functions
 * This is the initialization EMM code. Its memory is
 * returned to DOS when the TSR becomes resident
 */

#include "tsr.h"

static char emmsig[9];

/* ------- test for EMM installed in system -------- */
BOOL emm_present(void)
{
	void interrupt (*emmvect)(void);
	unsigned emmseg;

	emmvect = getvect(EMM);
	emmseg = FP_SEG(emmvect);
	movedata(emmseg, 10, _DS, (unsigned) emmsig, 9);
	return (strncmp(emmsig, "EMMXXXX0", 8) == 0);
}

/* ------- test if EMM is activated -------- */
BOOL emm_working(void)
{
	_AH = TESTEMM;
	return (emmint() != -1);
}

/* ------- get the EMM page frame segment address ------ */
int emm_pageframe(void)
{
 	_AH = FRAME;
	if (emmint() == -1)
		return -1;
	return _BX;
}

/* ------- return number of EMM pages available --------- */
int emm_pages_available(void)
{
	_AH = PAGECT;
	if (emmint() == -1)
		return -1;
	return _BX;
}

/* ------- allocate pages of EMM to an application ------ */
int emm_allocate(int pagect)
{
	_AH = ALLOCATE;
	_BX = pagect;
	if (emmint() == -1)
		return -1;
	return _DX;
}

