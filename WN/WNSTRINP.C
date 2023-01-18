/*
    wnstrinp - Ввод строки символов в окне

bool wnstrinp (int row, int col, char *ttl, char *buf, int max);

int     row         Номер строки окна ввода на экране
int     col         Номер колонки окна ввода на экране
char    *ttl        Заголовок окна ввода (или NULL)
char    *buf        Буфер строки ввода
int     max         Максимальная длина строки

Выход:  YES         Строка введена
        NO          Строка не введена
*/

#include <slwind.h>

static  BORDER bor = { BORD_SSSS | BORD_SHADOW, NULL, NULL, NULL };

bool wnstrinp (int row, int col, char *ttl, char *buf, int max) {
    WINDOW  * pwin, * psav;
    bool      ret;
    int       wid;


    if ((bor.pttitle = ttl) != NULL)  bor.type |= BORD_TCT;
	wid = max (strlen (ttl), max + 1);
    psav = Cur_Wind;
    pwin = wncreate (1, wid, row, col, &bor, &Att_Mnu2); /* БП */
    wndsplay (pwin, row, col);
    wnwrtbuf (pwin, 0, 0, 0, buf, WN_NATIVE);
    ret = wnstredt (pwin, 0, 0, pwin->attr.text, pwin->attr.high, buf, max);
    wndstroy (pwin);
    wnselect (psav);
    return (ret);
}

