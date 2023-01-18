#include <dos.h>
#include <sledit.h>
#include <slutil.h>
#include <slsymb.def>

static void erasesym (EDT *pe);

void kpgdn (EDT *pe) {
    int     H = Ed_pwin->img.dim.h;
    int     i, npnr;
    int     rlen;

    if (Ed_pnr - Ed_pnrlast < (H << 1) - pe->row) loadright (pe);
    if (pe->lf > 0) {
        pe->lf += H - 1; pe->line += H - 1; goto ret;
    }
    npnr = (int) (Ed_pnr - Ed_pnrlast);
    if (npnr >= H - 1) {
        npnr = H - 1;
        rlen = pnrlen (npnr);
        i = pe->col + pe->visicol;
        if (rlen > i) pe->blank = 0;
        else   { pe->blank = i - rlen; i = rlen; }
        i += (int) (Ed_pr - Ed_pright) - *(Ed_pnr - npnr) - 1;
    } else {
        pe->row += npnr + 1;
        if (pe->row > H - 1) pe->row = H - 1;
        i = (int) (Ed_pr - Ed_pright);
        pe->blank = pe->col + pe->visicol;
        pe->lf = 1;
    }
    MOVRIGHT ((long) i);
ret:
    if (pe->blkmark) pe->cur.nline = pe->line - pe->cur.start;
    pe->needout |= NEED_OUTSCR;
    return;
}

static void erasesym (EDT *pe) {
    bool    insert = pe->insert;

    pe->insert = NO;
    setsym (pe, ' ', 1);
    pe->insert = insert;
    return;
}

void ksdown (EDT *pe) {
    int     sym;

    if (pe->single == 2) {
        if (pe->lf > 0) { kdown (pe); return; }
        if (*Ed_pright == ' ') kdown (pe);
        erasesym (pe);
    } else {
        sym = ldgetcod (*Ed_pright);
        if (sym != 0 && (sym == 80 && pe->single || sym == 160 && !pe->single
                            || *Ed_pright == setld (pe, NO)))       kdown (pe);
        setld (pe, YES);
    }
    kleft (pe);
    return;
}

void ksup (EDT *pe) {
    int     sym;

    if (pe->single == 2) {
        if (pe->lf > 0) { kup (pe); return; }
        if (*Ed_pright == ' ') kup (pe);
        erasesym (pe);
    } else {
        sym = ldgetcod (*Ed_pright);
        if (sym != 0 && (sym == 80 && pe->single || sym == 160 && !pe->single ||
                            *Ed_pright == setld (pe, NO)))       kup (pe);
        setld (pe, YES);
    }
    kleft (pe);
    return;
}

void ksright (EDT *pe) {
    int     sym;

    if (pe->single == 2) {
        if (pe->lf > 0) { kright (pe); return; }
        if (*Ed_pright == ' ') kright (pe);
        erasesym (pe);
        kleft (pe);
        return;
    }
    sym = ldgetcod (*Ed_pright);
    if (sym != 0 && (sym == 5 && pe->single || sym == 10 && !pe->single ||
                         *Ed_pright == setld (pe, NO)))       kright (pe);
    setld (pe, YES);
    kleft (pe);
    return;
}

void ksleft (EDT *pe) {
    int     sym, i = pe->col + pe->visicol;

    if (pe->single == 2) {
        if (pe->lf > 0) { if (i > 0) kleft (pe); return; }

        if (*Ed_pright == ' ' && i > 0) kleft (pe);
        erasesym (pe);
    } else {
        sym = ldgetcod (*Ed_pright);
        if (sym != 0 && (sym == 5 && pe->single || sym == 10 && !pe->single ||
                            *Ed_pright == setld (pe, NO)))
            { if (i == 0) return;     kleft (pe); }
        setld (pe, YES);
    }
    kleft (pe);
    return;
}

void setsym (EDT *pe, char sym, int count) {
    int         W = Ed_pwin->img.dim.w;
    register    i = pe->blank;
    int         savlf = pe->lf;

    if (sym == '\n' || sym == '\0' || sym == '\t') return;
    if ((long) ((char *) Ed_pnl - Ed_pleft) <
        (long) (pe->lf * 2 + i) + 21L) swapleft (pe, NO);
    if (pe->lf) {
        pe->total += pe->lf;
        for (Ed_pleft--; pe->lf > 0; pe->lf--) {
            *++Ed_pleft = '\n';
            *Ed_pnl = (word) (Ed_pleft - Ed_pl) + 1;
            Ed_pnl--;
        }
        Ed_pnl++; Ed_pnr++;
        *--Ed_pright = '\n';
    }
    for (; i > 0; i--)   *++Ed_pleft = ' ';
    for (i = count; i > 0; i--) *++Ed_pleft = sym;
    if (!pe->insert)  {
        for (i = pe->blank + count; i > 0; i--) {
            if (*Ed_pright != '\n') Ed_pright++;
        }
    }
    pe->col += count;
    if (pe->col > W - 1) {
        pe->visicol += pe->col - (W - 1);
        pe->col = W - 1;
        savlf = 1;
    }
    pe->blank = 0;
    if (savlf)  pe->needout |= NEED_OUTSCR;
    else        pe->needout |= NEED_OUTLIN;
    pe->changed = YES;
    return;
}

void khome (EDT *pe) {

    if ((pe->col + pe->visicol) == 0) return;
    if (pe->lf == 0) MOVLEFT ((long) (Ed_pleft - Ed_pl) - *Ed_pnl);
    pe->col = pe->visicol = pe->blank = 0;
    pe->needout |= NEED_OUTSCR;
	return;
}

void kend (EDT *pe) {
    int     W = Ed_pwin->img.dim.w;
    word    i = pe->col + pe->visicol;
    int     llen;

    llen = pnllen (0);
    if (pe->lf > 0 || llen == 0) { pe->col = pe->visicol = 0; goto ret;}
    if (pe->blank) {
        if (pe->visicol >= llen)
            { pe->col = 1; pe->visicol = llen - 1; }
        else  pe->col = llen - pe->visicol;
        goto ret;
	}
    if   (llen >= W + pe->visicol)
        { pe->col = W - 1; pe->visicol = llen - pe->col; }
    else if (llen >= i)   { pe->col = llen - pe->visicol; }
    else { pe->col = llen; pe->visicol = 0; }
    if (i <= 0 || llen <= 0) { MOVRIGHT ((long) llen - 1L); }
    else { MOVRIGHT ((long) llen - ((long) (Ed_pleft - Ed_pl) - *Ed_pnl) - 2L); }
ret:
    pe->blank = 0;
    pe->needout |= NEED_OUTSCR;
	return;
}

void kleft (EDT *pe) {
    if   (pe->col > 0) {
        if (pe->blank > 0) pe->blank--;
        else       MOVLEFT (0L);
        pe->col--;  return;
    }
    if   (pe->visicol > 0) {
        if (pe->blank > 0) pe->blank--;
        else       MOVLEFT (0L);
        pe->visicol--; pe->needout |= NEED_OUTSCR; return;
    }
    if (pe->line > 0) { kup (pe); kend (pe); }
	return;
}

void kright (EDT *pe) {
    if (*Ed_pright == '\n' || pe->lf) pe->blank++;
    else                       MOVRIGHT (0L);
    if (pe->col < Ed_pwin->img.dim.w - 1) pe->col++;
    else             { pe->visicol++; pe->needout |= NEED_OUTSCR; }
	return;
}

void kup (EDT *pe) {
    word    i = pe->col + pe->visicol, m;
    int     llen;

    if (pe->line <= 0L) return;
    if (pe->blkmark) pe->cur.nline--;
    llen = pnllen (1);
    if (pe->lf == 0)  {
        if (pe->blkmark && pe->cur.nline >= 0L) paintlin (pe);
        if ((int) (Ed_pnllast - Ed_pnl) < 3) loadleft (pe);
        if (i > llen)
            { pe->blank = i - llen; m = llen; }
        else  { m = i; pe->blank = 0; }
        m += Ed_pnl[1];
        MOVLEFT ((long) (Ed_pleft - Ed_pl) - (long) m);
    } else if (pe->lf == 1) {
        pe->lf--;
        MOVLEFT (0L);
        llen = pnllen (0);
        if (i > llen)  pe->blank = i - llen;
        else {
            MOVLEFT ((long) llen - (long)(i + 1));
            pe->blank = 0;
        }
    } else { pe->lf--; pe->line--; }
    if (pe->row > 0) {
        pe->row--;
        if (pe->blkmark && pe->lf == 0) paintlin (pe);
    } else {
        viroll (WN_SCR_DOWN);
        if (pe->lf < 2) pe->needout |= NEED_OUTLIN;
	}
	return;
}

void kcpgup (EDT *pe) {
    int     j;

    j = pe->lf;
    if (pe->lf > 0) {
        if (pe->lf > pe->row){pe->line -=pe->row; pe->lf -= pe->row; goto ret;}
        pe->row -= pe->lf;
        pe->lf = 0;
        pe->line++;
        MOVLEFT (0L);
    }
    MOVLEFT ((long) (Ed_pleft - Ed_pl) - Ed_pnl[pe->row]);
    pe->line -= j;
ret:
    pe->col = pe->visicol = pe->blank = pe->row = 0;
    if (pe->blkmark) pe->cur.nline = pe->line - pe->cur.start;
    pe->needout |= NEED_OUTSCR;
    return;
}

void kdown (EDT *pe) {
    int     H = Ed_pwin->img.dim.h;
    word    i = pe->col + pe->visicol;
    int     len;
    int     j = 0;

    if (pe->blkmark) pe->cur.nline++;
    if (pe->lf == 0) {
        if (pe->blkmark && pe->cur.nline <= 0L) paintlin (pe);
        if (pe->blank == 0)  j = pnllen (0) - i;
        if (Ed_pnr - Ed_pnrlast < 2) loadright (pe);
        if (Ed_pnr > Ed_pnrlast) {
            len = pnrlen (1);
            if (i > len) { j += len; pe->blank = i - len; }
            else  { j += i; pe->blank = 0; }
        } else { pe->lf++; pe->blank = i; }
        MOVRIGHT ((long) j);
    } else { pe->lf++; pe->line++; }
    if   (pe->row == H - 1) {
        viroll (WN_SCR_UP);
        if (pe->lf < 3) pe->needout |= NEED_OUTLIN;
	}
    else  {
        pe->row++;
        if (pe->blkmark && pe->lf == 0) paintlin (pe);
    }
	return;
}

void kcpgdn (EDT *pe) {
    int     H = Ed_pwin->img.dim.h;
    int     j, npnr;
    long    i;

    i = (long) (Ed_pr - Ed_pright);
    if (pe->row == H - 1) { khome (pe); return; }
    j = H - 1 - pe->row;
    npnr = (int) (Ed_pnr - Ed_pnrlast);
    if (pe->lf == 0) {
        if (npnr >= j) i -= *(Ed_pnr - j) + 1L;
        else { pe->lf = j - npnr; pe->line += pe->lf - 1; }
        MOVRIGHT (i);
    }
    else { pe->lf += j; pe->line += j; }
    pe->col = pe->visicol = pe->blank = 0;
    pe->row = H - 1;
    if (pe->blkmark) pe->cur.nline = pe->line - pe->cur.start;
    pe->needout |= NEED_OUTSCR;
    return;
}

void kpgup (EDT *pe) {
    int     H = Ed_pwin->img.dim.h;
    int     i, npnl;

    if (pe->line == 0L) return;
    if (Ed_pnllast - Ed_pnl < (H << 1)) loadleft (pe);
    npnl = (int) (Ed_pnllast - Ed_pnl);
    if (pe->lf == 0) {
        if (npnl < H - 1 + pe->row) {
            if (npnl <= H - 1)  pe->row = 0;
            else { i = npnl - pe->row; npnl = pe->row; pe->row = i; }
        } else  npnl = H - 1;
    } else {
        npnl--;
        if (pe->lf >= H + pe->row)
            { pe->line -= H - 1; pe->lf -= H - 1; goto ret; }
        if (Ed_pleft < Ed_pl)
            { pe->lf = 1; pe->row = 0; pe->line = 0; goto ret; }
        if (pe->lf <= pe->row ) {
            i = pe->row - pe->lf;
            pe->line -= pe->lf - 1; pe->row = 0; npnl = i; pe->lf = 0;
        } else if (pe->lf <= H - 1) {
            i = H - 1 - pe->lf;
            if (npnl > i + pe->row) npnl = i;
            else { pe->row  = npnl; npnl = 0; }
            pe->line -= pe->lf - 1; pe->lf = 0;
        } else {
            i = H - 1 + pe->row - pe->lf;
            if (npnl > i)
                { pe->line -= H - 1; pe->lf -= H - 1; goto ret; }
            else {pe->line -= pe->lf -1; pe->row = npnl; npnl = 0; pe->lf = 0;}
        }
        MOVLEFT (0L);
    }
    i = pnllen (npnl) - (pe->col + pe->visicol);
    if (i < 0) { pe->blank = -i; i = 0; }
    else  pe->blank = 0;
    if (npnl > 0) i += (int) (Ed_pleft - Ed_pl) - Ed_pnl[npnl-1] + 1;
    else   i--;
    MOVLEFT ((long) i);
ret:
    if (pe->blkmark) pe->cur.nline = pe->line - pe->cur.start;
    pe->needout |= NEED_OUTSCR;
    return;
}

void kbs (EDT *pe) {
    int     H = Ed_pwin->img.dim.h;
    int     W = Ed_pwin->img.dim.w;
    int     llen;
    int     i = pe->col + pe->visicol;

    if (pe->lf) { kleft (pe); return;}
    if (Ed_pnr - Ed_pnrlast <= H - pe->row + 1) loadright (pe);
    if (Ed_pnllast - Ed_pnl <= (H << 1)) loadleft (pe);
    if (pe->line == 0L && i == 0) return;
    if (*Ed_pleft != '\n' || pe->blank > 0) {
        if (pe->blank > 0) pe->blank--;
        else {
            if (pe->insert || *Ed_pright == '\n')  Ed_pleft--;
            else { *Ed_pleft = ' '; MOVLEFT (0L); }
        }
        if (pe->col) { pe->col--; pe->needout |= NEED_OUTLIN; }
        else { pe->visicol--; pe->needout |= NEED_OUTSCR;}
    }  else {
        blkcorrect (pe);
        llen = pnllen (1);
        if (pe->insert) { Ed_pleft--; Ed_pnl++; pe->line--; pe->total--; }
        else MOVLEFT (0L);
        if (pe->row > 0) pe->row--;
        if   (llen >= W + pe->visicol)
            { pe->col = W - 1; pe->visicol = llen - pe->col; }
        else if (llen >= i) pe->col = llen - pe->visicol;
        else { pe->col = llen; pe->visicol = 0; }
        pe->needout |= NEED_OUTSCR;
    }
    pe->changed = YES;
    return;
}

void kdel (EDT *pe) {
    if (pe->lf > 0) return;
    if (Ed_pnr - Ed_pnrlast <= (Ed_pwin->img.dim.h << 1)) loadright (pe);
    if (*Ed_pright != '\n') { Ed_pright++; pe->needout |= NEED_OUTLIN; }
    else {
        if ((long)(Ed_pnr - Ed_pnrlast) <= 0L) {
            if (pnrlen (0) > 0)  return;
            else  pe->lf++;
        } else {
            if (pe->blank > 0) {
                if ((long)((char *) Ed_pnl - Ed_pleft) < pe->blank + 20L)
                    swapleft (pe, NO);
                for (; pe->blank > 0; pe->blank--) *++Ed_pleft = ' ';
            }
        }
        Ed_pright++; Ed_pnr--; pe->total--;
        blkcorrect (pe); pe->needout |= NEED_OUTSCR;
    }
    pe->changed = YES;
    return;
}

void kcy (EDT *pe) {
    int     i = pe->col + pe->visicol;
    int     llen;

    if (pe->lf > 0) return;
    if (Ed_pnr - Ed_pnrlast < (Ed_pwin->img.dim.h << 1)) loadright (pe);
    Ed_pleft = Ed_pl + *Ed_pnl - 1;
    while (*Ed_pright++ != '\n');
    Ed_pnr--; pe->total--;
    blkcorrect (pe);
    if (Ed_pnr < Ed_pnrlast) { pe->lf = 1; pe->blank = i; }
    else {
        llen = pnllen (0);
        if (llen < i) { pe->blank = i - llen; i = llen; }
        else  pe->blank = 0;
        MOVRIGHT ((long) i - 1L);
    }
    pe->changed = YES;
    pe->needout |= NEED_OUTSCR;
    return;
}

void kenter (EDT *pe) {

    if (!pe->insert) { khome (pe); kdown (pe); return; }
    if ((char *) Ed_pnl - Ed_pleft < 20) swapleft (pe, NO);
    pe->col = pe->visicol = pe->blank = 0;
    if (pe->lf > 0)
        { setsym (pe,' ',1); blankclr (pe); kleft (pe);  kdown (pe);  return; }
    while (*Ed_pleft == ' ')  Ed_pleft--;
    *(++Ed_pleft) = '\n'; Ed_pnl--;
    blkcorrect (pe);
    *Ed_pnl = (word) (Ed_pleft + 1 - Ed_pl);
    pe->line++;  pe->total++;
    if (pe->row < Ed_pwin->img.dim.h - 1) pe->row++;
    pe->changed = YES;
    pe->needout |= NEED_OUTSCR;
    return;
}

void blkcorrect (EDT *pe) {
    long    start, end;

    if (!pe->block) return;
    if (pe->blkmark) {
        switch (Kb_sym) {
            case    BS:
                if (!pe->insert || pe->cur.nline > 0L) pe->cur.nline--;
                else pe->cur.start--;
                break;
            case    CtrlY:
                if (pe->cur.nline == 0L)  { kcf9 (pe); break; }
            case    Del:
                if (pe->cur.nline < 0L) { pe->cur.start--; pe->cur.nline++; }
                break;
            case    GrayEnter:
            case    Enter:
                if (pe->cur.nline >= 0L) pe->cur.nline++;
                else  pe->cur.start++;
                break;
            default :     break;
        }
    } else {
        start = pe->cur.start  - pe->line;
        end =  pe->cur.start + pe->cur.nline - pe->line;
        if (end < 0L) return;
        switch (Kb_sym) {
            case    BS:
                if (!pe->insert) break;
                if (start < 0L) pe->cur.nline--;
                else  pe->cur.start--;
                break;
            case    Del:
                if (end == 0L) break;
            case    CtrlY:
                if (start <= 0L) pe->cur.nline--;
                else  pe->cur.start--;
                if (pe->cur.nline < 0L) kcf9 (pe);
                break;
            case    GrayEnter:
            case    Enter:
                if (start <= 0L) pe->cur.nline++;
                else pe->cur.start++;
                break;
            default      :     break;
        }
    }
    return;
}

void kck (EDT *pe) {

    if (pe->lf > 0 || pe->blank > 0 || pnllen (0) == 0) return;
    while (*Ed_pright != '\n')  Ed_pright++;
    pe->changed = YES;
    pe->needout |= NEED_OUTLIN;
    return;
}

void kcright (EDT *pe) {
    int     W = Ed_pwin->img.dim.w;

    if (pe->lf > 0 || *Ed_pright == '\n') return;
    if (*Ed_pright != ' ' && *Ed_pright != '\n')
        while (*Ed_pright != ' ' && *Ed_pright != '\n') {
            MOVRIGHT (0L);
            if (pe->col < W - 1) pe->col++;
            else pe->visicol++;
        }
    else if (*Ed_pright == ' ')
        while (*Ed_pright == ' ') {
            MOVRIGHT (0L);
            if (pe->col < W - 1) pe->col++;
            else pe->visicol++;
        }
    if (pe->col == W - 1) pe->needout |= NEED_OUTSCR;
    return;
}

void kcleft (EDT *pe) {

    if (pe->lf > 0 || *Ed_pleft == '\n') return;
    if (pe->blank > 0)
        while (pe->blank > 0) {
            pe->blank--;
            if (pe->col > 0) pe->col--;
            else pe->visicol--;
        }
    else if (*Ed_pleft != ' ' && *Ed_pleft != '\n')
        while (*Ed_pleft != ' ' && *Ed_pleft != '\n' && Ed_pleft >= Ed_pl){
            MOVLEFT (0L);
            if (pe->col > 0) pe->col--;
            else pe->visicol--;
        }
    else if (*Ed_pleft == ' ')
        while (*Ed_pleft == ' ' && Ed_pleft >= Ed_pl) {
            MOVLEFT (0L);
            if (pe->col > 0) pe->col--;
            else pe->visicol--;
        }
    if (pe->col == 0) pe->needout |= NEED_OUTSCR;
    return;
}

char setld (EDT *pe, bool need) {
    SYM_LD  right, left, up, down, center;
    int pos =  pe->col + pe->visicol, len;
    bool insert = pe->insert;

    center.x = 0;

    if (pe->lf > 0 || pnllen (0) < pos) { right.x = left.x = 0; }
    else {
        left.x = (pos == 0) ? 0  : ldgetcod (*Ed_pleft);
        right.x = (*Ed_pright == '\n') ? 0 : ldgetcod ( *(Ed_pright + 1) );
    }

    if (pe->line == 0L || pnllen (1) <= pos) up.x = 0;
    else up.x    = ldgetcod (*(Ed_pl + Ed_pnl[1] + pos));

    len = pnrlen (1);
    if (Ed_pnr - 1 < Ed_pnrlast || len <= pos) down.x = 0;
    else down.x  = ldgetcod ( *(Ed_pr - *(Ed_pnr - 1) + pos) );

    center.b.lt_s = left.b.rt_s;
    center.b.lt_d = left.b.rt_d;
    center.b.rt_s = right.b.lt_s;
    center.b.rt_d = right.b.lt_d;
    center.b.up_s = up.b.dn_s;
    center.b.up_d = up.b.dn_d;
    center.b.dn_s = down.b.up_s;
    center.b.dn_d = down.b.up_d;

    if (!need) return ldgetchr (center.x, pe->single);
    pe->insert = NO;
    setsym (pe, ldgetchr (center.x, pe->single), 1);
    pe->insert = insert;
    return 0;
}

void  blankclr (EDT *pe) {
    if (*Ed_pright != '\n') return;
    while (*Ed_pleft == ' ') { Ed_pleft--; pe->blank++; }
}

int  pnllen (word i) {
    int     len;

    if (i == 0) {
        if (Ed_pnr < Ed_pnrlast) return 0;
        len = (int) ((word) (Ed_pleft - Ed_pl) - Ed_pnl[0] + 1);
        if (Ed_pnr == Ed_pnrlast)   len += (int) (Ed_pr - Ed_pright);
        else   len += (int) ((word) (Ed_pr - Ed_pright) - *(Ed_pnr -1) - 1);
    } else  len = (int) (Ed_pnl[i - 1] - Ed_pnl[i] - 1);
    return len;
}

int  pnrlen (word i) {
    int     len;

    if (i == 0)  return  pnllen (0);
    if ((Ed_pnr - i) == Ed_pnrlast) len = *(Ed_pnr - i);
    else len = *(Ed_pnr - i) - *(Ed_pnr - (i + 1)) - 1;
    return  len;
}

void    kf2 (EDT *pe) {

    pe->single--;
    if (pe->single < 0) pe->single = 2;
    return;
}

