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
    { "         ªà ­            ",       "ŠªRr", NULL },
    { "         Œëèì             ",       "Œ¬Vv", NULL },
    { "     à ¬¥âàë ¯à¨­â¥à     ", "¯Gg", NULL }
};

static int      hor, ver;

static MENU         * specmnu;
static ATTRIB       * pattrib;
static char         * ttl = "‡ £®«®¢®ª";
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
        { "  ¥¤ ªâ®à           ",  "àHh",  NULL },
        { "  „¥à¥¢®             ",  "„¤Ll",  NULL },
        { "  Œ¥­î 1             ",  "1!",    NULL },
        { "  Œ¥­î 2             ",  "2@",    NULL },
        { "  ‘¯à ¢®ç­ë¥ áâà®ª¨  ",  "‘áCc",  NULL },
        { "  ”®­ íªà ­          ",  "”äAa",  NULL },
        { "",                        NULL ,  NULL },
        { "   «¨âà             ",  "¯Gg",  NULL }
    };
    static ITEM  itm2[] = {
        { "  ‘¨¬¢®«     ",  "‘áCc",  NULL },
        { "  –¢¥â       ",  "–æWw",  NULL },
        { "  ’¥­ì       ",  "’âNn",  NULL },
        { "",                NULL ,  NULL },
        { "  ‘®åà ­¨âì  ",  "•å{" ,  NULL }
    };
    static ITEM  itm3[] = {
        { "  ‘â âãá­ ï áâà®ª       ",  "‘áCc",  NULL },
        { "  ®¤áª §ª  ¯® ª« ¢¨è ¬ ",  "¯Gg",  NULL },
        { "",                           NULL ,  NULL },
        { "  ‘®åà ­¨âì             ",  "•å{" ,  NULL }
    };
    static ITEM  itm4[] = {
        { "  “áâ ­®¢¨âì            ",  "“ãEe",  NULL },
        { "  ‘®åà ­¨âì             ",  "‘áCc",  NULL }
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
                    mnwarnin (NULL, 0, 3, "", " €¤ ¯â¥à ­¨§ª®£® ª« áá  ", "");
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
            "¥ ¬®£ã á®åà ­¨âì æ¢¥â : ®è¨¡ª  ¯à¨ ®âªàëâ¨¨ ä ©« " , " ");
    return;
}

static ITEM msitm [] = {
    { " ‘ª®à®áâì ¯® ¢¥àâ¨ª «¨    @@@@ ", "‚¢Dd", "     " },
    { " ‘ª®à®áâì ¯® £®à¨§®­â «¨  @@@@ ", "ƒ£Uu", "     " },
    { "",                                  NULL,    NULL },
    { " ‘®åà ­¨âì                     ", "‘áCc",    NULL }
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
    { " ‚¥«¨ç¨­   ¢â®§ ¯à ¢ª¨       @@  ¬¬  ", "‚¢Dd", "   "  },
    { " Œ¨­¨¬ «ì­ë© ¯¥à¥¢®¤ áâà®ª¨  n / @@@ ", "Œ¬Vv", "    " },
    { "",                                       NULL,   NULL  },
    { " ‘®åà ­¨âì                           ", "‘áCc",  NULL  }

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
        { "  ’¥ªáâ              ",     "’âNn", NULL },     /* TEXT */
        { "  ®à¤îà             ",     "¡`" , NULL },     /* BORD */
        { "  ‡ £®«®¢®ª          ",     "‡§Pp", NULL },     /* TITL */
        { "  «®ª               ",     "‹«Kk", NULL },     /* BLCK */
        { "  EOF                ",     "Ee“ã", NULL },     /* HIGH */
        { "",                           NULL,  NULL },
        { "  ‘®åà ­¨âì          ",     "‘áCc", NULL }
    };
    static char   * txt[] = { "         ’¥ªáâ      ",
                              "    «®ª            ",
                              "      ’¥ªáâ         ",
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

    wnmovpa (pdata,          4 * W, Att_Edit.text);         /* ’¥ªáâ       */
    wnmovpa (pdata + W,          W, Att_Edit.blck);         /* «®ª        */
    wnmovpa (pdata + 3 * W,      W, Att_Edit.high);         /* >>EOF<<     */

    pbuf = (char far *) tstwind->img.pdata;
    pscreen = wnviptrl (tstwind->where_shown.row, tstwind->where_shown.col);
    wnvicopy (&pbuf, &pscreen, 4, W, NUM_COLS*2, 0, Cmd[1]);
    tstwind->attr = Att_Edit;
    wnputbor (tstwind);
    return;
}

/*******************************************************************************
                            Ž Š  Ž
 57
          10
 0123456789 123456789
ÚÄÄÄÄÄÄ‡ £®«®¢®ªÄÄÄÄÄ·
³         ’¥ªáâ      º°°0
³    «®ª            º°°1
³        ’¥ªáâ       º°°2
³ >>EOF<<            º°°3
ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼°°
 °°°°°°°°°°°°°°°°°°°°°°°
*******************************************************************************/

void treecolor (void) {
    static ITEM  itm[] = {
        { "  ®à¤îà                    ",     "¡`" , NULL },     /* BORD */
        { "  ¥ ªâ¨¢­ë© ã§¥«           ",     "­Yy", NULL },     /* TEXT */
        { "  €ªâ¨¢­ë© ã§¥«             ",     "€ Ff", NULL },     /* HIGH */
        { "  ‡ ¬®à®¦¥­­ë© ã§¥«         ",     "‡§Pp", NULL },     /* BOLD */
        { "  Ž¯à¥¤¥«¥­­ë© ã§¥«         ",     "Ž®Jj", NULL },     /* PROT */
        { "  €ªâ¨¢­ë© ­  ®¯à¥¤¥«¥­­®¬  ",     "ŠªRr", NULL },     /* NICE */
        { "  ¥¤ ªâ¨àã¥¬ ï ç áâì       ",     "àHh", NULL },     /* AFLD */
        { "",                                  NULL,  NULL },
        { "  ‘®åà ­¨âì                 ",     "‘áCc", NULL }
    };
    static char   * txt[] = { " ¥ ªâ¨¢­ë©         ",
                              " ÆÍÍ¥ ªâ¨¢­ë©     ",
                              " ³   ÃÄ €ªâ¨¢­ë©    ",
                              " ³   ÀÄ ¥ ªâ¨¢­ë©  ",
                              " ÃÄ ‡ ¬®à®¦¥­­ë©    ",
                              " ÃÄ ¥ ªâ¨¢­ë©      ",
                              " ÃÄ Ž¯à¥¤¥«¥­­ë©    ",
                              " ÃÄ ¥ ªâ¨¢­ë©      ",
                              " ÃÄ €ªâ¨¢­.­  ®¯à.  ",
                              " ÃÄ ¥ ªâ¨¢­ë©      ",
                              " ÀÄ ¥¤ ªâà. ç áâì  "  };

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

    wnmovpa (pdata,           11 * W, Att_Tree.text);         /* ¥ ªâ¨¢­ë©   */
    wnmovpa (pdata +  2 * W + 8,  10, Att_Tree.high);         /* €ªâ¨¢­ë©     */
    wnmovpa (pdata +  4 * W + 4,  12, Att_Tree.bold);         /* ‡ ¬®à®¦¥­­ë© */
    wnmovpa (pdata +  6 * W + 4,  12, Att_Tree.prot);         /* Ž¯à¥¤¥«ñ­­ë© */
    wnmovpa (pdata +  8 * W + 4,  14, Att_Tree.nice);         /* €ªâ. ­  ®¯à. */
    wnmovpa (pdata + 10 * W + 4,  16, Att_Tree.afld);         /* ¥¤. ç áâì   */

    pbuf = (char far *) tstwind->img.pdata;
    pscreen = wnviptrl (tstwind->where_shown.row, tstwind->where_shown.col);
    wnvicopy (&pbuf, &pscreen, 11, W, NUM_COLS*2, 0, Cmd[1]);
    tstwind->attr = Att_Tree;
    wnputbor (tstwind);
    return;
}

/*******************************************************************************
                            „ …  … ‚ Ž
57
          10
 0123456789 123456789
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ·
³ ¥ ªâ¨¢­ë©         º°°0
³ ÆÍÍ¥ ªâ¨¢­ë©     º°°1
³ ³   ÃÄ €ªâ¨¢­ë©    º°°2
³ ³   ÀÄ ¥ ªâ¨¢­ë©  º°°3
³ ÃÄ ‡ ¬®à®¦¥­­ë©    º°°4
³ ÃÄ ¥ ªâ¨¢­ë©      º°°5
³ ÃÄ Ž¯à¥¤¥«ñ­­ë©    º°°6
³ ÃÄ ¥ ªâ¨¢­ë©      º°°7
³ ÃÄ €ªâ¨¢­.­  ®¯à.  º°°8
³ ÃÄ ¥ ªâ¨¢­ë©      º°°9
³ ÀÄ ¥¤ ªâà. ç áâì  º°°10
ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼°°
 °°°°°°°°°°°°°°°°°°°°°°°
*******************************************************************************/

void  menucolor   (int num) {
    static ITEM  itm[] = {
        { "  ®à¤îà              ",     "¡`" , NULL },     /* BORD */
        { "  ‡ £®«®¢®ª           ",     "ƒ£Uu", NULL },     /* TITL */
        { "  €ªâ¨¢­ë© ¯ã­ªâ      ",     "€ Ff", NULL },     /* HIGH */
        { "  ¥ ªâ¨¢­ë© ¯ã­ªâ    ",     "­Yy", NULL },     /* TEXT */
        { "  ‡ é¨é¥­­ë© ¯ã­ªâ    ",     "‡§Pp", NULL },     /* PROT */
        { "  ¥à¥¬¥­­ ï ç áâì    ",     "¯Gg", NULL },     /* NICE  */
        { "  Š« ¢¨è  ¢ë¡®à       ",     "ŠªRr", NULL },     /* BOLD */
        { "  ¥¤ ªâ¨àã¥¬ ï ç áâì ",     "àHh", NULL },     /* AFLD */
        { "",                            NULL,  NULL },
        { "  ‘®åà ­¨âì           ",     "‘áCc", NULL }
    };

    static char   * txt[] = { "  ¥ ªâ¨¢­ë©        ",
                              "  ‡ é¨é¥­­ë©        ",
                              "  ¥ ªâ¨¢­ë©   ¯/ç  ",
                              "  ¥ ªâ¨¢­ë©        ",
                              "  €ªâ¨¢­ë©          ",
                              "  ¥ ªâ¨¢­ë©        ",
                              "  ‡ é¨é¥­­ë©        ",
                              "  ¥¤ ªâ¨à. ç áâì   ",
                              "  ¥ ªâ¨¢­ë©   ¯/ç  "  };
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

    wnmovpa (pdata,            9 * W, pattrib->text);         /* ¥ ªâ¨¢­ë©  */
    wnmovpa (pdata + W,            W, pattrib->prot);         /* ‡ é¨é¥­­ë©  */
    wnmovpa (pdata + 2 * W + 15,   3, pattrib->nice);         /* /ç         */
    wnmovpa (pdata + 4 * W,        W, pattrib->high);         /* €ªâ¨¢­ë©    */
    wnmovpa (pdata + 6 * W,        W, pattrib->prot);         /* ‡ é¨é¥­­ë©  */
    wnmovpa (pdata + 7 * W,        W, pattrib->afld);         /* ¥¤.  ç áâì */
    wnmovpa (pdata + 8 * W + 15,   3, pattrib->nice);         /* /ç         */
    wnmovpa (pdata + 2,            1, pattrib->bold);         /* Š« ¢¨è  ¢ë¡.*/
    wnmovpa (pdata + 2 * W + 4,    1, pattrib->bold);         /* Š« ¢¨è  ¢ë¡.*/
    wnmovpa (pdata + 3 * W + 3,    1, pattrib->bold);         /* Š« ¢¨è  ¢ë¡.*/
    wnmovpa (pdata + 5 * W + 6,    1, pattrib->bold);         /* Š« ¢¨è  ¢ë¡.*/
    wnmovpa (pdata + 8 * W + 5,    1, pattrib->bold);         /* Š« ¢¨è  ¢ë¡.*/

    pbuf = (char far *) tstwind->img.pdata;
    pscreen = wnviptrl (tstwind->where_shown.row, tstwind->where_shown.col);
    wnvicopy (&pbuf, &pscreen, 9, W, NUM_COLS*2, 0, Cmd[1]);
    tstwind->attr = * pattrib;
    wnputbor (tstwind);
    return;
}

/*******************************************************************************
                             Œ …  ž
57
          10
 0123456789 123456789
ÚÄÄÄÄÄÄ‡ £®«®¢®ªÄÄÄÄÄ·
³  ¥ ªâ¨¢­ë©        º°°0
³  ‡ é¨é¥­­ë©        º°°1
³  ¥ ªâ¨¢­ë©   ¯/ç  º°°2
³  ¥ ªâ¨¢­ë©        º°°3
³  €ªâ¨¢­ë©          º°°4
³  ¥ ªâ¨¢­ë©        º°°5
³  ‡ é¨é¥­­ë©        º°°6
³  ¥¤ ªâ¨à. ç áâì   º°°7
³  ¥ ªâ¨¢­ë©   ¯/ç  º°°8
ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼°°
 °°°°°°°°°°°°°°°°°°°°°°°
*******************************************************************************/

void statcolor (void) {
    static ITEM  itm[] = {
        { "  ¥¨§¬¥­ï¥¬ ï ç áâì  ",     "­Yy", NULL },     /* NFLD */
        { "  ˆ§¬¥­ï¥¬ ï ç áâì    ",     "ˆ¨Bb", NULL }      /* AFLD */
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
        { "  ’¥ªáâ           ",     "’âNn", NULL },     /* TEXT */
        { "  Š« ¢¨è  ¢ë¡®à   ",     "ŠªRr", NULL }     /* BOLD */
    };

    static char   * txt[] = { "Esc  ’¥ªáâ   Enter ’¥ªáâ",
                              " 8’¥ªáâ  9’¥ªáâ  0’¥ªáâ "  };
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

    wnmovpa (pdata,       2 * W, pattrib->text);       /* ’¥ªáâ          */
    wnmovpa (pdata,           4, pattrib->bold);       /* Š« ¢¨è  ¢ë¡®à  */
    wnmovpa (pdata + 13,      6, pattrib->bold);       /* Š« ¢¨è  ¢ë¡®à  */
    wnmovpa (pdata + W,       2, pattrib->bold);       /* Š« ¢¨è  ¢ë¡®à  */
    wnmovpa (pdata + W + 8,   2, pattrib->bold);       /* Š« ¢¨è  ¢ë¡®à  */
    wnmovpa (pdata + W + 16,  2, pattrib->bold);       /* Š« ¢¨è  ¢ë¡®à  */

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
Esc  ’¥ªáâ   Enter ’¥ªáâ
 8’¥ªáâ  9’¥ªáâ  0’¥ªáâ
*******************************************************************************/

void foncolor (void) {

    pattrib = &Att_User;
    wncolors (&pattrib->nice, "–¢¥â ä®­ ", updfon);
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
    wncolors (&pattrib->shdw, "–¢¥â â¥­¨", updshd);
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

