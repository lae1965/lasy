#include <slutil.h>

char *utstrexp (register char *ptarget, char *psource, int incr, int tarsize) {
    char c;
    register int len;
    int numspaces;

    len = 0;
    tarsize--;
    while (((c = *psource) != EOS) && (len < tarsize)) {
        switch (c) {
            case TAB:
                if ((incr > 0) && ((numspaces = (incr - (len % incr))) != 0)) {
                    if ((len += numspaces) < tarsize) {
                        while (numspaces--) *ptarget++ = BLANK;
                        break;
                    } else  continue;
                }
            default:
                *ptarget++ = c;
                len++;
                break;
        }
        psource++;
    }
    *ptarget = EOS;
    return (c ? psource : NULL);
}

