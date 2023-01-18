#include    <sledit.h>

#define     RBUF        wblk.b.rbuf
#define     LBUF        wblk.b.lbuf

typedef union   {
    struct {
        unsigned    rfile     : 1;
        unsigned    rbuf      : 1;
        unsigned    lbuf      : 1;
        unsigned    lfile     : 1;
        unsigned    dummy     : 12;
    } b;
    int blk;
} WBLK;

static int      savlf;
static word     sizerb, sizelb;
static long     sizerf, sizelf;
static long     startlb, startrb;
static long     nlinelb, nlinerb;


void kcf10 (EDT *pe, BLK *blk, bool move, bool interwin) {

    int     H = Ed_pwin->img.dim.h;
    int     tmps, i, rlen;
    int     savnbank, numbank, rightoff, cur_roff = 0;
    long    offset;
    char   *p, *q;
    word   *s, *f;
    bool    inblk = NO;
    WBLK    wblk;

    if (!pe->block) return;

    WORKING;   
    *(Ed_pnrlast - 1) = 0;
    *Ed_pnr = *(Ed_pnr - 1) + pnllen (0) + ((Ed_pnr == Ed_pnrlast) ? 0 : 1);
    if (pe->blkmark)  kf7 (pe);
    if (interwin) {
        RBUF = LBUF = 0;          sizelb = sizerb = 0;
        sizelf = pe->cur.lsize;   sizerf = pe->cur.rsize;
    } else {
        if (pe->line >= blk->start && pe->line <= blk->start + blk->nline) {
            MOVLEFT ((long) (pe->col + pe->visicol - pe->blank) - 1L);
            inblk = YES;
        }
        wblk.blk = foundblk (pe, blk);
    }

    if (blk->loff > -1L) {
        if (!LBUF) {
            tmps = BUF_WRK;
            offset = blk->loff + sizelf;
            while (1) {
                if (offset + tmps > pe->leftoff)
                    tmps = (int) (pe->leftoff - offset);
                if (offset >= pe->leftoff) break;
                utlseek (pe->handleft, offset, UT_BEG);
                tmps = utread (pe->handleft, Ed_pw, tmps);
                utlseek (pe->handleft, blk->loff, UT_BEG);
                utwrite (pe->handleft, Ed_pw, tmps);
                offset += tmps;
                blk->loff += tmps;
            }
        }
        pe->leftoff -= sizelf;
        utchsize (pe->handleft, pe->leftoff);
        utlseek (pe->handleft, pe->leftoff, UT_BEG);
    }

    if (LBUF) {
        p = Ed_pl + Ed_pnl[(word) (startlb + nlinelb)];
        q = Ed_pl + Ed_pnl[(word) startlb] + pnllen ((word) startlb) + 1;
        i = (int) (Ed_pleft - q) + 1;
        Ed_pleft -= sizelb;
        utmovmem ((char far *) p, (char far *) q, (word) i);
        f = &Ed_pnl[(word) startlb];
        i = (int) ((char *) f - (char  *) Ed_pnl);
        s = &Ed_pnl[(word) nlinelb + 1];
        utmovmem ((char far *) s, (char far *) Ed_pnl, (word) i);
        i = (int) (f - Ed_pnl);
        Ed_pnl = s;
        for (i--; i >= 0; i--) Ed_pnl[i] -= sizelb;
        if (Ed_pnl > Ed_pnllast) Ed_pnl = Ed_pnllast;
    }

    if (RBUF) {
        p = Ed_pr - *(Ed_pnr - (word) startrb) + sizerb - 1;
        q = Ed_pr - *(Ed_pnr - (word) startrb) - 1;
        i = (int) (q - Ed_pright) + 1;
        utmovabs ((char far *) p, (char far *) q, (word) i, 1);
        Ed_pright += sizerb;
        f = Ed_pnr - (word) startrb + 1;
        s = Ed_pnr - (word) (startrb + nlinerb);
        i = (int) (Ed_pnr - f) + 1;
        utmovabs ((char far *) s, (char far *) f, (word) i * 2, 0);
        Ed_pnr -= (word) nlinerb + 1;
        for (i--; i >= 0; i--) *(Ed_pnr - i) -= sizerb;
    }

    if (blk->nbank > -1) {
        savnbank = pe->nbank;    pe->nbank = blk->nbank;
        offset = (long) (savnbank - blk->nbank) * (long) BUF_WRK +
                                        (long) blk->roff - (long) pe->rightoff;
        if (sizerf < offset) {
            rightoff = pe->rightoff;
            offset = sizerf;
            pe->rightoff = blk->roff + 1;
            numbank = blk->nbank;
            if (sizerf > blk->roff) {
                offset -= pe->rightoff;
                numbank += (int) (offset / BUF_WRK) + 1;
                offset = BUF_WRK - (offset % BUF_WRK);
            } else   offset = (long) blk->roff + 1L - sizerf;
            for (; numbank <= savnbank; numbank++) {
                if (numbank == savnbank) cur_roff = rightoff;
                offset -= cur_roff;
                if (move && numbank == pe->cur.nbank) p = Ed_pw +pe->cur.roff;
                else p = NULL;
                utlseek (pe->handright, (long)numbank * (long)BUF_WRK, UT_BEG);
                utread (pe->handright, Ed_pw, BUF_WRK);
                wrtort (pe, Ed_pw + cur_roff, p, (word) offset);
                offset = (long) BUF_WRK;
            }
        } else  {
            if (blk->nbank == 0 && blk->roff == BUF_WRK - 1) {
                pe->rightoff = 0;
                pe->nbank = -1;
                if (interwin)
                    { pe->lf = 1; pe->blank = pe->col + pe->visicol; }
            } else  pe->rightoff = blk->roff + 1;
        }
        utchsize (pe->handright, (long) (pe->nbank + 1) * (long) BUF_WRK);
    }
    if (!interwin) {
        if (Ed_pnr - Ed_pnrlast < (H << 1))  loadright (pe);
        if (Ed_pnllast - Ed_pnl < (H << 1))  loadleft (pe);
    }
    pe->cur.loff = -1L;
    if (pe->line > blk->start) {
        if (blk->start + blk->nline < pe->line) pe->line -= blk->nline + 1L;
        else                              pe->line -= pe->line - blk->start;
    }
    if (pe->line < (long) pe->row) pe->row = (int) pe->line;
    if (!move) {
        if (inblk && !interwin) {
            rlen = pnllen (0);
            if (pe->col + pe->visicol <= rlen) pe->blank = 0;
            else  pe->blank = pe->col + pe->visicol - rlen;
            if (Ed_pnrlast > Ed_pnr && pe->lf == 0) pe->lf = 1;
            if (pe->lf == 0)
                MOVRIGHT ((long) (pe->col + pe->visicol - pe->blank) - 1L);
        }
        pe->total -= blk->nline + 1;
        pe->block = NO; blk->roff = blk->nbank = -1;
        if (!interwin) pe->needout |= NEED_OUTSCR;
    }
    pe->changed = YES;
    return;
}

int kf9 (EDT *pef, EDT *pet, bool move) {

    register int  j;
    long     blksize, offset, i;
    int      endoff = 0, nbank = pef->cur.nbank, rblkoff = pef->cur.roff;
    char    *p;
    word     tmps;
    int      llen, rlen;
    WBLK     wblk;

    if (!pef->block) return 0;

    WORKING;   
    *(Ed_pnrlast - 1) = 0;
    *Ed_pnr = *(Ed_pnr - 1) + pnllen (0) + ((Ed_pnr == Ed_pnrlast) ? 0 : 1);
    if (pet != pef && pet->block) { pet->cur.nbank = -1; pet->cur.loff = -1L; }
    if (pef->blkmark) kf7 (pef);
    savlf = pet->lf;
    if (pet->lf > 0) {
        pet->total += pet->lf - 1;
        for (Ed_pleft--; pet->lf > 0; pet->lf--) {
            *++Ed_pleft = '\n';
            *Ed_pnl = (word) (Ed_pleft - Ed_pl) + 1;
            Ed_pnl--;
        }
        Ed_pnl++;
    } else  {
        movleft (pet, &pet->line,(long)(pet->col+pet->visicol-pet->blank)-1L);
        *Ed_pnr = (word) (Ed_pr - Ed_pright);
    }
    if (pef == pet) wblk.blk = foundblk (pef, &pef->cur);
    else {
        RBUF = LBUF = 0;          sizelb = sizerb = 0;
        sizelf = pef->cur.lsize;  sizerf = pef->cur.rsize;
        pet->block = YES;         pet->cur.nline = pef->cur.nline;
    }
    blksize = sizerf + (long) sizerb + (long) sizelb + sizelf +
                                                    (pef->cur.nline + 1) * 2;
    if (blksize >= (long)(Ed_pright - (char *) Ed_pnr) - Ed_rsize / 10) {
        swapright (pet, YES);
        pef->old.rsize = pef->cur.rsize;
        if (RBUF) wblk.b.rfile = 1;
        wblk.b.rbuf = 0;
        sizerf += sizerb;
        nbank = pef->cur.nbank;     rblkoff = pef->cur.roff + 1;
        pet->cur.start = pet->line;  pet->cur.nbank = pet->nbank;
        if (pet->rightoff > 0) pet->cur.roff = pet->rightoff - 1;
        else { pet->cur.nbank++; pet->cur.roff = BUF_WRK - 1; }

        if (nbank > -1L) {
            while (sizerf > 0L) {
                offset = (long) nbank * (long) BUF_WRK;
                if (sizerf <= (long) rblkoff)  endoff = rblkoff - (int) sizerf;
                utlseek (pef->handright, offset, UT_BEG);
                utread (pef->handright, Ed_pw, BUF_WRK);
                tmps = (word) (rblkoff - endoff);
                wrtort (pet, Ed_pw + endoff, NULL, tmps);
                nbank++;
                sizerf -= tmps;
                rblkoff = BUF_WRK;
            }
        }

        if (LBUF) {
            p = Ed_pl + Ed_pnl[(word) (startlb + nlinelb)];
            wrtort (pet, p, NULL, sizelb);
        }

        if (pef->cur.loff > -1L) {
            tmps = BUF_WRK;
            utlseek (pef->handleft, pef->cur.loff + sizelf, UT_BEG);
            while (sizelf > 0L) {
                if (sizelf <= (long) tmps)  tmps = (word) sizelf;
                utlseek (pef->handleft, - (long) tmps, UT_CUR);
                tmps = utread (pef->handleft, Ed_pw, tmps);
                utlseek (pef->handleft, - (long) tmps, UT_CUR);
                wrtort (pet, Ed_pw, NULL, tmps);
                sizelf -= tmps;
            }
            utlseek (pef->handleft, pef->leftoff, UT_BEG);
        }
        loadright (pet);
        goto ret;
    }

    nbank = pef->cur.nbank;  rblkoff = pef->cur.roff;
    pet->cur.start = pet->line;  pet->cur.nbank = -1;

    if (nbank > -1L) {
        sizerf--; *(--Ed_pright) = '\n';
        while (sizerf > 0L) {
            offset = (long) nbank * (long) BUF_WRK;
            if (sizerf <= (long) rblkoff)  endoff = rblkoff - (int) sizerf;
            utlseek (pef->handright, offset, UT_BEG);
            utread (pef->handright, Ed_pw, BUF_WRK);
            for (j = rblkoff - 1; j >= endoff; j--) {
                if (Ed_pw[j] == '\n') {
                    Ed_pnr++;
                    *Ed_pnr = (word) (Ed_pr - Ed_pright);
                    startrb++;
                }
                *(--Ed_pright) = Ed_pw[j];
            }
            nbank++;
            sizerf -= rblkoff - endoff;
            rblkoff = BUF_WRK;
        }
        Ed_pnr++;
        *Ed_pnr = (word) (Ed_pr - Ed_pright);
        startrb++;
    }

    if (RBUF) {
        i = startrb + nlinerb;
        p = Ed_pr - *(Ed_pnr - (word) i) + pnrlen ((word) i);
        for (; nlinerb >= 0L; nlinerb--) {
            rlen = pnrlen ((word) i);
            for (j = 0; j <= rlen; j++)  *(--Ed_pright) = *p--;
            Ed_pnr++;
            *Ed_pnr = (word) (Ed_pr - Ed_pright);
        }
    }

    if (LBUF) {
        p = Ed_pl + Ed_pnl[(word) startlb] +pnllen ((word) startlb);
        for (; nlinelb >= 0L; nlinelb--, startlb++) {
            llen = pnllen ((word) startlb);
            for (j = 0; j <= llen; j++)  *(--Ed_pright) = *p--;
            Ed_pnr++;
            *Ed_pnr = (word) (Ed_pr - Ed_pright);
        }
    }

    if (pef->cur.loff > -1L) {
        tmps = BUF_WRK;
        *(--Ed_pright) = '\n';
        sizelf--;
        utlseek (pef->handleft, pef->cur.loff + sizelf, UT_BEG);
        while (sizelf > 0L) {
            if (sizelf <= (long) tmps)  tmps = (word) sizelf;
            utlseek (pef->handleft, - (long) tmps, UT_CUR);
            tmps = utread (pef->handleft, Ed_pw, tmps);
            utlseek (pef->handleft, - (long) tmps, UT_CUR);
            for (j = tmps - 1; j >= 0; j--) {
                if (Ed_pw[j] == '\n') {
                    Ed_pnr++;
                    *Ed_pnr = (word) (Ed_pr - Ed_pright);
                }
                *(--Ed_pright) = Ed_pw[j];
            }
            sizelf -= tmps;
        }
        Ed_pnr++;
        *Ed_pnr = (word) (Ed_pr - Ed_pright);
        utlseek (pef->handleft, pef->leftoff, UT_BEG);
    }

ret:
    if (!move) {
        llen = pnllen (0);
        if (pet->col + pet->visicol <= llen) pet->blank = 0;
        else  pet->blank = pet->col + pet->visicol - llen;
        movright(pet,&pet->line,(long)(pet->col+pet->visicol-pet->blank) - 1L);
        pet->total += pet->cur.nline + 1L;
        pet->cur.loff = -1L; pet->needout |= NEED_OUTSCR;
    }
    pet->changed = YES;
    return wblk.blk;
}

void kf10 (EDT *pe) {
    int      H = Ed_pwin->img.dim.h;
    int      rlen;
    long     i;
    WBLK     wblk;

    if (!pe->block) return;

    WORKING;   
    *(Ed_pnrlast - 1) = 0;
    *Ed_pnr = *(Ed_pnr - 1) + pnllen (0) + ((Ed_pnr == Ed_pnrlast) ? 0 : 1);
    if (pe->blkmark) kf7 (pe);
    if (pe->line == pe->cur.start) return;
    if (pe->line > pe->cur.start && pe->line <= pe->cur.start +pe->cur.nline){
        if (pe->cur.loff > -1L) {
            kchome (pe, NO);
            pe->line = pe->cur.start;
            loadleft (pe);
            if (pe->line <= H - 1) pe->row = (int) pe->line;
        } else {
            foundblk (pe, &pe->cur);
            i = (long) (Ed_pleft - (Ed_pl + Ed_pnl[(word) nlinelb + 1]));
            MOVLEFT (i);
            if (pe->line - nlinelb <= (long) (H - 1)) {
                pe->row -= (int) nlinelb + 1;
                if (pe->row < 0) pe->row = 0;
            }
        }
    } else {
        utmovmem ((char far *) &pe->old, (char far *) &pe->cur, sizeof (BLK));
        wblk.blk = kf9 (pe, pe, YES);
        if (RBUF || pe->old.nbank > -1L)
            pe->old.start += pe->old.nline + 1L + (long) savlf;
        kcf10 (pe, &pe->old, YES, NO);
        pe->cur.start = pe->line;
    }
    rlen = pnllen (0);
    if (pe->col + pe->visicol <= rlen) pe->blank = 0;
    else  pe->blank = pe->col + pe->visicol - rlen;
    MOVRIGHT ((long) (pe->col + pe->visicol - pe->blank) - 1L);
    pe->needout |= NEED_OUTSCR;
    return;
}

int  foundblk (EDT *pe, BLK *blk) {

    long     i, j;
    long     npnr = Ed_pnr - Ed_pnrlast + 1L;
    long     npnl = Ed_pnllast - Ed_pnl + 1L;
    WBLK     wblk;

    *(Ed_pnrlast - 1) = 0;
    *Ed_pnr = *(Ed_pnr - 1) + pnllen (0) + ((Ed_pnr == Ed_pnrlast) ? 0 : 1);
    i = blk->start - (pe->line - pe->lf);   /*Ск. строк от line до нач. блока*/
    j = blk->start + blk->nline - (pe->line - pe->lf);/*От line до кон. блока*/
    wblk.blk = 0;
    if (i >= 0L && pe->lf == 0)
        { if (i < npnr)  wblk.b.rbuf = 1; }
    else {
        if (j < 0L || pe->lf > 0) {
            j = -j;
            if (j <= npnl) wblk.b.lbuf = 1;
        } else { wblk.b.rbuf = wblk.b.lbuf = 1; }
        i = -i;
    }
    if (blk->nbank > -1L) wblk.b.rfile = 1;
    if (blk->loff  > -1L) wblk.b.lfile = 1;
    sizerf = sizelf = 0L;
    sizerb = sizelb = 0;

    if (wblk.b.rfile) {
        if (!RBUF) sizerf = blk->rsize;
        else sizerf = (long) (pe->nbank - blk->nbank) * (long) BUF_WRK  +
                (long) blk->roff + 1L - (long) pe->rightoff;
    }

    if (RBUF) {
        if (!LBUF) {
            startrb = i;
            if (!wblk.b.rfile) nlinerb = blk->nline;
            else nlinerb = npnr - i - 1L;
        }  else {
            startrb = 0L;
            if (!wblk.b.rfile) nlinerb = j;
            else nlinerb = npnr - 1L;
        }
        sizerb = (word) (*(Ed_pnr - (word) startrb) -
                            *(Ed_pnr - (word) (startrb + nlinerb)) +
                                   pnrlen ((word) (startrb + nlinerb))) +1;
    }

    if (LBUF) {
        if (!RBUF) {
            if (pe->lf > 0) j++;
            startlb = j;
            if (!wblk.b.lfile) nlinelb = blk->nline;
            else nlinelb = npnl - j - 1L;
        } else {
            startlb = 1L;
            if (!wblk.b.lfile) nlinelb = i - 1L;
            else nlinelb = npnl - 2L;
        }
        sizelb = (word) (Ed_pnl[(word) startlb] + pnllen ((word) startlb) -
                                        Ed_pnl[(word) (startlb + nlinelb)] + 1);
    }

    if (wblk.b.lfile) {
        if (!LBUF) sizelf = blk->lsize;
        else sizelf = pe->leftoff - blk->loff;
    }
    return  wblk.blk;
}

void kf7 (EDT *pe){

    if (pe->lf > 0 && !pe->blkmark) return;
    pe->blkmark = !pe->blkmark;
    if (pe->blkmark) {
        if (pe->block) {
            pe->block = NO;
            pe->cur.loff = -1L; pe->needout |= NEED_OUTSCR;
        }
        pe->cur.nbank = -1;
        pe->cur.nline = 0L;
        pe->cur.start = pe->line;
        pe->block = YES;
        paintlin (pe);
    } else {
        if (pe->cur.start >= pe->line) {
            pe->cur.nline = pe->cur.start - pe->line;
            pe->cur.start = pe->line;
        } else  pe->cur.nline = pe->line - pe->cur.start - pe->lf;
    }
    return;
}

void kcf9 (EDT *pe) {

    pe->block = NO;
    pe->blkmark = NO;
    pe->cur.nbank = -1; pe->cur.loff = -1L;
    pe->needout |= NEED_OUTSCR;
    return;
}

void kchome (EDT *pe, bool total) {
    WBLK     wblk;
    long     i, offset;
    long     loff = 0L, lfile = 0L;
    long     lbuf = -1L, rbuf = -1L;
    char    *cur;


    if (pe->line + (long) (pe->col + pe->visicol) == 0L) return;

    WORKING;   
    if (pe->leftoff <= 0L) {
        MOVLEFT ((long) (Ed_pleft - Ed_pl));
        goto ret;
    }

    *(Ed_pnrlast - 1) = 0;
    *Ed_pnr = *(Ed_pnr - 1) + pnllen (0) + ((Ed_pnr == Ed_pnrlast) ? 0 : 1);
    if (pe->block) {
        if (!total) loff = pe->cur.loff;
        if (pe->blkmark) pe->cur.nline = 0L;
        wblk.blk = foundblk (pe, &pe->cur);
        if ((wblk.blk & 0x7) == 0)            /* Конец блока в левом файле */
            lfile = pe->cur.loff + sizelf - 1L;
        else if ((wblk.blk & 0x3) == 0)       /* Конец блока в левом буфере */
            lbuf = (long) (Ed_pnl[(word) startlb] + pnllen ((word) startlb));
        else if ((wblk.blk & 0x1) == 0)       /* Конец блока в правом буфере */
            rbuf = (long) (*(Ed_pnr - (word) startrb) - sizerb + 1);
        if (pe->blkmark)  pe->cur.nline = -pe->cur.start;
        else   pe->cur.rsize = sizelf + (long) (sizelb + sizerb) + sizerf;
        pe->cur.loff = -1L;
    }

    wrtort (pe, Ed_pright, ((rbuf > -1L) ? Ed_pr - (word) rbuf : NULL),
                                                    (word) (Ed_pr - Ed_pright + 1));

    wrtort (pe, Ed_pl, ((lbuf > -1L) ? Ed_pl + (word) lbuf : NULL),
                                                    (word) (Ed_pleft - Ed_pl + 1));

    Ed_lbufempty = Ed_rbufempty = YES;
    while (1) {
        if (pe->leftoff <= loff) break;
        if (pe->leftoff - loff >= (long) Ed_lsize) {
            i = (long) Ed_lsize;
            utlseek (pe->handleft, - i, UT_CUR);
        } else {
            i =  pe->leftoff - loff;
            utlseek (pe->handleft, loff, UT_BEG);
        }
        i = (long) utread (pe->handleft, Ed_pl, (word) i);
        pe->leftoff -= i;
        utlseek (pe->handleft, - i, UT_CUR);
        offset = lfile - pe->leftoff;
        if (offset > 0L && offset <= i) cur = Ed_pl + (word) offset;
        else                            cur = NULL;
        wrtort (pe, Ed_pl, cur, (word) i);
    }
    utchsize (pe->handleft, pe->leftoff);

    loadright (pe);  loadleft (pe);

ret:
    if (total) {
        pe->line = 0L;
        pe->col = pe->visicol = pe->row = pe->blank = pe->lf = 0;
        if (Ed_pnrlast > Ed_pnr) pe->lf = 1;
        pe->needout |= NEED_OUTSCR;
    }
    return;
}

void kcend (EDT *pe) {
    int             H = Ed_pwin->img.dim.h;
    long            ii, i;
    WBLK            wblk;
    bool            oldblock = NO;
    char           *p;

    if (pe->lf > 0) { pe->line -= pe->lf - 1; goto ret; }

    WORKING;   
    *(Ed_pnrlast - 1) = 0;
    *Ed_pnr = *(Ed_pnr - 1) + pnllen (0) + ((Ed_pnr == Ed_pnrlast) ? 0 : 1);
    if (pe->block) {
        if (pe->blkmark) pe->cur.nline = 0L;
        wblk.blk = foundblk (pe, &pe->cur);
        if ((wblk.blk & 0xE) == 0)           /* Начало блока в правом файле */
            pe->cur.loff = pe->leftoff + (long) (Ed_pleft - Ed_pl + 1) +
                (long) (Ed_pr - Ed_pright + 1) +
                (long)(pe->nbank + 1) * (long)BUF_WRK - (long)pe->rightoff -
                ((long)pe->cur.nbank*(long)BUF_WRK+(long)pe->cur.roff-sizerf);
        else if ((wblk.blk & 0xC) == 0)      /* Начало блока в правом буфере */
            pe->cur.loff = pe->leftoff + (long) (Ed_pleft - Ed_pl + 1) +
                        (long) (Ed_pr - Ed_pright) - (long) *(Ed_pnr - (word) startrb);
        else if ((wblk.blk & 0x8) == 0)      /* Начало блока в левом буфере */
            pe->cur.loff = pe->leftoff + Ed_pnl[(word) (startlb + nlinelb)];
        if (!pe->blkmark)
            pe->cur.lsize = sizelf + (long) (sizelb + sizerb) + sizerf;
        pe->cur.nbank = -1;
    }

    if (pe->nbank < 0) {
        if (pe->block) { pe->block = NO; oldblock = YES; }
        MOVRIGHT ((long) (Ed_pr - Ed_pright));
        if (oldblock)  pe->block = YES;
        if (pe->leftoff < pe->cur.loff || pe->leftoff  == 0L)
            pe->cur.loff = -1L;
        goto ret;
    }

    i = (long) (Ed_pleft - Ed_pl + 1);
    utwrite (pe->handleft, Ed_pl, (word) i);
    pe->leftoff += i;

    i = (long) (Ed_pr - Ed_pright + 1);
    utwrite (pe->handleft, Ed_pright, (word) i);
    pe->leftoff += i;
    Ed_lbufempty = Ed_rbufempty = YES;

    for (i = 0L, p = Ed_pl; pe->nbank >= 0; pe->nbank--) {
        utlseek (pe->handright,
            (long) pe->nbank * (long) BUF_WRK + (long) pe->rightoff, UT_BEG);
        ii = BUF_WRK - pe->rightoff;
        i += utread (pe->handright, p, (word) ii);
        if (i > Ed_lsize - BUF_WRK || pe->nbank == 0) {
            utwrite (pe->handleft, Ed_pl, (word) i);
            pe->leftoff += i;
            i = 0L;
        }
        p = Ed_pl + (word) i;
        pe->rightoff = 0;
    }

    pe->line = pe->total + 1L;
    loadleft (pe); loadright (pe);
ret:
    if (pe->blkmark) pe->cur.nline = pe->line - pe->cur.start;
    pe->lf = 1;
    pe->col = pe->visicol = pe->blank = 0;
    if (Ed_pnllast - Ed_pnl < H) loadleft (pe);
    if (Ed_pnllast - Ed_pnl < H - 1)
        pe->row = (int) (Ed_pnllast - Ed_pnl);
    else pe->row = H - 1;
    pe->needout |= NEED_OUTSCR;
    return;
}

