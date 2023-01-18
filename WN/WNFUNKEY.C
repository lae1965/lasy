#include <slwind.h>

extern	int Kb_sym;

WINDOW * wnfunkey (void (* fn)(void), bool show) {

    WINDOW  * p, * ps;
    CELLSC  * pdata;
    register  i, pos;


    if ((p = wncreate (1, NUM_COLS, NUM_ROWS-1, 0, NULL, &Att_Khlp)) == NULL)
        return (NULL);

    pdata = p->img.pdata;

    pdata[0].at = Att_Khlp.bold;
    pdata[0].ch = '1';

    for (i = 1, pos = 7; i < 9; pos += 8, i++) {
        pdata[pos].at = Att_Khlp.bold;
        pdata[pos+1].at = Att_Khlp.bold;
        pdata[pos+1].ch = '1' + i;
    }
    pdata[pos].at = Att_Khlp.bold;
    pdata[pos+1].at = Att_Khlp.bold;
    pdata[pos+2].at = Att_Khlp.bold;
    pdata[pos+1].ch = '1';
    pdata[pos+2].ch = '0';

    if (show) {
        ps = Cur_Wind;
        if (!wndsplay (p, WN_NATIVE, WN_NATIVE)) return (NULL);
        Cur_Wind = ps;
    }
    if (fn != NULL) { Kb_sym = 0; (* fn) (); }
    return (p);

/*        10        20        30        40        50        60        70
0123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
1xxxxxx 2xxxxxx 3xxxxxx 4xxxxxx 5xxxxxx 6xxxxxx 7xxxxxx 8xxxxxx 9xxxxxx 10xxxxxx
*/
}

