#include "makedoc.h"
#include <slmous.h>
#include <alloc.h>
#include <mem.h>
#include <dos.h>
#include <io.h>
#include <slkeyb.h>
#include <slsymb.def>
#include <sledit.h>


static  void    sav2file    (EDT * pe, char * filnam);
static  void    outhlp      (void);
void            outdate     (void);
bool far        msedhand    (int ev, int bs, int xp, int yp, int xd, int yd);

int     edtrdsld (int, void far *, int, int, void far *, long, word, long *);
int     edtrdcur (int, void far *, int, int, void far *, long, word, long *);
int     edtrdtxt (int, void far *, int, int, void far *, long, word, long *);
void    filtitle (EDT *pe);
bool    sl_open     (EDT *pe);
void    kf3         (EDT *pe);
void    kf4_5       (EDT *pe, bool for_prn);
void    sl_remov    (EDT *pe, bool all);
void    sl_close    (EDT *pe);
void    sl_swap     (EDT *pe);
void    sl_load     (EDT *pe);
void    savwind     (EDT *pe);
void    ksf9f10     (EDT *pe, bool move);
EDT    *findwind    (EDT *pe, char *title, bool all);
EDT    *kaltw       (EDT *pe);

extern char        * mdotxt;

WINDOW             * Ed_pwin, * Ed_pstat;
char              ** pnames;
char               * Ed_ppl, * Ed_pl, * Ed_ppr, * Ed_pr, * Ed_ppw;
char               * Ed_pw, * Ed_pright, * Ed_pleft;
word               * Ed_pnl, * Ed_pnllast, * Ed_pnr, * Ed_pnrlast;
word                 Ed_lsize, Ed_rsize;
bool                 Ed_lbufempty, Ed_rbufempty;
int                  x, y, c_v;

static EDT         * firstpe;
static int           numwind;
static BORDER        bor = { BORD_NO_BORDER, NULL, NULL, NULL };
static char        * document   =  "DOCUMENT";
static char        * rname      =  "R.▓▓▓";
static char        * lname      =  "L.▓▓▓";
static char        * statlin    =
"L:      C:    LaSy-Edit 1.04   [Text Edit]  Single    Ins   300k 22/07/92 23:55";
static char far    * star = "*─";

int edtrec (WINDOW *pwn, char * inf) {
    int             event;
    bool            (far * _Cdecl old_msfunc)();
    register        Wrow, Wcol;
    WINDOW         *pwinhlp;
    bool            done = NO;
    EDT            *pe;

    Ed_lbufempty = Ed_rbufempty = YES;
    numwind = 0;

    Ed_ppr = NULL;    /* for free () */
    Ed_ppl = NULL;

    if ((Ed_ppw = calloc (BUF_WRK + 4, sizeof (char))) == NULL) goto ret;
    Ed_pw = Ed_ppw + 2;


    Ed_lsize = (word) 20000L;
    Ed_ppl = utallocb (&Ed_lsize);
    if (Ed_lsize < 3000) goto ret;

    Ed_rsize = Ed_lsize;
    Ed_ppr = utallocb (&Ed_rsize);
    if (Ed_rsize < 3000) goto ret;

    pnames = (char **) Ed_pw;
    Ed_pl = Ed_ppl + 3;                     Ed_pr = Ed_ppr + Ed_rsize - 3;
    Ed_rsize -= 6;                          Ed_lsize -= 6;

    if ((pe = iniedt (NULL, inf)) == NULL) goto ret;
    Wrow = Ed_pwin->where_shown.row;
    Wcol = Ed_pwin->where_shown.col;

    pwinhlp = wnfunkey (outhlp, YES);
    event = MS_LEFT_PRESS|MS_RIGHT_PRESS|MS_CENTER_PRESS|MS_MOVE;
    old_msfunc = msshadow (&event, msedhand);

    while (!done) {
        x = Wcol + pe->col;
        y = Wrow + pe->row;
        c_v = pe->col + pe->visicol;
        wntogcur (pe->insert, YES);
        wnsetcur (y, x);
        mssetpos (y, x);
        blankclr (pe);
        statupd (pe);
        if      ((pe->needout & NEED_OUTSCR) == NEED_OUTSCR) outscr (pe);
        else if ((pe->needout & NEED_OUTLIN) == NEED_OUTLIN) outlin (pe);
        pe->needout = 0;

        kbgetkey (outhlp);
/*        kbflushb ();  */

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
            case F8:             setsym (pe, '▓', 1);                   break;
            case F2:             kf2 (pe);                              break;
            case ShiftLeft:      ksleft (pe);                           break;
            case ShiftRight:     ksright (pe);                          break;
            case ShiftUp:        ksup (pe);                             break;
            case ShiftDown:      ksdown (pe);                           break;
            case GrayEnter:
            case Enter:          kenter (pe);                           break;
            case CtrlY:          kcy (pe);                              break;
            case AltX:           done = kaltx (pe);                     break;
            case BS:             kbs  (pe);                             break;
            case CtrlK:          kck  (pe);                             break;
            case CtrlW:          pe = kcw (pe);                         break;
            case F3:             kf3 (pe);                              break;
            case F4:             kf4_5 (pe, NO);                        break;
            case F5:             kf4_5 (pe, YES);                       break;
            case AltW:           pe = kaltw (pe);                       break;
            case ShiftF10:       ksf9f10 (pe, YES);                     break;
            case ShiftF9:        ksf9f10 (pe, NO);                      break;
            case AltF1:          pe = otherwind (pe, YES, YES);         break;
            case ShiftF1:        pe = otherwind (pe, YES, NO);          break;
            case CtrlF1:         pe = otherwind (pe, NO, NO);           break;
            case Del:            kdel (pe);                             break;
            case Ins:            pe->insert = !pe->insert;              break;
            case Tab:     setsym (pe,' ',TAB_SIZE-(pe->col+pe->visicol)%TAB_SIZE);
                                break;

            default:

                if (Kb_ch == 0) break;
                setsym (pe, Kb_ch, 1);
                break;
		}
	}
    msshadow (&event, old_msfunc);

ret:
    wncursor (pwn);
    wndstroy (pwinhlp);
    wndstroy (Ed_pstat);
    free (Ed_ppw);
    free (Ed_ppr);
    free (Ed_ppl);
	return 0;
}

EDT  *iniedt (EDT *curpe, char *inf) {
    EDT            *pe;
    long            len;

    numwind++;
    if ((pe = utalloc (EDT)) == NULL) goto badmem;

    pe->numwind = numwind;
    pe->handinf = pe->handright = pe->handleft = -1;
    utstrcvt (inf, ST_TOUP);
    if (strlen (inf) > 0 && numwind != 1)  strcpy (pe->fname, inf);

    utl2crdx ((long) pe->numwind, Ed_pw, 0, 10);
    strcat (Ed_pw, rname);
    if ((pe->handright = utcreate (Ed_pw)) == -1) goto ret;
    utl2crdx ((long) pe->numwind, Ed_pw, 0, 10);
    strcat (Ed_pw, lname);
    if ((pe->handleft = utcreate (Ed_pw)) == -1) goto ret;

    pe->handinf = utopen (inf, UT_R);

    if (curpe == NULL) {
        strcpy (pe->fname, document);
        firstpe = pe->next = pe->prev = pe;
        mkstatln ();
        zoomwin (); utspeakr (0);
        Ed_pwin = pwind;
        if (Ed_pwin == NULL || !wnvalwin (Ed_pwin)) goto badmem;
    } else {
        pe->prev = curpe;
        pe->next = curpe->next;
        pe->next->prev = pe;
        curpe->next = pe;
    }
    filtitle (pe);
    WORKING;
    /* Right and left buffers will be initialize in load* () */

    pe->single = YES;
    pe->total = pe->cur.loff = -1L;
    pe->nbank = -1;
    if (pe->handinf != -1 && (len = utfsize (pe->handinf)) > 0L)  {
        if (valdcb(pe->handinf, inf)) {
            utread (pe->handinf, &dcbwrk, sizeof (DCB));
            if (pe == firstpe) dcbcorrect ();
            len = dcbwrk.otxtlen;
            if (len > 0L) {
                pe->infoff = sizeof (DCB) +
                    dcbwrk.tnamlen + dcbwrk.dnamlen + dcbwrk.fnamlen +
                    dcbwrk.cnamlen + dcbwrk.idclen  + dcbwrk.idflen  +
                    dcbwrk.idllen  + dcbwrk.pndlen  + dcbwrk.dtxtlen +
                    dcbwrk.pnolen;
                utlseek (pe->handinf, pe->infoff, UT_BEG);
            }
            pe->rightoff = edtrdsld (pe->handright, Ed_pw, BUF_WRK,
                 pe->handinf, Ed_pl, len, Ed_lsize - 10, &pe->total);
        } else {
            if (strcmp (inf, mdotxt) == 0)
                pe->rightoff = edtrdcur (pe->handright, Ed_pw, BUF_WRK,
                        pe->handinf, Ed_pl, len, Ed_lsize - 10, &pe->total);
            else
                pe->rightoff = edtrdtxt (pe->handright, Ed_pw, BUF_WRK,
                        pe->handinf, Ed_pl, len, Ed_lsize - 10, &pe->total);
        }
        pe->nbank = (int) (utfsize (pe->handright) / BUF_WRK) - 1;
    }

    utclose (pe->handinf); pe->handinf = -1;

    if (!loadright (pe)) pe->lf++;
    loadleft (pe);
    pe->insert = YES;
    pe->needout |= NEED_OUTSCR;
    return pe;

badmem:
    mnwarnin (NULL, 0, 1, "Не хватает памяти");
    goto next;
ret:
    mnwarnin (NULL, 0, 1, "Ошибка при чтении файла");
next:
    if (pe != NULL) {
        utclose (pe->handinf);
        sl_close (pe);
        sl_remov (pe, YES);
        free (pe);
    }
    numwind--;
    return NULL;
}

void    ksf9f10 (EDT *pe, bool move) {
    EDT     *pef;

    if (move) {
        if ((pef =findwind (pe, "══ Межоконное перемещение ══",NO)) ==NULL)
            return;
    } else {
        if ((pef =findwind (pe, "══ Межоконное копирование ══",NO)) ==NULL)
            return;
    }
    sl_open (pef);
    kf9 (pef, pe, NO);
    if (move) kcf10 (pef, &pef->cur, NO, YES);
    sl_close (pef);
    return;
}

EDT  *findwind (EDT *pe, char *title, bool all) {
    EDT          *newpe;
    register      i;

    for (newpe = pe->next, i = -1; newpe != pe; newpe = newpe->next)
        if (all || newpe->block) { i++; pnames[i] = newpe->fname; }
    if (i < 0) return NULL;
    if ((i =mnrolmnu (WN_CENTER,WN_CENTER, i +1, 0, pnames, title, 10, 1)) < 0)
        return NULL;
    for (newpe = pe->next; newpe->fname != pnames[i]; newpe = newpe->next);
    return newpe;
}

EDT *otherwind (EDT *pe, bool next, bool altf1) {
    EDT     *savpe = pe;

    if (pe == pe->next) return pe;

    if (altf1) {
        if ((pe =findwind (pe, "═════════ Список окон ═════════",YES)) ==NULL)
            return  savpe;
    } else pe = (next) ? pe->next : pe->prev;

    sl_swap (savpe);
    sl_close (savpe);
    sl_open (pe);
    filtitle (pe);
    sl_load (pe);

    pe->needout |= NEED_OUTSCR;
    return pe;
}

EDT *kaltw (EDT *pe) {
    EDT     *savpe = pe->next;

    if (pe == firstpe) return pe;
    sl_swap (pe);
    sl_close (pe);
    sl_remov (pe, YES);
    pe->prev->next = pe->next;
    pe->next->prev = pe->prev;
    free (pe);
    pe = savpe;
    sl_open (pe);
    filtitle (pe);
    sl_load (pe);
    pe->needout |= NEED_OUTSCR;
    return pe;
}

bool  kaltx (EDT *pe) {

    WORKING;
    if (pe != firstpe) {
        sl_close (pe);
        pe = firstpe;
        sl_open (pe);
        Ed_lbufempty = Ed_rbufempty = YES;
    }
    savwind (pe);
    sl_close (pe);
    sl_remov (pe, NO);
    pe = pe->next;
    while (pe != firstpe) {
        sl_remov (pe, YES);
        pe = pe->next;
        free (pe->prev);
    }
    free (pe);
    return YES;
}

EDT *kcw (EDT *pe) {
    char    *fname = Ed_pw + BUF_WRK / 2;
    EDT     *nextpe;

    *fname = '\0';
    if (!mnfnminp (WN_CENTER, WN_CENTER,
                    "Введите имя файла или нажмите ENTER для выбоpa имени",
                                    fname, NULL, YES, YES)) return pe;

    sl_swap (pe);
    sl_close (pe);

    if ((nextpe = iniedt (pe, fname)) == NULL) {
        sl_open (pe);
        sl_load (pe);
        return pe;
    }
    return nextpe;
}

void    kf3 (EDT *pe) {

    if (pe != firstpe) {
        mnwarnin (NULL, 0, 3, " ", "Окно является несохраняемым", " ");
        return;
    }

    sav2file (pe, mdotxt);
    return;
}

void    kf4_5 (EDT * pe, bool for_prn) {
    int     handfrom, handto;
    char  * tmpnam = "tmp.nam";

    if (for_prn) handto = utopen ("prn", UT_W);
    else {
        * Ed_pw = EOS;
        if (!wnstrinp (WN_CENTER, WN_CENTER, " Введите имя файла ", Ed_pw, 13))
            return;
        handto = utcreate (Ed_pw);
    }
    sav2file (pe, tmpnam);
    handfrom = utopen (tmpnam, UT_R);

    txt2bin (handto, handfrom, Ed_pw, BUF_WRK);

    utclose (handfrom);
    utclose (handto);
    utremove (tmpnam);
    return;
}

void    savwind (EDT *pe) {
    word        i;
    register    nbank = pe->nbank, rightoff = pe->rightoff;
    int         hand = pe->handleft;


    utlseek (hand, 0L, UT_END);
    if (!Ed_lbufempty) utwrite (hand, Ed_pl, (word) (Ed_pleft - Ed_pl) + 1);
    if (!Ed_rbufempty) utwrite (hand, Ed_pright, (word) (Ed_pr - Ed_pright)+1);

    for (i = 0; nbank >= 0; nbank--) {
        utlseek (pe->handright,
            (long) nbank * (long) BUF_WRK + (long) rightoff, UT_BEG);
        i =  utread (pe->handright, Ed_pw, BUF_WRK - rightoff);
        utwrite (hand, (char *) Ed_pw, i);
        rightoff = 0;
    }
    utremove (mdotxt);
    utclose (hand);
    strcpy (Ed_pw, "1");
    strcat (Ed_pw, lname);
    utrename (Ed_pw, mdotxt);
    return;
}

bool   sl_open (EDT *pe) {

    utl2crdx ((long) pe->numwind, Ed_pw, 0, 10);
    strcat (Ed_pw, rname);
    if ((pe->handright = utopen (Ed_pw, UT_RW)) == -1) return NO;
    utl2crdx ((long) pe->numwind, Ed_pw, 0, 10);
    strcat (Ed_pw, lname);
    if ((pe->handleft = utopen (Ed_pw, UT_RW)) == -1) return NO;
    utlseek (pe->handleft, pe->leftoff, UT_BEG);
    return YES;
}

void    sl_close (EDT *pe) {

    utclose (pe->handright);              pe->handright = -1;
    utclose (pe->handleft);               pe->handleft = -1;
    return;
}

void    sl_remov (EDT *pe, bool all) {

    utl2crdx ((long) pe->numwind, Ed_pw, 0, 10);
    strcat (Ed_pw, rname);
    utremove (Ed_pw);
    if (all) {
        utl2crdx ((long) pe->numwind, Ed_pw, 0, 10);
        strcat (Ed_pw, lname);
        utremove (Ed_pw);
    }
    return;
}

void    sl_swap (EDT *pe) {

    if (pe->blkmark) kf7 (pe);
    if (pe->lf == 0) MOVLEFT ((long) (pe->col + pe->visicol - pe->blank) - 1L);
    swapright (pe, YES);
    swapleft  (pe, YES);
    return;
}

void    sl_load (EDT *pe) {

    loadright (pe);
    loadleft (pe);
    if (pe->lf == 0) MOVRIGHT ((long) (pe->col + pe->visicol - pe->blank) -1L);
    return;
}

/*        10        20        30        40        50        60        70
0123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
1xxxxxx 2xxxxxx 3xxxxxx 4xxxxxx 5xxxxxx 6xxxxxx 7xxxxxx 8xxxxxx 9xxxxxx 10xxxxxx
*/
static  char * fkey =
"1       2SgDbEr 3Save   4ToFile 5Print  6       7MarkBl 8DefFld 9CopyBl 10MoveBl";
static  char * shiffkey =
"1PrvWin 2       3       4       5       6       7       8       9WnCopy 10WnMove";
static  char * ctrlfkey =
"1NxtWin 2       3       4       5       6       7       8       9BlkOff 10DelBlk";
static  char * altfkey =
"1LstWin 2       3       4       5       6       7       8       9       10      ";
static  char * caps = "";

static void outhlp (void) {
    char far    * pscreen, * pbuf;

    if      (Kb_sym & KB_ALTSTAT)   pbuf = (char far *) altfkey;
    else if (Kb_sym & KB_SHIFTSTAT) pbuf = (char far *) shiffkey;
    else if (Kb_sym & KB_CTRLSTAT)  pbuf = (char far *) ctrlfkey;
    else                            pbuf = (char far *) fkey;

    pscreen = wnviptrl (NUM_ROWS - 1, 0);
    wnvicopy (&pbuf, &pscreen, 1, NUM_COLS, NUM_COLS*2, 0, Cmd[0]);

    if (Kb_status.caps_state)   pbuf = (char far *) caps;
    else                        pbuf = (char far *) (caps + 1);
    Ed_pstat->img.pdata [52].ch = *pbuf;

    pscreen = wnviptrl (0, 52);
    wnvicopy (&pbuf, &pscreen, 1, 1, NUM_COLS*2, Att_Khlp.afld, Cmd[2]);
}

void filtitle (EDT *pe) {
    char far    * pscreen, * pbuf;
    register      len, j;
    CELLSC      * pdata     = Ed_pstat->img.pdata + NUM_COLS;

    if (pe == firstpe) strcpy (Ed_pw, document);
    else {
        strcpy (Ed_pw, "UNSAVED !─────");
        strcat (Ed_pw, pe->fname);
    }
    strcat (Ed_pw, "─");
    len = strlen (Ed_pw);
    j = NUM_COLS - len;
    wnmovpca (pdata, '─', j, Att_Khlp.nfld);
    wnmovpsa (pdata + j, Ed_pw, len, Att_Khlp.nfld);
    pbuf =  (char far *) pdata;
    pscreen = wnviptrl (1, 0);
    wnvicopy(&pbuf, &pscreen, 1, NUM_COLS, NUM_COLS*2, 0, Cmd[1]);
    return;
}

void    mkstatln (void) {
    char far      * pscreen, * pbuf;
    CELLSC        * pdata;

    Ed_pstat = wncreate (2, NUM_COLS, 0, 0, &bor, &Att_Khlp);
    Ed_pstat->options.delayed = 0;
    Ed_pstat->options.cur_on  = 0;
    wndsplay (Ed_pstat, -1, -1);

    pdata = Ed_pstat->img.pdata;
    wnmovps (pdata, statlin, 61);

    pbuf =  (char *) (Ed_pstat->img.pdata);
    pscreen = wnviptrl (0, 0);
    wnvicopy (&pbuf, &pscreen, 1, 61, NUM_COLS*2, 0, Cmd[1]);
    return;
}

void statupd (EDT *pe) {
/*                                WORKING    Double     Ovr
"L:      C:    LaSy-Edit 1.04   [Text Edit]  Single    Ins   300k 22/07/92 23:55";
 01234567890123456789012345678901234567890123456789012345678901234567890123456789
		   1         2         3         4         5         6         7
*/

    CELLSC      * pdata;
    char        * p = "       ";
    char        * pld [] = { "Double", "Single", "Erase " };
    char        * pins [] = { "Ins", "Ovr" };
    byte          afld = Ed_pstat->attr.afld, nfld = Ed_pstat->attr.nfld;
    char far    * pbuf,   * pscreen;
    register      j;

    pdata = Ed_pstat->img.pdata;

    wnmovpsa (pdata + 44, pld [pe->single], 6, afld);
    wnmovpsa (pdata + 55, pins [pe->insert], 3, afld);

    j = utl2crdx ((long) pe->line, p, 0, 10);
    wnmovpca (pdata + 2, ' ', 6, nfld);
    wnmovpsa (pdata + 2, p, j, afld);

    j = utl2crdx ((long) (pe->col + pe->visicol), p, 0, 10);
    wnmovpca (pdata + 10, ' ', 4, nfld);
    wnmovpsa (pdata + 10, p, j, afld);

    pbuf =  (char far *) (Ed_pstat->img.pdata);
    pscreen = wnviptrl (0, 0);
    wnvicopy (&pbuf, &pscreen, 1, 61, NUM_COLS*2, 0, Cmd[1]);

    pscreen = wnviptrl (1, 1);
    if (pe->changed) {
        pbuf = (char far *) &star[0];
        nfld = afld;
    } else  pbuf = (char far *) &star[1];
    wnvicopy(&pbuf, &pscreen, 1, 1, NUM_COLS*2, nfld, Cmd[2]);
    return;
}

static void sav2file (EDT * pe, char * filnam) {
    word        i;
    register    nbank = pe->nbank, rightoff = pe->rightoff;
    int         handout;

    WORKING;
    utlseek (pe->handleft, 0L, UT_BEG);
    handout = utcreate (filnam);

    while (1) {
        i =  utread (pe->handleft, Ed_pw, BUF_WRK);
        if (i == 0) break;
        utwrite (handout, Ed_pw, i);
    }
    utwrite (handout, Ed_pl, (word) (Ed_pleft - Ed_pl) + 1);
    utwrite (handout, Ed_pright, (word) (Ed_pr - Ed_pright) + 1);

    for (i = 0; nbank >= 0; nbank--) {
        utlseek (pe->handright,
            (long) nbank * (long) BUF_WRK + (long) rightoff, UT_BEG);
        i =  utread (pe->handright, Ed_pw, BUF_WRK - rightoff);
        utwrite (handout, (char *) Ed_pw, i);
        rightoff = 0;
    }
    utclose (handout);
    return;
}

#pragma warn -par
bool far _Cdecl msedhand (int ev, int bs, int xp, int yp, int xd, int yd) {
    bool        ret = YES;

    kbflushb ();
    switch (ev) {
        case MS_LEFT_PRESS:      kbsetsym (F9);         break;
        case MS_RIGHT_PRESS:     kbsetsym (F7);         break;
        case MS_CENTER_PRESS:    kbsetsym (CtrlF9);     break;
        default :
            if (xp < x && c_v != 0) kbsetsym (Left);
            else if (xp > x)        kbsetsym (Right);
            if (yp < y)             kbsetsym (Up);
            else if (yp > y)        kbsetsym (Down);
            if (xp == x && yp == y) ret = NO;
            break;
    }
    return ret;
}
#ifdef TC_lang
#pragma warn +par
#endif

