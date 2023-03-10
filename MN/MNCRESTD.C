/**********************************************************************
    mncrestd - ??????? ?⠭???⭮? ???? MENU

MENU *mncrestd (ITEM *array, int count, int typ, int row, int col,
                BORDER *pbor, ATTRIB *patt);

ITEM    *array      ???ᨢ ??????⮢ ????
int     count       ??᫮ ??????⮢ ????
int     typ         ??? ????
                    MN_HOR          ??ਧ??⠫쭮? ????
                    MN_VERT         ???⨪??쭮?   ????
                    MN_GRID | n     ??אַ㣮?쭮?  ????
                                    n   ??᫮ ??. ? ??ப?
int     row         ??ப?  ?????? ???孥?? 㣫? ???? ????
int     col         ??????? ?????? ???孥?? 㣫? ???? ????
BORDER  *pbor       ?????⥫? ?? ????????? BORDER
ATTRIB  *patt       ?????⥫? ?? ????????? ATTRIB

??室:   ?????⥫? ?? ᮧ?????? ????????? (NULL)
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

