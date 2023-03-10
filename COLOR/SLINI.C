/**************************** 5.03.1992 ***************************************/
#include    <slwind.h>
#include    <slmenu.h>
#include    <slkeyb.h>
#include    <slmous.h>

char          mdini [] = "md.ini";
bool          mod_mono;
char          fonch;

BORDER       mnubrd  = { BORD_SHADOW,           NULL, NULL, NULL };
BORDER       vmnubrd = { BORD_DDSS|BORD_SHADOW, NULL, NULL, NULL };

static ITEM specitm [] = {
    { "         ??࠭            ",       "??Rr", NULL },
    { "         ????             ",       "??Vv", NULL },
    { "    ??ࠬ???? ?ਭ????    ", "??Gg", NULL }
};

static int      hor, ver;

static MENU         * specmnu;
static ATTRIB       * pattrib;
static char         * ttl = "?????????";
static WINDOW       * tstwind;
static BORDER         tstbor = { BORD_DDSS|BORD_SHADOW,NULL,NULL,NULL };

static  void    fmouse      (void);
static  void    fspprn      (void);
static  void    fcolor      (void);
static  void    edtcolor    (void);
static  void    treecolor   (void);
static  void    menucolor   (int num);
static  void    statcolor   (void);
static  void    fkeycolor   (void);
static  void    foncolor    (void);
static  void    shdcolor    (void);
static  void    updshd      (void);
static  void    updfon      (void);
static  void    updedt      (void);
static  void    updmenu     (void);
static  void    updtree     (void);
static  void    updfkey     (void);
static  void    updstat     (void);
static  void    savecolor   (ATTRIB * pattrib, int place);


void main (void) {
    register    i, hand;
    double      xxx = 0.01;

    if (!wniniwin (_TEXTC80, _MINROWS)) return;
    wnblinks (YES);
    Kb_fun = utoutdat;
    utworkin ();

    fonch = 176;   hor = 16;  ver = 8;
    utmovabs (Pal_Cur, Pal_Stnd, 17, 0);
    if ((hand = utopen (mdini, UT_R)) != -1) {
        utread (hand, &mod_mono, sizeof (bool));
        utread (hand, &Att_Tree, sizeof (ATTRIB));
        utread (hand, &Att_Mnu1,  sizeof (ATTRIB));
        utread (hand, &Att_Mnu1, sizeof (ATTRIB));
        utread (hand, &Att_Mnu2, sizeof (ATTRIB));
        utread (hand, &Att_Khlp, sizeof (ATTRIB));
        utread (hand, &Att_Chlp, sizeof (ATTRIB));
        utread (hand, &Att_Edit, sizeof (ATTRIB));
        utread (hand, &Att_User, sizeof (ATTRIB));
        utread (hand, &fonch, 1);
        utread (hand, Pal_Cur, 17);
        utread (hand, &hor, sizeof (int));
        utread (hand, &ver, sizeof (int));
        utread (hand, &specprn, sizeof (SPECPRN));
        utclose (hand);
    }
    msspeed (hor, ver);
    wnsetpal (Pal_Cur);
    wndisfon (fonch, Att_User.nice);

    specmnu = mncrestd (specitm,3,MN_HOR,0,0,&vmnubrd,&Att_Mnu2);
    mndsplay (specmnu, 0, 0);

    while ((i = mnreadop (specmnu, 0)) != -1) {
        switch (i) {
            case  0:  fcolor ();  break;
            case  1:  fmouse ();  break;
            case  2:  fspprn ();  break;
            default:              break;
        }
    }
    mndstroy (specmnu);
ret:
    wnfinwin ();
    return;
}

static void     fcolor (void) {
    static ITEM  itm[] = {
        { "  ????????           ",  "??Hh",  NULL },
        { "  ??ॢ?             ",  "??Ll",  NULL },
        { "  ???? 1             ",  "1!",    NULL },
        { "  ???? 2             ",  "2@",    NULL },
        { "  ??ࠢ????? ??ப?  ",  "??Cc",  NULL },
        { "  ??? ??࠭?         ",  "??Aa",  NULL },
        { "",                        NULL ,  NULL },
        { "  ???????            ",  "??Gg",  NULL }
    };
    static ITEM  itm2[] = {
        { "  ??????     ",  "??Cc",  NULL },
        { "  ????       ",  "??Ww",  NULL },
        { "  ????       ",  "??Nn",  NULL },
        { "",                NULL ,  NULL },
        { "  ???࠭???  ",  "??{" ,  NULL }
    };
    static ITEM  itm3[] = {
        { "  ?????᭠? ??ப?      ",  "??Cc",  NULL },
        { "  ???᪠??? ?? ?????蠬 ",  "??Gg",  NULL },
        { "",                           NULL ,  NULL },
        { "  ???࠭???             ",  "??{" ,  NULL }
    };
    static ITEM  itm4[] = {
        { "  ??⠭?????            ",  "??Ee",  NULL },
        { "  ???࠭???             ",  "??Cc",  NULL }
    };

    MENU    * mnu, * psav, * mnu2, * mnu3, * mnu4;
    bool      done = NO, done1;
    register  i;

    psav = Cur_Menu;
    mnu4 = mncrestd (itm4, 2, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu2);
    mnu3 = mncrestd (itm3, 4, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu2);
    mnu2 = mncrestd (itm2, 5, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu2);
    mnu  = mncrestd (itm,  8, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu1);
    mnsetpro (mnu3, 2, 1);
    mnsetpro (mnu2, 3, 1);
    mnsetpro (mnu,  6, 1);
    wnsplitw (mnu3->pwin, 2, WN_HSPLIT|WN_SINGLE);
    wnsplitw (mnu2->pwin, 3, WN_HSPLIT|WN_SINGLE);
    wnsplitw (mnu->pwin,  6, WN_HSPLIT|WN_SINGLE);
    mndsplay (mnu, WN_CURMNU, WN_CURMNU);
    while (!done) {
        wnselect (psav->pwin);
        Cur_Menu = psav;
        i = mnreadop (mnu, MN_HORINVER);
        switch (i) {
            case 0:   edtcolor  ();              break;
            case 1:   treecolor ();              break;
            case 2:
            case 3:
                menucolor (i - 1);
                mnupdmnu (&Att_Mnu2, 1, specmnu);
                mnupdmnu (&Att_Mnu1, 1, mnu);
                break;
            case 4:
                mndsplay (mnu3, WN_CURMNU, WN_CURMNU);
                done1 = NO;
                while (!done1) {
                    switch (mnreadit (mnu3)) {
                        case 0:  statcolor (); break;
                        case 1:  fkeycolor (); break;
                        case 3:  savecolor (&Att_Khlp, 5);
                        default : done1 = YES;  break;
                    }
                }
                mnremove (mnu3);
                break;
            case 5:
                mndsplay (mnu2, WN_CURMNU, WN_CURMNU);
                while (1) {
                    switch (mnreadit (mnu2)) {
                        case 0:
                            if (!mngetasc(&fonch,&vmnubrd,&Att_Mnu2)) continue;
                            mnremove (mnu2);
                            mnremove (mnu);
                            mnremove (specmnu);
                            break;
                        case 1:
                            mnremove (mnu2);
                            mnremove (mnu);
                            mnremove (specmnu);
                            foncolor ();
                            break;
                        case 2:
                            shdcolor ();
                            mnremove (mnu2);
                            mnremove (mnu);
                            mnremove (specmnu);
                            break;
                        case 4:   savecolor (&Att_User, 8);
                        default : goto loop;
                    }
                    wndisfon (fonch, Att_User.nice);
                    mndsplay (specmnu, WN_NATIVE, WN_NATIVE);
                    mndsplay (mnu, WN_NATIVE, WN_NATIVE);
                    mndsplay (mnu2, WN_NATIVE, WN_NATIVE);
                }
loop:
                mnremove (mnu2);
                break;
            case 7:
                if (!wnishigh ()) {
                    mnwarnin (NULL, 0, 3, "", " ??????? ??????? ?????? ", "");
                    break;
                }
                mndsplay (mnu4, WN_CURMNU, WN_CURMNU);
                done1 = NO;
                while (!done1) {
                    switch (mnreadit (mnu4)) {
                        case 0:   mnpalett (Pal_Cur);  break;
                        case 1:   savecolor (NULL, 9);
                        default : done1 = YES;  break;
                    }
                }
                mnremove (mnu4);
                break;
            case -1:  done = YES; break;
            default : break;
        }
    }
    mndstroy (mnu);
    mndstroy (mnu2);
    mndstroy (mnu3);
    mndstroy (mnu4);
    return;
}

static void savecolor (ATTRIB * pattrib, int place) {
    int hand;

    if ((hand = utopen (mdini, UT_W)) != -1) {
        if (place == 9) {
            utlseek (hand, (long) (8 * sizeof (ATTRIB) + 3), UT_BEG);
            utwrite (hand, Pal_Cur, 17);
        } else if (place == 10) {
            utlseek (hand, (long) (8 * sizeof (ATTRIB) + 20), UT_BEG);
            utwrite (hand, &hor, sizeof (int));
            utwrite (hand, &ver, sizeof (int));
        } else if (place == 11) {
            utlseek (hand, (long) (8 * sizeof (ATTRIB) + 24), UT_BEG);
            utwrite (hand, &specprn, sizeof (specprn));
        } else {
            utlseek (hand, (long) ((place - 1) * sizeof (ATTRIB) + 2), UT_BEG);
            utwrite (hand, pattrib, sizeof (ATTRIB));
            if (place == 8)  utwrite (hand, &fonch, 1);
        }
    } else {
        if ((hand = utcreate (mdini)) == -1) goto ret;
        utwrite (hand, &mod_mono, sizeof (bool));
        utwrite (hand, &Att_Tree, sizeof (ATTRIB));
        utwrite (hand, &Att_Mnu1, sizeof (ATTRIB));
        utwrite (hand, &Att_Mnu1, sizeof (ATTRIB));
        utwrite (hand, &Att_Mnu2, sizeof (ATTRIB));
        utwrite (hand, &Att_Khlp, sizeof (ATTRIB));
        utwrite (hand, &Att_Chlp, sizeof (ATTRIB));
        utwrite (hand, &Att_Edit, sizeof (ATTRIB));
        utwrite (hand, &Att_User, sizeof (ATTRIB));
        utwrite (hand, &fonch, 1);
        utwrite (hand, Pal_Cur, 17);
        utwrite (hand, &hor, sizeof (int));
        utwrite (hand, &ver, sizeof (int));
        utwrite (hand, &specprn, sizeof (specprn));
    }
    utclose (hand);
    return;
ret:
    mnwarnin (NULL, 0, 3, " ",
            "?? ???? ???࠭??? 梥??: ?訡?? ??? ?????⨨ 䠩??" , " ");
    return;
}

static ITEM msitm [] = {
    { " ???????? ?? ???⨪???    @@@@ ", "??Dd", "     " },
    { " ???????? ?? ??ਧ??⠫?  @@@@ ", "??Uu", "     " },
    { "",                                  NULL,    NULL },
    { " ???࠭???                     ", "??Cc",    NULL }
};

static void fmouse (void) {
    MENU * pmenu;
    bool   done = NO;
    long   i;
    char * p;
    int  * pi, tag;

    utl2crdx ((long) ver, msitm [0].vars, 4, 10);
    utl2crdx ((long) hor, msitm [1].vars, 4, 10);
    pmenu = mncrestd (msitm, 4, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu1);
    wnsplitw (pmenu->pwin, 2, WN_HSPLIT|WN_SINGLE);
    mnsetpro (pmenu, 2, 1);

    mndsplay (pmenu, WN_CURMNU, WN_CURMNU);

    while (!done) {
        switch (tag = mnreadop (pmenu, MN_HORINVER)) {
            case  0: pi = &ver;  break;
            case  1: pi = &hor;  break;
            case  3: savecolor (NULL, 10);
            default: done = YES; continue;
        }
        while (1) {
            wnstredt (pmenu->pwin, tag, 26, pmenu->pwin->attr.afld,
                        pmenu->pwin->attr.afld, pmenu->pcur->vloc, 4);
            p = pmenu->pcur->vloc;
            if (utc2lrdx (&p, &i, 10) && utisempt ((byte *) p) && i >= 0L)
                break;
            utalarm ();
        }
        utl2crdx (i, pmenu->pcur->vloc, 4, 10);
        * pi = (int) i;
        mndisvri (pmenu, pmenu->pcur);
    }
    mndstroy (pmenu);
    msspeed (hor, ver);
    return;
}

static ITEM sppritm [] = {
    { " ????稭? ??⮧??ࠢ??       @@  ??  ", "??Dd", "   "  },
    { " ??????????? ??ॢ?? ??ப?  n / @@@ ", "??Vv", "    " },
    { "",                                       NULL,   NULL  },
    { " ???࠭???                           ", "??Cc",  NULL  }

};
static  char * minlf [] = { "180", "216" };

static void fspprn (void) {
    MENU      * pmnu;
    char      * p;
    long        i;
    bool        done = NO;

    utl2crdx ((long) specprn.autopull, sppritm [0].vars, 2, 10);
    sppritm [1].vars = minlf [specprn.n_216];
    pmnu = mncrestd (sppritm, 4, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu1);
    wnsplitw (pmnu->pwin, 2, WN_HSPLIT|WN_SINGLE);
    mnsetpro (pmnu, 2, 1);
    mndsplay (pmnu, WN_CURMNU, WN_CURMNU);
    while (!done) {
        switch (mnreadop (pmnu, MN_HORINVER)) {
            case  0:
                while (1) {
                    wnstredt (pmnu->pwin, 0, 29, pmnu->pwin->attr.afld,
                                   pmnu->pwin->attr.afld, pmnu->pcur->vloc, 2);
                    p = pmnu->pcur->vloc;
                    if (utc2lrdx (&p,&i,10) && utisempt ((byte *)p) && i >= 0L)
                        break;
                    utalarm ();
                }
                utl2crdx (i, pmnu->pcur->vloc, 2, 10);
                specprn.autopull = (int) i;
                break;
            case  1:
                specprn.n_216 = !specprn.n_216;
                pmnu->pcur->vloc = minlf [specprn.n_216];
                break;
            case  3: savecolor (NULL, 11);
            default: done = YES; continue;
        }
        mndisvri (pmnu, pmnu->pcur);
    }
    mndstroy (pmnu);
    return;
}

void edtcolor (void) {
    static ITEM  itm[] = {
        { "  ?????              ",     "??Nn", NULL },     /* TEXT */
        { "  ??????             ",     "??`" , NULL },     /* BORD */
        { "  ?????????          ",     "??Pp", NULL },     /* TITL */
        { "  ????               ",     "??Kk", NULL },     /* BLCK */
        { "  EOF                ",     "Ee??", NULL },     /* HIGH */
        { "",                           NULL,  NULL },
        { "  ???࠭???          ",     "??Cc", NULL }
    };
    static char   * txt[] = { "         ?????      ",
                              "    ????            ",
                              "      ?????         ",
                              " >>EOF<<            " };

    MENU          * mnu, * psav;
    CELLSC        * pdata;
    byte          * att;
    bool            done = NO;
    int             i;
    static int      lastitem;

    psav = Cur_Menu;

    tstbor.pttitle = ttl;
    tstbor.type |= BORD_TCT;
    if ((tstwind = wncreate (4, 20, 0, 0, &tstbor, &Att_Edit)) == NULL) return;
    mnu = mncrestd (itm, 7, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu2);
    mnsetpro (mnu, 5, 1);
    mnsetcur (mnu, lastitem);
    wnsplitw (mnu->pwin, 5, WN_HSPLIT|WN_SINGLE);
    pdata = tstwind->img.pdata;
    for (i = 0; i < 4; i++, pdata += 20)
        wnmovpsa (pdata, txt[i], 20, Att_Edit.text);
    while (!done) {
        wnselect (psav->pwin);
        Cur_Menu = psav;
        mndsplay (mnu, WN_CURMNU, WN_CURMNU);
        i = mnreadit (mnu);
        if (i >= 0 && i != 6) { wndsplay (tstwind,WN_CENTER, 57); lastitem = i;}
        switch (i) {
            case  0:   att = &Att_Edit.text;         break;
            case  1:   att = &Att_Edit.bord;         break;
            case  2:   att = &Att_Edit.titl;         break;
            case  3:   att = &Att_Edit.blck;         break;
            case  4:   att = &Att_Edit.high;         break;
            case  6:   savecolor (&Att_Edit, 7);
            case -1:   done = YES;
            default :                            continue;
        }
        wncolors (att, itm[i].text, updedt);
lab:
        wnremove (tstwind);
    }
    tstbor.type ^= BORD_TCT;
    mndstroy (mnu);
    wndstroy (tstwind);
    return;
}

static void     updedt (void) {
    CELLSC      * pdata = tstwind->img.pdata;
    char far    * pbuf, * pscreen;
    register    W = tstwind->img.dim.w;

    wnmovpa (pdata,          4 * W, Att_Edit.text);         /* ?????       */
    wnmovpa (pdata + W,          W, Att_Edit.blck);         /* ????        */
    wnmovpa (pdata + 3 * W,      W, Att_Edit.high);         /* >>EOF<<     */

    pbuf = (char far *) tstwind->img.pdata;
    pscreen = wnviptrl (tstwind->where_shown.row, tstwind->where_shown.col);
    wnvicopy (&pbuf, &pscreen, 4, W, NUM_COLS*2, 0, Cmd[1]);
    tstwind->attr = Att_Edit;
    wnputbor (tstwind);
    return;
}

/*******************************************************************************
                            ? ? ? ?
 57
          10
 0123456789 123456789
??????ć????????????ķ
?         ?????      ???0
?    ????            ???1
?        ?????       ???2
? >>EOF<<            ???3
????????????????????ͼ??
 ???????????????????????
*******************************************************************************/

void treecolor (void) {
    static ITEM  itm[] = {
        { "  ??????                    ",     "??`" , NULL },     /* BORD */
        { "  ????⨢??? 㧥?           ",     "??Yy", NULL },     /* TEXT */
        { "  ??⨢??? 㧥?             ",     "??Ff", NULL },     /* HIGH */
        { "  ????஦????? 㧥?         ",     "??Pp", NULL },     /* BOLD */
        { "  ??।??????? 㧥?         ",     "??Jj", NULL },     /* PROT */
        { "  ??⨢??? ?? ??।???????  ",     "??Rr", NULL },     /* NICE */
        { "  ????????㥬?? ?????       ",     "??Hh", NULL },     /* AFLD */
        { "",                                  NULL,  NULL },
        { "  ???࠭???                 ",     "??Cc", NULL }
    };
    static char   * txt[] = { " ????⨢???         ",
                              " ???????⨢???     ",
                              " ?   ?? ??⨢???    ",
                              " ?   ?? ????⨢???  ",
                              " ?? ????஦?????    ",
                              " ?? ????⨢???      ",
                              " ?? ??।???????    ",
                              " ?? ????⨢???      ",
                              " ?? ??⨢?.?? ???.  ",
                              " ?? ????⨢???      ",
                              " ?? ???????. ?????  "  };

    CELLSC        * pdata;
    MENU          * mnu, * psav;
    byte          * att;
    bool            done = NO;
    int             i;
    static int      lastitem;

    psav = Cur_Menu;

    if ((tstwind = wncreate (11, 20, 0, 0, &tstbor, &Att_Tree)) == NULL) return;
    mnu = mncrestd (itm, 9, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu2);
    mnsetpro (mnu, 7, 1);
    mnsetcur (mnu, lastitem);
    wnsplitw (mnu->pwin, 7, WN_HSPLIT|WN_SINGLE);
    pdata = tstwind->img.pdata;
    for (i = 0; i < 11; i++, pdata += 20)
        wnmovpsa (pdata, txt[i], 20, Att_Tree.text);
    while (!done) {
        wnselect (psav->pwin);
        Cur_Menu = psav;
        mndsplay (mnu, WN_CURMNU, WN_CURMNU);
        i = mnreadit (mnu);
        if (i >= 0 && i != 8) { wndsplay (tstwind,WN_CENTER,57); lastitem = i;}
        switch (i) {
            case  0:   att = &Att_Tree.bord;               break;
            case  1:   att = &Att_Tree.text;               break;
            case  2:   att = &Att_Tree.high;               break;
            case  3:   att = &Att_Tree.bold;               break;
            case  4:   att = &Att_Tree.prot;               break;
            case  5:   att = &Att_Tree.nice;               break;
            case  6:   att = &Att_Tree.afld;               break;
            case  8:   savecolor (&Att_Tree, 1);
            case -1:   done = YES;
            default :                                  continue;
        }
        wncolors (att, itm[i].text, updtree);
        wnremove (tstwind);
    }
    mndstroy (mnu);
    wndstroy (tstwind);
    return;
}

static void     updtree (void) {
    CELLSC      * pdata = tstwind->img.pdata;
    char far    * pbuf, * pscreen;
    register      W = tstwind->img.dim.w;

    wnmovpa (pdata,           11 * W, Att_Tree.text);         /* ????⨢???   */
    wnmovpa (pdata +  2 * W + 8,  10, Att_Tree.high);         /* ??⨢???     */
    wnmovpa (pdata +  4 * W + 4,  12, Att_Tree.bold);         /* ????஦????? */
    wnmovpa (pdata +  6 * W + 4,  12, Att_Tree.prot);         /* ??।??????? */
    wnmovpa (pdata +  8 * W + 4,  14, Att_Tree.nice);         /* ???. ?? ???. */
    wnmovpa (pdata + 10 * W + 4,  16, Att_Tree.afld);         /* ???. ?????   */

    pbuf = (char far *) tstwind->img.pdata;
    pscreen = wnviptrl (tstwind->where_shown.row, tstwind->where_shown.col);
    wnvicopy (&pbuf, &pscreen, 11, W, NUM_COLS*2, 0, Cmd[1]);
    tstwind->attr = Att_Tree;
    wnputbor (tstwind);
    return;
}

/*******************************************************************************
                            ? ? ? ? ? ?
57
          10
 0123456789 123456789
????????????????????ķ
? ????⨢???         ???0
? ???????⨢???     ???1
? ?   ?? ??⨢???    ???2
? ?   ?? ????⨢???  ???3
? ?? ????஦?????    ???4
? ?? ????⨢???      ???5
? ?? ??।???????    ???6
? ?? ????⨢???      ???7
? ?? ??⨢?.?? ???.  ???8
? ?? ????⨢???      ???9
? ?? ???????. ?????  ???10
????????????????????ͼ??
 ???????????????????????
*******************************************************************************/

void  menucolor   (int num) {
    static ITEM  itm[] = {
        { "  ??????              ",     "??`" , NULL },     /* BORD */
        { "  ?????????           ",     "??Uu", NULL },     /* TITL */
        { "  ??⨢??? ?㭪?      ",     "??Ff", NULL },     /* HIGH */
        { "  ????⨢??? ?㭪?    ",     "??Yy", NULL },     /* TEXT */
        { "  ????饭??? ?㭪?    ",     "??Pp", NULL },     /* PROT */
        { "  ??६????? ?????    ",     "??Gg", NULL },     /* NICE  */
        { "  ??????? ?롮??      ",     "??Rr", NULL },     /* BOLD */
        { "  ????????㥬?? ????? ",     "??Hh", NULL },     /* AFLD */
        { "",                            NULL,  NULL },
        { "  ???࠭???           ",     "??Cc", NULL }
    };

    static char   * txt[] = { "  ????⨢???        ",
                              "  ????饭???        ",
                              "  ????⨢???   ?/?  ",
                              "  ????⨢???        ",
                              "  ??⨢???          ",
                              "  ????⨢???        ",
                              "  ????饭???        ",
                              "  ????????. ?????   ",
                              "  ????⨢???   ?/?  "  };
    CELLSC        * pdata;
    MENU          * mnu, * psav;
    byte          * att;
    bool            done = NO;
    int             i;
    static int      lastitem;

    psav = Cur_Menu;
    pattrib = (num == 1) ? &Att_Mnu1 : &Att_Mnu2;

    tstbor.pttitle = ttl;
    tstbor.type |= BORD_TCT;
    if ((tstwind = wncreate (9, 20, 0, 0, &tstbor, pattrib)) == NULL) return;
    mnu = mncrestd (itm, 10, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu2);
    mnsetpro (mnu, 8, 1);
    mnsetcur (mnu, lastitem);
    wnsplitw (mnu->pwin, 8, WN_HSPLIT|WN_SINGLE);
    pdata = tstwind->img.pdata;
    for (i = 0; i < 9; i++, pdata += 20)
        wnmovpsa (pdata, txt[i], 20, pattrib->text);
    while (!done) {
        wnselect (psav->pwin);
        Cur_Menu = psav;
        mndsplay (mnu, WN_CURMNU, WN_CURMNU);
        i = mnreadit (mnu);
        if (i >= 0 && i != 9) { wndsplay (tstwind,WN_CENTER,57); lastitem=i;}
        switch (i) {
            case  0:   att = &pattrib->bord;            break;
            case  1:   att = &pattrib->titl;            break;
            case  2:   att = &pattrib->high;            break;
            case  3:   att = &pattrib->text;            break;
            case  4:   att = &pattrib->prot;            break;
            case  5:   att = &pattrib->nice;            break;
            case  6:   att = &pattrib->bold;            break;
            case  7:   att = &pattrib->afld;            break;
            case  9:   savecolor (pattrib, 2 + num);
            case -1:   done = YES;
            default :                                continue;
        }
        wncolors (att, itm[i].text, updmenu);
        wnremove (tstwind);
    }
    tstbor.type ^= BORD_TCT;
    wndstroy (tstwind);
    mndstroy (mnu);
    return;
}

static void     updmenu (void) {
    CELLSC      * pdata = tstwind->img.pdata;
    char far    * pbuf, * pscreen;
    register      W = tstwind->img.dim.w;

    wnmovpa (pdata,            9 * W, pattrib->text);         /* ????⨢???  */
    wnmovpa (pdata + W,            W, pattrib->prot);         /* ????饭???  */
    wnmovpa (pdata + 2 * W + 15,   3, pattrib->nice);         /* ?/?         */
    wnmovpa (pdata + 4 * W,        W, pattrib->high);         /* ??⨢???    */
    wnmovpa (pdata + 6 * W,        W, pattrib->prot);         /* ????饭???  */
    wnmovpa (pdata + 7 * W,        W, pattrib->afld);         /* ???.  ????? */
    wnmovpa (pdata + 8 * W + 15,   3, pattrib->nice);         /* ?/?         */
    wnmovpa (pdata + 2,            1, pattrib->bold);         /* ??????? ???.*/
    wnmovpa (pdata + 2 * W + 4,    1, pattrib->bold);         /* ??????? ???.*/
    wnmovpa (pdata + 3 * W + 3,    1, pattrib->bold);         /* ??????? ???.*/
    wnmovpa (pdata + 5 * W + 6,    1, pattrib->bold);         /* ??????? ???.*/
    wnmovpa (pdata + 8 * W + 5,    1, pattrib->bold);         /* ??????? ???.*/

    pbuf = (char far *) tstwind->img.pdata;
    pscreen = wnviptrl (tstwind->where_shown.row, tstwind->where_shown.col);
    wnvicopy (&pbuf, &pscreen, 9, W, NUM_COLS*2, 0, Cmd[1]);
    tstwind->attr = * pattrib;
    wnputbor (tstwind);
    return;
}

/*******************************************************************************
                             ? ? ? ?
57
          10
 0123456789 123456789
??????ć????????????ķ
?  ????⨢???        ???0
?  ????饭???        ???1
?  ????⨢???   ?/?  ???2
?  ????⨢???        ???3
?  ??⨢???          ???4
?  ????⨢???        ???5
?  ????饭???        ???6
?  ????????. ?????   ???7
?  ????⨢???   ?/?  ???8
????????????????????ͼ??
 ???????????????????????
*******************************************************************************/

void statcolor (void) {
    static ITEM  itm[] = {
        { "  ???????塞?? ?????  ",     "??Yy", NULL },     /* NFLD */
        { "  ?????塞?? ?????    ",     "??Bb", NULL }      /* AFLD */
    };

    static char   * txt = "L:12    C:25   LaSy-Edit";
    CELLSC        * pdata;
    MENU          * mnu, * psav;
    bool            done = NO;
    int             i;

    psav = Cur_Menu;
    pattrib = &Att_Khlp;

    tstbor.type = 0;
    if ((tstwind = wncreate (1, 24, 0, 0, &tstbor, pattrib)) == NULL) return;
    mnu = mncrestd (itm, 2, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu1);
    pdata = tstwind->img.pdata;
    wnmovpsa (pdata, txt, 24, pattrib->nfld);
    while (!done) {
        wnselect (psav->pwin);
        Cur_Menu = psav;
        mndsplay (mnu, WN_CURMNU, WN_CURMNU);
        i = mnreadit (mnu);
        if (i >= 0) wndsplay (tstwind, WN_CENTER, 56);
        switch (i) {
            case  0:
                wncolors (&pattrib->nfld, itm[0].text, updstat);
                break;
            case  1:
                wncolors (&pattrib->afld, itm[1].text, updstat);
                break;
            case -1:   done = YES;
            default:   continue;
        }
        wnremove (tstwind);
    }
    tstbor.type |= BORD_DDSS|BORD_SHADOW;
    wndstroy (tstwind);
    mndstroy (mnu);
    return;
}

static void     updstat (void) {
    CELLSC      * pdata = tstwind->img.pdata;
    char far    * pbuf, * pscreen;
    register      W = tstwind->img.dim.w;

    wnmovpa (pdata,      W, pattrib->nfld);
    wnmovpa (pdata + 2,  2, pattrib->afld);
    wnmovpa (pdata + 10, 2, pattrib->afld);

    pbuf = (char far *) tstwind->img.pdata;
    pscreen = wnviptrl (tstwind->where_shown.row, tstwind->where_shown.col);
    wnvicopy (&pbuf, &pscreen, 1, 24, NUM_COLS*2, 0, Cmd[1]);
    return;
}

/*******************************************************************************
                          S T A T U S    L I N E
56
         10        20
0123456789 123456789 123
L:12    C:25   LaSy-Edit
*******************************************************************************/

void fkeycolor (void) {
    static ITEM  itm[] = {
        { "  ?????           ",     "??Nn", NULL },     /* TEXT */
        { "  ??????? ?롮??  ",     "??Rr", NULL }     /* BOLD */
    };

    static char   * txt[] = { "Esc  ?????   Enter ?????",
                              " 8?????  9?????  0????? "  };
    CELLSC        * pdata;
    MENU    * mnu, * psav;
    bool      done = NO;
    int             i;

    psav = Cur_Menu;
    pattrib = &Att_Khlp;

    tstbor.type = 0;
    if ((tstwind = wncreate (2, 24, 0, 0, &tstbor, pattrib)) == NULL)
        return;
    mnu = mncrestd (itm, 2, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu1);
    pdata = tstwind->img.pdata;
    for (i = 0; i < 2; i++, pdata += 24)
        wnmovpsa (pdata, txt[i], 24, pattrib->text);
    while (!done) {
        wnselect (psav->pwin);
        Cur_Menu = psav;
        mndsplay (mnu, WN_CURMNU, WN_CURMNU);
        i = mnreadit (mnu);
        if (i >= 0) wndsplay (tstwind, WN_CENTER, 56);
        switch (i) {
            case  0:
                wncolors (&pattrib->text, itm[0].text, updfkey);
                break;
            case  1:
                wncolors (&pattrib->bold, itm[1].text, updfkey);
                break;
            case -1:   done = YES;
            default:   continue;
        }
        wnremove (tstwind);
    }
    tstbor.type |= BORD_DDSS|BORD_SHADOW;
    wndstroy (tstwind);
    mndstroy (mnu);
    return;
}

static void     updfkey (void) {
    CELLSC      * pdata = tstwind->img.pdata;
    char far    * pbuf, * pscreen;
    register      W = tstwind->img.dim.w;

    wnmovpa (pdata,       2 * W, pattrib->text);       /* ?????          */
    wnmovpa (pdata,           4, pattrib->bold);       /* ??????? ?롮?? */
    wnmovpa (pdata + 13,      6, pattrib->bold);       /* ??????? ?롮?? */
    wnmovpa (pdata + W,       2, pattrib->bold);       /* ??????? ?롮?? */
    wnmovpa (pdata + W + 8,   2, pattrib->bold);       /* ??????? ?롮?? */
    wnmovpa (pdata + W + 16,  2, pattrib->bold);       /* ??????? ?롮?? */

    pbuf = (char far *) tstwind->img.pdata;
    pscreen = wnviptrl (tstwind->where_shown.row, tstwind->where_shown.col);
    wnvicopy (&pbuf, &pscreen, 2, 24, NUM_COLS*2, 0, Cmd[1]);
    return;
}

/*******************************************************************************
                             H E L P
56
         10        20
0123456789 123456789 123
Esc  ?????   Enter ?????
 8?????  9?????  0?????
*******************************************************************************/

void foncolor (void) {

    pattrib = &Att_User;
    wncolors (&pattrib->nice, "???? 䮭?", updfon);
    return;
}

static void     updfon (void) {
    char far    * pscreen, * pbuf = &fonch;

    pscreen = wnviptrl (0, 54);
    wnvicopy (&pbuf, &pscreen, 1, 26, NUM_COLS*2, pattrib->nice, Cmd[3]);

    pscreen = wnviptrl (1, 56);
    wnvicopy (&pbuf, &pscreen,NUM_ROWS-2,24,NUM_COLS*2, pattrib->nice, Cmd[3]);

    pscreen = wnviptrl (NUM_ROWS - 2, 0);
    wnvicopy (&pbuf, &pscreen, 1, 2, NUM_COLS*2, pattrib->nice, Cmd[3]);

    pscreen = wnviptrl (NUM_ROWS - 1, 0);
    wnvicopy (&pbuf, &pscreen, 1, 80, NUM_COLS*2, pattrib->nice,Cmd[3]);

    return;
}

void shdcolor (void) {

    pattrib = &Att_User;
    wncolors (&pattrib->shdw, "???? ⥭?", updshd);
    return;
}

static void     updshd (void) {
    char far    * pscreen;

    pscreen = wnviptrl (1, 54);
    wnvicopy (NULL,&pscreen,NUM_ROWS-3, 2, NUM_COLS*2, pattrib->shdw, Cmd[4]);

    pscreen = wnviptrl (NUM_ROWS - 2, 2);
    wnvicopy (NULL, &pscreen, 1, 54, NUM_COLS*2, pattrib->shdw, Cmd[4]);

    return;
}

