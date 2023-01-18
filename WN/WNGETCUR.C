/***********************************************************************
    wngetcur - Returns the cursor position and size

int wngetcur (int *prow, int *pcol, int *phigh, int *plow);

int * prow;     Pointer to row value      returned
int * pcol;     Pointer to column value   returned
int * phigh;    Pointer to high scan line returned
int * plow;     Pointer to low scan line  returned

Description wngetcur returns the current cursor location on the
            current display device and page and the current size of
            the cursor.  It also reports whether the cursor is on or
            off.

Returns     1       if cursor is on
            0       if off
************************************************************************/
#include <dos.h>
#include <slwind.h>

int wngetcur (int *prow, int *pcol, int *phigh, int *plow) {
    union REGS inregs,outregs;

    inregs.h.ah = 0x03;
	inregs.h.bh = (unsigned char) n_w_envr.n_a_page;
    int86 (WN_V_INT, &inregs, &outregs);

    *prow  = (int)outregs.h.dh;
    *pcol  = (int)outregs.h.dl;
    *phigh = (int)utlonyb(outregs.h.ch);
    *plow  = (int)utlonyb(outregs.h.cl);
    return ((outregs.h.ch & 0x60) == 0);
}

