/**************************************************************************
    wndistxt - Создание и вывод окна с текстом

WINDOW *wndistxt (int row, int col, int hei, int wid, char **txt,
                  char *ttl, ATTRIB *patt);

int     row         Номер строки окна на экране
int     col         Номер колонки окна на экране
int     hei         Высота окна в строках
int     wid         Ширина окна в колонках
char    **txt       Указатель на массив строк (или NULL)
char    *ttl        Заголовок окна (или NULL)
ATTRIB  *patt       Указатель на структуру аттрибутов окна

Выход:  Указатель на структуру созданного окна
        NULL        Ошибка
***************************************************************************/

#include <slwind.h>

static  BORDER  bor = { BORD_SSSS | BORD_SHADOW, NULL, NULL, NULL };

WINDOW *wndistxt (int row, int col, int hei, int wid, char **txt,
                                                    char *ttl, ATTRIB *patt) {
    WINDOW        * pw;
    register int    i, j;
    int             tmplen = 0, tmphei = 0;
    CELLSC        * pdata;

    if (txt == NULL) return  NULL;
    for (i = 0; txt[i] != NULL; i++) {
        tmphei++;
        j = strlen (txt[i]);
        tmplen = max (tmplen, j);
    }
    if (wid == 0) wid = tmplen;
    if ((bor.pttitle = ttl) != NULL) {
        bor.type |= BORD_TCT;
        wid = max (wid, (int)strlen (ttl));
    }
    if (hei == 0) hei = tmphei;
    if (col + wid + 3 > NUM_COLS) wid = NUM_COLS - col - 3;
    if (row + hei + 3 > NUM_ROWS) hei = NUM_ROWS - row - 3;

    pw = wncreate (hei, wid, row, col, &bor, patt);    /*БП*/
    if (txt != NULL) {
        pdata = pw->img.pdata;
        for (i = 0; i < hei; i++, pdata += wid) {
            tmplen = strlen (txt[i]);
            tmplen = min (tmplen, wid);
            wnmovps (pdata, txt[i], tmplen);
        }
    }
    wndsplay (pw, row, col);                        /* БП */
    return (pw);
}

