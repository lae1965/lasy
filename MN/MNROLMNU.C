/************************************************************************
    mnrolmnu - Создание рулонного меню, выбор элемента

int mnrolmnu (int row, int col, int max, int stl, char **txt, char *ttl,
              int hei, int pos);

int     row     Номер строки меню на экране
int     col     Номер колонки меню на экране
int     max     Максимальное число элементов меню
int     stl     Начальный элемент при входе в меню
char    **txt   Указатель на массив строк меню
char    *ttl    Заголовок окна (или NULL)
int     max     Максимальная высота окна в строках
int     pos     Позиция для сравнения первых букв
                -1  Без сравнения букв

Выход:  -2      Ошибка
        -1      Выход по ESC
        0,1,... Номер выбранного элемента меню
****************************************************************************/

#include <slmenu.h>
#include <slkeyb.h>
#include <slsymb.def>
#include <slmous.h>

static BORDER bor = { BORD_DDDD | BORD_SHADOW, NULL, NULL, NULL };

#define TEXT    Att_Roll.text
#define HIGH    Att_Roll.high
#define BORD    Att_Roll.bord

#define UP      "\030"
#define DN      "\031"

#define DIS_ITEM(itm)                                         \
    wnwrtbuf(Cur_Wind, itm - top, 0, 0, txt[itm], TEXT)

#define ATT_ITEM(itm, attr)                                   \
    wnatrblk(Cur_Wind, itm - top, 0, 1, wid, attr, WN_UPDATE)

#define BOT_LINE    (top + hei - 1)

static char more[4];
static int wid;
static int itm;
static int cur;
static int top;
static int  mou_flag;

static  void change_cur (int, char **, int);
static  bool far  msmnhand (int ev, int bs, int xp, int yp, int xd, int yd);

int mnrolmnu (int row, int col, int max, int stl, char **txt, char *ttl,
                                                          int hei, int pos) {
    void change_cur (int, char **, int);

    WINDOW            * win, * ps;
    int                 tmp, ch, ch_prev = 0;
    int                 btyp = 0, ntyp;
    int                 done = 0;
    register int        i;
    int                 event;
    bool                (far * _Cdecl old_msfunc)();

	wid = itm = cur = top = 0;

    for (i = 0; txt[i] != NULL; i++) {
        itm++;
        tmp = strlen (txt[i]);
        if (tmp > wid)     wid = tmp;
        if (max == itm)    break;
    }
    if (itm < hei) hei = itm;
    wid = max ((size_t)wid, strlen (ttl));

    if ((bor.pttitle = ttl) != NULL) bor.type |= BORD_TCT;
    bor.pbtitle = more;
    bor.type   |= BORD_BRT;

    *more = EOS;
    if ((win = wncreate (hei, wid, row, col, &bor, &Att_Roll)) == NULL)
        return (-2);
    win->options.delayed = 1;
    ps = Cur_Wind;
    if (!wndsplay (win, row, col))  return (-2);

    for (i = top; i <= BOT_LINE; i++)   DIS_ITEM (i);
	ATT_ITEM (cur, HIGH);
    if (stl < 0 || stl >= itm) stl = 0;
	change_cur (stl, txt, hei);

    msptsmsk (SL_WHITE, SL_WHITE, 0, MS_XOR);
    msunhide ();
    event = MS_LEFT_PRESS|MS_LEFT_RELEAS|MS_RIGHT_PRESS;
    old_msfunc = msshadow (&event, msmnhand);
    while (!done) {
        *more = '\0';
        ntyp = 0;
        if (top > 0)            { strcat (more, UP); ntyp |= 0x02; }
        if (itm > BOT_LINE + 1) { strcat (more, DN); ntyp |= 0x01; }
        if (ntyp != btyp)       { wnputbor (win);    btyp = ntyp;  }
        if (mou_flag == -1) { kbsetsym (Up); utsleept (1); }
        else if (mou_flag == 1) { kbsetsym (Down); utsleept (1); }
        switch (kbgetkey (NULL)) {
            case Home:  change_cur (0, txt, hei);       break;
            case End:   change_cur (itm - 1, txt, hei); break;
            case Up:
                if (cur > 0) change_cur (cur - 1, txt, hei);
                break;
            case Down:
            case Tab:
                if (cur < itm - 1) change_cur (cur + 1, txt, hei);
                break;
            case PgUp:
                if (cur > 0) {
                    tmp = cur - hei + 1;
                    if (tmp < 0) tmp = 0;
                    change_cur (tmp, txt, hei);
                }
                break;
            case PgDn:
                if (cur < itm - 1) {
                    tmp = cur + hei - 1;
                    if (tmp > itm - 1) tmp = itm - 1;
                    change_cur (tmp, txt, hei);
                }
                break;
            case Esc:   cur = -1;
            case GrayEnter:
            case Enter: done = 1;     break;
            case 0:                   break;
            default:
                if (Kb_ch == 0) break;
                else ch = utcupper (Kb_ch);
                if (pos == -1) break;
                utbound (i, 0, itm - 1);
                for (i = (ch != ch_prev) ? 0 : i ; i < itm; i++) {
                    if (ch == utcupper(txt[i][pos])) {
                        change_cur (i, txt, hei);
                        i++;
                        break;
                    }
                }
                if (i == itm) { utalarm();  i = 0; }
                else  ch_prev = ch;
                break;
        }
    }
    mshide ();
    msshadow (&event, old_msfunc);
    wndstroy (win);
    wnselect (ps);
	return (cur);
}

static void change_cur (int new, char *txt[], int hei) {
    int     off, i;
    int     H = Cur_Wind->img.dim.h, W = Cur_Wind->img.dim.w;

    ATT_ITEM (cur, TEXT);
    if ((off = top - new) > 0) {
		top -= off;
        if (off > hei) off = hei;
        wnscrblk (Cur_Wind, 0, 0, H, W, WN_NATIVE, WN_SCR_DOWN, off, 0);
        for (i = new; i < new + off; i++)
            DIS_ITEM (i);
	}
    else if ((off = new - BOT_LINE) > 0) {
		top += off;
        if (off > hei) off = hei;
        wnscrblk (Cur_Wind, 0, 0, H, W, WN_NATIVE, WN_SCR_UP, off, 0);
        for (i = new; i > new - off; i--)
            DIS_ITEM (i);
	}
    cur = new;
    ATT_ITEM (cur, HIGH);
    wnupdate (Cur_Wind);
}

#pragma warn -par
static bool far _Cdecl msmnhand (int ev, int bs, int xp, int yp,
                                                            int xd, int yd) {
    int           wrow  = Cur_Wind->where_shown.row;
    int           wcol  = Cur_Wind->where_shown.col;
    int           whei  = Cur_Wind->img.dim.h - 1;
    int           wwid  = Cur_Wind->img.dim.w - 1;
    int           tcur  = cur;
    char          sym[2];
    bool          ret = NO;
    char far    * pscreen, *pbuf;

    kbflushb ();
    mou_flag = 0;
    switch (ev) {
        case MS_LEFT_PRESS:
            if (yp >= wrow  &&  yp <= wrow + whei  &&
                xp >= wcol  &&  xp <= wcol + wwid) {
                    ATT_ITEM (cur, TEXT);
                    cur = yp - wrow + top;
                    ATT_ITEM (cur, HIGH);
                    wnupdate (Cur_Wind);
                    if (tcur == cur) kbsetsym (Enter);
                    return YES;
            }
            if (yp == wrow + whei + 1) {
                pbuf = (char far *) sym;
                pscreen = wnviptrl (yp, xp);
                wnvicopy (&pscreen, &pbuf, 1, 1, NUM_COLS * 2, 0, Cmd[5]);
                if (sym[0] == UP[0]) {
                    mou_flag = -1;
                    kbsetsym (Up);
                }
                else if (sym[0] == DN[0]) {
                    mou_flag = 1;
                    kbsetsym (Down);
                }
                return YES;
            }
            break;
        case MS_LEFT_RELEAS:
            kbsetsym (0);
            ret = YES;
            break;
        case MS_RIGHT_PRESS:
            kbsetsym (Esc);
            ret = YES;
            break;
    }
    return ret;
}
#pragma warn +par

