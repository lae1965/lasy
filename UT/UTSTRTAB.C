#include <slutil.h>

char *utstrtab (char *psource, int incr) {
    char c;
    register int col = 0;
    register int numblanks = 0;

    char	  *pfrom = psource;
    register char *pto	 = psource;

    do {
        switch (c = *pfrom++) {
            case BLANK:
                numblanks++;
                col++;
                if ((incr <= 0) || (col % incr == 0)) {
                    *pto++    = (char) ((numblanks > 1) ? TAB : BLANK);
                    numblanks = 0;
                }
                break;

            case TAB:
                col    = numblanks = 0;
                *pto++ = TAB;
                break;

            default:
                col++;
                for (; numblanks; numblanks--)  *pto++ = BLANK;
                *pto++ = c;
                break;
        }
    } while (c);
    return (psource);
}

