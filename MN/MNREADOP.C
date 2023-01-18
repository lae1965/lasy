/**********************************************************************
    mnreadop - Выбирает элемент меню
    mnreadit - Выбирает элемент меню

int mnreadop (MENU *pmenu, int option);

MENU    *pmenu      Указатель на структуру меню MENU
int     *pch        Буфер ASCII-кода введенной клавиши
int     *psc        Буфер скан-кода введенной клавиши
int     option      Режим выбора
                    MN_REMOVE   Убрать меню после выбора
                    MN_DSTROY   Разрушить меню после выбора
                    MN_TRANSMIT Пропустить неопознанные клавиши
                    MN_DIS_ESC  Запретить выход по ESC
                    MN_HORINVER Горизонтальные перемещения в VERT меню
                    MN_NOREM    Не убирать меню по ESC
                    MN_TRAN_ALL Пpопустить все клавиши.
                    MN_DIS_DN   Запретить выход по DOWN в HOR меню.

Выход:  0,1,2,...   Номер выбранного элемента меню
        -1          ESC
        -2          Ошибка
***********************************************************************/
#include <slmenu.h>
#include <slkeyb.h>
#include <slsymb.def>
#include <slmous.h>

static void      makecur (void);
static bool far  msmnhand (int ev, int bs, int xp, int yp, int xd, int yd);
static bool     lTransmitAll = NO; /* for handler use. (Yudin) */

static  bool     horinver;
static  MNITEM * p;

int mnreadop (MENU *pmenu, int option) {
    WINDOW      * psav;
    MNITEM      * q, * pcur;
    bool          we_showed = NO;
    bool          done = NO;
    int           event, ret, pover, qover;
    bool  (far * _Cdecl old_msfunc)();
    bool          found = NO;

    p = pmenu->pcur;
    do {
        if (!p->prot) { found = YES; break; }
        p = p->next;
    } while (p != pmenu->pcur);
    if (!found) return -2;
    found = NO;
    psav = Cur_Wind;
	if (!pmenu->pwin->options.shown) {
        if (!mndsplay (pmenu, WN_CURMNU, WN_CURMNU)) return (-2);
		we_showed = YES;
    }  else  wnselect (pmenu->pwin);
    Cur_Menu = pmenu;

    lTransmitAll = ((option & MN_TRAN_ALL) == MN_TRAN_ALL) ? YES:NO;

    msptsmsk (SL_WHITE, SL_WHITE, 0, MS_XOR);
    msunhide ();
    event = MS_LEFT_PRESS|MS_RIGHT_PRESS;
    old_msfunc = msshadow (&event, msmnhand);

    while (1) {
        makecur ();
        pcur = pmenu->pcur;
        if (done) break;

        kbgetkey (NULL);
        if (lTransmitAll == YES) done = YES;

        switch (Kb_sym) {
            case Left:
                if (pmenu->type == MN_VERT) {
                    if (!(option & MN_HORINVER)) break;
                    horinver = YES; p = NULL;  done = YES;
                    kbsetsym (Left);
                    break;
                }
                do p = p->prev;
                while (p->prot);
                if (pmenu->type == MN_HOR && horinver)
                    { Kb_sym = Down; done = YES; }
                break;
            case ShiftTab:
                do p = p->prev;
                while (p->prot);
                break;
            case Right:
                if (pmenu->type == MN_VERT) {
                    if (!(option & MN_HORINVER)) break;
                    horinver = YES;
                    p = NULL;
                    done = YES;
                    kbsetsym (Right);
                    break;
                }
                do p = p->next;
                while (p->prot);
                if (pmenu->type == MN_HOR && horinver)
                    { Kb_sym = Down; done = YES; }
                break;
            case Tab:
                do p = p->next;
                while (p->prot);
                break;
            case Up:
                if (pmenu->type == MN_HOR) break;
                do  p = p->prev;
                while (p != pcur && (p->row == pcur->row || p->prot));
                if (p == pcur) break;
                q = p;
                while (q->col > pcur->col && p->row == q->prev->row) {
                    if (!q->prot) p = q;
                    q = q->prev;
                }
                if (!q->prot && p != q) {
                    qover = q->col    + q->len    - pcur->col;
                    pover = pcur->col + pcur->len - p->col;
                    if (qover >= pover) p = q;
                }
                break;
            case Down:
                if (pmenu->type == MN_HOR) {
                    if ((option & MN_DIS_DN) == 0) done = YES;
                    break;
                }
                do  p = p->next;
                while (p != pcur && (p->row == pcur->row || p->prot));
                if (p == pcur) break;
                q = p;
                while (q->col < pcur->col && p->row == q->next->row) {
                    if (!q->prot) p = q;
                    q = q->next;
                }
                if (!q->prot && p != q) {
                    pover = p->col    + p->len    - pcur->col;
                    qover = pcur->col + pcur->len - q->col;
                    if (qover > pover) p = q;
                }
                break;
            case Home:
                p = pmenu->pitems;
                while (p->prot)  p = p->next;
                break;
            case End:
                p = pmenu->pitems->prev;
                while (p->prot) p = p->prev;
                break;
            case GrayEnter:
            case Enter:  horinver = NO;     done = YES;        break;
            case Esc:
                if (option & MN_DIS_ESC) break;
                p = NULL;  done = YES; horinver = NO;
                break;
            default:
                if (Kb_ch == 0) { utalarm (); break; }
                q = p;
                do {
                    if (strchr (q->keys, Kb_ch) != NULL) {
                        if (!q->prot) { found = YES; p = q; }
                        break;
                    }
                    q = q->next;
                } while (q != p);
                if (found) { Kb_sym = Enter;  done = YES; horinver = NO;}
                else if (option & MN_TRANSMIT) { p = NULL; done = YES; }
                else                             utalarm ();
                break;
        }
    }
    mshide ();
    msshadow (&event, old_msfunc);
    ret = (p == NULL) ? -1 : pcur->tag;
    if (we_showed || (option & MN_REMOVE) ||                         /*?????*/
        (Kb_sym == Esc && !(option & MN_NOREM)))  mnremove (pmenu);
    if (option & MN_DSTROY)   mndstroy (pmenu);
    wnselect (psav);
    return (ret);
}

static  void makecur (void) {
    MNITEM  * q = Cur_Menu->pcur;

    Cur_Menu->pcur = p;
    mnitmatr (Cur_Menu, q);
    if (p == NULL)  Cur_Menu->pcur = q;
    else            mnitmatr (Cur_Menu, p);
    return;
}

#pragma warn -par

static bool far _Cdecl msmnhand (int ev, int bs, int xp, int yp, int xd, int yd)
{
    int wrow  = Cur_Menu->pwin->where_shown.row;
    int wcol  = Cur_Menu->pwin->where_shown.col;
    bool    ret = NO;

    kbflushb ();
    switch (ev) {
        case MS_LEFT_PRESS:
            do {
                if (yp == p->row + wrow  &&
                    xp >= p->col + wcol  &&
                    xp <= p->col + wcol + p->len) {
                    if (p == Cur_Menu->pcur) {
                        kbsetsym (Enter);
                        ret = YES;
                    } else if (!p->prot) {
                        makecur ();
                        ret = YES;
                        if ( lTransmitAll == YES ) kbsetsym (0);
                    }
                    return ret;
                }
                p = p->next;
            } while (p != Cur_Menu->pcur);
            ret = msfunkey (xp, yp);
            break;
        case MS_RIGHT_PRESS:
            kbsetsym (Esc);
            ret = YES;
            break;
    }
    return ret;
}
#pragma warn +par

