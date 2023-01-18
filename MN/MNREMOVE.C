/*****************************************************************
    mnremove - Удаляет меню с экрана

bool mnremove (MENU *pmenu);

MENU    *pmenu;     Указатель на структуру меню MENU

Выход:  YES         Меню удалено
        NO          Ошибка
*******************************************************************/

#include <slmenu.h>

bool mnremove (MENU *pmenu) {
    MNITEM *p = pmenu->pcur;

    if (!wnremove (pmenu->pwin))  return (NO);
    pmenu->pcur = NULL;
    mnitmatr (pmenu, p);
    pmenu->pcur = p;
    if (pmenu == Cur_Menu) Cur_Menu = NULL;
    return (YES);
}

