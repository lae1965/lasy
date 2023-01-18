/***************************************************************************
    utctlbrk - Set or return state of CTRL-BREAK checking.

int utctlbrk (int set, int new_state);

int     set         CBRK_SET (1) to set the state,
                    CBRK_GET (0) to return it.
int     new_state   If "set" is nonzero, this is the new CTRL-BREAK state:
                    CBRK_ON  (1) to enable checking,
                    CBRK_OFF (0) to disable it.

Description UTCTLBRK returns the state of DOS CTRL-BREAK checking
            and optionally sets that state.

            If the state is "off" (i.e., checking is disabled), then
            DOS will check for a CTRL-BREAK only when I/O is
            performed to the standard input, output, print, or
            auxiliary devices.  If the state is "on", then DOS will
            check for CTRL-BREAK whenever most DOS functions are
            called.  (DOS does not check for CTRL-BREAK during the
            operation of UTCTLBRK.)

Returns     The previous CTRL-BREAK state:
            CBRK_ON  (1) if checking is enabled,
            CBRK_OFF (0) if not.
***************************************************************************/

#include <dos.h>
#include <slutil.h>


int utctlbrk (int set, int new_state) {
    union REGS regs;
    int        old_state;

    regs.x.ax = 0x3300;             /* return CTRL-BREAK state  */
    intdos(&regs,&regs);
    old_state = regs.h.dl;

    if (set != CBRK_GET && new_state != old_state) {
        regs.h.al = 0x01;               /* set CTRL-BREAK state     */
        regs.h.dl = (unsigned char) new_state;
        intdos (&regs,&regs);
    }
    return (old_state);
}

