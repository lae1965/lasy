/*
    ivdisabl - Disable an installed intervention function.

int ivdisabl (IV_CTRL far *pctrl);

IV_CTRL far *pctrl  Double word address of the intervention
                    control block to disable.

Description This function disables an installed intervention
            function by reinstalling the former values of the
            interrupt vectors that point to the intervention
            filters.

            It will fail if pctrl is not a valid pointer to an
            installed intervention control block or if the interrupt
            vectors do not currently point to the associated
            intervention filters.

Returns     0                   if successful,
            IV_NULL_PTR         if pctrl is FARNIL,
            IV_NOT_ENABLED      if not enabled,
            IV_NOT_INSTALLED    if the intervention
                                filters are not currently installed in
                                the proper interrupt vectors or if
                                pctrl is not the address of a valid
                                intervention control block.

*/

#include "language.h"
#include <slintr.h>

int ivdisabl (IV_CTRL far *pctrl)
{
    IV_VECTORS vecs;

    if (FARNIL == pctrl)
        return (IV_NULL_POINTER);
    if (!pctrl->enabled)
        return (IV_NOT_ENABLED);
    ivvecsin (IV_RETVEC,&vecs);
    if (pctrl != ivsensin (&vecs,pctrl->ident))
        return (IV_NOT_INSTALLED);

    pctrl->enabled = 0;
    ivvecsin (IV_SETVEC,&pctrl->prev_vec);
    return (IV_NO_ERROR);
}
