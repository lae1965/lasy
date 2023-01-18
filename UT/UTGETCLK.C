/***************************************************************************
    utgetclk - Read BIOS clock tick count.

int utgetclk (long *pcount);

long * pcount;      The count of timer ticks since midnight.

Description This function returns the BIOS's four-byte count of
            clock ticks since midnight.  On standard IBM PCs, timer
            ticks occur 1193180/65536 (about 18.2) times per second.

            If the value of the function is 1, then the midnight
            rollover has occurred but has not yet been cleared.
            This function does NOT clear the rollover flag, thus
            allowing another program (presumably DOS) to respond to
            the rollover and to clear it.

Returns     1   if midnight rollover occurred on the most recent clock tick;
            0   if not.
****************************************************************************/

#include <slutil.h>

int utgetclk (long *pcount) {
    int             were_on;
    unsigned char   rollover;

    were_on = utintoff();
    utpeekn ((char far *) UTCLOCKADDR, (void *) pcount, sizeof (*pcount));
    rollover = utpeekb (UTROLLADDR);
    if (were_on) utinton();
    return (rollover != 0);
}

