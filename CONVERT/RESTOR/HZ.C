#include    <sldbas.h>
#include    <slkeyb.h>
#include    <slsymb.def>
#include    <slmous.h>
#include    <alloc.h>
#include    <io.h>
#include    "colbasa.h"

SLHD    newslh;

void    main (void) {
    int handb, handh;
    long reclen, off, sizeb;

    newslh.total = 0;
    newslh.len = 0;
    off = newslh.treelen = 770;

    handh  = utcreate ("53506719.slh");
    handb = utopen   ("53506719.slb", UT_R);
    sizeb = utfsize (handb) - off;

    utwrite (handh, &newslh, sizeof (SLHD));

    while (off <sizeb) {
        utwrite (handh, &off, sizeof (long));
        utlseek (handb, off, UT_BEG);
        utread  (handb, &reclen, sizeof (long));
        newslh.total++;
        newslh.len += reclen;
        off += reclen;
    }
    utlseek (handh, 0L, UT_BEG);
    utwrite (handh, &newslh, sizeof (SLHD));

    utclose (handb);
    utclose (handh);

    return;
}

