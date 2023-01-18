/*************************************************************************
    mncreate - Создает структуру меню MENU

MENU *mncreate (int hei, int wid, int row, int col, BORDER *pbor, ATTRIB *patt);

int     height      Число строк в области данных
int     width       Число колонок в области данных
int     row         Строка  левого верхнего угла окна меню
int     col         Колонка левого верхнего угла окна меню
BORDER  *pbor       Указатель на структуру BORDER
ATTRIB  *patt       Указатель на структуру ATTRIB

Выход:   Указатель на созданную структуру (NULL)
*************************************************************************/

#include <slmenu.h>

MENU *mncreate (int hei,int wid,int row,int col, BORDER *pbor, ATTRIB *patt) {
    MENU *pm;

    pm = utalloc (MENU);  /*БП*/
    pm->pwin = wncreate (hei, wid, row, col, pbor, patt); /* БП */
    strcpy (pm->sign, MN_SIGN);
    pm->pcur   = NULL;
    pm->pitems = NULL;
    pm->type = MN_HOR;
    return (pm);
}

