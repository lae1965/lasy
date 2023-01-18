#include <slmenu.h>

bool mnsetcur (MENU *pmenu, int tag) {
    MNITEM *p, *pcur;
    if ((p = mnfnditm (pmenu->pitems, tag)) == NULL)    return (NO);
    pcur = pmenu->pcur;
    pmenu->pcur = p;
    if (pcur != pmenu->pcur) {
        mnitmatr (pmenu, pcur);
        mnitmatr (pmenu, pmenu->pcur);
    }
    return (YES);
}

