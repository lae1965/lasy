#include <slutil.h>

char *utstrxlt (register char *psource, char *ptable, char *ptrans) {
    register int tindex, i;
    int      len_trans  = (int) strlen (ptrans);
    char     ch, *ptemp = psource;

    for (i = 0; ((ch = *psource) != EOS); i++) {
        tindex = utstrind (ch, ptable);
        if (tindex >= 0)
            if ((tindex + 1) > len_trans)
                *psource = BLANK;
            else
                *psource = ptrans[tindex];
        psource++;
    }
    return (ptemp);
}

