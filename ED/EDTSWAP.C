#include    <sledit.h>
#include    <slkeyb.h>



bool loadleft (EDT *pe) {
    register int    ii, tmps;
    bool            first = YES, empty = YES;
    long            offset, i, j;
    word            polsize = Ed_lsize / 2;
    word           *q;
    char           *p, *savplf = Ed_pl + polsize;

    if (Ed_lbufempty) {
        Ed_pnllast = (word *) (Ed_pl + Ed_lsize); Ed_pnllast--;
        Ed_pnl = Ed_pnllast; *Ed_pnl = 0;
        Ed_pleft = Ed_pl - 1;
        Ed_lbufempty = NO;
    }
    if (pe->leftoff <= 0L) return NO;

    WORKING;   

    if (Ed_pleft < Ed_pl) {
        Ed_pleft = Ed_pl + polsize;
        p = Ed_pleft;
        q = (word *) Ed_pleft + 8;
        Ed_pnl = q;
    } else {
        i = (long) (Ed_pleft - Ed_pl) + 1L;
        if ((word) ((char *) Ed_pnl - Ed_pleft) <= polsize + 20) return NO;
		empty = NO;
        Ed_pleft = Ed_pl + polsize;
        p = Ed_pleft - (word) i;
        utmovmem (p + 1, Ed_pl, (word) i);
        i = (long) (p  - Ed_pl) + 1L;

        q = (word *) Ed_pleft + 8;
        j = (long) ((Ed_pl + Ed_lsize) - (char *) Ed_pnl);
        tmps = (int) (Ed_pnllast - Ed_pnl);
        utmovmem ((char far *) q, (char far *) Ed_pnl, (word) j);
        Ed_pnl = q;
        q += tmps + 1;
        for (; tmps >= 0; tmps--) Ed_pnl[tmps] += (word) i;
    }

    while (pe->leftoff > 0L) {
        if (pe->leftoff >= (long) BUF_WRK) {
            tmps = BUF_WRK;
            utlseek (pe->handleft, - (long) tmps, UT_CUR);
        } else {
            tmps = (int) pe->leftoff;
            utlseek (pe->handleft, 0L, UT_BEG);
        }
        tmps = utread (pe->handleft, Ed_pw, tmps);
        pe->leftoff -= tmps;
        utlseek (pe->handleft, - (long) tmps, UT_CUR);
        if  (tmps <= 0) break;
        if ((ii = loadlt (&p, Ed_pl, &(char far *)q, pe->leftoff, &offset,
             &savplf, Ed_pw + tmps - 1, tmps, Ed_lsize / 3, (int) first)) == 1)
            break;

        if (first) first = NO;
        kbflushb ();
    }
    p++;
    if (ii > 0) {
        p = savplf + 1;
        pe->leftoff = offset;
        utlseek (pe->handleft, pe->leftoff, UT_BEG);
	} else {
        *q = (word) (p - Ed_pl);
		q++;
	}
    utmovmem (Ed_pl, p, (word) (Ed_pleft - p + 1));
    tmps = (int) (p - Ed_pl);
    Ed_pleft = Ed_pleft - tmps;
    p = (char *) Ed_pnl + (word) ((Ed_pl + Ed_lsize) - (char *) q);
    utmovmem ((char far *) p, (char far *) Ed_pnl,
                                        (word) ((char *) q - (char *) Ed_pnl));
    i = (long) (q - Ed_pnl - 1);
    Ed_pnl = (word *) p;
    for (; i >= 0; i--) Ed_pnl[(word) i] -= tmps;

	if (empty){
        Ed_pnl--;
        *Ed_pnl = (word) (Ed_pleft - Ed_pl) + 1;
	}
    if (pe->leftoff < pe->cur.loff) pe->cur.loff = -1L;
    utchsize (pe->handleft, pe->leftoff);
    return YES;
}

bool loadright (EDT *pe) {
    bool           empty = YES;
    long           i, j;
    word           polsize = Ed_rsize / 2;
    int            tmps, nbank = pe->nbank, rightoff = pe->rightoff;
    word           *q, *qmidle;
    char           *p, *pmidle, *savplf;

    if (Ed_rbufempty) {
        Ed_pnr = Ed_pnrlast = (word *) (Ed_ppr + 3);  Ed_pnrlast++;
        Ed_pright = Ed_pr + 1; *Ed_pnr = *Ed_pnrlast = 0;
        Ed_rbufempty = NO;
    }
    if (pe->nbank < 0)  return  NO;

    WORKING;   

    if (Ed_pright > Ed_pr) {               /* если буфер пуст */
        pmidle = Ed_pr - polsize;
        p = pmidle;
        qmidle = (word *) pmidle - 8;
        q = qmidle;
    } else {                                 /* если буфер не пуст */
        if ((word) (Ed_pright - (char *) Ed_pnr) <= polsize + 20) return NO;
        i = (int) (Ed_pr - Ed_pright) + 1;    /* кол-во символов в пр. буфере */
        empty = NO;
        pmidle = Ed_pr - polsize;
        p = pmidle + (word) i;
        utmovmem (pmidle, Ed_pright, (word) i); /* сим. с края в середину*/
        tmps = (int) (Ed_pr - p) + 1;         /* на сколько передвинули */

        q = qmidle = (word *) pmidle - 8;
        j = (int) ((char *) (Ed_pnr + 1) - (char *) Ed_pnrlast); /* в char */
        i = (int) (Ed_pnr - Ed_pnrlast);                         /* в word */
        utmovabs ((char far *) qmidle + 1, (char far *)Ed_pnr + 1, (word) j, 1);
        q -= (word) i + 1;
        for (; i >= 0L; i--) *(qmidle -(word) i) += (word) tmps; /*коррект.pnr*/
    }
    *q = (word) (Ed_pr - p);

    while (nbank >= 0) {
        utlseek (pe->handright, (long) nbank * (long) BUF_WRK, UT_BEG);
        if (utread (pe->handright, Ed_pw, BUF_WRK) <= 0) break;

        if (loadrt (&p, Ed_pr, &(char far *)q, &pe->nbank,
            &pe->rightoff, &savplf, Ed_pw + rightoff,BUF_WRK - rightoff,
                                  Ed_rsize / 3, nbank, rightoff) == 1) break;
        nbank--;
        rightoff = 0;
        kbflushb ();
    }
    if (pe->rightoff == BUF_WRK)    { pe->nbank--; pe->rightoff = 0; }
    p = savplf;
    tmps = (int) (Ed_pr - p);          /* на ск. двигать от середины к краю */
    utmovmem (pmidle + tmps, pmidle, (word) (p - pmidle + 1));
    Ed_pright = pmidle + tmps;
    j = (int) ((char *) qmidle - (char *) q);               /* в char */
    utmovmem ((char far *) Ed_pnrlast, (char far *) (q + 1), (word) j);
    i = (int) (qmidle - q - 1);                             /* в word */
    Ed_pnr = Ed_pnrlast + (word) i;
    for (; i >= 0L; i--)  *(Ed_pnr - (word) i) -= tmps;    /* коррект. pnr */

    if (empty)  *Ed_pnl = (word) (Ed_pleft - Ed_pl) + 1;

    utchsize (pe->handright, (long) (pe->nbank + 1) * (long) BUF_WRK);
    if (pe->nbank < pe->cur.nbank ||
       (pe->nbank == pe->cur.nbank && pe->rightoff > pe->cur.roff))
           pe->cur.nbank = -1;
    return YES;
}

void swapleft (EDT *pe, bool all) {
    long    i, j = 0L, k;
    char   *p;

    if (Ed_pleft < Ed_pl) return;

    WORKING;   
    i = (long) (Ed_pnllast - Ed_pnl + 1);
    if (!all) { j = i/2L;  p = Ed_pl + Ed_pnl[(word) j]; }
    else      p = Ed_pleft + 1;

    if (pe->block) {
        k = pe->line - pe->cur.start - ((pe->lf == 0) ? 0 : pe->lf - 1);
        if (pe->cur.loff == -1L && k > j)
            pe->cur.loff = pe->leftoff + Ed_pnl[(word) k];
        k -= pe->cur.nline;
        if (all && k <= j) k = j + 1;
        if (k < i && k > j)
            pe->cur.lsize = pe->leftoff - pe->cur.loff + Ed_pnl[(word) k - 1];
    }

    i = (long) ((word) (p - Ed_pl));
    utwrite (pe->handleft, Ed_pl, (word) i);
    pe->leftoff += i;

    if (!all) {
        utmovabs ((char far *) Ed_pnllast + 1, (char far *) &Ed_pnl[(word) j] + 1,
                                                       (word) (j + 1) * 2, 1);
        Ed_pnl = Ed_pnllast - (word) j;
        i = (long) ((word) (Ed_pleft - p) + 1);
        utmovmem (Ed_pl, p, (word) i);
        Ed_pleft = Ed_pl + (word) i - 1;
        i = (long) (p - Ed_pl);
        for (; j >= 0; j--) Ed_pnl[(word) j] -= (word) i;
    } else  Ed_lbufempty = YES;
    return;
}

void swapright (EDT *pe, bool all) {
    long    i, k, j = 0L;
    word   *q, count;
    char   *p, *pcur = NULL;

    if (Ed_pright > Ed_pr) return;

    WORKING;   
    i = (long) (Ed_pnr - Ed_pnrlast + 1);
    if (!all) j = i/2L;
    q = Ed_pnr - (word) j;
    if (!all) p = Ed_pr - *q;
    else      p = Ed_pright;

    if (pe->block) {
        *Ed_pnr = (word) (Ed_pr - Ed_pright);
        k = pe->cur.start - pe->line + ((pe->blkmark) ? 0L : pe->cur.nline);
        if (pe->cur.nbank == -1L)
            if (k >= j) pcur = Ed_pr - *(Ed_pnr - (word) k) + pnrlen ((word) k);
        k -= (pe->cur.nline > 0L) ? pe->cur.nline : -pe->cur.nline;
        if (all && k < j) k = j;
        if (k < i && k >= j) {
            if (pcur != NULL)
                pe->cur.rsize = (long) (pcur - (Ed_pr - *(Ed_pnr - (word) k))) + 1L;
            else
                pe->cur.rsize = (long) *(Ed_pnr - (word) k) + 2L +
                         (long) pe->cur.roff - (long) pe->rightoff +
                           (long) (pe->nbank - pe->cur.nbank) * (long) BUF_WRK;
        }
    }

    count = (word) (Ed_pr - p) + 1;
    wrtort (pe, p, pcur, count);
    if (!all) {
        utmovmem ((char far *) Ed_pnrlast, (char far *) (q + 1), (word) j * 2);
        Ed_pnr -= ((word) (i - j));
        p--;
        i = (long) ((word) (p - Ed_pright));
        utmovmem (Ed_pr - (word) i, Ed_pright, (word) i + 1);
        Ed_pright = Ed_pr - (word) i;
        for (j--; j >= 0; j--) *(Ed_pnr - (word) j) -= count;
    } else Ed_rbufempty = YES;
}

void  wrtort (EDT *pe, char *from, char *pcur, word count) {
    char    *p;
    long     offset = 0L;
    word     i = (word) pe->rightoff;

    if (pe->rightoff > 0) {             /* дописываем символы в текущий банк */
        if (count <= pe->rightoff) {
            offset = pe->rightoff - count;
            pe->rightoff -= count;
            i = count;
        }
        count -= i;
        p = from + count;
        if (pcur >= p && pcur < p + i) {
            pe->cur.nbank = pe->nbank;
            pe->cur.roff = (int) (pcur - p) + (int) offset;
        }
        offset += (long) pe->nbank * (long) BUF_WRK;
        utlseek (pe->handright, offset, UT_BEG);
        utwrite (pe->handright, p, i);
    }
    while (count >= BUF_WRK) {          /* валим целыми банками */
        pe->nbank++;
        utlseek (pe->handright, (long) pe->nbank * (long) BUF_WRK , UT_BEG);
        count -= BUF_WRK;
        p = from + count;
        if (pcur >= p && pcur < p + BUF_WRK) {
            pe->cur.nbank = pe->nbank;
            pe->cur.roff = (int) (pcur - p);
        }
        utwrite (pe->handright, p, BUF_WRK);
        pe->rightoff = 0;
    }
    if (count > 0) {                    /* заполняем последний банк */
        pe->nbank++;
        pe->rightoff = BUF_WRK - count;
        utlseek (pe->handright,
            (long) pe->nbank * (long) BUF_WRK + (long) pe->rightoff, UT_BEG);
        if (pcur >= from && pcur < from + count) {
            pe->cur.nbank = pe->nbank;
            pe->cur.roff = (int) (pcur - from) + pe->rightoff;
        }
        utwrite (pe->handright, from, count);
    }
    return;
}

