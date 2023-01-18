#include "makedoc.h"
#include <slmous.h>
#include <alloc.h>
#include <slkeyb.h>
#include <slsymb.def>
#include <sledit.h>

static void     outhlp   (void);
static void     filtitle (char * ptbuf);
void            outdoc   (WINDOW * pwdoc, EDT * pe);
void            outdate  (void);
bool far        msedhand (int ev, int bs, int xp, int yp, int xd, int yd);


static bool         insert = YES;
static BORDER       bordupl = { BORD_NO_BORDER, NULL, NULL, NULL };
static BORDER       borwrap = { BORD_DDSS|BORD_SHADOW," Значение ",NULL,NULL };
extern int          x, y, c_v;

int  edtdupl (int *length, int row, int col, int num, char *ptbuf, bool dupl) {
    int             event;
    bool            (far * _Cdecl old_msfunc)();
    WINDOW        * pwinhlp, * pwdoc = NULL;
    int             visicol = 6;
    register        Wrow, Wcol;
    bool            done = NO;
    EDT           * pe;
    long            total = -1;

    pe      = NULL;
    Ed_ppr  = NULL;

    if ((Ed_ppw = calloc (BUF_WRK + 4, sizeof (char))) == NULL) goto badmem;
    Ed_pw = Ed_ppw + 2;

    mkstatln ();
    pwinhlp = wnfunkey (outhlp, YES);

    Ed_rsize = Ed_lsize;
    Ed_ppr = utallocb (&Ed_rsize);
    if (Ed_rsize < 2400) goto badmem;
    Ed_pl = Ed_ppl + 3;                  Ed_pr = Ed_ppr + Ed_rsize - 3;
    Ed_rsize -= 6;                       Ed_lsize -= 6;

    if ((pe = utalloc (EDT)) == NULL) goto badmem;

    Ed_lbufempty = Ed_rbufempty = YES;

    pe->handinf = pe->handright = pe->handleft = -1;
    pe->infempty = YES;

    /* Right and left buffers will be initialize in load* () */

    pe->single = YES;
    pe->insert = insert;
    pe->total = (int) num;
    pe->cur.loff = -1L;
    pe->nbank = -1;


    if (num == 0) num = 1;
    if (dupl) {
        pwdoc = wncreate (NUM_ROWS - 3, NUM_COLS, 2, 0, &bordupl, &dcb.att);
        Ed_pwin  = wncreate (num, NUM_COLS, row + 3, col, &bordupl, &Att_Edit);
    } else {
        borwrap.type |= BORD_TCT;
        Ed_pwin  = wncreate (num, 61, row, col, &borwrap, &Att_Edit);
        wndsplay (Ed_pwin, WN_NATIVE, WN_NATIVE);
    }
    Wrow = Ed_pwin->where_shown.row;
    Wcol = Ed_pwin->where_shown.col;

    if (dupl) filtitle (ptbuf);

    loadright (pe);
    Ed_pleft = Ed_pl + *length - 1;
    MOVLEFT ((long) (*length - 1));  pe->line = 0L;
    if (*length == 0) pe->lf = 1;
    loadleft (pe);
    pe->needout |= NEED_OUTSCR;
    event = MS_LEFT_PRESS|MS_RIGHT_PRESS|MS_CENTER_PRESS|MS_MOVE;
    old_msfunc = msshadow (&event, msedhand);

    while (!done) {
        wntogcur (NO, NO);
        if (dupl && (pe->visicol != visicol || pe->total != total)) {
            visicol = pe->visicol;
            if (pe->total != total) {
                total   = pe->total;
                num = (int) total;
                utbound (num, 1, 15);
                while (pe->row >= num) kup (pe);
                wndstroy (Ed_pwin);
                Ed_pwin = wncreate (num, NUM_COLS, row + 3, 0, &bordupl,
                                                                    &Att_Edit);
                Wrow = Ed_pwin->where_shown.row;
                Wcol = Ed_pwin->where_shown.col;
            }
            outdoc (pwdoc, pe);
        }

        blankclr (pe);
        statupd (pe);
        x = Wcol + pe->col;
        y = Wrow + pe->row;
        c_v = pe->col + pe->visicol;
        wntogcur (pe->insert, YES);
        wnsetcur (y, x);
        mssetpos (y, x);

        if (pe->total > 0) {
            if      ((pe->needout & NEED_OUTSCR) == NEED_OUTSCR) outscr (pe);
            else if ((pe->needout & NEED_OUTLIN) == NEED_OUTLIN) outlin (pe);
        }
        pe->needout = 0;

        kbgetkey (outhlp);

        switch (Kb_sym) {
            case PgDn:           kpgdn (pe);                            break;
            case PgUp:           kpgup (pe);                            break;
            case Up:             kup(pe);                               break;
            case Down:           kdown(pe);                             break;
            case Left:           kleft(pe);                             break;
            case Right:          kright (pe);                           break;
            case CtrlLeft:       kcleft (pe);                           break;
            case CtrlRight:      kcright (pe);                          break;
            case CtrlHome:       kchome (pe, YES);                      break;
            case Home:           khome (pe);                            break;
            case CtrlEnd:        kcend (pe);                            break;
            case End:            kend (pe);                             break;
            case CtrlPgDn:       kcpgdn (pe);                           break;
            case CtrlPgUp:       kcpgup (pe);                           break;
            case F7:             kf7 (pe);                              break;
            case CtrlF9:         kcf9(pe);                              break;
            case F10:            kf10 (pe);                             break;
            case CtrlF10:        kcf10 (pe, &pe->cur ,NO, NO);          break;
            case F9:             kf9 (pe,pe,NO);                        break;
            case F8:             if (dupl) setsym (pe, '▓', 1);         break;
            case F2:             kf2 (pe);                              break;
            case ShiftLeft:      ksleft (pe);                           break;
            case ShiftRight:     ksright (pe);                          break;
            case ShiftUp:        ksup (pe);                             break;
            case ShiftDown:      ksdown (pe);                           break;
            case GrayEnter:
            case Enter:
                if (dupl && num < 15 && pe->insert) Ed_pwin->img.dim.h = num+1;
                kenter (pe);
                break;
            case CtrlY:          kcy (pe);                              break;
            case AltX:           done = YES;                            break;
            case BS:             kbs  (pe);                             break;
            case CtrlK:          kck  (pe);                             break;
            case Del:            kdel (pe);                             break;
            case Ins:            pe->insert = !pe->insert;
                                 wntogcur (pe->insert, YES);
                                 break;
            case Tab:     setsym (pe,' ',TAB_SIZE-(pe->col+pe->visicol)%TAB_SIZE);
                                break;

            default:

                if (Kb_ch == 0) break;
                setsym (pe, Kb_ch, 1);
                break;
		}
	}
    MOVRIGHT ((long) (Ed_pr - Ed_pright));
    msshadow (&event, old_msfunc);
ret:
    wndstroy (pwinhlp);
    wndstroy (Ed_pstat);
    wntogcur (NO, NO);
    wndstroy (Ed_pwin);
    wndstroy (pwdoc);
badmem:
    free (Ed_ppw);
    free (Ed_ppr);
    free (pe);
    if (Ed_ppw == NULL || Ed_ppr == NULL || pe == NULL) {
        mnwarnin (NULL, 0, 1, "Не хватает памяти");
        return -1;
    }
    insert = pe->insert;
    *length = * Ed_pnl;
    return ((int) pe->line);
}

/*        10        20        30        40        50        60        70
0123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
1xxxxxx 2xxxxxx 3xxxxxx 4xxxxxx 5xxxxxx 6xxxxxx 7xxxxxx 8xxxxxx 9xxxxxx 10xxxxxx
*/
static  char fkey[] =
"1       2SgDbEr 3       4       5       6       7MarkBl 8DefFld 9CopyBl 10MoveBl";
static  char shiffkey[] =
"1       2       3       4       5       6       7       8       9       10      ";
static  char ctrlfkey[] =
"1       2       3       4       5       6       7       8       9BlkOff 10DelBlk";
static  char altfkey[] =
"1       2       3       4       5       6       7       8       9       10      ";
static  char caps[] = "";

static void outhlp (void) {
    char far    * pscreen, * pbuf;

    if      (Kb_sym & KB_ALTSTAT)   pbuf = (char far *) altfkey;
    else if (Kb_sym & KB_SHIFTSTAT) pbuf = (char far *) shiffkey;
    else if (Kb_sym & KB_CTRLSTAT)  pbuf = (char far *) ctrlfkey;
    else                            pbuf = (char far *) fkey;
    pscreen = wnviptrl (NUM_ROWS - 1, 0);
    wnvicopy (&pbuf, &pscreen, 1, NUM_COLS, NUM_COLS*2, 0, Cmd[0]);
    if (Kb_status.caps_state)   pbuf = (char far *) &caps[0];
    else                        pbuf = (char far *) &caps[1];
    Ed_pstat->img.pdata [52].ch = *pbuf;
    pscreen = wnviptrl (0, 52);
    wnvicopy (&pbuf, &pscreen, 1, 1, NUM_COLS*2, Att_Khlp.afld, Cmd[2]);
}

static void filtitle (char * ptbuf) {
    char far    * pscreen, * pbuf;
    register      len, j;
    CELLSC      * pdata     = Ed_pstat->img.pdata + NUM_COLS;

    len = strlen (ptbuf);
    j = (NUM_COLS - len) / 2;

    wnmovpca (pdata, '─', NUM_COLS, Att_Khlp.nfld);
    wnmovpsa (pdata + j, ptbuf, len, Att_Khlp.afld);
    pbuf =  (char far *) pdata;
    pscreen = wnviptrl (1, 0);
    wnvicopy(&pbuf, &pscreen, 1, NUM_COLS, NUM_COLS*2, 0, Cmd[1]);
    return;
}

