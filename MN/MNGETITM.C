#include <slmenu.h>

void    mngetitm (char  *buf, int buflen) {
    CELLSC *cel;
    int     len, vpos;

    *buf = EOS;
    if (Cur_Menu == NULL) return;
    len  = Cur_Menu->pcur->len;
    vpos = Cur_Menu->pcur->vpos;
    cel  = Cur_Menu->pwin->img.pdata +
            Cur_Menu->pwin->img.dim.w * Cur_Menu->pcur->row +
            Cur_Menu->pcur->col;
    len  = (vpos >= 0) ? min (len, vpos) : len;
    len  = min (len, buflen - 1);
    wnmovsp (buf, cel, len);
    buf[len] = EOS;
    utstrcvt (buf, ST_RLWHITE | ST_RTWHITE);
    return;
}

