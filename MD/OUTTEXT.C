#include    "makedoc.h"


void    outtext  (int row, int col, int visicol, int npno) {
    byte       txtatt  = pwind->attr.text;      /* Атрибут текста */
    byte       fldatt  = pwind->attr.nfld;      /* Атрибут неактивного поля */
    byte       afldatt  = pwind->attr.afld;     /* Атрибут активного поля */
    int        nrow = 0, len;
    int        H = pwind->img.dim.h;
    int        W = pwind->img.dim.w;
    char far * pbuf, * pscreen;
    char     * p;
    CELLSC   * pdata;
    register   i;

    pbuf    = (char far *) pwind->img.pdata;
    pscreen = wnviptrl (pwind->where_shown.row, pwind->where_shown.col);

    afldatt |= SL_MONOBLINK;
    if ((col -= visicol) > W) col = W;
    npno -= row;
    /*************  Вывод строк над текущим полем ********************/
    for (; nrow < row && nrow < H; nrow++, npno++) {
        pdata = pwind->img.pdata + nrow * W;
        p = potxt + pno [npno] + visicol;
        len = pno [npno + 1] - pno [npno] - 1 - visicol;
        if (len > W) len = W;
        for (i = 0; i < len; i++) {
            if ((byte) p[i] != 178)
                             { pdata[i].ch = p[i];  pdata[i].at = txtatt; }
            else             { pdata[i].ch = ' ';   pdata[i].at = fldatt; }
        }
        wnmovpca (pdata + i, ' ', W - i, txtatt);
    }

    /************* Вывод строки с текущим полем ********************/
    if (nrow == row && nrow < H) {
        pdata = pwind->img.pdata + nrow * W;
        p = potxt + pno [npno] + visicol;
        len = pno [npno + 1] - pno [npno] - 1 - visicol;
        utbound (len, 0, W);
        for (i = 0; i < col; i++) {
            if ((byte) p[i] != 178) {pdata[i].ch = p[i]; pdata[i].at = txtatt;}
            else { pdata[i].ch = ' '; pdata[i].at = fldatt; }
        }
        for (; (byte) p[i] == 178 && i < W; i++)
            { pdata[i].ch = p[i]; pdata[i].at = afldatt; }
        wnmovpsa (pdata + i, p + i,  len - i, txtatt);
        wnmovpca (pdata + len, ' ', W - len, txtatt);
        nrow++; npno++;
    }

    /*************  Вывод строк под текущим полем ********************/
    for (; nrow < H && npno <= lastpno; nrow++, npno++) {
        pdata = pwind->img.pdata + nrow * W;
        p = potxt + pno [npno] + visicol;
        len = pno [npno + 1] - pno [npno] - 1 - visicol;
        utbound (len, 0, W);
        wnmovpsa (pdata, p,  len, txtatt);
        wnmovpca (pdata + len, ' ', W - len, txtatt);
    }
    wnvicopy (&pbuf, &pscreen, H, W, NUM_COLS * 2, 0, Cmd[1]);
    return;
}

