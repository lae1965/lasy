#include <sldbas.h>
#include <slkeyb.h>
#include <alloc.h>
#include <dos.h>

static long     oldday = 0L;
static long     oldmem = 0L;
static byte     oldmin = 0;


/*           1         */
/* 0123456789012345678 */
/* 236k 09/11/94 02:32 */

void utoutdat (void) {   /** Вызывается во время ожидания клавиши **/
    struct  time  ptime;
    CELLSC        pdata [25];
    char far    * pbuf, * pscreen;
    char          p [10];
    long          day, mem;
    byte          afld = Att_Khlp.afld;
    int           i;


    day = utdtoday ();
    gettime (&ptime);
    mem = coreleft ();

    if (Kb_redraw || day != oldday || ptime.ti_min != oldmin || mem != oldmem) {
        wnmovpa (pdata + 5, 16, Att_Khlp.nfld);

        /* Свободная память */
        i = (int) (mem / 1024L);
        if (i < 65) afld |= SL_MONOBLINK;
        utl2crdx ((long) i, p, 3, 10);
        p [3] = 'k'; p [4] = ' ';
        wnmovpsa (pdata, p, 5, afld);

        /* Дата */
        utdate2c (day, p, '/', CV_JST|CV_ZERO);
        p [8] = ' ';
        wnmovps (pdata + 5, p, 9);

        /* Время */
        utl2crdx ((long) ptime.ti_hour, p, -2, 10);
        p [2] = ':';
        utl2crdx ((long) ptime.ti_min, p + 3, -2, 10);
        wnmovps (pdata + 14, p, 5);
        pdata [16].at |= SL_MONOBLINK;

        pbuf =  (char far *) pdata;
        pscreen = wnviptrl (0, 61);
        wnvicopy (&pbuf, &pscreen, 1, 19, NUM_COLS*2, 0, Cmd[1]);

        Kb_redraw = NO; oldmin = ptime.ti_min; oldday = day; oldmem = mem;
    }
    return;
}

void  utworkin (void) {
    byte       att = Att_Khlp.afld | SL_MONOBLINK;
    char far * pbuf = "<< WORKING >> ", * pscreen;

    pscreen = wnviptrl (0, 66);
    wnvicopy (&pbuf, &pscreen, 1, 14, NUM_COLS*2, att, Cmd[2]);
    return;
}

