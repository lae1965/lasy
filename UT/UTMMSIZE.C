/**************************************************************************
    utmmsize - Report program size.

unsigned utmmsize (void);

Description This function reports the size of the memory block
            occupied by the program's code and data.  The size is
            reported in units called paragraphs, which are sixteen
            bytes long.

Method      We inspect the memory control block that governs the
            memory block occupied by the program.

Renurns     Memory size of the program in paragraphs,
            0       if error from utmmctrl.
*************************************************************************/

#include <slutil.h>

unsigned utmmsize (void) {
    MEMCTRL  ctlblock;
    unsigned nextblock;
    int      ercode;

    ercode = utmmctrl (utpspseg, &ctlblock, &nextblock);
    if (ercode == 0 || ercode == 18)   return (ctlblock.size);
    else                               return (0);
}

