/*
    iscurprc - Return or set PSP address of currently executing process.

unsigned iscurprc (int option, unsigned newproc);

int option          IS_SETPROC  to change the currently executing process
                    IS_RETPROC  to merely report it.
unsigned newproc    If option is IS_SETPROC, newproc is the PSP address
                    of the new currently executing process.

Description This function returns (and possibly changes) DOS's
            record of the program segment prefix (PSP) of the
            currently executing process.

Returns     Segment address of PSP of currently executing process
            before the call.
*/

#include "language.h"
#include <slintr.h>

unsigned iscurprc (int option, unsigned newproc)
{
    unsigned   result;
    union REGS regs;

    regs.h.ah = (unsigned char) ((utdosmajor < 3) ? 0x51 : 0x62);
    intdos(&regs,&regs);
    result = regs.x.bx;

    if (option == IS_SETPROC) {
        regs.h.ah = 0x50;
        regs.x.bx = newproc;
        intdos(&regs,&regs);
    }
    return (result);
}
