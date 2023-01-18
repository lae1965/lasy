/**********************************************************************
    mnwarnin - Создание меню предупреждения, выбор элемента меню

int mnwarnin (ITEM *arr, int max, int num[, char *msg1, char *msg2 ...]);

ITEM    *arr        Массив элементов меню
int     max         Число элементов меню
int     num         Число строк сообщений
char    *msg        Строки сообщений в меню

Выход:  -2          Ошибка
        -1          Выход по ESC
        0,1,...     Номер выбранного элемента меню
**********************************************************************/

#include "language.h"
#include <stdarg.h>
#include <slkeyb.h>
#include <slsymb.def>
#include <slmenu.h>

static  BORDER msbor = { BORD_DDDD, NULL, NULL, NULL };
static  BORDER phbor = { BORD_NO_BORDER | BORD_SHADOW, NULL,NULL,NULL };

int mnwarnin (ITEM *arr, int max, int num, ...) {
#ifdef ENGLISH
    static ITEM anykey =  { " press any key ", NULL, NULL };
#else
    static ITEM anykey =  { " нажмите любую клавишу ", NULL, NULL };
#endif
    ITEM      * itm;
    WINDOW    * pfon, *pswin;
    MENU      * pmsg, *psmnu;
    va_list     arg;
    int         len, pos, i;
    int         wid, hei;
    int         lenmnu;
    int         ret;
    bool        ok;
    char      * str;

    if ((itm = arr) == NULL || max == 0) {
        itm = &anykey;
        max = 1;
    }
    va_start (arg, num);
    for (wid = 0, i = 0; i < num; i++) {
        len = strlen (va_arg (arg, char *));
        if (len > wid)
            wid = len;
    }
    va_end (arg);
    for (lenmnu = 0, i = 0; i < max; i++)  lenmnu += strlen (itm[i].text) + 1;
    lenmnu--;
    wid = max (wid, lenmnu) + 2;
	hei = num + 1;

    pmsg = mncreate (hei, wid, 0, 0, &msbor, &Att_Warn);
    va_start (arg, num);
    for (i = 0; i < num; i++) {
        str = va_arg (arg, char *);
        len = strlen (str);
        wnwrtbuf (pmsg->pwin, i, (wid-len)/2, len, str, WN_NATIVE);
    }
    va_end (arg);
    for (pos = (wid - lenmnu)/2, i = 0; i < max; i++) {
        if (!(ok = mnitmadd (pmsg, i, num , pos, &itm[i]))) break;
        pos += strlen (itm[i].text) + 1;
	}
    if (!ok) { mndstroy (pmsg); return (-1); }
    pfon  = wncreate (hei+4, wid+8, 0, 0, &phbor, &Att_Warn);
    pswin = Cur_Wind;
    psmnu = Cur_Menu;
    wndsplay (pfon, WN_CENTER, WN_CENTER);
    mndsplay (pmsg, WN_CENTER, WN_CENTER);
    utalarm ();
    ret = mnreadop (pmsg, (max > 1 ? 0 : MN_TRAN_ALL) | MN_DIS_DN);
    if (max == 1 && Kb_sym != Esc) ret = 0;
    mndstroy (pmsg);
    wndstroy (pfon);
    Cur_Menu = psmnu;
    wnselect (pswin);
    return (ret);
}

