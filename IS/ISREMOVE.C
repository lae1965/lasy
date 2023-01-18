/*
    isremove - Remove resident program

int isremove (psp_seg);

unsigned psp_seg    Segment of Program Segment Prefix of resident program.

Description This function removes from memory a program that has
            previously terminated and stayed resident via ISRESEXT.

            WARNING:  Any interrupt vectors or chained interrupt
            service that invoke any portion of the resident program
            MUST be re-vectored or otherwise disabled BEFORE the
            program is removed.  Serious problems may result otherwise.

Method      This function works by following the chain of DOS memory
            control blocks via MMCTRL.  The control blocks are
            located at increasing memory addresses, so we begin
            searching at psp_seg, its environment segment, or the
            calling program's PSP or environment, whichever has a
            lower address.  For each control block that belongs to
            psp_seg, we free the associated memory block via DOS
            function 0x49.  Any associated memory blocks residing at
            lower addresses than our starting point will remain reserved.

Results     0       if okay
            Error code from DOS function 0x49:

*/

#include "language.h"
#include <slintr.h>

static int mm_free (unsigned);

#define  PSP_ENV_OFFSET  0x2c

int isremove (unsigned psp_seg)
{
    unsigned env_seg;
    unsigned my_env_seg;
    unsigned memblock, nextblock = 0;
    int      result;
    MEMCTRL  ctlblock;

    env_seg    = utpeekw(uttofaru(psp_seg, PSP_ENV_OFFSET));
    my_env_seg = utpeekw(uttofaru(utpspseg,PSP_ENV_OFFSET));
    result = 9;
    for (memblock  = utmin(env_seg, utmin(my_env_seg, utmin(utpspseg,psp_seg)));
         memblock != 0; memblock  = nextblock) {
        if (9 != utmmctrl (memblock, &ctlblock, &nextblock)
            && ctlblock.owner_psp == psp_seg) {
                result = mm_free (memblock);
            if (result != 0)
                break;
        }
    }
    return (result);
}

/*
    mm_free - Release an allocated memory block

int mm_free (unsigned seg);

unsigned seg    Segment address of memory block.

Description This function frees memory blocks that have been
            allocated by DOS function 0x48.  The segment address
            given must be associated with a previously allocated
            block.

Returns     DOS function return code.

*/

static int mm_free (unsigned seg)
{
    union  REGS regs;
    struct SREGS sregs;

    regs.h.ah = 0x49;
    sregs.es  = seg;
    intdosx(&regs,&regs,&sregs);
    return ((regs.x.cflag) ? (regs.h.al) : (0));
}
