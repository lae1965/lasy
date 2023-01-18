#include <slutil.h>
#include <io.h>


int    uthndcopy (int handto, int handfrom, long len) {
    char      * pbuf;
    long        savoffto, savofffrom;
    int         i = 512;
    bool        retcode = YES;

    savoffto   = utlseek (handto,   0L, UT_CUR);
    savofffrom = utlseek (handfrom, 0L, UT_CUR);
    if ((pbuf = malloc (512)) == NULL)  { retcode = NO; goto ret; }

    while (len > 0L) {
        if ((long) i > len) i = (int) len;
        i = utread (handfrom, pbuf, i);
        if (!utwrite (handto, pbuf, i)) { retcode = NO; break; }
        if (i < 512) break;
        len -= i;
    };

ret:
    free (pbuf);
    if (!retcode) {
        utlseek (handto,   savoffto,   UT_BEG);
        utlseek (handfrom, savofffrom, UT_BEG);
    }
    return retcode;
}

