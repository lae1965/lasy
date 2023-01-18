/************************************************************************
    utsleept - Suspend processing for some duration.

unsigned utsleept (unsigned period);

unsigned period The number of timer ticks the process is to be suspended.

Description UTSLEEPT suspends processing for at least the number of
            ticks requested, and returns the actual number as the
            functional value.  Because the function checks the
            system clock to determine if the period has transpired,
            it is possible that the actual duration is longer than
            requested.

            On standard IBM PCs, timer ticks occur 1193180/65536
            (about 18.2) times per second.

            This function temporarily enables hardware interrupts
            but restores the state of the interrupt flag before it
            returns.

Returns     The number of timer ticks the process actually slept.

****************************************************************************/

#include <slutil.h>

unsigned utsleept (unsigned period) {
    long     initclk;
    long     nowclk = 0L;
    unsigned elpticks;
    int      ints_were_on;

    ints_were_on = utinton();
    utgetclk (&initclk);

    for (elpticks=0; elpticks<period; elpticks = (unsigned)(nowclk-initclk)) {
        utgetclk(&nowclk);
        if (nowclk < initclk)
            nowclk += 0x1800b0L;
    }
    if (!ints_were_on)  utintoff ();
    return (elpticks);
}

