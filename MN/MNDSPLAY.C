/**********************************************************************
    mndsplay - Выводит меню на экран

bool mndsplay (MENU *pmenu, int row, int col);

MENU  *pmenu;   Указатель на структуру меню MENU
int    row;     Строка  левого верхнего угла окна меню
                WN_NATIVE   Как было указано при создании
                WN_CENTER   Центрирование по вертикали
                WN_CURMNU   Привязка к текущему меню
                >= 0        Само значение row
int    col;     Колонка левого верхнего угла окна меню
                WN_NATIVE   Как было указано при создании
                WN_CENTER   Центрирование по вертикали
                WN_CURMNU   Привязка к текущему меню
                >= 0        Само значение col

Выход:  YES     Меню выведено
        NO      Ошибка
**********************************************************************/

#include <slmenu.h>

bool  mndsplay (MENU *pmenu, int row, int col) {
    mnitmatr (pmenu, pmenu->pcur);
    return (wndsplay (pmenu->pwin, row, col));
}

