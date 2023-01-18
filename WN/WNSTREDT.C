/*****************************************************************
    wnstredt - Редактор строки

bool wnstredt (WINDOW *pw, int r, int c, int tat, int hat, char *str, int max);

char    *buf                Буфер редактируемой строки
int     max                 Максимальная длина буфера строки

Выход:  YES                 Выход по Enter  с записью редакции
        NO                  Выход по Esc    без записи редакции

                 Функции редактора строки:

 <Left>          Перемещение курсора на позицию влево
 <Right>         Перемещение курсора на позицию вправо
 <Home>          Перемещение курсора в начало строки
 <End>           Перемещение курсора в конец строки
 <BS>            Удаление символа слева от курсора
 <Del>           Удаление символа над курсором
 <Ins>           Переключение режимов ВСТАВКА / ЗАМЕЩЕНИЕ
 <Ctrl-Left>     Перемещение курсора на слово влево
 <Ctrl-Right>    Перемещение курсора на слово вправо
 <Ctrl-K>        Удаление до конца строки

*******************************************************************/

#include <slmous.h>
#include <slwind.h>
#include <slkeyb.h>
#include <slsymb.def>

#define DMPSTR          wnmovpsa ((CELLSC far *) pbuf, buf, max, tat);
#define CLRZAD          utmovsc  (&buf[len], ' ', max - len)
#define CURMOV          wnsetcur (r, x)

static int x;


static bool far msmnhand (int ev, int bs, int xp, int yp, int xd, int yd);

bool wnstredt (WINDOW *pw, int r, int c, int tat, int hat, char *str, int max) {
    int                 event, Mx, My, Mb;
    bool                (far * _Cdecl old_msfunc)();
    char              * buf, * p;
    int                 col, len, first = 1;
    bool                term, done = NO, insert = YES;
    char far          * pbuf, * pscreen, * pblank = " ";
    CELLSC            * savscr;
    register            WROW = pw->where_shown.row;
    register            WCOL = pw->where_shown.col;

    p = calloc (max + 4, 1);  /* БП */
    buf = p + 2;
    savscr = calloc (max + 1, sizeof (CELLSC));  /* БП */

    wntogcur (insert, YES);
    pbuf    = (char far *) (pw->img.pdata + pw->img.dim.w * r + c);
    pscreen = wnviptrl (WROW + r, WCOL + c);
    wnvicopy (&pscreen, &(char far *) savscr, 1, max, NUM_COLS*2, 0, Cmd[6]);
    len = strlen (str);
    if (len >= max) { len = max; col = len - 1; }
    else                         col = len;
    strncpy (buf, str, len);
    CLRZAD;
    DMPSTR;
    wnvicopy (&pblank, &pscreen, 1, max, NUM_COLS*2, tat, Cmd[3]);

    if (len > 0) wnvicopy (&buf, &pscreen, 1, len, NUM_COLS*2, hat, Cmd[2]);

    event = MS_MOVE;
    old_msfunc = msshadow (&event, msmnhand);
    msgetpos (&My, &Mx, &Mb);
    mshide ();
    r += WROW;
    while (!done) {
        x = WCOL + c + col;
        CURMOV;
        mssetpos (r, x);
        switch (kbgetkey (NULL)) {
            case GrayEnter:
            case Enter:       term = done = YES;                      break;
            case Esc:         term = NO;  done = YES;                 break;
            case BS:
                if (col > 0) {
                    if (insert) {
                        utmovabs (&buf[col - 1], &buf[col], len - col, 0);
                        len--;
                    } else {
                        if (col == len)             len--;
                        else                        buf[col-1] = ' ';
                    }
                    col--;
                }
                break;
            case CtrlK:
                if (col < len) len = col;
                break;
            case Left:
                if (col > 0)   col--;
                break;
            case Right:
                if (col < len) col++;
                break;
            case Home:        col = 0;                                break;
            case End:         col = len;                              break;
            case CtrlLeft:
                if (col > 0) {
                    while (buf[col-1] == ' ' && col > 0) col--;
                    while (buf[col-1] != ' ' && col > 0) col--;
                }
                break;
            case CtrlRight:
                if (col < len) {
                    while (buf[col] != ' ' && col < len) col++;
                    while (buf[col] == ' ' && col < len) col++;
                }
                break;

            case Del:
                if (col < len) {
                    utmovabs (&buf[col], &buf[col + 1], len - col - 1, 0);
                    len--;
                }
                break;
            case Ins:
                insert = !insert;
                wntogcur (insert, YES);
                break;
            default:
                if (Kb_ch == 0) { utalarm (); break; }
                if (first && (len > 0)) {
                    col = len = 0;
                }
                if (col < len) {
                    if (insert) {
                        utmovmem (&buf[col + 1], &buf[col], len - col);
                        if (len < max) len++;
                    }
                    buf[col] = (char) Kb_ch;
                    col++;
                } else if (len < max) {
                    buf[len++] = (char) Kb_ch;
                    col++;
                } else { utalarm ();   break; }
        }
        if (col == max) col--;
        CLRZAD;
        DMPSTR;
        wnvicopy (&pbuf, &pscreen, 1, max, NUM_COLS*2, 0, Cmd[1]);
        first = 0;
    }
    col = 0;
    buf [len] = EOS;
    if (term)   strcpy (str, buf);
    else  wnvicopy (&(char far *)savscr,&pscreen,1,max,NUM_COLS*2,0,Cmd[1]);
    wncursor (pw);
    mssetpos (My, Mx);
    msunhide ();
    msshadow (&event, old_msfunc);
    free (p);
    free (savscr);
    return (term);
}

#pragma warn -par
static bool far _Cdecl msmnhand (int ev, int bs, int xp, int yp, int xd, int yd)
{
    kbflushb ();
    return YES;
}
#ifdef TC_lang
#pragma warn +par
#endif

