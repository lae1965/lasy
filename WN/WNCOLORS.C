#include <slkeyb.h>
#include <slsymb.def>
#include <slwind.h>
#include <slmous.h>
#include <alloc.h>
#include <string.h>
static bool far msmnhand (int ev, int bs, int xp, int yp, int xd, int yd);

static int  Row_Up;
static int  Col_Lf;

static  byte    Ccol;
static  BORDER  bc = { BORD_DDSS|BORD_SHADOW,NULL,NULL,NULL };
static  char    * txt = "Ú Ä Ä Ä ¿ ³   x   ³ À Ä Ä Ä Ù ";

bool  wncolors (byte *pcol, char *titl, void (* fn)(void)) {
    int         event;
    bool        (far * _Cdecl old_msfunc)();
    WINDOW    * wbig;
    CELLSC      cel, * pdata;
    int         sc, i, j, row, col;
    byte        att_sav  = *pcol;
    bool        ret  = NO, don = NO;
    char      * p, * q;
    char far  * pscreen, * pcolscr;

    Ccol = *pcol;

    if (titl != NULL) {
        p = titl; q = titl + strlen (titl) - 1;
        while (*p == ' ') p++;
        while (*q == ' ') q--;
        i = (int) (q - p + 1);
        if ((pscreen = calloc (i + 1, 1)) == NULL) return NO;
        strncpy (pscreen, p, i);
        bc.pttitle = pscreen;
        bc.type   |= BORD_TCT;
    }
    if ((wbig = wncreate (21, 52, 0, 0, &bc, &Att_Wind)) == NULL)
        { free (pscreen); return NO; }

    wnsplitw (wbig, 17, WN_HSPLIT|WN_SINGLE);


    pdata = wbig->img.pdata + 938;
    wnmovpca (pdata, ' ', 48, Ccol);
    pdata += 52;
    wnmovpca (pdata, ' ', 48, Ccol);
    wnmovps (pdata + 14, "à¨¬¥à áâà®ª¨ â¥ªáâ ", 20);
    pdata += 52;
    wnmovpca (pdata, ' ', 48, Ccol);

    pdata = wbig->img.pdata;
    for (sc = 0, col = 2, i = 0; i < 16; col +=3, i++)
        for (row = 1, j = 0; j < 16; row++, j++, sc++)
            wnmovpsa (pdata + 52*row + col, " x ", 3, sc);

    wndsplay (wbig, 0, 0);
    Row_Up = wbig->where_shown.row + 1;
    Col_Lf = wbig->where_shown.col + 2;
    free (pscreen);

    pscreen = wnviptrl (wbig->where_shown.row + 18, wbig->where_shown.col + 2);

    msptsmsk (SL_WHITE, SL_WHITE, 0, MS_XOR);
    msunhide ();
    event = MS_LEFT_PRESS|MS_RIGHT_PRESS;
    old_msfunc = msshadow (&event, msmnhand);
    while (!don) {
        row = (int)( Ccol % 16);
        col = (int)((Ccol / 16) * 3 + 1);

        pcolscr = wnviptrl (wbig->where_shown.row + row,
                                         wbig->where_shown.col + col);
        pdata = wbig->img.pdata + 52 * row + col;

        for (i = 0; i < 3; i++, pdata += 52) {
            for (j = 0; j < 5; j++) {
                cel = pdata[j];
                txt [i * 10 + j * 2 + 1] = (i == 1 && j == 2) ?
                            cel.at : (cel.at & 0x70) | SL_WHITE | SL_INTENSITY;
            }
        }

        wnvicopy (&txt, &pcolscr, 3, 5, NUM_COLS*2, 0, Cmd[1]);
        wnvicopy (&pscreen, &pscreen, 3, 48, NUM_COLS*2, Ccol, Cmd[4]);

        if (fn != NULL) (* fn)();

        switch (kbgetkey (NULL)) {
            case F3:     Ccol = att_sav;     break;
            case F10:
            case GrayEnter:
            case Enter:  don = ret = YES;    break;
            case Esc:    don = YES;          break;
            case Home:   Ccol -= 17;         break;
            case End:    Ccol -= 15;         break;
            case PgDn:   Ccol += 17;         break;
            case PgUp:   Ccol += 15;         break;
            case Left:   Ccol -= 16;         break;
            case Right:  Ccol += 16;         break;
            case Up:     Ccol --;            break;
            case Down:   Ccol ++;            break;
            default :                        break;
        }
        wnupdblk (wbig, row, col, 3, 5);
        * pcol = Ccol;
    }
    mshide ();
    msshadow (&event, old_msfunc);

    wndstroy (wbig);
    * pcol = (ret) ? Ccol : att_sav;
    return (ret);
}
#pragma warn -par

#define KB_C_N_ENTER    13
#define KB_S_N_ENTER    28
#define KB_C_N_ESC      27
#define KB_S_N_ESC      1
static bool far _Cdecl msmnhand (int ev, int bs, int xp, int yp, int xd, int yd)
{
    int tmp;
    bool ret = NO;

    kbflushb ();
    switch (ev) {
        case MS_LEFT_PRESS:
            if (yp >= Row_Up  &&  yp <= Row_Up + 15  &&
                xp >= Col_Lf  &&  xp <= Col_Lf + 47) {
                tmp = (xp - Col_Lf) / 3 * 16 + yp - Row_Up;
                if (tmp == Ccol)   kbsetsym (Enter);
                else               kbsetsym (0);
                Ccol = tmp;
                return YES;
            }
            break;
        case MS_RIGHT_PRESS:
            kbsetsym (Esc);
            ret= YES;
            break;
    }
    return ret;
}
#ifdef TC_lang
#pragma warn +par
#endif

/*
    0         10        20        30        40        50
     0123456789 123456789 123456789 123456789 123456789 1
        0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ·
 0  ³                                                    º°°
 1 0³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°°
 2 1³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°°
 3 2³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°°
 4 3³   x  xÚÄÄÄ¿x  x  x  x  x  x  x  x  x  x  x  x  x   º°ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ·
 5 4³   x  x³ x ³x  x  x  x  x  x  x  x  x  x  x  x  x   º°³ ’¥ªáâ          º°°
 6 5³   x  xÀÄÄÄÙx  x  x  x  x  x  x  x  x  x  x  x  x   º°³ ’¥ªáâ          º°°
 7 6³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°³ €ªâ¨¢­ë©       º°°
 8 7³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°³ ’¥ªáâ          º°°
 9 8³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°³ ’¥ªáâ  ‚ë¤¥«¥­ º°°
10 9³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°³ ’¥ªáâ          º°°
11 0³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°³ ¥¤®áâã¯­ë©    º°°
12 1³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°³ ’¥ªáâ          º°°
13 2³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼°°
14 3³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°°°°°°°°°°°°°°°°°°°°°
15 4³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°°
16 5³   x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x   º°°
17  ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶°°
18  ³   ‘¨¬¢®«       15    F                             º°°
19  ³   ”®­          15    F     à¨¬¥à áâà®ª¨ â¥ªáâ     º°°
20  ³   €âà¨¡ãâ     255   FF                             º°°
    ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼°°
     °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
     0123456789 123456789 123456789 123456789 123456789 1
    0         10        20        30        40        50
*/

