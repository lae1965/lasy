#include    <alloc.h>
#include    <slmenu.h>

bool   mngetasc (char * ch, BORDER * bor, ATTRIB * attr) {
    MENU        * mnu;
    bool          ret = NO;
    byte        * p, * q;
    register      i, j;
    MNITEM      * pnew, * prev, * h = NULL;
    CELLSC      * pdata;
    int           col;

    mnu = mncreate(12,63,WN_CENTER,WN_CENTER,bor,attr); /* Åè */
    p = calloc (506, 1);       /* Åè */
    q = p;
    for (i = 1; i < 253; i++, q++)  *q++ = (byte) i;

    pdata = mnu->pwin->img.pdata + 1;
    pnew = utalloc (MNITEM);       /* Åè */
    h = prev = mnu->pitems = mnu->pcur = pnew;

    for (i = 0, q = p; i < 12; i++) {
        for (j = 0, col = 1; j < 21 ; j++, q += 2, pdata += 3, col += 3) {
            pnew->row = i;
            pnew->col = col;
            pnew->len = 1;
            pnew->bpos = pnew->vpos = -1;
            pnew->tag = (int) * q;
            pdata->ch = * q;
            if (* q == (byte) (* ch)) mnu->pcur = pnew;
            pnew->prev = prev; prev->next = pnew; prev = pnew;
            pnew = utalloc (MNITEM);     /* Åè */
        }
    }
    prev->next  = h;
    h->prev     = prev;
    mnu->type = MN_GRID;
    mndsplay (mnu, WN_CENTER, WN_CENTER);
    i = (int) mnreadit (mnu);
    if (i > 0) { * ch = (char) i; ret = YES; }
    free (p);
    mndstroy (mnu);
    return ret;
}

