#include    <dos.h>
#include    <sledit.h>
#include    <alloc.h>

void outlin (EDT *pe) {
    CELLSC      *pdata;
    int         W = Ed_pwin->img.dim.w;
    int         tattr = Ed_pwin->attr.text, attr;
    int         blkattr = Ed_pwin->attr.blck;
    int         eofattr = Ed_pwin->attr.high;
    long        blkstart, blkend;
    char far  * pbuf,   * pscreen;
    register    r1 = pe->row;

    pdata  = Ed_pwin->img.pdata + r1 * W;

    pbuf    = (char far *) pdata;
    pscreen = wnviptrl (Ed_pwin->where_shown.row + r1, Ed_pwin->where_shown.col);

    if (pe->lf > 0) {
        attr = (pe->lf == 1) ? eofattr : tattr;
        wnmovpca (pdata, ' ', W, attr);
        if (pe->lf == 1) wnmovps (pdata, ">> EOF <<", 9);
        goto ret;
	}
    attr = tattr;
    if (pe->block) {
        blkstart = (pe->cur.nline >= 0L) ?
                         pe->cur.start  - (pe->line - pe->row) :
                         pe->cur.start + pe->cur.nline - (pe->line - pe->row);
        blkend =  blkstart + ((pe->cur.nline >= 0L) ? pe->cur.nline :
                                                              -pe->cur.nline);
        if ((long) r1 >= blkstart && (long) r1 <= blkend) attr = blkattr;
    }
    outcur ((char far *)pdata, (int) attr, pe->visicol, W);
ret:
    wnvicopy (&pbuf, &pscreen, 1, W, NUM_COLS*2, 0, Cmd[1]);
    return;
}

void outscr (EDT *pe) {
    char          * p;
    CELLSC        * pdata;
    int             H = Ed_pwin->img.dim.h;
    int             W = Ed_pwin->img.dim.w;
    int             tattr = Ed_pwin->attr.text, n, attr;
    int             blkattr = Ed_pwin->attr.blck, m = 0;
    int             eofattr = Ed_pwin->attr.high;
    register int    j, i;
    long            blkstart, blkend;
    int             llen, rlen;
    bool            curstr = YES,  eofstr = NO;
    char far      * pbuf,   * pscreen;


    pbuf    = (char far *) Ed_pwin->img.pdata;
    pscreen = wnviptrl (Ed_pwin->where_shown.row, Ed_pwin->where_shown.col);

    if (pe->block) {
        blkstart = (pe->cur.nline >= 0L) ?
                         pe->cur.start  - (pe->line - pe->row) :
                         pe->cur.start + pe->cur.nline - (pe->line - pe->row);
        blkend =  blkstart + ((pe->cur.nline >= 0L) ? pe->cur.nline :
                                                              -pe->cur.nline);
    }
    if (pe->lf) {
        i = pe->row - pe->lf + 1;
        if (i < 0)   goto clear;
        if (i == 0)  { eofstr = YES; goto outeof; }
        curstr = NO;
        j = -1;
        eofstr = YES;
        goto outleft;
    }
    i = pe->row;
    j = (int) (Ed_pnr - Ed_pnrlast);
    if (j < H - i - 1)  eofstr = YES;
    else                j = H - i - 1;

		/************   От первой строки до текущей     **************/
outleft:
    for (; i > 0; i--, m++) {
        pdata  = Ed_pwin->img.pdata + W * m;
        llen = pnllen (i);
        if (pe->block && (long)m >= blkstart && (long)m <= blkend) attr = blkattr;
        else                                                       attr = tattr;
        if (pe->visicol < llen) {
            p = Ed_pl + Ed_pnl[i] + pe->visicol;
            n = llen - pe->visicol;
            if (n > W) n = W;
        } else n = 0;
        outpim ((char far *) pdata, p, (int) attr, n, W);
    }

		/************   Текущая  строка     **************/
    if (!curstr) goto outeof;
    pdata  = Ed_pwin->img.pdata + W * m;
    if (pe->block && (long)m >= blkstart && (long)m <= blkend) attr = blkattr;
    else                                                       attr = tattr;
    m++;
    outcur ((char far *)pdata, (int) attr, pe->visicol, W);

		/************   От текущей  строки и ниже    **************/
    for (i = 1; i <= j; i++, m++) {
        pdata  = Ed_pwin->img.pdata + W * m;
        rlen = pnrlen (i);
        if (pe->block && (long)m >= blkstart && (long)m <= blkend) attr = blkattr;
        else                                                       attr = tattr;
        if (pe->visicol < rlen) {
            p = Ed_pr - *(Ed_pnr - i) + pe->visicol;
            n = rlen - pe->visicol;
            if (n > W) n = W;
        } else n = 0;
        outpim ((char far *) pdata, p, (int) attr, n, W);
    }

        /****************  Выставляем   >>EOF<<   *********************/
outeof:
    if (!eofstr) goto clear;
    pdata  = Ed_pwin->img.pdata + W * m;
    m++;
    wnmovpsa (pdata, ">> EOF <<", 9, eofattr);
    wnmovpca (pdata + 9, ' ', W - 9, eofattr);

       /*******  Очистка экрана за     >>EOF<<    *********/
clear:
    pdata  = Ed_pwin->img.pdata + W * m;
    wnmovpca (pdata, ' ', (H - m) * W, tattr);
loop:
    wnvicopy (&pbuf, &pscreen, H, W, NUM_COLS*2, 0, Cmd[1]);
}

void paintlin (EDT *pe) {
    register    attr = Ed_pwin->attr.text, r1 = pe->row;
    long        blkstart, blkend;
    char far   * pscreen;

    if (pe->lf > 0 || !pe->block) return;

    pscreen = wnviptrl (Ed_pwin->where_shown.row + r1, Ed_pwin->where_shown.col);

    blkstart = (pe->cur.nline >= 0L) ?
                        pe->cur.start  - (pe->line - pe->row) :
                        pe->cur.start + pe->cur.nline - (pe->line - pe->row);
    blkend =  blkstart + ((pe->cur.nline >= 0L) ? pe->cur.nline :
                                                            -pe->cur.nline);
    if ((long)r1 >= blkstart && (long)r1 <= blkend) attr = Ed_pwin->attr.blck;
    wnvicopy (NULL,&pscreen, 1, Ed_pwin->img.dim.w, NUM_COLS*2, attr, Cmd[4]);
    return;
}

void viroll (int dir) {
    union REGS  inregs,outregs;
    char  far * pfrom, *pto;
    int         H = Ed_pwin->img.dim.h;
    int         W = Ed_pwin->img.dim.w;
    int         com = 1;

    if (H == 1) return;
	if (n_w_envr.n_v_page == n_w_envr.n_a_page ) {
		inregs.h.ah = (unsigned char) (dir == WN_SCR_UP ? 6 : 7);
        inregs.h.al = (unsigned char) 1;
        inregs.h.bh = (unsigned char) Ed_pwin->attr.text;
        inregs.h.ch = (unsigned char) Ed_pwin->where_shown.row;
        inregs.h.cl = (unsigned char) Ed_pwin->where_shown.col;
        inregs.h.dh = (unsigned char) H - 1 + Ed_pwin->where_shown.row;
        inregs.h.dl = (unsigned char) W - 1 + Ed_pwin->where_shown.col;
        int86 (16,&inregs,&outregs);
    }
    else {
        if (dir == WN_SCR_DOWN) {
            com = 9;
            pfrom = wnviptrl (Ed_pwin->where_shown.row, Ed_pwin->where_shown.col);
            pto = wnviptrl (1 + Ed_pwin->where_shown.row, Ed_pwin->where_shown.col);
        }
        else {
            com = 7;
            pfrom = wnviptrl(1 + Ed_pwin->where_shown.row, Ed_pwin->where_shown.col);
            pto = wnviptrl (Ed_pwin->where_shown.row, Ed_pwin->where_shown.col);
        }
        wnvicopy (&pfrom, &pto, H - 1, W, NUM_COLS*2, 0, Cmd[com]);
    }
}

