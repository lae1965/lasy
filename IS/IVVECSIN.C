/*
    ivvecsin - Set or return the interrupt vectors

int ivvecsin (int option, IV_VECTORS far *pvectors);

int option                  IV_SETVEC to set the vectors,
                            IV_RETVEC to return them.
IV_VECTORS far *pvectors    Double word address of structure
                            containing a set of interrupt vectors.

Description This function sets or returns the current contents of
            the vectors used by the intervention code filters (see
            ivctrlbl.asm).  If IV_RETVEC is specified, the vectors are
            returned in the structure pointed to by pvectors; if
            IV_SETVEC is specified, the vectors are set using the
            values in the structure pointed to by pvectors.

Returns     0           if successful
            IV_BAD_OPT  if option is unknown.

*/

#include "language.h"
#include <slintr.h>

int ivvecsin (int option, IV_VECTORS far *pvectors)
{
    int result;

    switch (option) {
        case IV_RETVEC:
            pvectors->ptimer = isgetvec(0x08);
            pvectors->pkeybd = isgetvec(0x09);
            pvectors->pdisk  = isgetvec(0x13);
            pvectors->pdos   = isgetvec(0x21);
            pvectors->pidle  = isgetvec(0x28);
            result = IV_NO_ERROR;
            break;

        case IV_SETVEC:
            isputvec(0x13,pvectors->pdisk);
            isputvec(0x28,pvectors->pidle);
            isputvec(0x08,pvectors->ptimer);
            isputvec(0x21,pvectors->pdos);
            isputvec(0x09,pvectors->pkeybd);
            result = IV_NO_ERROR;
            break;

        default:
            result = IV_BAD_OPT;
            break;
    }
    return result;
}
