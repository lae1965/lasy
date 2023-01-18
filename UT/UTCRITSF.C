/***************************************************************************
    utcritsf - Obtain address of DOS critical section flag.

void utcritsf (void);

Description This function loads the global variable Adr_Crit
            with the address of the DOS critical section flag.
            The DOS critical section flag is a one-byte value
            indicating whether any DOS functions are in progress and
            therefore whether DOS services are available to an
            interrupting process.  The byte has a zero value when
            DOS services are available.

Returns     Nothing.

****************************************************************************/

#include <dos.h>
#include <slutil.h>

unsigned char far *Adr_Crit = FARNIL;

unsigned char far *utcritsf (void) {
    union  REGS  regs;
    struct SREGS sregs;

    regs.x.ax = 0x3400;     /* DOS function number.		    */
    int86x (UTDOSINT, &regs, &regs, &sregs);
    Adr_Crit = uttofar (sregs.es, regs.x.bx, unsigned char);
    return (Adr_Crit);
}

