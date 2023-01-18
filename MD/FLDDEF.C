#include    "makedoc.h"
#include    <io.h>
#include    <slkeyb.h>
#include    <slmous.h>
#include    <slsymb.def>
#include    <slmenu.h>
#include    <sledit.h>


#define         NPNO         (int) (curpno - firstpno)
#define         OUTTEXT      outtext (row,col,visicol,NPNO)

#define         IN_CUR       1
#define         IN_NEXT      0
#define         NO_FOUND    -1
#define         NO_VAL      -2

typedef struct stnode {
    SLNODE          * papa;
    bool              up;
    bool              down;
    int               nsons;
    int               lownsons;
    struct  stnode  * prev;
    struct  stnode  * next;
} STNODE;

static bool far _Cdecl mstrhand (int ev, int bs, int xp, int yp, int xd, int yd);
static void     outhlp          (void);
static void     undoline        (void);
static void     makestnode      (void);
static void     savedupl        (SLNODE * pnode);
static void     unlocknode      (SLNODE * first, SLNODE * last);
static SLNODE * nextfld         (SLNODE * pnode);
static STNODE * getlowvisibl    (void);
static void     finalwrite      (void);
static int      nextbrick       (void);
static bool     inipn           (void);
static bool     vfydark         (void);
static bool     inputdupl       (void);
static void     makedupl        (STNODE * stnode);
static void     frstnode        (void);
static bool     testline        (bool correct);
static bool     vfydupl         (STNODE * pstnode, int numrow);
static int      loaddupl        (SLNODE * pnode, int dupl);
void            reloadpno       (int npno);
void            outdoc          (WINDOW * pwdoc, EDT * pe);

static BORDER   nobord = { BORD_NO_BORDER, NULL, NULL, NULL };
static STNODE * crtlst;
static SLNODE * mult, * firstnode, * lastlock;
static bool     noload, nofirst, first_field, no_fld, no_brick;
static int      curpno, savpno, totalpnd, beginpno, polH;
static int      handotxt, handpno;
static int      handdtxt, handpnd, handtmpd;
static char   * pdup;
static int      row, col, visicol, dark, duprow;
static char     actsym[] = "";
static  int     y;


void ffield (void) {
    int       event;
    bool      (far * _Cdecl old_msfunc)();
    int        key, i, j, instr = 1, level = 0;
    int        savrow, savvisicol, where_brick = IN_CUR;
    int        oldrow, oldcol, oldvisicol, Wrow;
    WINDOW   * pwnmsg, * pwnhz1, * pwnhz2, * pwinhlp;
    SLNODE   * pnode;
    MDNODE   * puser;
    char far * pbuf, * pscreen1, * pscreen2;
    char far * sym = "─";
    bool       hide = NO, valline, txtwind = YES;
    bool       fld, darklock = NO, was_visibl = NO, needfirst = YES;

    if  (access (mdotxt, 0) != 0) return;
    if  (ptree->root == NULL &&
        !trmkroot (ptree, "Имя документа", NULL, inipuser)) return;
    valdoc = noload = nofirst = no_fld = no_brick = NO;
    first_field = YES;
    col = totalpnd = 0;
    if (!inipn ()) goto err;
    i = (NUM_ROWS - 4) / 2;
    polH = j = NUM_ROWS - 4 - i;
    pwnmsg    = wncreate (1, NUM_COLS, 0,     0, &nobord, &Att_Khlp);
    pwnhz1    = wncreate (1, NUM_COLS, 1,     0, &nobord, &dcb.att);
    pwind     = wncreate (j, NUM_COLS, 2,     0, &nobord, &dcb.att);
    pwnhz2    = wncreate (1, NUM_COLS, j + 2, 0, &nobord, &Att_Tree);
    ptree->pwin = wncreate (i, NUM_COLS, j + 3, 0, &nobord, &Att_Tree);

    if (nextbrick () == NO_FOUND) {
        mnwarnin (NULL, 0, 3, " ", "Текст не содержит полей", " ");
        goto next;
    }
    savpno = curpno;
    pwinhlp = wnfunkey (outhlp, YES);
    wndsplay (pwnmsg,    WN_NATIVE, WN_NATIVE);
    wndsplay (pwnhz1,    WN_NATIVE, WN_NATIVE);
    wndsplay (pwind,     WN_NATIVE, WN_NATIVE);
    wndsplay (pwnhz2,    WN_NATIVE, WN_NATIVE);
    wndsplay (ptree->pwin, WN_NATIVE, WN_NATIVE);
    pscreen1 = wnviptrl (1, 0);
    wnvicopy (&sym,&pscreen1,1,NUM_COLS,NUM_COLS*2,pwind->attr.text, Cmd[3]);
    pscreen1 = wnviptrl (j + 2, 0);
    pscreen2 = wnviptrl (j + 2, NUM_COLS - 2);
    OUTTEXT;

    lastlock = ptree->root;
    mult = NULL;
    crtlst = utalloc (STNODE);
    crtlst->up = YES;
    crtlst->papa = ptree->root;

    trhome (ptree);
    unlocknode (ptree->root, NULL);


    if ((ptree->ice = nextfld (ptree->cur)) == NULL) {
        trouttr (ptree, hide);
        while ((i = trinslow (ptree, YES, 1, NULL, inipuser)) != 1)
            if (i == -1)    goto ret;
        makeice (ptree);
    } else  trgo2nod (ptree, ptree->ice);
    firstnode  = NULL;
    event = MS_LEFT_PRESS|MS_RIGHT_PRESS|MS_CENTER_PRESS|MS_MOVE;
    old_msfunc = msshadow (&event, mstrhand);
    Wrow = ptree->pwin->where_shown.row;
    while (1) {
        wnvicopy (&sym,&pscreen1,1,NUM_COLS, NUM_COLS*2,Att_Tree.text,Cmd[3]);
        if (txtwind) pbuf = (char far *) &actsym[0];
        else         pbuf = (char far *) &actsym[1];
        wnvicopy (&pbuf, &pscreen2, 1, 1, NUM_COLS*2, Att_Tree.high, Cmd[3]);
        if (!hide) {
            savvisicol = visicol;
            savrow     = row;
        }
        trouttr (ptree, hide);
        y = Wrow + ptree->row;
        mssetpos (y, 8);
        key = kbgetkey (outhlp);
        kbflushb ();
        if (hide && key != ShiftLeft && key != ShiftRight &&
                    key != ShiftUp   && key != ShiftDown) {
            hide = NO;
            if (txtwind) {
                visicol = savvisicol;
                row = savrow;
                reloadpno (curpno);
                OUTTEXT;
            }
            continue;
        }
        switch (key) {
            case GrayShiftEnter:
            case ShiftEnter:
                vfytree ();
                if (!valtree) continue;
                if (instr == 1 || where_brick != IN_CUR) goto errline;
                if (!no_fld) trgo2nod (ptree, ptree->ice);
                else         trend  (ptree);
                trouttr (ptree, hide);
                if (darklock) goto errline;
                dark++;
                goto label;
            case GrayCtrlEnter:
            case CtrlEnter:
            case GrayEnter:
            case Enter:
                if (no_fld) goto errline;
                vfytree ();
                if (!valtree) continue;
                trgo2nod (ptree, ptree->ice);
                puser = (MDNODE *) ptree->cur->puser;
                puser->idl.npn = curpno;
                trouttr (ptree, hide);
                if (key == Enter || key == GrayEnter) optbox (ptree->cur);
                if (no_brick && puser->idf.opt.visibl) goto errline;
                if (firstnode != NULL)  {
                    valline = testline (NO);
                    if (puser->idf.opt.visibl) {
                        if (where_brick != IN_CUR || !was_visibl && !valline) {
                            if (!inputdupl ()) goto errline;
                            needfirst = YES;
                        }
                        was_visibl = YES;
                    } else  if (!valline ||
                           where_brick != IN_CUR && ptree->cur->level > level) {
                        if (was_visibl || !inputdupl ()) goto errline;
                        needfirst = YES;
                        where_brick = IN_CUR;
                        OUTTEXT;
                    }
                } else was_visibl = puser->idf.opt.visibl;
                if (needfirst) {
                    firstnode = ptree->ice;
                    ((MDNODE *) firstnode->puser)->idf.opt.first = YES;
                    savpno     = curpno;
                    oldrow     = row;
                    oldcol     = col;
                    oldvisicol = visicol;
                    instr      = 1;
                    level      = 0;
                    darklock   = NO;
                    needfirst  = NO;
                } else if (ptree->ice != firstnode)
                    ((MDNODE *) ptree->ice->puser)->idf.opt.first = NO;
                makestnode ();
                if (!puser->idf.opt.multy || !puser->idf.opt.visibl)
                    puser->idd.num = 0;

                lastlock = ptree->cur;
                if (dark > 0 || mult != NULL || ptree->cur->level < level)
                    darklock = YES;
                if (dark > 0 && mult == NULL && ptree->cur->level >= level)
                    goto errline;
                if (!testline (YES)) goto errline;
                ptree->cur->opt.lock = YES;
                level = (getlowvisibl ())->papa->level + 1;

                if (puser->idf.opt.visibl) puser->idf.instr = instr;
                else                       puser->idf.instr = 0;
                if ((ptree->ice = nextfld (ptree->cur)) == NULL) no_fld = YES;
                else                             trgo2nod (ptree, ptree->ice);
                if (no_fld && no_brick) {
                    if (!inputdupl ()) goto errline;
                    valdoc = YES;
                    goto ret;
                }

                if (!((MDNODE *) lastlock->puser)->idf.opt.visibl) break;
label:
                if (!no_brick) where_brick = nextbrick ();
                if (where_brick == NO_VAL) goto errline;
                if (where_brick == IN_CUR) instr++;
                else {
                    was_visibl = NO;
                    ((MDNODE *) firstnode->puser)->idl.places    = instr;
                }
                if (where_brick == NO_FOUND) {
                    if (no_fld) {
                        if (!inputdupl ()) goto errline;
                        valdoc = YES;
                        goto ret;
                    } else {
                        no_brick = YES;
                        reloadpno (savpno);
                        curpno = savpno;
                        col = 32000;
                    }
                }

                OUTTEXT;
                break;
errline:
            case    CtrlBS:
                    utalarm ();
                    if (firstnode == NULL) break;
                    where_brick = IN_CUR;
                    reloadpno  (savpno);
                    curpno = savpno;
                    unlocknode (firstnode, ptree->cur);
                    undoline ();
                    makeice (ptree);
                    was_visibl = NO;
                    if (oldcol != 32000) no_brick   = NO;
                    no_fld     = NO;
                    darklock   = NO;
                    lastlock   = firstnode;
                    row        = oldrow;
                    col        = oldcol;
                    visicol    = oldvisicol;
                    instr      = 1;
                    level      = 0;
                    OUTTEXT;
                    break;
            case    AltF10:      goto ret;
            case    ShiftLeft:
                if (txtwind) {
                    if (visicol > 0) { visicol--; hide = YES; OUTTEXT; }
                } else {
                    if (trsleft (ptree))  hide = YES;
                    else                  utalarm ();
                }
                break;
            case    ShiftRight:
                if (txtwind) { visicol++; hide = YES; OUTTEXT; }
                else
                    if (trsright (ptree))  hide = YES;
                break;
            case    ShiftUp:
                if (txtwind) {
                    if (row == NPNO) {
                        if (firstpno == 0) { utalarm (); break; }
                        reloadpno (firstpno);
                    }
                    row++;
                    hide = YES;
                    OUTTEXT;
                }
                else {
                    if (trsup (ptree))  hide = YES;
                    else                utalarm ();
                }
                break;
            case    ShiftDown:
                if (txtwind) {
                    if (pwind->img.dim.h-1-row == (int) (lastpno - curpno)) {
                        if (lastpno == totalpno - 2L) { utalarm (); break; }
                        reloadpno (lastpno);
                    }
                    row--;
                    hide = YES;
                    OUTTEXT;
                }
                else {
                    if (trsdown (ptree))  hide = YES;
                    else                  utalarm ();
                }
                break;
            case    CtrlEnd:    if (!trcend    (ptree))  utalarm ();     break;
            case    CtrlHome:   if (!trchome   (ptree))  utalarm ();     break;
            case    CtrlPgUp:   if (!trcpgup   (ptree))  utalarm ();     break;
            case    CtrlPgDn:   if (!trcpgdn   (ptree))  utalarm ();     break;
            case    Up:         if (!trup      (ptree))  utalarm ();     break;
            case    PgDn:       if (!trpgdn    (ptree))  utalarm ();     break;
            case    Down:       if (!trdown    (ptree))  utalarm ();     break;
            case    PgUp:       if (!trpgup    (ptree))  utalarm ();     break;
            case    Home:       if (!trhome    (ptree))  utalarm ();     break;
            case    End:        if (!trend     (ptree))  utalarm ();     break;
            case    F1:         txtwind = !txtwind;                      break;
            case    F2:         vfytree        ();                       break;
            case    F3:         trtbleft       (ptree);                  break;
            case    F4:         trtbrght       (ptree);                  break;
            case    F5:
            case    F6:
                if (key == F6) fld = YES;
                else           fld = NO;
                if (ptree->cur->opt.lock) { utalarm (); break; }
                if (!ptree->cur->opt.fld) {
                    pnode = nextfld (ptree->cur);
                    if (pnode != NULL && pnode->opt.lock) { utalarm (); break; }
                }
                if ((j = trinshig (ptree, fld, NULL, inipuser)) == -1)
                    utalarm ();
                else if (j == 1 && fld && tricenxt (ptree))   makeice (ptree);
                break;
            case    F7:
            case    F8:
                if (key == F8) fld = YES;
                else           fld = NO;
                if ((pnode = nextfld (ptree->cur)) == NULL) {
                    j = trinslow (ptree, fld, whereins (fld), NULL, inipuser);
                    if (j == 1 && fld && no_fld) {
                        makeice (ptree);
                        no_fld = NO;
                    }
                    break;
                }
                if (ptree->cur->opt.lock || ptree->cur == ptree->root) {
                    if (ptree->cur->father->opt.lock)  { utalarm (); break; }
                    if (ptree->cur->opt.fld) {
                        if (pnode->opt.lock)           { utalarm (); break; }
                        j = trinslow (ptree, fld, 0, NULL, inipuser);
                    } else {
                        pnode = trnxtbrt (ptree->cur);
                        if (pnode != NULL && !pnode->opt.fld)
                            pnode = nextfld (pnode);
                        if (pnode != NULL && pnode->opt.lock)
                            { utalarm (); break; }
                        j = trinslow (ptree, fld, (ptree->cur == ptree->root),
                                                               NULL, inipuser);
                    }
                } else  j = trinslow (ptree, fld,
                    ((pnode->opt.lock) ? 0 : whereins (fld)), NULL, inipuser);

                if (fld && j == 1 && (no_fld || tricenxt (ptree)))
                    { makeice (ptree); no_fld = NO; }
                break;
            case    F9:
                if (ptree->cur == ptree->ice) {
                    if ((pnode = nextfld (ptree->cur)) == NULL)
                        { utalarm (); break; }
                    trdelnod  (ptree, delnode);
                    while (!ptree->cur->opt.fld) trdown (ptree);
                    makeice (ptree);
                } else if (!ptree->cur->opt.lock) trdelnod (ptree, delnode);
                else utalarm ();
                break;
            case    F10:        trrename (ptree);       break;
            default :                                   break;
        }
    }
err:
    mnwarnin (NULL, 0, 3, " ", "Ошибка чтения файла", " ");
    goto next;
ret:
    finalwrite ();
    frstnode ();
    killice (ptree);
    msshadow (&event, old_msfunc);
next:
    utclose (handpno);    utclose (handotxt);
    utclose (handpnd);    utclose (handdtxt);
    utclose (handtmpd);

    utremove (mddtxt);

    utrename  (mdtmpd, mddtxt);

    free (pno);   pno   = NULL;
    free (potxt); potxt = NULL;

    wndstroy (pwnmsg);
    wndstroy (pwnhz1);
    wndstroy (pwind);
    wndstroy (pwnhz2);
    wndstroy (ptree->pwin); ptree->pwin = NULL;
    wndstroy (pwinhlp);

    return;
}

static STNODE  * getlowvisibl (void) {
    STNODE  * pstnode = crtlst;

    while (pstnode->next != NULL && pstnode->nsons == 0)
        pstnode = pstnode->next;
    return pstnode;
}

static bool  inputdupl (void) {
    STNODE  * stnode = crtlst;
    MDNODE  * puser;
    bool      first = YES;

    if (!vfydark ()) return NO;
    if (mult != NULL) {
        ((MDNODE *) firstnode->puser)->idl.numdark = dark + 1;
        makedupl (NULL);
        mult = NULL;
        first = NO;
    }

    while (stnode->next != NULL) {
        puser = (MDNODE *) stnode->papa->puser;
        if (stnode->nsons != 0) {
            if (first) {
                if (stnode->lownsons != -1)
                    ((MDNODE *) firstnode->puser)->idl.numdark =
                                                    stnode->lownsons + dark;
                first = NO;
            }
            makedupl (stnode);
            puser->copt.visibl = YES;
        } else {
            puser->idd.num     = 0;
            puser->copt.visibl = NO;
        }
        stnode->up = YES;
        stnode = stnode->next;
        if (!crtlst->down) {
            stnode->prev = NULL;
            free (crtlst);
            crtlst = stnode;
        }
    }
    reloadpno (curpno);
    dark = 0;
    return YES;
}

static bool   vfydark (void) {
    SLNODE      * pnode = ptree->cur;
    STNODE      * pstnode, * pst2;

    pstnode = getlowvisibl ();
    pst2 = pstnode->next;

    while (1) {
        ((MDNODE *) pnode->puser)->copt.dark = 0;
        if (pnode == firstnode) break;
        pnode = pnode->prev;
    }
    if (!pstnode->up) ((MDNODE *) pstnode->papa->puser)->copt.dark = NO;
    if (mult != NULL) {
        ((MDNODE *) mult->puser)->copt.dark = YES;
        if (!pstnode->up) pstnode->lownsons = 1;
    }

/****** Поля кортежа выше или ниже текущей строки, а есть "темные" поля   *****/
    else {
        if (pst2 == NULL) return YES;
        if ((pstnode->up || pstnode->down) && dark > 0) return NO;
        if (pstnode->up) return YES;
        if (pstnode->down)   pstnode->lownsons = -1;
        else {
            ((MDNODE *) pstnode->papa->puser)->copt.dark = YES;
            pstnode->lownsons = pstnode->nsons;
        }
/****** если количество "темных" полей некратно количеству полей кортежа  *****/
        if (dark % pstnode->nsons != 0) return NO;
    }

    while (pst2 != NULL) {
        if (!pst2->up) {
            pst2->lownsons = pstnode->lownsons;
            ((MDNODE *) pst2->papa->puser)->copt.dark = NO;
        }
        pst2 = pst2->next;
    }
    return YES;
}

static bool   testline (bool correct) {
    SLNODE  * pnode = ptree->cur;
    STNODE  * pstnode = crtlst;
    MDNODE  * puser = (MDNODE *) pnode->puser;

/***** когда в строке имеется более одного множественного либо  ************/
/*****    текстового поля или их комбинация                     ************/
    if (correct)
        if (puser->idf.opt.visibl && puser->idf.opt.multy) {
            if (mult != NULL) return NO;    /* Хочим вякнуть */
            mult = pnode;
        }

    if (mult != NULL && crtlst->papa->level != mult->father->level) return NO;
    while (pstnode != NULL) {
        if (pstnode->papa != pnode->father) {
/***** когда в строке имеются поля нескольких кортежей одного уровня *******/
            if (pstnode->papa->level + 1 == pnode->level)      return NO;
/***** если последнее поле кортежа находится в одной строке с полем ********/
/*****              кортежа более низкого level'a                   ********/
            if (pstnode->up && pstnode->papa->level + 1 > pnode->level)
                return NO;
        }
        if (correct) {
            if (puser->idf.opt.visibl && !pstnode->up &&
                                  pstnode->papa->level <= pnode->father->level)
                pstnode->nsons++;

            if (pnode->next==NULL||pstnode->papa->level >= pnode->next->level)
                pstnode->down = NO;
        }
        pstnode = pstnode->next;
    }

    return YES;
}

static void makestnode (void) {
    STNODE      * pstnode;

    trgo2nod (ptree, lastlock);
    if (ptree->cur == ptree->ice) return;
    while (1) {
        trdown (ptree);
        if (ptree->cur == ptree->ice) return;
        pstnode = utalloc (STNODE);
        pstnode->next   = crtlst;
        crtlst->prev    = pstnode;
        crtlst          = pstnode;
        crtlst->papa    = ptree->cur;
        crtlst->down    = YES;
    }
}

static void   undoline (void) {
    STNODE  * pstnode;

    if (lastlock == NULL) trend (ptree);
    else                  trgo2nod (ptree, lastlock);

    while (1) {
        if (!ptree->cur->opt.fld) {
            crtlst = crtlst->next;
            free (crtlst->prev);
            crtlst->prev = NULL;
        }
        if (ptree->cur == firstnode) break;
        trup (ptree);
    }
    for (pstnode = crtlst; pstnode != NULL; pstnode = pstnode->next) {
        if (!pstnode->up) pstnode->nsons = 0;
        pstnode->down = YES;
    }
    mult = NULL; dark = 0;
    return;
}

static void   frstnode (void) {
    STNODE  * pstnode = crtlst;

    if (pstnode == NULL) return;

    while (1) {
        if (pstnode->next == NULL) break;
        pstnode = pstnode->next;
        free (pstnode->prev);
    }
    free (pstnode); crtlst = NULL;
    dark = 0;   mult = NULL;
    return;
}

static void   unlocknode (SLNODE * first, SLNODE * last) {

    while (first != NULL) {
        first->opt.lock = NO;
        if (first == last) return;
        first = first->next;
    }
    return;
}

static int    nextbrick (void) {
    char    * p;
    int       ret = IN_CUR;
    int       W_5 = pwind->img.dim.w - 5;
    int       H = pwind->img.dim.h;

    savpno = curpno;
    visicol = 0;
    if (first_field) {
        row = 0;
        curpno = 0;
        reloadpno (0);
        p = potxt;
        first_field = NO;
    } else {
        p = potxt + pno[NPNO] + col;
        while ((byte) *p == 178) p++;
    }

    while (1) {
        if ((byte) *p == 178) break;
        if (*p == '\n') {
            if (curpno == lastpno) {
                reloadpno (lastpno);
                if (curpno == lastpno) return NO_FOUND;
            } else  ret = IN_NEXT;
            curpno++;
            p = potxt + pno[NPNO];
        } else p++;
    }
    if (ret == IN_NEXT && no_fld) return NO_VAL;
    col = (int) (p - (potxt + pno [NPNO]));
    if (col > W_5) visicol = col - W_5;
    if (ret == IN_NEXT) {
        if (lastpno - curpno < (long) H && lastpno != totalpno - 2L)
            reloadpno (curpno);
        row = H / 2;
        if (totalpno - curpno - 1L < (long) (H - row))
            row = H - (int) (totalpno - curpno) + 1;
        if (curpno < (long) row) row = (int) curpno;
    }
    return ret;
}

static void  finalwrite (void) {
    SLNODE      * pnode = ptree->ice;
    STNODE      * pstnode = crtlst;

    Ed_lsize = 32500;
    Ed_ppl = utallocb (&Ed_lsize);
    if (mult != NULL) savedupl (mult);
    while (pstnode->papa != ptree->root) {
        savedupl (pstnode->papa);
        pstnode = pstnode->next;
    }
    while (pnode != NULL) {
        savedupl (pnode);
        pnode = pnode->next;
    }

    free (Ed_ppl);
    return;
}

static void savedupl (SLNODE * pnode) {
    register      length;
    MDNODE      * puser = (MDNODE *) pnode->puser;

    length = loaddupl (pnode, 1);
    puser->idd.npn = totalpnd;
    utwrite (handtmpd, Ed_ppl + 3, length);
    totalpnd += puser->idd.num;
    return;
}

static bool     vfydupl (STNODE * pstnode, int numrow) {
    IDLIN     * pidd = &(((MDNODE *) pstnode->papa->puser)->idd);
    char      * p = Ed_ppl + 3;
    register    row = 1, i = 0;
    int         places;

    if (numrow == 0) return YES;
    while (row < numrow) {
        while (*p != '\n') {
            if (*p == '▓') return NO;
            p++;
        }
        row++; p++;
    }
    while (*p != '\n') {
        if (*p == '▓') {
            while (*p == '▓') p++;
            i++;
        } else p++;
    }
    if (i == 0) return NO;
    places = i;
    if (pstnode != NULL) {
        if (pstnode->lownsons != -1) {
            i -= pstnode->nsons;
            if (i < 0) return NO;
            if (i % pstnode->lownsons != 0) return NO;
            pidd->numdark    = i + pstnode->lownsons;
        } else {
            if (i != pstnode->nsons)  return NO;
            pidd->numdark    = 0;
        }
        pidd->places = (byte) places;
    } else ((MDNODE *) mult->puser)->idd.places =
                ((MDNODE *) mult->puser)->idd.numdark = (byte) places;

    return YES;
}

static int      loaddupl (SLNODE * pnode, int dupl) {
    MDNODE * puser = (MDNODE *) pnode->puser;
    word   * pword;
    long     offset;
    int      len, num, handpn, handtxt;

    if (dupl == 0) {
        num     = 1;                       handpn  = handpno;
        offset  = beginpno;                handtxt = handotxt;
    } else {
        num     = puser->idd.num;          handpn  = handpnd;
        offset  = puser->idd.npn;          handtxt = handdtxt;
    }

    if (handpn == -1 || handtxt == -1) return 0;
    offset *= sizeof (word);
    pword = (word *) (Ed_ppl + 3);

    utlseek (handpn, offset, UT_BEG);
    utread  (handpn, Ed_ppl + 3, (num + 1) * sizeof (word));
    len = (int) (pword [num] - pword [0]);
    utlseek (handtxt, (long) pword [0], UT_BEG);
    utread  (handtxt, Ed_ppl + 3, len);

    return len;
}

void     reloadpno (int npno) {
    word            txtsize, begtxt;
    int             numpno = SL_NUMPN;
    register        i;

    if (noload) return;
    if (!nofirst) {
        if (totalpno <= numpno) { numpno = totalpno; noload = YES; }
        firstpno = 0; lastpno = numpno - 2;
        nofirst = YES;
    } else {
        firstpno = npno - numpno / 2;
        lastpno  = npno + numpno / 2 - 1;
        if (firstpno < 0) { lastpno -= firstpno; firstpno = 0; }
        if (lastpno > totalpno - 2) {
            firstpno -= lastpno - totalpno + 2;
            lastpno = totalpno - 2;
        }
        numpno = lastpno - firstpno + 2;
    }

    utfree (&pno);
    utfree (&potxt);

    if ((pno  = malloc (numpno * sizeof (word))) == NULL)  goto ret;

    utlseek (handpno, firstpno * sizeof (word), UT_BEG);

    utread (handpno, pno, numpno * sizeof (word));
    begtxt = pno [0];
    for (i = numpno - 1; i >= 0; i--) pno [i] -= begtxt;
    txtsize = pno [numpno - 1];
    utlseek (handotxt, (long) begtxt, UT_BEG);

    if ((potxt = malloc (txtsize)) == NULL) goto ret;

    utread (handotxt, potxt, txtsize);

    return;
ret:
    free (pno);
    free (potxt);
    return;
}

static  bool   inipn (void) {
    word         sizefrom = 32000, sizeto = sizefrom;
    char far   * pfrom, * pto;
    bool         retcode = NO;

    wndiswrk (2L, 3, " ", "Компиляция документа", " ");

    if ((pfrom = utallocb (&sizefrom)) == NULL) goto ret;

    if ((pto = utallocb (&sizeto)) == NULL) goto ret;
    sizeto = (sizeto / 4) * 4;


    if ((handotxt = utopen (mdotxt, UT_R)) == -1) goto ret;
    handpno = utcreate (mdpno);
    totalpno = makepn (handpno, pto, sizeto, handotxt, pfrom,
                                                utfsize (handotxt), sizefrom);
    wnupdwrk (1L);

    handdtxt = utopen (mddtxt, UT_R);
    if (handdtxt != -1)  {
        handpnd  = utcreate (mdpnd);
        makepn (handpnd, pto, sizeto, handdtxt, pfrom,
                                                 utfsize (handdtxt), sizefrom);
    }
    wnupdwrk (1L);

    handtmpd = utcreate (mdtmpd);
    utsleept (8);
    retcode = YES;
ret:
    wnremwrk ();
    free (pfrom);
    free (pto);
    return retcode;
}

static char * tcrt = " кортежа─────<";
static char * tfld = " поля─────<";
static char * tdup = "Шаблон для";
static char * tend = ">";

static void  makedupl (STNODE * stnode) {
    char      * ptbuf;
    bool        need = YES, multy = YES;
    int         duplen, savrow = row;
    SLNODE    * papa, * pnode;
    MDNODE    * puser;


    Ed_lsize = /*32500*/2500;
    pdup = utallocb (&Ed_lsize);
    ptbuf = calloc (80, 1);

    if (savpno >= 5) row = 5;
    else             row = savpno;

    if (stnode == NULL) papa = pnode = mult;
    else { papa = stnode->papa; pnode = nextfld (papa); need = !stnode->down; }

    puser = (MDNODE *) papa->puser;
    if (need) {
        beginpno = ((MDNODE *) (pnode->puser))->idl.npn;
        Ed_ppl = pdup;
        if ((duprow = puser->idd.num) > 0)   duplen = loaddupl (papa, 1);
        else {
lab:
            duplen = loaddupl (papa, 0);
            duprow = 1;
        }
        strcpy (ptbuf, tdup);
        if (papa->opt.fld)  strcat (ptbuf, tfld);
        else                strcat (ptbuf, tcrt);
        strcat (ptbuf, papa->name);
        strcat (ptbuf, tend);

        do  {
            duprow = edtdupl (&duplen, row, 0, duprow, ptbuf, YES);
            if (duprow == 0) goto lab;
        } while (!vfydupl (stnode, duprow));

        puser->idd.npn = totalpnd;     /* Новые параметры шаблонной строки */
        puser->idd.num = duprow;
        utwrite (handtmpd, Ed_ppl + 3, duplen);
        totalpnd += duprow;
        papa->opt.lock = YES;
        if (!papa->opt.fld && !stnode->up) {
            pnode = papa->next;
            while (pnode != NULL) {
                if (!pnode->opt.fld) {
                    if (pnode->level > papa->level) multy = NO;
                    break;
                } else {
                    if (pnode->level <= papa->level) break;
                    if (((MDNODE *) pnode->puser)->idf.opt.multy)
                        { multy = NO; break; }
                }
                if (!((MDNODE *) pnode->puser)->idf.opt.wrap)
                    { multy = NO; break; }
                pnode = pnode->next;
            }
            puser->copt.multy = multy;
        }
    }

    free (ptbuf);
    free (pdup);
    row = savrow;
    return;
}

void    outdoc  (WINDOW * pwdoc, EDT * pe) {
    byte       txtatt  = pwdoc->attr.text;      /* Атрибут текста */
    int        nrow = row, len, npno;
    int        H = pwdoc->img.dim.h, H_edt = Ed_pwin->img.dim.h;
    int        W = pwdoc->img.dim.w;
    char far * pbuf, * pscreen;
    char     * p;
    CELLSC   * pdata;
    register   j;

    /*************  Вывод строк над (и с) текущим полем ********************/
    if (utrange (savpno, firstpno + row, lastpno)) reloadpno (savpno);
    npno = savpno - firstpno;

        /***** Вывод оригинала доверху *****/

    for (; nrow >= 0; nrow--, npno--) {
        pdata = pwdoc->img.pdata + nrow * W;
        p = potxt + pno [npno] + pe->visicol;
        len = pno [npno + 1] - pno [npno] - 1 - pe->visicol;
        utbound (len, 0, W);
        wnmovpsa (pdata, p,  len, txtatt);
        wnmovpca (pdata + len, ' ', W - len, txtatt);
    }
    nrow = row + 1;
    pbuf    = (char far *) pwdoc->img.pdata;
    pscreen = wnviptrl (pwdoc->where_shown.row, pwdoc->where_shown.col);
    wnvicopy (&pbuf, &pscreen, nrow, W, NUM_COLS * 2, 0, Cmd[1]);

    /************** Вывод редактора ***********************************/

    pe->needout = 0;
    if (pe->total != 0) {
        outscr (pe);
        nrow += H_edt;
    }
    /*************  Вывод строк под текущим полем ********************/

    j = nrow;
    pbuf    = (char far *) (pwdoc->img.pdata + j * W);
    pscreen = wnviptrl (pwdoc->where_shown.row + j, pwdoc->where_shown.col);

    if (utrange (beginpno, firstpno, lastpno - (H-nrow))) reloadpno (beginpno);
    npno = beginpno - firstpno + 1;

    for (; nrow < H && firstpno + npno <= lastpno; nrow++, npno++) {
        pdata = pwdoc->img.pdata + nrow * W;
        p = potxt + pno [npno] + pe->visicol;
        len = pno [npno + 1] - pno [npno] - 1 - pe->visicol;
        utbound (len, 0, W);
        wnmovpsa (pdata, p,  len, txtatt);
        wnmovpca (pdata + len, ' ', W - len, txtatt);
    }
    wnmovpca (pwdoc->img.pdata + nrow * W, ' ', (H - nrow) * W, txtatt);
    wnvicopy (&pbuf, &pscreen, H - j, W, NUM_COLS * 2, 0, Cmd[1]);
    return;
}

static SLNODE   * nextfld (SLNODE * pnode) {

    if (pnode != NULL)
        while ((pnode = pnode->next) != NULL)
            if (pnode->opt.fld) break;
    return pnode;
}

#pragma warn -par
static bool far _Cdecl mstrhand (int ev, int bs, int xp, int yp, int xd, int yd) {
    bool        ret = YES;

    kbflushb ();
    Kb_2_tail = Kb_2_head;

    switch (ev) {
        case MS_LEFT_PRESS:      kbsetsym (Enter);             break;
        case MS_RIGHT_PRESS:     kbsetsym (AltF10);            break;
        case MS_CENTER_PRESS:    kbsetsym (F1);                break;
        default :
            if (yp < y)             kbsetsym (Up);
            else if (yp > y)        kbsetsym (Down);
            else ret = NO;
            break;
    }
    return ret;
}
#ifdef TC_lang
#pragma warn +par
#endif

/*        10        20        30        40        50        60        70
0123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
1xxxxxx 2xxxxxx 3xxxxxx 4xxxxxx 5xxxxxx 6xxxxxx 7xxxxxx 8xxxxxx 9xxxxxx 10xxxxxx
*/
static  char fkey[] =
"1WinSwt 2Verify 3LvTab\33 4LvTab\32 5CrIns\30 6FlIns\30 7CrIns\31 8FlIns\31 9Delate 10Rename";
static  char shiffkey[] =
"1       2       3       4       5       6       7       8       9       10      ";
static  char ctrlfkey[] =
"1       2       3       4       5       6       7       8       9       10      ";
static  char altfkey[] =
"1       2       3       4       5       6       7       8       9       10Quit  ";

static void outhlp (void) {
    char far    * pbuf, *pscreen;

    if      (Kb_sym & KB_ALTSTAT)   pbuf = (char far *) altfkey;
    else if (Kb_sym & KB_SHIFTSTAT) pbuf = (char far *) shiffkey;
    else if (Kb_sym & KB_CTRLSTAT)  pbuf = (char far *) ctrlfkey;
    else                            pbuf = (char far *) fkey;
    pscreen = wnviptrl (NUM_ROWS - 1, 0);
    wnvicopy (&pbuf, &pscreen, 1, NUM_COLS, NUM_COLS * 2, 0, Cmd[0]);
    return;
}

