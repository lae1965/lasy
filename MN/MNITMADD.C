/*********************************************************************
      mnitmadd - Добавляет элемент к меню

bool  mnitmadd (MENU *pmenu, int tag, int row, int col, ITEM *pitm);

MENU    *pmenu      Указатель на структуру меню MENU
int     tag         Номер элемента меню (уникальный)
int     row         Номер строки элемента в окне меню
int     col         Номер колонки элемента в окне меню
ITEM    *pitm       Указатель на структуру элемента ITEM

Выход:   YES        Элемент добавлен к меню
         NO         Ошибка
**********************************************************************/

#include <slmenu.h>

static int      trans_item (MNITEM *pmitem, ITEM *pitem);
static int      find_bold  (char *text, char *keys);

bool  mnitmadd (MENU *pmenu, int tag, int row, int col, ITEM *pitm) {

    MNITEM * pnew, * q = pmenu->pitems, * h = pmenu->pitems;
    int      len, W = pmenu->pwin->img.dim.w;
    int      typrow, typ = 0;
    CELLSC * pdata;

    pnew = utalloc (MNITEM); /* БП */
    pnew->tag = tag;
    pnew->row = row;
    pnew->col = col;
    len = strlen (pitm->text);
    pnew->len = (len + col > W) ? W - col : len;

    pdata = pmenu->pwin->img.pdata + row * W + col;
        /* Check for duplicate keys */

    pnew->keys = pitm->keys;
    if (pitm->keys != NULL) pnew->bpos = find_bold (pitm->text, pitm->keys);
    else                    pnew->bpos = -1;

        /* Construct item's var part description */

    pnew->vpos = trans_item (pnew, pitm);

        /* Build item into linked item's list */

    if (h == NULL) { pnew->next = pnew->prev = pmenu->pitems = pnew; }
    else if (row < h->row || row == h->row && col < h->col) {
        if (row == h->row && col + len > h->col) goto ret;
        pnew->prev = h->prev; pnew->next = h;
        h->prev->next = pnew; h->prev = pnew;
        pmenu->pitems = pnew;
    }  else {
        q = h->next;
        while (q != h && q->row < row) q = q->next;
        while (q != h && q->row == row && q->col < col) q = q->next;
        if ((q->prev->row == row && q->prev->col + q->prev->len > col) ||
            (q != h && q->row == row && col + len > q->col)) goto ret;
        pnew->prev = q->prev; pnew->next = q;
        q->prev->next = pnew; q->prev = pnew;
    }
    q = h = pmenu->pitems;
    if (pmenu->type != MN_GRID) {
        do {
            typrow = q->row;
            q = q->next;
            if (q->row != typrow) typ |= 1;
            else                  typ |= 2;
        } while (q != h );

        switch (typ) {
            case 1:     pmenu->type = MN_VERT;                break;
            case 2:     pmenu->type = MN_HOR;                 break;
            default :   pmenu->type = MN_GRID;                break;
        }
    }
        /* Display new item */

    wnmovps  (pdata, pitm->text, pnew->len);

    if (pnew->bpos >= 0) wnmovpa (pdata + pnew->bpos, 1, pmenu->pwin->attr.bold);
    if (pnew->vpos >= 0) {
        wnmovpca (pdata + pnew->vpos, ' ', pnew->vlen, pmenu->pwin->attr.nice);
        len = min (strlen (pnew->vloc), pnew->vlen);
        wnmovps (pdata + pnew->vpos, pnew->vloc, len);
    }

        /* Make item's list head returned by ADD_ITEM current */

    pmenu->pcur = pmenu->pitems;
    return (YES);
ret:
    free (pnew);
    return (NO);
}

static int find_bold (char *text, char *keys) {

    int i;

    while (*keys != '\0') {
        for (i = 0; text[i] != '\0'; i++)
            if (text[i] == *keys) return (i);
        keys++;
    }
    return (-1);
}

static int trans_item (MNITEM *pmitem, ITEM *pitem) {

    char *p;
    int vpos = 0, vlen = 0;

    if (pitem->vars == NULL)  return (-1);
    p = pitem->text;
    while (*p != MN_VAR_CHAR && *p != '\0') { vpos++; p++; }

    if (vpos >= pmitem->len) return (-1);

    while (*p == MN_VAR_CHAR) { vlen++; p++; }

    if (vlen == 0) return (-1);

    if (vpos + vlen > (int) pmitem->len) vlen = pmitem->len - vpos;
    pmitem->vlen = vlen;

    pmitem->vloc = pitem->vars;
    return (vpos);
}

