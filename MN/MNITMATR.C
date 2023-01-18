/**************************************************************************
    mnitmatr - Выводит аттрибуты элемента меню

void mnitmatr (pmenu, pitem);

MENU    *pmenu;     Указатель на структуру меню MENU
MNITEM  *pitem;     Указатель на элемент меню

**************************************************************************/

#include <slmenu.h>

void  mnitmatr (MENU *pmenu, MNITEM *p) {
    byte          attr;
    bool          sritem = NO;
    CELLSC      * pdata = pmenu->pwin->img.pdata;

    if (p == NULL) return;

    pdata += pmenu->pwin->img.dim.w * p->row + p->col;

    if (pmenu->pcur == p)  attr = pmenu->pwin->attr.high;
    else if (p->prot)      attr = pmenu->pwin->attr.prot;
    else { sritem = YES;   attr = pmenu->pwin->attr.text; }
    wnmovpa (pdata, p->len, attr);
    if (sritem) {
        if (p->bpos >= 0) wnmovpa (pdata + p->bpos, 1, pmenu->pwin->attr.bold);
        if (p->vpos >= 0) wnmovpa(pdata+p->vpos,p->vlen,pmenu->pwin->attr.nice);
    }
    wnupdblk (pmenu->pwin, p->row, p->col, 1, p->len);
    return;
}

