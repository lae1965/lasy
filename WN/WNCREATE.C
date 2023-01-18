/************************************************************************
    wncreate - Создает структуру окна WINDOW

WINDOW *wncreate (int height, int width, int row, int col,
                                              BORDER *pbor, ATTRIB *patt);

int     height      Число строк в области данных
int     width       Число колонок в области данных
int     row         Строка  левого верхнего угла окна
int     col         Колонка левого верхнего угла окна
BORDER  *pbor       Указатель на структуру BORDER
ATTRIB  *patt       Указатель на структуру ATTRIB

Выход:   *presult   Указатель на созданную структуру или NULL
**************************************************************************/

#include <slwind.h>

static BORDER Null_Bord = { BORD_NO_BORDER, NULL, NULL, NULL };

WINDOW * wncreate (int height, int width, int row, int col,
                                                BORDER *pbor, ATTRIB *patt) {
    WINDOW      *pwin;
    register     area;

    pwin = utalloc (WINDOW);                /* БЕЗ ПРОВЕРКИ */
    pwin->img.dim.h = (byte) height;
    pwin->img.dim.w = (byte) width;
    pwin->where_shown.row = (byte) row;
    pwin->where_shown.col = (byte) col;

    area = (height + 3) * (width + 4);
    pwin->prev.pdata = (CELLSC*) calloc ((word)area,sizeof(CELLSC)); /* БП */

    area = height * width;
    pwin->img.pdata = (CELLSC*) calloc ((word)area,sizeof(CELLSC)); /* БП */

    wnmovpca (pwin->img.pdata, ' ', area, (byte) patt->text);
    pwin->bord = (pbor == NULL) ? Null_Bord : *pbor;
    pwin->attr = *patt;

    pwin->cur_typ.high     =
    pwin->cur_typ.low      = (byte)N_V_STATE.byte_char - 2;
    pwin->cur_typ.low++;   /* Default : two lowerst scan lines */

    strcpy (pwin->sign, WN_SIGN);

    pwin->options.removable = 1;
    return (pwin);
}

