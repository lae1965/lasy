/**************************************************************************
    utmmctrl - Retrieve a DOS memory control block

int utmmctrl (unsigned memblk, MEMCTRL *pctrlblk, unsigned *pnextblk);

unsigned    memblk      Segment address of DOS memory block
                        as allocated by DOS function 0x48.
MEMCTRL  *  pctrlblk    Address of structure in which to return copy
                        of memory control block associated with memblk.
unsigned *  pnextblk    Segment of next memory block, or 0 if memblk
                        is invalid or if there is no next block.

Description UTMMCTRL fetches a copy of the control block associated
            with a DOS memory block and returns the segment address
            of the next memory block.

            The control block is a 16-byte data structure located
            exactly 16 bytes (one paragraph) before the memory block
            (i.e., at segment (memblk-1), offset 0).  Its leading
            byte should be 'M' or 'Z'; a 'Z' indicates the final
            block in the chain.

            UTMMCTRL fails if memblk is not a currently-allocated DOS
            memory block.  In that case 9 is returned as the value
            of the function.

Returns     0   if memblk is a legitimate DOS memory block, but not the last;
            9   if memblk is not a legitimate DOS memory block;
            18  if memblk is the last memory block.
***************************************************************************/

#include <slutil.h>


int utmmctrl (unsigned memblk, MEMCTRL  *pctrlblk, unsigned *pnextblk) {
    register int result;

    utpeekn (uttofaru (memblk-1,0), (char far *) &pctrlblk->ident, 16);
    switch (pctrlblk->ident) {
        case 'M':
            result    = 0;
            *pnextblk = memblk + pctrlblk->size + 1;
            break;
        case 'Z':
            result    = 18;
            *pnextblk = 0;
            break;
        default:
            result    = 9;
            *pnextblk = 0;
            break;
    }
    return (result);
}

