#include    "makedoc.h"
#include    <slkeyb.h>
#include    <slsymb.def>

static byte         ATT_SH;
static byte         ATT_FN;

static  char far  * pscreen, * pbuf;
static  int         bord, shd;

static void chbord   (int type);
static void outshdw  (int att);
static void clearbox (void);

void bordtype (void) {
    static char    *help[] = {
        "\030 \031 \032 \033 ", "Тип бордюра",
        " Ins ", "Уд./вст.бордюра",
        " Gray+ ", "Уд./вст.тени",
        " Enter ", "Выход", " Esc ", "Отказ",
        NULL
    };
    int             type = pwind->bord.type;
    int             sh = 0, rt = 0, dn = 0, up = 0, lt = 0;
    WINDOW         *phlp;
    bool            done = NO;


    ATT_SH  = Att_User.shdw;
    ATT_FN  = Att_User.nice;
    pbuf = (char far *) &fonch;
    zoomwin ();
    phlp = wnkeyhlp (help);
    utspeakr (0);
    if (pwind->bord.type & BORD_TYPE)   bord = 1;
    if (pwind->bord.type & BORD_SHADOW) { sh = 2; shd = 1; }
	while (!done) {
        switch (kbgetkey (NULL)) {
            case    Left:  chbord (1);  break;
            case    Up:    chbord (2);  break;
            case    Right: chbord (4);  break;
            case    Down:  chbord (8);  break;
            case    Ins:
                if (bord) {
                    if (shd) outshdw (ATT_FN);
                    bord = 0;
                    clearbox ();
                    if (shd) pwind->bord.type = BORD_SHADOW;
                    else  pwind->bord.type = 0;
                } else {
                    if (dcb.col < 1)     lt = 1;
                    if (dcb.row < 3)     up = 1;
                    if (dcb.row + dcb.h + shd >= NUM_ROWS - 1) dn = 1;
                    if (dcb.col + dcb.w + sh  >= NUM_COLS)     rt = 1;
                    if (dn > 0 || rt > 0 || up > 0 || lt > 0) {
                        if (pwind->img.dim.h - (up + dn) <= 2  ||
                                    pwind->img.dim.w - (lt + rt) <= 4) break;
                        if (shd) outshdw (ATT_FN);
                        pwind->img.dim.h -= up + dn;
                        pwind->img.dim.w -= lt + rt;
                        pwind->where_shown.row += up;
                        pwind->where_shown.col += lt;
                        dcb.h   = (int) pwind->img.dim.h;
                        dcb.w   = (int) pwind->img.dim.w;
                        dcb.row = (int) pwind->where_shown.row;
                        dcb.col = (int) pwind->where_shown.col;
                    }
                    bord = 1;
                    pwind->bord.type += BORD_DDSS;
                    wnputbor (pwind);
                }
                if (shd) outshdw (ATT_SH);
                break;
            case    GrayPlus:
                if (shd) {
                    pwind->bord.type -= BORD_SHADOW;
                    sh = shd = 0;
                    outshdw (ATT_FN);
                } else {
                    if (dcb.row + dcb.h + bord >= NUM_ROWS - 1)
                        dn = -1;
                    if (dcb.col + dcb.w + bord + 1 >= NUM_COLS - 1)
                        rt = NUM_COLS - dcb.col - dcb.w - bord - 2;
                    if (dn < 0 || rt < 0) {
                        if (pwind->img.dim.h  + dn <= 2  ||
                                    pwind->img.dim.w  + rt <= 4) break;
                        if (dn < 0) {
                            pscreen = wnviptrl (dcb.row + dcb.h + bord - 1,
                                                              dcb.col - bord);
                            wnvicopy (&pbuf,&pscreen, 1, dcb.w + 2 * bord,
                                                NUM_COLS*2, ATT_FN, Cmd[3]);
                        }
                        if (rt < 0) {
                            pscreen = wnviptrl(dcb.row - bord,
                                                dcb.col + dcb.w + rt + bord);
                            wnvicopy (&pbuf, &pscreen, dcb.h + 2 * bord,
                                          -rt, NUM_COLS*2, ATT_FN, Cmd[3]);
                        }
                        pwind->img.dim.h += dn;
                        pwind->img.dim.w += rt;
                        dcb.h   = (int) pwind->img.dim.h;
                        dcb.w   = (int) pwind->img.dim.w;
                        if (bord) wnputbor (pwind);
                    }
                    pwind->bord.type += BORD_SHADOW;
                    sh = 2; shd = 1;
                    outshdw (ATT_SH);
                }
                break;
            case    GrayEnter:
            case    Enter:
                dcb.bor.type = pwind->bord.type;
            case    Esc:
                outshdw (ATT_FN);
                if (bord) clearbox ();
                done = YES;
                break;
            default    :     break;
        }
        dn = rt = up = lt = 0;
    }
    if (pwind->bord.type != type) chwnbor = YES;
    wndstroy (pwind);
    wndstroy (phlp);
    return;
}

static void chbord (int type) {
    if (bord) {
        if ((pwind->bord.type - 1) & type) pwind->bord.type -= type;
        else                              pwind->bord.type += type;
        wnputbor (pwind);
    }
    return;
}

static void outshdw (int att) {
    pscreen = wnviptrl (dcb.row + 1 - bord, dcb.col + dcb.w + bord);
    wnvicopy (&pscreen, &pscreen, dcb.h + bord*2, 2, NUM_COLS*2, att, Cmd[4]);
    pscreen = wnviptrl(dcb.row + dcb.h + bord, dcb.col + 2 - bord);
    wnvicopy (&pscreen, &pscreen, 1, dcb.w + bord*2, NUM_COLS*2, att, Cmd[4]);
    return;
}

static void clearbox (void) {
    pscreen = wnviptrl (dcb.row - 1, dcb.col - 1);
    wnvicopy (&pbuf, &pscreen, 1, dcb.w + 2, NUM_COLS*2, ATT_FN, Cmd[3]);
    pscreen = wnviptrl(dcb.row + dcb.h, dcb.col - 1);
    wnvicopy (&pbuf, &pscreen, 1, dcb.w + 2, NUM_COLS*2, ATT_FN, Cmd[3]);
    pscreen = wnviptrl (dcb.row - 1, dcb.col - 1);
    wnvicopy (&pbuf, &pscreen, dcb.h + 2, 1, NUM_COLS*2, ATT_FN, Cmd[3]);
    pscreen = wnviptrl (dcb.row - 1, dcb.col + dcb.w);
    wnvicopy (&pbuf, &pscreen, dcb.h + 2, 1, NUM_COLS*2, ATT_FN, Cmd[3]);
    return;
}

