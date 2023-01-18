#include   <slmenu.h>


SPECPRN       specprn = { 20, YES };

static BORDER  mnubrd = { BORD_DDSS|BORD_SHADOW, NULL, NULL, NULL };

static void     makebrk (PAGBRK * curbrk, long remain);
static PAGBRK * insbrk  (PAGBRK * curbrk);
static void     delbrk  (PAGBRK * curbrk);
static bool     vfypag  (void);


static char * noyes [] = { "¥â", " „ " };
static char * wnote [] = { "   ¥â", "‘¢¥àåã", " ‘­¨§ã" };

static ITEM mirror = { " „¢ãáâ®à®­­ïï ¯¥ç âì     @@@      ", "„¤Ll", NULL   };
static ITEM ff     = { " à®£®­ «¨áâ             @@@      ", "ƒ£Uu", NULL   };
static ITEM numcop = { " Š®«¨ç¥áâ¢® íª§¥¬¯«ïà®¢  @@@      ", "ŠªRr", "    " };
static ITEM linspac= { " Œ¥¦áâà®ç­ë© ¨­â¥à¢ «     @@      ", "Œ¬Vv", "   "  };
static ITEM newpage= {
  " Š ¦¤ë© íª§¥¬¯«ïà ¯¥ç â âì á ­®¢®£® «¨áâ                     @@@      ",
                                                               "€ Ff", NULL };
static ITEM full   = {
  " ¥à §àë¢­®áâì íª§¥¬¯«ïà  ­  «¨áâ¥                           @@@      ",
                                                               "àHh", NULL };
static ITEM eq     = {
  "  ¢­®¬¥à­®¥ à á¯à¥¤¥«¥­¨¥ ¢á¥å íª§¥¬¯«ïà®¢ ­  ®¤­®¬ «¨áâ¥   @@@      ",
                                                               "ˆ¨Bb", NULL };
static ITEM copspac= {
  "  ááâ®ï­¨¥ ¬¥¦¤ã íª§¥¬¯«ïà ¬¨                                @@ (áâà)",
                                                               "ŸïZz", "   "};
static ITEM wid    = { " ˜¨à¨­                  @@@@ (¬¬) ", "˜èIi", "     "};
static ITEM hei    = { " ‚ëá®â                  @@@@ (¬¬) ", "‚¢Dd", "     "};
static ITEM left   = { " ‘«¥¢                   @@@@ (¬¬) ", "‹«Kk", "     "};
static ITEM right  = { " ‘¯à ¢                  @@@@ (¬¬) ", "¯Gg", "     "};
static ITEM top    = { " ‘¢¥àåã                 @@@@ (¬¬) ", "‘áCc", "     "};
static ITEM bot    = { " ‘­¨§ã                  @@@@ (¬¬) ", "­Yy", "     "};
static ITEM style  = { " ˜à¨äâ                            ", "”äAa", NULL   };
static ITEM note   = { " ‚ë¢®¤¨âì             @@@@@@      ", "›ëSs", NULL   };
static ITEM number = { " ã¬¥à æ¨ï áâà ­¨æ       @@@      ", "“ãEe", NULL   };
static ITEM tonote = { " Žâáâã¯                 @@@@ (¬¬) ", "Ž®Jj", "     "};
static ITEM preview= { "        §¡¨¢ª  ¯® áâà ­¨æ ¬      ", "àHh", NULL   };
static ITEM prn    = { "             ¥ç â âì             ", "—çXx", NULL   };
static ITEM note1  = {
    " ‚ë¢®¤¨âì ­  ¯¥à¢®© áâà ­¨æ¥                                 @@@      ",
                                                                "…¥Tt", NULL  };
static ITEM text   = {
    " ’¥ªáâ: @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
                                                                "’âNn", NULL  };

static PRNPAG * prnpag;
static long     total;
static int      numstr;

bool    mnprnpag (PRNPAG * prnpg, ATTRIB * pattrib, void (* fn) (void),
                                                       long tot, bool prnt) {
    PAGBRK    * curbrk;
    MENU      * pmenu;
    MNITEM    * pnewpage, * pfull, * pcopspac;
    char      * p;
    bool        done, retcode, eql;
    long        i;
    int         row, col, len, * pi;

    pmenu = mncreate ((prnt) ? 19 : 17, 72, 0, 0, &mnubrd, pattrib);

    prnpag = prnpg;
    total  = tot;
    prnpag->opt.brk = (fn != NULL);
    if (fn != NULL && prnt && prnpag->firstbrk == NULL) {
        prnpag->firstbrk = utalloc (PAGBRK);
        curbrk = utalloc (PAGBRK);
        curbrk->fix = prnpag->firstbrk->fix = YES;
        curbrk->line = total;
        curbrk->prev = prnpag->firstbrk;
        prnpag->firstbrk->next = curbrk;
    }
    mirror.vars  = noyes [prnpag->opt.mirror];
    ff.vars      = noyes [prnpag->opt.ff];
    number.vars  = noyes [prnpag->opt.number];
    note.vars    = wnote [prnpag->opt.note];
    note1.vars   = noyes [prnpag->opt.note1st];
    full.vars    = noyes [prnpag->opt.full];
    eq.vars      = noyes [prnpag->opt.eq];
    newpage.vars = noyes [prnpag->opt.newpage];
    if (prnpag->numcop < 1) prnpag->numcop = 1;
    utl2crdx ((long) prnpag->numcop, numcop.vars, 3, 10);
    utl2crdx ((long) prnpag->linspac, linspac.vars, 2, 10);
    utl2crdx ((long) prnpag->copspac, copspac.vars, 2, 10);
    if (prnpag->wid < 1) prnpag->wid = 210;
    utl2crdx ((long) prnpag->wid, wid.vars, 4, 10);
    if (prnpag->hei < 1) prnpag->hei = 270;
    utl2crdx ((long) prnpag->hei, hei.vars, 4, 10);
    utl2crdx ((long) prnpag->left, left.vars, 4, 10);
    utl2crdx ((long) prnpag->right, right.vars, 4, 10);
    utl2crdx ((long) prnpag->top, top.vars, 4, 10);
    utl2crdx ((long) prnpag->bot, bot.vars, 4, 10);
    utl2crdx ((long) prnpag->tonote, tonote.vars, 4, 10);
    text.vars = prnpag->pnote;

    mnitmadd (pmenu,  1,  0,  1, &mirror);
    mnitmadd (pmenu,  2,  0, 37, &ff);
    mnitmadd (pmenu,  3,  1,  1, &numcop);
    mnitmadd (pmenu,  4,  1, 37, &linspac);
    mnitmadd (pmenu,  5,  3,  1, &newpage);
    mnitmadd (pmenu,  6,  4,  1, &full);
    mnitmadd (pmenu,  7,  5,  1, &eq);
    mnitmadd (pmenu,  8,  6,  1, &copspac);
    mnitmadd (pmenu,  9,  8,  1, &wid);
    mnitmadd (pmenu, 10,  8, 37, &hei);
    mnitmadd (pmenu, 11, 10,  1, &left);
    mnitmadd (pmenu, 12, 10, 37, &right);
    mnitmadd (pmenu, 13, 11,  1, &top);
    mnitmadd (pmenu, 14, 11, 37, &bot);
    mnitmadd (pmenu, 15, 13,  1, &style);
    mnitmadd (pmenu, 16, 13, 37, &note);
    mnitmadd (pmenu, 17, 14,  1, &number);
    mnitmadd (pmenu, 18, 14, 37, &tonote);
    mnitmadd (pmenu, 19, 15,  1, &note1);
    mnitmadd (pmenu, 20, 16,  1, &text);
    if (prnt) {
        if (fn != NULL) {
            mnitmadd (pmenu, 21, 18,  1, &preview);
            mnitmadd (pmenu, 22, 18,  37, &prn);
        } else  mnitmadd (pmenu, 22, 18, 18, &prn);
    }

    pnewpage = mnfnditm (pmenu->pitems, 5);
    pfull    = mnfnditm (pmenu->pitems, 6);
    pcopspac = mnfnditm (pmenu->pitems, 8);

    wnsplitw (pmenu->pwin,  2, WN_HSPLIT|WN_SINGLE);
    wnsplitw (pmenu->pwin,  7, WN_HSPLIT|WN_SINGLE);
    wnsplitw (pmenu->pwin,  9, WN_HSPLIT|WN_SINGLE);
    wnsplitw (pmenu->pwin, 12, WN_HSPLIT|WN_SINGLE);
    if (prnt) {
        wnsplitw (pmenu->pwin, 17, WN_HSPLIT|WN_SINGLE);
        mnsetcur (pmenu, 22);
    }
    mndsplay (pmenu, WN_CENTER, WN_CENTER);

    wnwrtbuf (pmenu->pwin,  2, 18, 35, "  á¯®«®¦¥­¨¥ íª§¥¬¯«ïà®¢ ­  «¨áâ¥ ",
                                                                    WN_NATIVE);
    wnwrtbuf (pmenu->pwin,  7, 26, 15, "  §¬¥àë «¨áâ  ", WN_NATIVE);
    wnwrtbuf (pmenu->pwin,  9, 30,  8, " Žâáâã¯ ",        WN_NATIVE);
    wnwrtbuf (pmenu->pwin, 12, 28, 11, " ‡ £®«®¢®ª ",     WN_NATIVE);

lab:
    done = retcode = NO;
    while (!done) {
        eql = prnpag->opt.eq;
        mnsetpro (pmenu, 5, eql);
        mnsetpro (pmenu, 6, eql);
        mnsetpro (pmenu, 8, eql);
        if (eql) {
            prnpag->opt.newpage = NO;
            pnewpage->vloc = noyes [0];
            mndisvri (pmenu, pnewpage);
            prnpag->opt.full = YES;
            pfull->vloc = noyes [1];
            mndisvri (pmenu, pfull);
            utmovsc (pcopspac->vloc, ' ', 2);
        } else utl2crdx ((int) prnpag->copspac, pcopspac->vloc, 2, 10);
        mndisvri (pmenu, pcopspac);
        if (prnt) mnsetpro (pmenu, 21,
                                (!prnpag->opt.newpage && prnpag->numcop > 1));
        pi = NULL;
        switch (mnreadop (pmenu, 0)) {
            case  1:
                prnpag->opt.mirror = !prnpag->opt.mirror;
                pmenu->pcur->vloc = noyes [prnpag->opt.mirror];
                break;
            case  2:
                prnpag->opt.ff = !prnpag->opt.ff;
                pmenu->pcur->vloc = noyes [prnpag->opt.ff];
                break;
            case  3: row = 1; col = 26; len = 3; pi = &prnpag->numcop;  break;
            case  4: row = 1; col = 63; len = 2; pi = &prnpag->linspac; break;
            case  5:
                prnpag->opt.newpage = !prnpag->opt.newpage;
                pmenu->pcur->vloc = noyes [prnpag->opt.newpage];
                break;
            case  6:
                prnpag->opt.full = !prnpag->opt.full;
                pmenu->pcur->vloc = noyes [prnpag->opt.full];
                break;
            case  7:
                prnpag->opt.eq = !prnpag->opt.eq;
                pmenu->pcur->vloc = noyes [prnpag->opt.eq];
                break;
            case  8: row = 6; col = 63; len = 2; pi = &prnpag->copspac; break;
            case  9: row = 8; col = 25; len = 4; pi = &prnpag->wid;     break;
            case 10: row = 8; col = 61; len = 4; pi = &prnpag->hei;     break;
            case 11: row =10; col = 25; len = 4; pi = &prnpag->left;    break;
            case 12: row =10; col = 61; len = 4; pi = &prnpag->right;   break;
            case 13: row =11; col = 25; len = 4; pi = &prnpag->top;     break;
            case 14: row =11; col = 61; len = 4; pi = &prnpag->bot;     break;
            case 15:
                mnprnopt (&prnpag->prnopt, " ˜à¨äâ § £®«®¢ª  ", &Att_Mnu2);
                continue;
            case 16:
                if (prnpag->opt.note < 2) prnpag->opt.note++;
                else                      prnpag->opt.note = 0;
                pmenu->pcur->vloc = wnote [prnpag->opt.note];
                break;
            case 17:
                prnpag->opt.number = !prnpag->opt.number;
                pmenu->pcur->vloc = noyes [prnpag->opt.number];
                break;
            case 18: row = 14; col = 61; len = 4; pi = &prnpag->tonote;   break;
            case 19:
                prnpag->opt.note1st = !prnpag->opt.note1st;
                pmenu->pcur->vloc = noyes [prnpag->opt.note1st];
                break;
            case 20:
                wnstredt (pmenu->pwin, 16, 9, pmenu->pwin->attr.afld,
                              pmenu->pwin->attr.afld, pmenu->pcur->vloc, 62);
                break;
            case 21:
                mnbrkcor ();
                mnremove (pmenu);
                (* fn) ();
                mndsplay (pmenu, WN_CENTER, WN_CENTER);
                continue;
            case 22:
                if (fn != NULL) mnbrkcor ();
                retcode = YES;
            case -1: done = YES; continue;
            default :            continue;
        }
        if (pi != NULL) {
            while (1) {
                wnstredt (pmenu->pwin, row, col, pmenu->pwin->attr.afld,
                            pmenu->pwin->attr.afld, pmenu->pcur->vloc, len);
                p = pmenu->pcur->vloc;
                if (utc2lrdx (&p, &i, 10) && utisempt ((byte *) p) && i >= 0L)
                    break;
                utalarm ();
            }
            utl2crdx (i, pmenu->pcur->vloc, len, 10);
            * pi = (int) i;
        }
        mndisvri (pmenu, pmenu->pcur);
    }
    if (retcode && !vfypag ()) {
        mnwarnin (NULL, 0, 3, " ", "¥á®®â¢¥âáâ¢¨¥ à §¬¥à®¢", " ");
        goto lab;
    }
ret:
    mndstroy (pmenu);
    return retcode;
}

/***************************************************************************
  0         1         2         3         4         5         6         7
  012345678901234567890123456789012345678901234567890123456789012345678901
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³  „¢ãáâ®à®­­ïï ¯¥ç âì     ˜˜˜         à®£®­ «¨áâ             ˜˜˜       ³0
 ³  Š®«¨ç¥áâ¢® íª§¥¬¯«ïà®¢  ˜˜˜         Œ¥¦áâà®ç­ë© ¨­â¥à¢ «     ˜˜       ³1
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ  á¯®«®¦¥­¨¥ íª§¥¬¯«ïà®¢ ­  «¨áâ¥ ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´2
 ³  Š ¦¤ë© íª§¥¬¯«ïà ¯¥ç â âì á ­®¢®£® «¨áâ                     ˜˜˜       ³3
 ³  ¥à §àë¢­®áâì íª§¥¬¯«ïà  ­  «¨áâ¥                           ˜˜˜       ³4
 ³   ¢­®¬¥à­®¥ à á¯à¥¤¥«¥­¨¥ ¢á¥å íª§¥¬¯«ïà®¢ ­  ®¤­®¬ «¨áâ¥   ˜˜˜       ³5
 ³   ááâ®ï­¨¥ ¬¥¦¤ã íª§¥¬¯«ïà ¬¨                                ˜˜ (áâà) ³6
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ  §¬¥àë «¨áâ  ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´7
 ³  ˜¨à¨­                  ˜˜˜˜ (¬¬)    ‚ëá®â                  ˜˜˜˜ (¬¬)  ³8
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Žâáâã¯ ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´9
 ³  ‘«¥¢                   ˜˜˜˜ (¬¬)    ‘¯à ¢                  ˜˜˜˜ (¬¬)  ³10
 ³  ‘¢¥àåã                 ˜˜˜˜ (¬¬)    ‘­¨§ã                  ˜˜˜˜ (¬¬)  ³11
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ‡ £®«®¢®ª ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´12
 ³  ˜à¨äâ                               ‚ë¢®¤¨âì             ˜˜˜˜˜˜       ³13
 ³  ã¬¥à æ¨ï áâà ­¨æ       ˜˜˜         Žâáâã¯                 ˜˜˜˜ (¬¬)  ³14
 ³  ‚ë¢®¤¨âì ­  ¯¥à¢®© áâà ­¨æ¥                                 ˜˜˜       ³15
 ³  ’¥ªáâ: ˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜ ³16
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´17
 ³   §¡¨¢ª  ¯® áâà ­¨æ ¬               ¥ç â âì                          ³18
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
******************************************************************************/

static bool   vfypag (void) {

    if (prnpag->hei < 50 || prnpag->wid < 50) return NO;
    if (prnpag->top + prnpag->bot + 10 > prnpag->hei) return NO;
    if (prnpag->left + prnpag->right + 10 > prnpag->wid) return NO;
    if (prnpag->opt.note == 1 && prnpag->tonote >= prnpag->top) return NO;
    if (prnpag->opt.note == 2 && prnpag->tonote >= prnpag->bot) return NO;
    return YES;
}

       /* “áâ ­ ¢«¨¢ ¥â ­¥ä¨ªá¨à®¢ ­­ë¥ ¯¥à¥¢®¤ë áâà ­¨æ ¤«ï ¢á¥£® â¥ªáâ  */
void mnbrkcor (void) {
    PAGBRK    * curbrk = prnpag->firstbrk, * nextbrk = curbrk->next;
    int         _4 = 4, _216 = 216, n;
    float       li;

    if (!specprn.n_216) { _4 = 2; _216 = 180; }
    li = 25.4 / (float) _216;
    n = min ((_4 + prnpag->linspac) * 3, _216);
    numstr = (int) ((float) (prnpag->hei - prnpag->top - prnpag->bot) /
            ((float) n * li));    /* Š®«¨ç¥áâ¢® áâà®ª ­  «¨áâ¥ */


    do {
        if (!curbrk->fix) delbrk (curbrk);
        curbrk = nextbrk;
        nextbrk = curbrk->next;
    } while (nextbrk != NULL);  /* “¤ «¨«¨ ­¥ä¨ªá¨à®¢ ­­ë¥ ¯¥à¥¢®¤ë áâà ­¨æ */


    curbrk = prnpag->firstbrk;
    nextbrk = curbrk->next;
    do {
        makebrk (curbrk, nextbrk->line - curbrk->line);
        curbrk = nextbrk;
        nextbrk = curbrk->next;
    } while (nextbrk != NULL);

    return;
}

        /* “áâ ­ ¢«¨¢ ¥â ä¨ªá¨à®¢ ­­ë© ¯¥à¥¢®¤ áâà ­¨æë */
void mnmakebrk (long line) {
    PAGBRK  * curbrk = prnpag->firstbrk;

    while (1) {
        if (line == curbrk->line) {
            if (!curbrk->fix) { curbrk->fix = YES; return; }
            if (curbrk->prev == NULL || curbrk->next == NULL) return;
            delbrk (curbrk);
            goto lab;
        }
        if (line < curbrk->line) break;
        curbrk = curbrk->next;
    }

    curbrk = insbrk (curbrk->prev);
    curbrk->fix = YES;
    curbrk->line = line;
lab:
    mnbrkcor ();
    return;
}

/* “áâ ­ ¢«¨¢ ¥â ­¥ä¨ªá¨à®¢ ­­ë¥ ¯¥à¥¢®¤ë áâà ­¨æ ¬¥¦¤ã ¤¢ã¬ï ä¨ªá¨à®¢ ­­ë¬¨ */
static void makebrk (PAGBRK * curbrk, long remain) {
    long        line;
    register    i;

    i = (int) (remain / numstr);
    if ((int) (remain % numstr) == 0) i--;
    for (line = curbrk->line; i > 0; i--) {
        curbrk = insbrk (curbrk);
        line += numstr;
        curbrk->line = line;
    }
    return;
}

    /* Ž¡à §ã¥â ­®¢ë© ¯¥à¥¢®¤ áâà ­¨æë, ®à£ ­¨§ã¥â á¢ï§ªã */
static PAGBRK * insbrk (PAGBRK * curbrk) {
    PAGBRK    * newbrk;

    newbrk = utalloc (PAGBRK);
    newbrk->prev       = curbrk;
    newbrk->next       = curbrk->next;
    curbrk->next->prev = newbrk;
    curbrk->next       = newbrk;
    return newbrk;
}

    /* “¤ «ï¥â ¨§ á¢ï§ª¨ ¯¥à¥¢®¤ áâ ­¨æë */
static void delbrk (PAGBRK * curbrk) {

    curbrk->prev->next = curbrk->next;
    curbrk->next->prev = curbrk->prev;
    free (curbrk);
    return;
}

void mnfrebrk (PAGBRK ** ppbrk) {
    PAGBRK    * nextbrk, * pbrk = * ppbrk;

    if (pbrk == NULL) return;

    while (1) {
        nextbrk = pbrk->next;
        free (pbrk);
        if (nextbrk == NULL) break;
        pbrk = nextbrk;
    }
    * ppbrk = NULL;
    return;
}

