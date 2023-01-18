/**********************************************************************
    mncrestd - Создает стандартное меню MENU

MENU *mncrestd (ITEM *array, int count, int typ, int row, int col,
                BORDER *pbor, ATTRIB *patt);

ITEM    *array      Массив элементов меню
int     count       Число элементов меню
int     typ         Тип меню
                    MN_HOR          Горизонтальное меню
                    MN_VERT         Вертикальное   меню
                    MN_GRID | n     Прямоугольное  меню
                                    n   Число эл. в строке
int     row         Строка  левого верхнего угла окна меню
int     col         Колонка левого верхнего угла окна меню
BORDER  *pbor       Указатель на структуру BORDER
ATTRIB  *patt       Указатель на структуру ATTRIB

Выход:   Указатель на созданную структуру (NULL)
*************************************************************************/

#include <alloc.h>
#include <slmenu.h>

MENU *mncrestd (ITEM *array, int count, int typ, int row, int col,
                                            BORDER *pbor, ATTRIB *patt) {
    MENU            *   p;
    register int        i, j;
    int                 pos, tmp, one, len;
    int                 hei = 0, wid = 0;

    for (i = 0; i < count; i++) {
        tmp = strlen (array[i].text);
        if (typ == MN_HOR)    wid += tmp;
        else wid = max (tmp, wid);
    }
    switch (typ) {
        case MN_HOR:    hei = 1;                            break;
        case MN_VERT:   hei = count;                        break;
        default:
            one = wid;
            wid *= typ;
            hei = count / typ + (count % typ == 0 ? 0 : 1);
            break;
    }
    p = mncreate (hei, wid, row, col, pbor, patt);

    switch (typ) {
        case MN_HOR:
            pos = 0;
            for (i = 0; i < count; i++) {
                len = strlen (array[i].text);
                if (len > 0)
                    if (!mnitmadd (p, i, 0, pos, &array[i])) goto ret;
                pos += len;
            }
            break;
        case MN_VERT:
            for (i = 0; i < count; i++) {
                if (strlen (array[i].text) > 0)
                    if (!mnitmadd (p, i, i, 0, &array[i])) goto ret;
            }
            break;

        default :                           /* MN_GRID */
            tmp = 0;
            for (i = 0; i < hei; i++) {
                pos = 0;
                for (j = 0; j < typ; j++) {
                    if (strlen (array[i].text) > 0)
                        if (!mnitmadd (p,tmp,i,pos,&array[tmp])) goto ret;
                    pos += one;
                    if (++tmp == count)  return (p);
                }
            }
            break;
    }
    p->type = typ;
    return (p);
ret:
    mndstroy (p);
    return NULL;
}

