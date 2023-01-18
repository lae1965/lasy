/*
    issensin - Detect an installed interrupt service routine (ISR)

ISRCTRL far *issensin (void far *ptr, char *pident);

void far * ptr      Far pointer to potential ISR entry point.
char     * pident   Pointer to array of 16 bytes of identifying information

Description This function examines the contents of memory pointed to
            by an interrupt vector to see whether it contains a
            particular Turbo C TOOLS, Quick C TOOLS, Light Tools, or
            C TOOLS PLUS ISR control block.  If so, the function
            returns the address of the found ISR control block;
            if not, the function returns FARNIL.

            The ISR control block contains the information necessary
            to disable the ISR and to remove the program that
            contains it.

Limitation  This version does NOT trace chains of "cascaded" ISRs.
            That is, if the ISR pointed to by ptr is not the desired
            ISR but merely passes control to the desired ISR, the
            desired ISR will not be found.

Returns     Address of ISR control block, or
            FARNIL if not found.

*/

#include "language.h"
#include <slintr.h>

ISRCTRL far *issensin (void far *ptr, char *pident)
{
    unsigned int offset;
    ISRCTRL far *ptest;
    int 	 i;

    if (ptr == FARNIL)
        return (FARNIL);
    offset = utoff(ptr);
    if (offset <  ICB_ENTRY_OFFSET
        || offset >= (unsigned) 0xffff - sizeof(ISRCTRL) + 2)
            return (FARNIL);

    ptest = uttofar (utseg(ptr),offset - ICB_ENTRY_OFFSET,ISRCTRL);
    if (ptest->sign != ICB_SIGNATURE)
        return (FARNIL);
    if (ptest->sign2 != (unsigned) ~ICB_SIGNATURE)
        return (FARNIL);
    for (i=0; (i<sizeof(ptest->ident)) && (ptest->ident[i] == pident[i]); i++)
        ;
    if (i < sizeof (ptest->ident))
        return (FARNIL);
    return (ptest);
}
