/*******************************************************************
    mndisvar - Выводит переменную часть элемента меню

bool mndisvar (MENU *pmenu, int tag);

MENU    *pmenu;     Указатель на структуру меню MENU
int     tag;        Номер элемента меню

Выход:  YES         Элемент выведен
        NO          Ошибка
*******************************************************************/

#include <slmenu.h>

bool mndisvar (MENU *pmenu, int tag) {
    MNITEM *p;

    if ((p = mnfnditm (pmenu->pitems, tag)) == NULL)  return (NO);
    return (mndisvri (pmenu, p));
}

bool  mndisvri (MENU *pmenu, MNITEM *pitem) {
    int         attr, len;
    CELLSC    * pdata = pmenu->pwin->img.pdata;

    if (pitem->vpos < 0)        return (NO);
    if (pmenu->pcur == pitem)   attr = pmenu->pwin->attr.high;
    else if (pitem->prot)       attr = pmenu->pwin->attr.prot;
    else                        attr = pmenu->pwin->attr.nice;

    pdata += pmenu->pwin->img.dim.w * pitem->row + pitem->col + pitem->vpos;
    len = min (strlen(pitem->vloc), pitem->vlen);
    wnmovpca (pdata, ' ', pitem->vlen, attr);
    wnmovpsa (pdata, pitem->vloc, len, attr);
    wnupdblk (pmenu->pwin, pitem->row, pitem->col+pitem->vpos, 1, pitem->vlen);
    return (YES);
}

