#include <slwind.h>

#define BORD_LEFT      0x0001
#define BORD_RIGHT     0x0004
#define BORD_TOP       0x0002
#define BORD_BOTTOM    0x0008

#define H (int)(pwin->img.dim.h)
#define W (int)(pwin->img.dim.w)

bool wnsplitw (WINDOW *pwin, int pos, int option) {
    static char js[2][8] = {
        /*   Left        Top        Right       Bottom      */
        { 0xC3, 0xC7, 0xC2, 0xD1, 0xB4, 0xB6, 0xC1, 0xCF },
        { 0xC6, 0xCC, 0xD2, 0xCB, 0xB5, 0xB9, 0xD0, 0xCA }};

    static char cr[2][4] = {
        /*   hsplit     vsplit          */
        { 0xC5, 0xD7, 0xC5, 0xD8 },
        { 0xD8, 0xCE, 0xD7, 0xCE }};

    static char lc[2][2] = {
        { 0xB3, 0xC4 },
        { 0xBA, 0xCD }};

    int bt = (pwin->bord.type & BORD_TYPE) - 1;
    int i = option & WN_DOUBLE;
    int j1, j2, k;
    CELLSC *p;

    if (option & WN_VSPLIT) {
        if (pos < 0 || pos > W - 1) return (NO);
        j1 = 2 + ((bt & BORD_TOP)    != 0);
        j2 = 6 + ((bt & BORD_BOTTOM) != 0);
        wnaddjoi (pwin,     0, pos + 1, js[i][j1]);
        wnaddjoi (pwin, H + 1, pos + 1, js[i][j2]);
        for (p = pwin->img.pdata + pos, k = 0; k < H;
             p->at = pwin->attr.bord, p += W, k++) {
                if (p->ch == lc[0][1])      p->ch = cr[i][2];
                else if (p->ch == lc[1][1]) p->ch = cr[i][3];
                else                        p->ch = lc[i][0];
        }
    } else {
        if (pos < 0 || pos > H - 1) return (NO);
        j1 = 0 + ((bt & BORD_LEFT)  != 0);
        j2 = 4 + ((bt & BORD_RIGHT) != 0);
        wnaddjoi (pwin, pos + 1,     0, js[i][j1]);
        wnaddjoi (pwin, pos + 1, W + 1, js[i][j2]);
        for (p = pwin->img.pdata + pos*W, k = 0; k < W;
                                       p->at = pwin->attr.bord, p++, k++) {
                if (p->ch == lc[0][0])        p->ch = cr[i][0];
                else if (p->ch == lc[1][0])   p->ch = cr[i][1];
                else                          p->ch = lc[i][1];
        }
	}
    pwin->options.dirty = 1;
    wnupdate (pwin);
    return (YES);
}

