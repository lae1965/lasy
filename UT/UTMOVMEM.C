/************************************************************************
    utmovmem - Copy memory from a far pointer to another.

void utmovmem (char far * to, char far * from, unsigned amount);

char far *  to;        A far pointer to the place the bytes are to be moved to.
char far *  from;      A far pointer to the bytes to be moved.
unsigned    amount;    The number of bytes to move.

************************************************************************/

#include <slutil.h>

void utmovmem (char far * to, char far * from, unsigned int num) {
    char far *snum;
    char far *tnum;

    from    = utnorm (from,         char);
    to      = utnorm (to,           char);
    snum    = utnorm (from + num,   char);
    tnum    = utnorm (to + num,     char);

    if (num != 0 && from != to) {
        if  ((dword) snum  <=  (dword) to     ||
                           (dword) from  >=  (dword) tnum   ||
                                ((dword) from  >   (dword) to     &&
                                    (dword) from  <=  (dword) tnum ))
            utmovabs (to, from, num, UT_MV_FORW);
        else {
            to    += num - 1;
            from  += num - 1;
            utmovabs (to, from, num, UT_MV_BACK);
        }
    }
}

