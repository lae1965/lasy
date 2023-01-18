#include <slwind.h>

static  BORDER bor = { BORD_DDSS | BORD_SHADOW, NULL, NULL, NULL };

bool wnnuminp (int row, int col, char *ttl, long *pnum, long min, long max) {
    WINDOW * pwin, *psav;
    bool     ret;
    int      wid_win, wid_num;
    char     buf[20], * p;
    long     tmp;

    if ((bor.pttitle = ttl) != NULL) bor.type |= BORD_TCT;
    wid_num = utl2crdx (max, buf, 0, 10) + 2;
    wid_win = utmax (wid_num, (int) strlen (ttl));
    utl2crdx (*pnum, buf, 0, 10);
    psav = Cur_Wind;
    pwin = wncreate (1, wid_win, row, col, &bor, &Att_Wind);
    wndsplay (pwin, row, col);

    wnwrtbuf (pwin, 0, 1, 0, buf, WN_NATIVE);
    while (1) {
        ret = wnstredt (pwin,0,1,Att_Wind.text,Att_Wind.high, buf, wid_num-2);
        if (ret == NO)  break;
        p = buf;
        if (utc2lrdx (&p, &tmp, 10) && tmp >= min && tmp <= max && *p == EOS)
            { *pnum = tmp; break; }
        utalarm ();
    }
    wndstroy (pwin);
    wnselect (psav);
    return (ret);
}

bool wndblinp (int row, int col, char *ttl, double *pnum, double min, double max) {
    WINDOW * pwin, *psav;
    bool     ret;
    int      wid_win, wid_num;
    char     buf[20], * p;
    double     tmp;

    if ((bor.pttitle = ttl) != NULL) bor.type |= BORD_TCT;
    wid_num = utl2crdx (max, buf, 0, 10) + 2;
    wid_win = utmax (wid_num, (int) strlen (ttl));
    buf [0] = EOS;
    psav = Cur_Wind;
    pwin = wncreate (1, wid_win, row, col, &bor, &Att_Wind);
    wndsplay (pwin, row, col);

    wnwrtbuf (pwin, 0, 1, 0, buf, WN_NATIVE);
    while (1) {
        ret = wnstredt (pwin,0,1,Att_Wind.text,Att_Wind.high, buf, wid_num-2);
        if (ret == NO)  break;
        p = buf;
        if (utc2dbl (&p, &tmp, YES) && tmp >= min && tmp <= max && *p == EOS)
            { *pnum = tmp; break; }
        utalarm ();
    }
    wndstroy (pwin);
    wnselect (psav);
    return (ret);
}

