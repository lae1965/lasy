/*********************************************************************
    mnsetpro - Устанавливает режим защиты элемента меню

bool mnsetpro (MENU *pmenu, int tag, int option);

MENU    *pmenu;     Указатель на структуру меню MENU
int     tag;        Номер элемента меню
int     option;     Режим защиты элемента меню
                    MN_NOPR     Снять защиту элемента
                    MN_PR       Установить защиту

Выход:  YES         Нормальная установка
        NO          Ошибка
**********************************************************************/

#include <slmenu.h>

bool  mnsetpro (MENU *pmenu, int tag, int option) {
    MNITEM *p;

    if ((p = mnfnditm (pmenu->pitems, tag)) == NULL)  return (NO);
    p->prot = option;
    mnitmatr (pmenu, p);
    return (YES);
}

