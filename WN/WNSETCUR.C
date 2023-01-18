/*********************************************************************
    wnsetcur - Set the cursor position on the current display page.

int wnsetcur (int row,int col);

int row;        row position
int col;        column position

Description wnsetcur sets the cursor to the position specified by
            row and col on the current page.  The current page must
            be the same as the active display page (i.e., currently
            displayed) for the cursor to be seen to move.

Returns     The cursor position with the row
            in the high order byte and the column
            in the low order byte.
*********************************************************************/

#include <dos.h>
#include <slwind.h>

int wnsetcur (int row,int col) {
   union REGS inregs,outregs;

   utbound (row, 0, NUM_ROWS - 1);
   utbound (col, 0, NUM_COLS - 1);

   inregs.h.ah = 0x02;
   inregs.h.bh = (unsigned char) n_w_envr.n_a_page;
   inregs.h.dh = (unsigned char) row;
   inregs.h.dl = (unsigned char) col;
   int86 (WN_V_INT, &inregs, &outregs);
   return (int) inregs.x.dx;
}

