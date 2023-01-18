#include    <string.h>
#include    <sltree.h>

static  SLNODE * getlastson     (SLTREE * ptree);
static  SLNODE * ininode        (int namlen, bool fld);

static  SLNODE    * trsavcur;
static  SLNODE    * trsavtop;
static  int         trsavrow;
static  int         trsavtopnum;
static  int         count;

void    trgo2nod (SLTREE * ptree, SLNODE * pnode) {
    bool          found;

    if (ptree->cur == pnode) return;
    found = trishigh (ptree, pnode);
    do  (found) ? trup (ptree) : trdown (ptree);
    while (ptree->cur != pnode);

    return;
}

bool    trishigh (SLTREE * ptree, SLNODE * pfind) {
    SLNODE      * pnode = ptree->cur;
    bool          found = NO;

    while ((pnode = pnode->prev) != NULL)
        if (pnode == pfind) { found = YES; break; }
    return found;
}

bool    tricenxt (SLTREE * ptree) {
    SLNODE      * pnode = ptree->cur;
    bool          found = NO;

    while ((pnode = pnode->next) != NULL) {
        if (!pnode->opt.fld) continue;
        if (pnode == ptree->ice)  found = YES;
        break;
    }
    return found;
}

void    trrename (SLTREE * ptree) {
loop:
    wnstredt (ptree->pwin, ptree->row, ptree->col, ptree->pwin->attr.afld,
                       ptree->pwin->attr.afld, ptree->cur->name, ptree->namlen);
    if (strlen (ptree->cur->name) == 0) { utalarm (); goto loop; }
    return;
}

void    trtbleft (SLTREE * ptree) {

    if (ptree->tab <= 3)  ptree->tab = 15;
    else                  ptree->tab--;
    return;
}

void    trtbrght (SLTREE * ptree) {

    if (ptree->tab >= 15)     ptree->tab = 3;
    else                      ptree->tab++;
    return;
}

bool    trcend (SLTREE * ptree) {
    SLNODE  * pnode;
    int     i;

    count = 0;
    if (ptree->cur == ptree->root ||
                    (pnode = trnxtbrt (ptree->cur)) == NULL)  return NO;
    i = count;
    while ((pnode = trnxtbrt (pnode)) != NULL) i = count;
    for (; i > 0; i--) trdown (ptree);
    return YES;
}

bool    trchome (SLTREE * ptree) {
    SLNODE  * pnode;
    int     i;

    count = 0;
    if ((pnode = trprvbrt (ptree->cur)) == NULL) return NO;
    i = count;
    while ((pnode = trprvbrt (pnode)) != NULL) i = count;
    for (; i > 0; i--) trup (ptree);
    return YES;
}

bool    trcpgup (SLTREE * ptree) {

    count = 0;
    if (trprvbrt (ptree->cur) == NULL) return NO;
    for (; count > 0; count--) trup (ptree);
    return YES;
}

bool    trcpgdn (SLTREE * ptree) {

    count = 0;
    if (trnxtbrt (ptree->cur) == NULL) return NO;
    for (; count > 0; count--) trdown (ptree);
    return YES;
}

bool   trsup (SLTREE * ptree) {

    if (ptree->prevtop == ptree->root) return NO;
    ptree->prevtop = ptree->prevtop->prev;
    return YES;
}

bool   trsleft (SLTREE * ptree) {

    if (ptree->minlev > 0) { ptree->minlev--;  return YES; }
    return NO;
}

bool   trsright (SLTREE * ptree) {

    ptree->minlev++;
    return YES;
}

bool   trsdown (SLTREE * ptree) {

    if (ptree->prevtop->next == NULL) return NO;
    ptree->prevtop = ptree->prevtop->next;
    return YES;
}

bool    trhome (SLTREE * ptree) {

    if (ptree->cur == ptree->root) return NO;
    ptree->cur = ptree->top = ptree->root;
    ptree->topnum = ptree->row = 0;
    return YES;
}

bool    trpgup (SLTREE * ptree) {
    register    i, n;

    if (ptree->cur == ptree->root) return NO;
    if (ptree->topnum < ptree->pwin->img.dim.h - 1) {
        n = ptree->topnum + ptree->row - (ptree->pwin->img.dim.h - 1);
        ptree->cur = ptree->top = ptree->root;
        ptree->topnum = 0;
        if (n < 0) ptree->row = 0;
        else {
            ptree->row = n;
            for (i = 0; i < n; i++)  ptree->cur = ptree->cur->next;
        }
    } else {
        for (i = 0; i < ptree->pwin->img.dim.h - 1; i++) {
            ptree->cur = ptree->cur->prev;
            ptree->top = ptree->top->prev;
            ptree->topnum--;
        }
    }
    return YES;
}

bool    trend (SLTREE * ptree) {
    register    i, n = ptree->total - ptree->topnum;

    if (ptree->cur->next == NULL) return NO;
    while (ptree->cur->next != NULL) ptree->cur = ptree->cur->next;
    if (n <= ptree->pwin->img.dim.h) ptree->row = n - 1;
    else {
        ptree->row = ptree->pwin->img.dim.h - 1;
        ptree->top = ptree->cur;
        ptree->topnum = ptree->total - 1;
        for (i = 0; i < ptree->pwin->img.dim.h - 1; i++) {
            ptree->top = ptree->top->prev;
            ptree->topnum--;
        }
    }
    return YES;
}

bool    trpgdn (SLTREE * ptree) {
    register    i, n = ptree->total - ptree->topnum;
    int         H = ptree->pwin->img.dim.h;

    if (ptree->cur->next == NULL) return NO;
    if (n <= H) {
        while (ptree->cur->next != NULL)  ptree->cur = ptree->cur->next;
        ptree->row = n - 1;
    } else if (n < (H - 1) * 2) {
        n -= H - 1;
        if (n > ptree->row) {
            for (i = 0; i < H - 1; i++)  ptree->cur = ptree->cur->next;
            ptree->row = H - (n - ptree->row);
        } else {
            while (ptree->cur->next != NULL) ptree->cur = ptree->cur->next;
            ptree->row = H - 1;
        }
        for (i = 0; i < n - 1; i++) {
            ptree->top = ptree->top->next;
            ptree->topnum++;
        }
    } else  {
        n = H - 1;
        for (i = 0; i < n; i++) {
            ptree->cur = ptree->cur->next;
            ptree->top = ptree->top->next;
            ptree->topnum++;
        }
    }
    return YES;
}

int trinshig (SLTREE * ptree, bool fld, char * name,
                                            bool (* fn) (SLNODE * pnode)) {
                                                  /* -1 Err, 0 - Esc, 1 - Ok */
    SLNODE  * high;
    SLNODE  * cur;
    SLNODE  * low;
    bool      ret;

    if ((low = ptree->cur) == ptree->root) return -1;
    high = low->prev;
    if ((cur = ininode (ptree->namlen, fld)) == NULL) return -1;

    cur->next  = low;
    cur->prev  = high;
    high->next = cur;
    low->prev  = cur;

    cur->father = low->father;
    cur->father->nsons++;
    ptree->total++;
    cur->level = cur->father->level + 1;
    trupdmsk (ptree, cur);
    if (ptree->row == 0) ptree->top = cur;
    ptree->cur = cur;

    if (name != NULL) { strcpy (cur->name, name); goto success; }

    trouttr (ptree, NO);
    ret = wnstredt (ptree->pwin, ptree->row, ptree->col, ptree->pwin->attr.afld,
                            ptree->pwin->attr.afld, cur->name, ptree->namlen);
    if (!ret || strlen (cur->name) == 0) { trdelnod (ptree, NULL); return 0; }
success:
    if (fn != NULL) (* fn) (cur);
    return 1;
}

int trinslow (SLTREE * ptree, bool fld, int where, char * name,
                                                bool (* fn) (SLNODE * pnode)) {
                                      /* where:  0 - brother, 1 - son */
                                      /* return:  -1 Err, 0 - Esc, 1 - Ok    */
    SLNODE  * high;
    SLNODE  * cur;
    SLNODE  * low;
    bool      ret;

    if (where < 0) return -1;
    if ((cur = ininode (ptree->namlen, fld)) == NULL) return -1;

    trpush (ptree);

    if (where == 0) high = getlastson (ptree);
    else            high = ptree->cur;
    low  = high->next;
    cur->prev = high;
    cur->next = low;
    if (low != NULL) low->prev = cur;
    high->next = cur;
    if (where == 1) {
        if (low != NULL) {
            if (low->father != high) cur->opt.last = YES;
        } else cur->opt.last = YES;
        cur->father = high;
        cur->level = cur->father->level + 1;
    } else {
        if (ptree->cur->opt.last) {
            cur->opt.last = YES;
            ptree->cur->opt.last = NO;}
        cur->father = ptree->cur->father;
        cur->level =  ptree->cur->level;
    }

    cur->father->nsons++;
    ptree->total++;
    trupdmsk (ptree, cur);
    if (where == 0) trupdmsk (ptree, ptree->cur);
    ptree->row++;
    for (; ptree->row > ptree->pwin->img.dim.h - 1; ptree->row--) {
        ptree->top = ptree->top->next;
        ptree->topnum++;
    }
    ptree->cur = cur;
    if (name != NULL) { strcpy (cur->name, name); goto success; }

    trouttr (ptree, NO);
    ret = wnstredt (ptree->pwin, ptree->row, ptree->col, ptree->pwin->attr.afld,
                            ptree->pwin->attr.afld, cur->name, ptree->namlen);
    if (!ret || strlen (cur->name) == 0)
        { trdelnod (ptree, NULL); trpop (ptree); return 0; }
success:
    if (fn != NULL) (* fn) (cur);
    return  1;
}

static SLNODE * ininode (int namlen, bool fld) {
    SLNODE * cur;

    if ((cur = utalloc (SLNODE)) ==  NULL) return NULL;
    cur->opt.fld = fld;
    if ((cur->name = calloc (namlen + 1, 1))== NULL) goto ret;
    return cur;
ret:
    free (cur->name);
    free (cur);
    return NULL;
}

void trupdmsk (SLTREE * ptree, SLNODE * cur) {
    SLNODE    * p = cur->next;
    SLNODE    * papa = cur;
    long        b = 1L;
    int         i;

    cur->pmask = 0L;    
    do {
        papa = papa->father;
        if (!papa->opt.last) cur->pmask += b;
        b = b << 1;
    }  while (papa != ptree->root);

    if (cur->opt.fld) return;
    for (i = cur->nsons; i > 0; i--) {
        trupdmsk (ptree, p);
        p = trnxtbrt (p);
    }
    return;
}

static  SLNODE * getlastson (SLTREE * ptree) {
    SLNODE  * pnode = ptree->cur;
    int       level = ptree->cur->level;

    while (pnode->next != NULL && pnode->next->level > level) {
        pnode = pnode->next;
        ptree->row++;
    }
    return pnode;
}

bool trup (SLTREE * ptree) {

    if (ptree->row > 0) {
        ptree->row--;
        ptree->cur = ptree->cur->prev;
    } else {
        if (ptree->top != ptree->root) {
            ptree->top = ptree->top->prev;
            ptree->topnum--;
            ptree->cur = ptree->top;
        }  else  return NO;
    }
    return YES;
}

bool    trdown (SLTREE * ptree) {

    if (ptree->row < ptree->pwin->img.dim.h - 1) {
        if (ptree->cur->next != NULL) {
            ptree->row++;
            ptree->cur = ptree->cur->next;
        } else return NO;
    } else {
        if (ptree->cur->next != NULL) {
            ptree->top = ptree->top->next;
            ptree->topnum++;
            ptree->cur = ptree->cur->next;
        } else return NO;
    }
    return YES;
}

void trdelnod (SLTREE * ptree, bool (* fn) (SLNODE * pnode)) {
    SLNODE  * high;
    SLNODE  * cur = ptree->cur;
    SLNODE  * low;
    SLNODE  * brt;


    if (cur == ptree->root) { utalarm (); return; }
    if (fn != NULL && !((* fn) (cur))) return;
    if (!cur->opt.fld && cur->nsons > 0) return;

    if (cur == ptree->ice) killice (ptree);

    high = cur->prev;
    low =  cur->next;
    cur->father->nsons--;
    ptree->total--;
    if (cur->opt.last && (brt = trprvbrt (cur)) != NULL) {
        brt->opt.last = YES;
        if (!brt->opt.fld) trupdmsk (ptree, brt);
    }
    if (low  != NULL) {
        low->prev  = high;
        ptree->cur = low;
        if (ptree->row == 0) ptree->top = low;
    } else  trup (ptree);
    high->next = low;
    free (cur->name);
    free (cur);
    return;
}

SLNODE * trprvbrt (SLNODE * pnode) {
    SLNODE  * papa = pnode->father;

    if (pnode->prev == pnode->father || pnode->prev == NULL) return NULL;
    do    { pnode = pnode->prev; count++; }
    while (pnode->father != papa);
    return pnode;
}

SLNODE * trnxtbrt (SLNODE * pnode) {
    SLNODE  * papa = pnode->father;

    if (pnode->opt.last) return NULL;
    do    { pnode = pnode->next; count++; }
    while (pnode->father != papa);
    return pnode;
}

bool    trmkroot (SLTREE * ptree, char * titl, char * name,
                                                bool (* fn) (SLNODE * pnode)) {
    SLNODE * proot;

    if ((proot = utalloc (SLNODE)) == NULL)  return NO;
    if ((proot->name = calloc (ptree->namlen + 1, 1)) == NULL) goto ret;
    if (name != NULL) strcpy (ptree->cur->name, name);
    else if (!wnstrinp (WN_CURMNU, WN_CURMNU, titl, proot->name,
                 ptree->namlen) || strlen (proot->name) == 0)  goto ret;
    if (fn != NULL && !((* fn) (proot))) goto ret;
    ptree->top = ptree->cur = proot;
    ptree->total = 1;       ptree->topnum = 0;
    proot->father = proot;
    proot->opt.last = YES;
    proot->level = -1;
    ptree->root = proot;
    return YES;
ret:
    free (proot->name);
    free (proot);
    return NO;
}

void    trpush (SLTREE * ptree) {

    trsavcur     = ptree->cur;
    trsavtop     = ptree->top;
    trsavrow     = ptree->row;
    trsavtopnum  = ptree->topnum;

    return;
}

void    trpop (SLTREE * ptree) {

    ptree->cur     = trsavcur;
    ptree->top     = trsavtop;
    ptree->row     = trsavrow;
    ptree->topnum  = trsavtopnum;

    return;
}

