#include    <sldbas.h>
#include    <alloc.h>

static char buf1 [80], buf2 [160];

char * dbtxtini (char * docnam, char * fldnam, int doctype) {
    char  * pfilnam;
    DCB     dcb;
    IDFLD * pidf;
    NAME  * pfnam;
    long    offidf, offtxt;
    int     hand, handtmp, len = 0, i, j, txtlen;

    * buf1 = EOS;

    if (!wnstrinp (10, 12, fldnam, buf1, 75) ||
                                    (txtlen = strlen (buf1)) == 0) return NULL;

    dbsl2nam (Db_ext [doctype] + 1, 1, NULL);
    pfilnam = dbfilnam (docnam, Db_ext [doctype] + 1);
    hand = utopen (pfilnam, UT_RW);
    utread (hand, &dcb, sizeof (DCB));

    utlseek (hand, dcb.cnamlen, UT_CUR);
    pfnam = malloc (dcb.fnamlen);
    utread (hand, pfnam, dcb.fnamlen);

    offidf = (long) sizeof (DCB) + dcb.cnamlen + dcb.fnamlen + dcb.tnamlen +
                                                      dcb.dnamlen + dcb.idclen;
    utlseek (hand, offidf, UT_BEG);
    pidf = malloc (dcb.idflen);
    utread (hand, pidf, dcb.idflen);
    j = dcb.idflen / sizeof(IDFLD);
    i = 0;
    while (1) {
        if (i == j) goto ret;
        if (strcmp ((char *) pfnam [pidf [i].idn].nam, fldnam) == 0) break;
        if (pidf [i].valtype == SL_TEXT) len += pidf [i].inival.s.len;
        i++;
    }
    if (pidf [i].valtype != SL_TEXT) goto ret;

    offtxt = dcb.inioff + (len + pidf [i].inival.s.len) * 2;

    if (pidf [i].opt.wrap) {
        buf1 [txtlen++] = '\n';
        pidf [i].inival.s.blank = 1;
    }
    buf1 [txtlen] = EOS;
    pidf [i].prnopt.full = YES;
    pidf [i].inival.s.len = txtlen;

    utlseek (hand, offidf, UT_BEG);
    utwrite (hand, pidf, dcb.idflen);

    utlseek (hand, offtxt, UT_BEG);

    handtmp = utcreate ("tmp.tmp");
    while (1) {
        j = utread (hand, buf2, 160);
        if (j == 0) break;
        utwrite (handtmp, buf2, j);
    }

    utcode (buf2, buf1, txtlen);
    utlseek (hand, dcb.inioff + len * 2, UT_BEG);
    utwrite (hand, buf2, txtlen * 2);

    utlseek (handtmp, 0L, UT_BEG);
    while (1) {
        j = utread (handtmp, buf2, 160);
        if (j == 0) break;
        utwrite (hand, buf2, j);
    }

    utchsize (hand, utlseek (hand, 0L, UT_CUR));
    utclose (handtmp);
    utremove ("tmp.tmp");
ret:
    utclose (hand);
    return buf1;
}

