#include <slmenu.h>

bool mnmoveit (MENU *pmenu) {

    MNITEM *p  = pmenu->pcur;
    MNITEM *pc = pmenu->pcur;

    while (1) {
        do  p = p->next;
        while (p != pmenu->pcur && p->row == pmenu->pcur->row);
        if (p != pmenu->pcur)
            while (p->col < pmenu->pcur->col && p->next->row == p->row)
                p = p->next;
        if (!p->prot || p == pc)  break;
    }
    if (p != pc) {
        pmenu->pcur = p;
        mnitmatr (pmenu, pc);
        mnitmatr (pmenu, p);
        return (YES);
    }
    return (NO);
}

