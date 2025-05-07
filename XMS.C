/* ------------- xms.c --------------- */

/*
 * Extended Memory Specification (XMS) Driver Functions
 * This is the resident XMS code.
 */

#include "tsr.h"

static struct xms_copyparams	{
	long length;
	int src_handle;
	long src_offset;
	int dest_handle;
	long dest_offset;
} xcp;

void xms_free(int handle)
{
	_DX = handle;
	_AH = 0x0a;
	(*xmscall)();
}

void copy_xmstoconv(int handle, void far *memblk, long offset, long length)
{
	xcp.src_handle = handle;
	xcp.src_offset = offset;
	xcp.dest_handle = 0;
	xcp.dest_offset = (long) memblk;
	xcp.length = length;
	_SI = (unsigned) &xcp;
	_AH = 0x0b;
	(*xmscall)();
}

void copy_convtoxms(int handle, void far *memblk, long offset, long length)
{
	xcp.src_handle = 0;
	xcp.src_offset = (long) memblk;
	xcp.dest_handle = handle;
	xcp.dest_offset = offset;
	xcp.length = length;
	_SI = (unsigned) &xcp;
	_AH = 0x0b;
	(*xmscall)();
}

