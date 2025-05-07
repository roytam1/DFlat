/* ------------- emm.c --------------- */

/*
 * LIM Expanded Memory Manager Driver Functions
 * This is the resident EMM code.
 */

#include "tsr.h"

/* ---- map EMM pages to addressable pages in memory ----- */
int emm_map(int pagect, int first_pageno,
			int first_logical_page, int handle)
{
	while (pagect--)	{
		_AL = first_pageno++;
		_AH = MAPEMM;
		_BX = first_logical_page++;
		_DX = handle;
		if (emmint() == -1)
			return -1;
	}
	return 0;
}

/* ----- release the pages allocated to a handle ------ */
int emm_release(int handle)
{
	_AH = RELEASEEMM;
	_DX = handle;
 	return emmint();
}

/* ---- save the current mapping context ----- */
int emm_savecontext(int handle)
{
	_AH = SAVE;
	_DX = handle;
 	return emmint();
}

/* ----- restore a saved mapping context ------- */
int emm_restorecontext(int handle)
{
	_AH = RESTOREEMM;
	_DX = handle;
 	return emmint();
}

/* ------ general EMM interrupt caller --------- */
int emmint(void)
{
	geninterrupt(EMM);
	if (_AH != 0)
		return -1;
	return 0;
}

