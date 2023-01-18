/*************************************************************
    mnfnditm - Поиск структуры элемента меню по номеру

MNITEM * mnfnditm (MNITEM *h, int tag);

MNITEM  *h;         Указатель на начальную структуру MNITEM
int     tag;        Номер элемента меню

Выход:  Указатель на структуру MNITEM
        NULL        Элемент меню не найден
**************************************************************/

#include <slmenu.h>

MNITEM * mnfnditm (MNITEM * h, int tag) {
    MNITEM * p = h;

    do  if (p->tag == tag)  return (p);
    while ((p = p->next) != h);
    return (NULL);
}

