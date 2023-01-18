/*
    ivsensin - Detect an installed intervention function

IV_CTRL far * ivsensin (pvectors,pident);

IV_VECTORS far *pvectors    Address of structure containing
                            a set of interrupt vectors to examine.
char far *pident            Pointer to array of 16 bytes of
                            identifying information

Description This function examines the contents of memory pointed to
            by a set of interrupt vectors to see whether it contains
            a particular intervention control block.  If so, the
            function returns the address of the control block; if
            not, the function returns FARNIL.

            The contents of the control block contain the
            information required to disable the intervention
            function and to remove the program containing it.

Limitation  This function assumes that the intervention filters were
            the most recently installed handlers for their
            respective interrupts.  This function does NOT trace
            chains of "cascaded" interrupt handlers.  If the ISR
            pointed to by any of the interrupt vectors does not
            belong to the intervention function but merely passes
            control to the intervention function, the intervention
            function will not be found.

Returns     Far pointer to the intervention control block of the
            found function, or FARNIL if the function is not found.

*/

#include "language.h"
#include <slintr.h>

static IV_CTRL far *check_vec(void far *);  /* Internal (see below).  */

IV_CTRL far *ivsensin (IV_VECTORS far *pvectors, char far *pident)
{
    IV_CTRL far *ptest;
    char	 scratch[sizeof(ptest->ident)];
    char	 scratch2[sizeof(ptest->ident)];

    ptest = check_vec(pvectors->ptimer);
    if (ptest == FARNIL)
        return FARNIL;

    if (ptest != check_vec(pvectors->pkeybd)
        || ptest != check_vec(pvectors->pdisk)
        || ptest != check_vec(pvectors->pdos)
        || ptest != check_vec(pvectors->pidle)
        || ptest->sign  != (unsigned int)IV_SIGNATURE
        || ptest->sign2 != (unsigned int)~IV_SIGNATURE) {
            return FARNIL;
    }
    utpeekn(ptest->ident,scratch,sizeof(scratch));
    utpeekn(pident	,scratch2,sizeof(scratch2));
    if (memcmp(scratch,scratch2,sizeof(scratch)))
        return FARNIL;
    return ptest;
}

/*
    check_vec - Return pointer to IV_CTRL structure (if any)
                used by an intervention filter.

int check_vec (void far *ptr);

void far *ptr   Address of entry point of purported filter.

Description This function examines the contents of memory pointed to
            by an interrupt vector to see whether it might be an
            entry point to a Blaise C TOOLS intervention filter.  The
            resulting value is checked so that it can be used as a
            pointer to an IV_CTRL structure without fear that
            segment boundaries may be crossed.

            By convention, intervention filters begin with a two-byte
            jump instruction followed by a two-byte pointer to their
            intervention control block.

            This function does NOT trace chains of "cascaded"
            interrupt handlers.

Returns     Far pointer to possible IV_CTRL structure, or
            FARNIL if ptr or the data pointed to is invalid.

*/

static IV_CTRL far *check_vec (void far *ptr)
{
    unsigned int offset;
    offset = utoff (ptr);

    if (offset >= 0xfffd)
        return FARNIL;
    offset = utpeekw (uttofaru (utseg (ptr),offset+2));
    if (offset >= (unsigned) 0xffff - sizeof (IV_CTRL) + 2)
        return FARNIL;
    return uttofar (utseg (ptr),offset,IV_CTRL);
}
