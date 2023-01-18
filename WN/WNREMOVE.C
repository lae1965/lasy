/***************************************************************************
    wnremove - Удаляет окно с экрана

bool wnremove (WINDOW *pwindow);

BWINDIW *pwindow    Указатель на структуру окна WINDOW

Выход:   YES        Окно удалено
         NO         Ошибка
***************************************************************************/

#include "language.h"
#include <slwind.h>

bool wnremove (WINDOW *pwindow) {
	bool ret = YES;

    if (!pwindow->options.shown) return (NO);
    if (Win_List == pwindow)     wnsizcur (0,0,0,0); /* cursor off */
    ret = wnhidewn (pwindow);
    wnpagrem (pwindow);
    if (ret == NO)               return (ret);
    pwindow->options.shown = 0;
    wnselect (Cur_Wind);
    return (YES);
}

