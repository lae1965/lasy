#include    "makedoc.h"
#include    <slkeyb.h>
#include    <slsymb.def>

/*                               1         2                         */
/*                     0123456789012345678901                        */
static char stat [] = "R:    C:    H:    W:  ";

void resize (void) {
    register        i;
    int             h, w, row, col;
    bool            done = NO;
    int             tmp, bord = 0, sh = 0, shd = 0;
    char far       *pbuf, *pscreen, buf [4];
    static CELLSC   fon[81];
    CELLSC         *pdata, *p, *pstatdata;
    WINDOW         *phlp, *pstat;
    static char    *help[] = {
        " \030 \031 \032 \033 Shift\030 Shift\031 Shift\032 Shift\033 ",
        " Изменение размеров ", " Enter ", "Выход", " Esc ", "Отказ",
        NULL
    };

    pstat = wncreate (1, 22, 0, 0, NULL, &Att_Khlp);
    wndsplay (pstat, WN_NATIVE, WN_NATIVE);
    pstatdata = pstat->img.pdata;

    if ((pdata = calloc (NUM_COLS * NUM_ROWS, sizeof (CELLSC))) == NULL)
        return;
    wnmovpca (fon, fonch, 80, Att_User.nice);

    zoomwin ();
    phlp = wnkeyhlp (help);
    if (pwind->bord.type & BORD_TYPE) bord = 1;
    if (pwind->bord.type & BORD_SHADOW) { shd = 1; sh = 2;}
    pbuf = (char far *) pdata;
    pscreen = wnviptrl (0, 0);
    wnvicopy (&pscreen, &pbuf, NUM_ROWS, NUM_COLS, NUM_COLS * 2, 0, Cmd[6]);
    utspeakr (0);

    row     = dcb.row;
    col     = dcb.col;
    h       = dcb.h;
    w       = dcb.w;

	while (!done) {
        wnmovps (pstatdata, stat, 22);

        i = utl2crdx ((long) row, buf, 0, 10);
        wnmovps (pstatdata + 8, buf, i);

        i = utl2crdx ((long) col, buf, 0, 10);
        wnmovps (pstatdata + 2, buf, i);

        i = utl2crdx ((long) h, buf, 0, 10);
        wnmovps (pstatdata + 14, buf, i);

        i = utl2crdx ((long) w, buf, 0, 10);
        wnmovps (pstatdata + 20, buf, i);

        pbuf = (char far *) pstatdata;
        pscreen = wnviptrl (0, 0);
        wnvicopy (&pbuf, &pscreen, 1, 22, NUM_COLS * 2, 0, Cmd[1]);
        switch (kbgetkey (NULL)) {
            case Up:
                tmp = row - bord;
                if (tmp > 2) {
                    row--;
                    h++;
                    p = &pdata[(tmp - 1) * NUM_COLS];
                    utmovmem ((char far *)p, (char far *)(p + NUM_COLS),
                                                   2 * NUM_COLS * (2 + bord));
                    pbuf = (char far *) p;
                    pscreen = wnviptrl (tmp - 1, 0);
                    wnvicopy (&pbuf, &pscreen, 2+bord, NUM_COLS, NUM_COLS * 2,
                                                                    0, Cmd[1]);
                }
                break;
            case Down:
                if (h > 2) {
                    tmp = row - bord;
                    row++;   h--;
                    p = &pdata[tmp * NUM_COLS];
                    utmovmem ((char far *)(p + NUM_COLS), (char far *)p,
                                                   2 * NUM_COLS * (2 + bord));
                    utmovmem ((char far *) p, (char far *) fon, NUM_COLS * 2);
                    pbuf = (char far *) p;
                    pscreen = wnviptrl (tmp, 0);
                    wnvicopy (&pbuf, &pscreen, 2+bord, NUM_COLS, NUM_COLS * 2,
                                                                    0, Cmd[1]);
                }
                break;
            case Left:
                tmp = col - bord;
                if (tmp > 0) {
                    col--;  w++;
                    p = &pdata[NUM_COLS * 2 + tmp - 1];
                    for (i = 2; i < NUM_ROWS - 1; i++,  p += NUM_COLS)
                        utmovabs ((char far *) p, (char far *) (p + 1),
                                                     2 * (bord + sh + 1), 0);
                    outcolomn (pdata, tmp - 1, bord + sh + 1, Cmd[1]);
                }
                break;
            case Right:
                if (w > 4) {
                    tmp = col - bord;
                    col++; w--;
                    p = &pdata[NUM_COLS * 2 + tmp];
                    for (i = 2; i < NUM_ROWS - 1; i++,  p += NUM_COLS) {
                        utmovmem ((char far *) (p + 1), (char far *) p,
                                                         2 * (bord + sh));
                        utmcopy (p, fon, CELLSC);
                    }
                    outcolomn (pdata, tmp, bord + sh + 1, Cmd[1]);
                }
                break;
            case ShiftUp:
                if (h > 2) {
                    h--;
                    tmp = row + h;
                    p = &pdata[tmp * NUM_COLS];
                    utmovmem ((char far *)p, (char far *)(p + NUM_COLS),
                                            2 * NUM_COLS * (bord + shd));
                    utmovmem ((char far *) (p + NUM_COLS * (bord + shd)),
                                              (char far *) fon, NUM_COLS * 2);
                    pbuf = (char far *) (p);
                    pscreen = wnviptrl (tmp, 0);
                    wnvicopy (&pbuf, &pscreen, 1+bord+shd, NUM_COLS,NUM_COLS*2,
                                                                    0, Cmd[1]);
                }
                break;
            case ShiftDown:
                tmp = row + h;
                if (tmp + bord + shd < NUM_ROWS - 1) {
                    h++;
                    p = &pdata[tmp * NUM_COLS];
                    utmovmem ((char far *)p, (char far *)(p - NUM_COLS),
                                            2 * NUM_COLS * (bord + shd + 1));
                    pbuf = (char far *) (p);
                    pscreen = wnviptrl (tmp, 0);
                    wnvicopy (&pbuf, &pscreen, 1+bord+shd, NUM_COLS,NUM_COLS*2,
                                                                    0, Cmd[1]);
                }
                break;
            case ShiftLeft:
                if (w > 4) {
                    tmp = col + w;             /* на бордюре */
                    w--;
                    p = &pdata[NUM_COLS * 2 + tmp];
                    for (i = 2; i < NUM_ROWS - 1; i++,  p += NUM_COLS) {
                        utmovabs ((char far *) (p - 1), (char far *) p,
                                                       2 * (bord + sh), 0);
                        utmcopy (p + bord + sh - 1, fon, CELLSC);
                    }
                    outcolomn (pdata, tmp - 1, bord + sh + 1, Cmd[1]);
                }
                break;
            case ShiftRight:
                tmp = col + w;
                if (tmp + bord + sh < NUM_COLS) {
                    w++;
                    p = &pdata[NUM_COLS * 2 + tmp - 1];
                    for (i = 2; i < NUM_ROWS - 1; i++,  p += NUM_COLS) {
                        utmovmem ((char far *) (p + 1), (char far *) p,
                                                     2 * (bord + sh + 1));
                    }
                    outcolomn (pdata, tmp, bord + sh + 1, Cmd[1]);
                }
                break;
            case GrayEnter:
            case Enter:    done = YES; break;
            case Esc:
                h   = (int) pwind->img.dim.h;
                w   = (int) pwind->img.dim.w;
                row = (int) pwind->where_shown.row;
                col = (int) pwind->where_shown.col;
                done = YES;
            default:       break;
		}
	}
    if (dcb.row != row || dcb.col != col || dcb.h != h || dcb.w != w)
        chwnsize = YES;
    dcb.row    =    row;
    dcb.col    =    col;
    dcb.h      =    h;
    dcb.w      =    w;

    pbuf = (char far *) fon;
    for (i = 2; i < NUM_ROWS - 1; i++) {
        pscreen = wnviptrl (i, 0);
        wnvicopy (&pbuf, &pscreen, 1, NUM_COLS, NUM_COLS * 2, 0, Cmd[1]);
    }
    pwind->options.shown = 0;
    wnpagrem (pwind);
    wndstroy (pwind);
    wndstroy (phlp);
    free (pdata);
    wndstroy (pstat);
	return;
}

void outcolomn (CELLSC *pdata, int col, int width, int command) {
    register     r;
    char far    *pbuf, *pscreen;
    CELLSC      *pfrom;

    pfrom = pdata + NUM_COLS * 2 + col;
    for (r = 2; r < NUM_ROWS - 1; r++,  pfrom += NUM_COLS) {
        pbuf    = (char far *) pfrom;
        pscreen = wnviptrl (r, col);
        wnvicopy (&pbuf,&pscreen, 1, width, NUM_COLS * 2, 0, command);
    }
    return;
}

