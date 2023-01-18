#include <slmenu.h>
#include <sledit.h>
#include "makedoc.h"

static char   * noyes [] = { "¥â", " „ " };
static char     pout [55], ptmp [55], ptmp2 [55];
static ITEM    fmtitm[] = {
                         { " F - ä®à¬ â ",                      "Ff€ ", NULL },
                         { " E - ä®à¬ â ",                      "Ee“ã", NULL }
};

static ITEM    typitm[] = {
                         { "          –¥«®¥  ",                 "–æWw", NULL },
                         { "      „¢. æ¥«®¥  ",                 "…¥Tt", NULL },
                         { "   ‚¥é¥áâ¢¥­­®¥  ",                 "‚¢Dd", NULL },
                         { "„¢.¢¥é¥áâ¢¥­­®¥  ",                 "™éOo", NULL },
                         { "      ’¥ªáâ®¢®¥  ",                 "’âNn", NULL },
                         { "         „¥­ì£¨  ",                 "„¤Ll", NULL },
                         { "           „ â   ",                 "€ Ff", NULL },
                         { "    Š®¤¨ä¨ª â®à  ",                 "ŠªRr", NULL },
                         { "    ‘¯¥æ¨ «ì­®¥  ",                 "‘áCc", NULL }
};                 /*          @@@@@@@@@@@@@@@         */
static char   for_user_type [] = " ‘¯¥æ¨ «ì­®¥   ";

static ITEM    iniitm[] = {
                         { "        Š®­áâ ­â   ",               "ŠªRr", NULL },
                         { "            ‘ã¬¬   ",               "‘áCc", NULL },
                         { "          ‘à¥¤­¥¥  ",               "àHh", NULL },
                         { "       Š®«¨ç¥áâ¢®  ",               "‹«Kk", NULL },
                         { "       ‘¯à ¢®ç­¨ª  ",               "¯Gg", NULL }
};

static ITEM    datitm[] = {
                         { " à®¨§¢®«ì­®¥ §­ ç¥­¨¥ ",           "¯Gg", NULL },
                         { " ’¥ªãé ï ¤ â           ",           "’âNn", NULL }
};

static ITEM    type    = { "  ’¨¯ §­ ç¥­¨ï @@@@@@@@@@@@@@@  ",  "’âNn", NULL };
static ITEM    multy   = { "  Œ­®¦¥áâ¢¥­­®¥            @@@  ",  "Œ¬Vv", NULL };
static ITEM    cust    = { "  „¨ ¯ §®­                      ",  "„¤Ll", NULL };
static ITEM    protect = { "  ‡ é¨é¥­­®¥               @@@  ",  "‡§Pp", NULL };
static ITEM    init    = { "  ˆ­¨æ¨ «¨§ æ¨ï                 ",  "ˆ¨Bb", NULL };
static ITEM    indx    = { "  ˆ­¤¥ªá¨àã¥¬®¥            @@@  ",  "­Yy", NULL };
static ITEM    jst     = { "  ‚ëà ¢­¨¢ ­¨¥      @@@@@@@@@@  ",  "àHh", NULL };
static ITEM    visibl  = { "  ‚¨¤¨¬®¥                  @@@  ",  "‚¢Dd", NULL };
static ITEM    sym     = { "  ‘¨¬¢®« ¤«ï ¢ëà ¢­¨¢ ­¨ï    @  ",  "‘áCc", "  " };
static ITEM    prn     = { "  ‚ë¡®à èà¨äâ  ¤«ï ¯à¨­â¥à      ",  "˜èIi", NULL };
static ITEM    noedit  = { "  ¥à¥¤ ªâ¨àã¥¬®¥          @@@  ",  "ŠªRr", NULL };
static ITEM    wrap    = { "  €¢â®¯¥à¥­®á­®¥           @@@  ",  "€ Ff", NULL };
static ITEM    doc     = {
"  „®ªã¬¥­â                                 @@@@@@@@@@@@@@@@@@@@  ", "Ž®Jj",
"                     " };
static ITEM    upfld   = {
"  ‡ ¢¨á¨¬®¥ ¯®«¥ á¢¥àåã                    @@@@@@@@@@@@@@@@@@@@  ", "…¥Tt",
"                     " };
static ITEM    dnfld   = {
"  ‡ ¢¨á¨¬®¥ ¯®«¥ á­¨§ã                     @@@@@@@@@@@@@@@@@@@@  ", "¯Gg",
"                     " };
static ITEM    task    = {
"  ‡ ¤ ç                                    @@@@@@@@              ", "—çXx",
"                     " };


static char * pjst [] = {"     ‚«¥¢®","    ‚¯à ¢®"," ® æ¥­âàã"," ¢­®¬¥à­®" };

static int       optjst [] = { 0, 1, 2, 3, 5, 6 };
static IDFLD   * pidf;

static void  optdate        (void);
static void  optdble        (bool dbl);
static void  optdblf        (bool dbl);
static void  optlong        (void);
static void  getexample     (WINDOW * pwin);
static int   val2c          (VAL * pval);
static bool  c2val          (VAL * pval);
static bool  vfyval         (VAL * pval, VAL * pdnval, VAL * pupval);
static void  inputname      (MENU * boxmnu, char * pname, int row, int len);

void   optbox (SLNODE * pnode) {
    MDNODE  * puser = (MDNODE *) pnode->puser;
    WINDOW  * fonwin, * pwin;
    MENU    * boxmnu, * typmnu, * inimnu, * datmnu;
    MENU    * fmtmnu, * savmnu = Cur_Menu;
    MNITEM  * txtwrap, * itmjst;
    byte    * p;
    char    * pp;
    int       tag, i;
    long      l;
    register  ijst;
    bool      done = NO, dbl, txt, qry;

    pidf = &puser->idf;
    treebrd.pttitle = " • à ªâ¥à¨áâ¨ª¨  ¯®«ï ";
    treebrd.type |= BORD_TCT;
    if ((boxmnu = mncreate (16, 65, 0, 0, &treebrd,&Att_Mnu1))== NULL) goto ret;
    if ((fonwin = wncreate (20, 73, 0, 0, &mnubrd, &Att_Mnu1))== NULL) goto ret;
    if ((typmnu = mncrestd (typitm,9,MN_VERT,0,0,&vmnubrd,&Att_Mnu2)) == NULL)
        goto ret;
    if ((fmtmnu = mncrestd (fmtitm,2,MN_VERT,0,0,&vmnubrd,&Att_Mnu2)) == NULL)
        goto ret;
    if ((inimnu = mncrestd (iniitm,5,MN_VERT,0,0,&vmnubrd,&Att_Mnu2)) == NULL)
        goto ret;
    if ((datmnu = mncrestd (datitm,2,MN_VERT,0,0,&vmnubrd,&Att_Mnu2)) == NULL)
        goto ret;
    pwin = boxmnu->pwin;
    if (pidf->sym == '\0') pidf->sym = ' ';

    for (ijst = 5; ijst > 0 && pidf->prnopt.jst != optjst [ijst]; ijst--);

    if ((qry = (maker.opt.exittyp == MD_QRY)) == YES) {
        pidf->opt.wrap = pidf->opt.multy = YES;
    } else if (pidf->valtype != SL_TEXT) pidf->opt.wrap = NO;

    if (pidf->valtype < SL_USER) {
        type.vars = typitm [pidf->valtype].text;
        for_user_type [13] = EOS;
    } else {
        utl2crdx ((long) pidf->valtype, for_user_type + 13, 2, 10);
        type.vars = for_user_type;
    }
    multy.vars    = noyes [pidf->opt.multy];
    visibl.vars   = noyes [pidf->opt.visibl];
    protect.vars  = noyes [pidf->opt.protect];
    indx.vars     = noyes [pidf->opt.indx];
    wrap.vars     = noyes [pidf->opt.wrap];
    noedit.vars   = noyes [pidf->opt.noedit];
    sym.vars [0]  = pidf->sym;
    i = pidf->prnopt.jst;
    jst.vars      = pjst  [utclrbit (i, 4)];
    doc.vars      = puser->docname;
    upfld.vars    = puser->fldup;
    dnfld.vars    = puser->flddown;
    task.vars     = puser->taskname;


    if (!mnitmadd (boxmnu,  1,  0,  0, &type))    goto ret;
    if (!mnitmadd (boxmnu,  2,  0, 33, &multy))   goto ret;
    if (!mnitmadd (boxmnu,  3,  1,  0, &cust))    goto ret;
    if (!mnitmadd (boxmnu,  4,  1, 33, &protect)) goto ret;
    if (!mnitmadd (boxmnu,  5,  2,  0, &init))    goto ret;
    if (!mnitmadd (boxmnu,  6,  2, 33, &indx))    goto ret;
    if (!mnitmadd (boxmnu,  7,  3,  0, &jst))     goto ret;
    if (!mnitmadd (boxmnu,  8,  3, 33, &visibl))  goto ret;
    if (!mnitmadd (boxmnu,  9,  4,  0, &sym))     goto ret;
    if (!mnitmadd (boxmnu, 10,  4, 33, &noedit))  goto ret;
    if (!mnitmadd (boxmnu, 11,  5,  0, &prn))     goto ret;
    if (!mnitmadd (boxmnu, 12,  5, 33, &wrap))    goto ret;
    if (!mnitmadd (boxmnu, 13, 12,  0, &doc))     goto ret;
    if (!mnitmadd (boxmnu, 14, 13,  0, &upfld))   goto ret;
    if (!mnitmadd (boxmnu, 15, 14,  0, &dnfld))   goto ret;
    if (!mnitmadd (boxmnu, 16, 15,  0, &task))    goto ret;

    txtwrap = mnfnditm (boxmnu->pitems, 12);
    itmjst  = mnfnditm (boxmnu->pitems, 7);

    wnsplitw (pwin,  6, WN_HSPLIT|WN_SINGLE);
    wnsplitw (pwin, 11, WN_HSPLIT|WN_DOUBLE);

    wndsplay (fonwin, WN_CENTER, WN_CENTER);
    mndsplay (boxmnu, WN_CENTER, WN_CENTER);

    wnwrtbuf (pwin,  7,  2,  7,               "à¨¬¥à:", WN_NATIVE);
    wnwrtbuf (pwin,  8,  2,  9,             "‡­ ç¥­¨¥:", WN_NATIVE);
    wnwrtbuf (pwin,  9,  2,  9,             "„¨ ¯ §®­:", WN_NATIVE);
    wnwrtbuf (pwin,  9, 12,  2,                    "®â", WN_NATIVE);
    wnwrtbuf (pwin,  9, 38,  2,                    "¤®", WN_NATIVE);
    wnwrtbuf (pwin, 10,  2, 21, "‘¯®á®¡ ¨­¨æ¨ «¨§ æ¨¨:", WN_NATIVE);

    mnsetpro (boxmnu, 2, qry);

    while (!done) {
        txt = (pidf->valtype == SL_TEXT);
        mnsetpro (boxmnu, 12, (qry || !txt));
        wnwrtbuf (pwin, 10, 48, 15, iniitm[pidf->initype].text+2,Att_Mnu1.text);
        val2c (&pidf->dnval);
        wnwrtbuf (pwin, 9, 15, 22, pout, Att_Mnu1.text);
        val2c (&pidf->upval);
        wnwrtbuf (pwin, 9, 41, 22, pout, Att_Mnu1.text);
        getexample (pwin);
        if (!vfyval (&pidf->inival, &pidf->dnval, &pidf->upval)) {
            utalarm ();
            goto ini;
        }
        Cur_Menu  = savmnu;
        switch (mnreadop (boxmnu, 0)) {
            case 1:
lab:
                mnsetcur (typmnu, pidf->valtype);
                Cur_Menu  = boxmnu;
                if (pidf->valtype == SL_TEXT)  p = pidf->inival.s.p;
                else                           p = NULL;
                if ((tag = mnreadit (typmnu)) >= 0) {
                    if (tag < SL_USER) boxmnu->pcur->vloc = typitm [tag].text;
                    else               boxmnu->pcur->vloc = for_user_type;
                    mndisvri (boxmnu, boxmnu->pitems);
                    dbl = NO;
                    Cur_Menu  = boxmnu;
                    switch (tag) {
                        case SL_DOUBLE:
                            dbl = YES;
                        case SL_FLOAT:
                            if ((i = mnreadit (fmtmnu)) < 0) {
                                boxmnu->pcur->vloc=typitm[pidf->valtype].text;
                                mndisvri (boxmnu, boxmnu->pitems);
                                goto lab;
                            }
                            Cur_Menu  = boxmnu;
                            if (i == 0) optdblf (dbl);
                            else        optdble (dbl);
                            if (pidf->valtype != tag) {
                                if (dbl) {
                                    pidf->dnval.d = -1.0e306;
                                    pidf->upval.d =  1.0e306;
                                    pidf->inival.d   =  0.0;
                                } else {
                                    pidf->dnval.f = -1.0e37;
                                    pidf->upval.f =  1.0e37;
                                    pidf->inival.f   =  0.0;
                                }
                            }
                            break;
                        case SL_INT:
                            optlong ();
                            if (pidf->valtype != tag) {
                                pidf->dnval.i = -32767;
                                pidf->upval.i =  32767;
                                pidf->inival.i   =  0;
                            }
                            break;
                        case SL_LONG:
                            optlong ();
                            if (pidf->valtype != tag) {
                                pidf->dnval.l = -2147483645L;
                                pidf->upval.l =  2147483645L;
                                pidf->inival.l   =  0L;
                            }
                            break;
                        case SL_DATE:
                            optdate ();
                            if (pidf->valtype != tag) {
                                pidf->dnval.l = utmkdate (1, 1,
                                        (pidf->valopt.date.centure - 1) * 100);
                                pidf->upval.l = utmkdate (31,12,3999);
                            }
                            break;
                        case SL_TEXT:
                            if (pidf->valtype != tag) {
                                pidf->inival.s.p = pidf->dnval.s.p =
                                                        pidf->upval.s.p = NULL;
                                pidf->inival.s.len = pidf->dnval.s.len =
                                                         pidf->upval.s.len = 0;
                            }
                            break;
                        case SL_MONEY:
                            pidf->valopt.d.len = 2;
                            pidf->valopt.d.opt = 0;
                            pidf->dnval.d = -1.0e306;
                            pidf->upval.d =  1.0e306;
                            pidf->inival.d   =  0.0;
                            break;
                        case SL_CDF:    break;
                        default:
                            while (1) {
                                pp = for_user_type + 13;
                                wnstredt (pwin, 0, 28, pwin->attr.afld,
                                                       pwin->attr.afld, pp, 2);
                                if (utc2lrdx (&pp, &l, 10) && (int)l >= SL_USER)
                                    break;
                                utalarm ();
                            }
                            tag = (int) l;
                            break;
                    }
                    if (pidf->valtype != tag) {
                        free (p);
                        ijst = pidf->prnopt.jst = pidf->prnopt.full = 0;
                        itmjst->vloc = pjst [0];
                        mndisvri (boxmnu, itmjst);
                        if (tag < SL_USER) for_user_type [13] = EOS;
                    }
                    pidf->valtype = tag;
                }
                if (!qry && pidf->valtype != SL_TEXT)
                    { pidf->opt.wrap = NO; goto clrwrap; }
                break;
            case 2:
                pidf->opt.multy = !pidf->opt.multy;
                boxmnu->pcur->vloc = noyes [pidf->opt.multy];
                mndisvri (boxmnu, boxmnu->pcur);
                break;
            case 3:
                if (pidf->opt.wrap || pidf->valtype == SL_CDF)
                    { utalarm (); break; }
cont:
                while (1) {
                    pout [val2c (&pidf->dnval)] = '\0';
                    wnstredt (pwin, 9, 15, pwin->attr.afld,
                                            pwin->attr.afld, pout, 22);
                    if (c2val (&pidf->dnval)) break;
                    utalarm ();
                }
                while (1) {
                    pout [val2c (&pidf->upval)] = '\0';
                    wnstredt (pwin, 9, 41, pwin->attr.afld,
                                            pwin->attr.afld, pout, 22);
                    if (c2val (&pidf->upval)) break;
                    utalarm ();
                }
                if (!vfyval (&pidf->upval, &pidf->dnval, &pidf->upval))
                    { utalarm (); goto cont; }
                break;
            case 4:
                pidf->opt.protect = !pidf->opt.protect;
                boxmnu->pcur->vloc = noyes [pidf->opt.protect];
                mndisvri (boxmnu, boxmnu->pcur);
                break;
            case 5:
                if ((tag = mnreadit (inimnu)) < 0) break;
                if (qry && tag != 4) { utalarm (); break; }
                pidf->initype = tag;
ini:
                if (pidf->initype == 0) {
                    if (pidf->valtype == SL_DATE) {
                        Cur_Menu = boxmnu;
                        mnsetcur (datmnu,
                                    ((pidf->valopt.date.opt & CV_TODAY) != 0));
                        if ((tag = mnreadit (datmnu)) < 0) break;
                        if (tag == 1) {
                            pidf->valopt.date.opt |= CV_TODAY;
                            pidf->inival.l  = utdtoday ();
                            pidf->prnopt.full = YES;
                            break;
                        }
                        if ((pidf->valopt.date.opt & CV_TODAY) != 0)
                            pidf->valopt.date.opt ^= CV_TODAY;
                    }
                    if (pidf->opt.wrap) {
                        Ed_lsize = 32500;
                        Ed_ppl   = utallocb (&Ed_lsize);
                        i = val2c (&pidf->inival);
                        pidf->inival.s.blank =
                            edtdupl (&i, boxmnu->pwin->where_shown.row + 9,
                                           boxmnu->pwin->where_shown.col + 2,
                                                                   8, NULL, NO);
                        pidf->inival.s.len = i;
                        c2val (&pidf->inival);
                        free (Ed_ppl);
                    } else while (1) {
                        pout [val2c (&pidf->inival)] = '\0';
                        wnstredt (pwin, 8, 13, pwin->attr.afld,
                                                pwin->attr.afld, pout, 50);
                        if (c2val (&pidf->inival)) break;
                        utalarm ();
                    }
                }
                break;
            case 6:
                pidf->opt.indx = !pidf->opt.indx;
                boxmnu->pcur->vloc = noyes [pidf->opt.indx];
                mndisvri (boxmnu, boxmnu->pcur);
                break;
            case 7:
                ijst++;
                if (pidf->opt.wrap) {
                    if (ijst > 3) ijst = 0; goto setjst;
                } else if (ijst == 3) ijst++;

                if (pidf->valtype > SL_DIGITS && ijst > 2 || ijst > 5)
                    ijst = 0;
setjst:
                i = pidf->prnopt.jst = optjst [ijst];
                itmjst->vloc = pjst [utclrbit (i, 4)];
                mndisvri (boxmnu, itmjst);
                break;
            case 8:
                pidf->opt.visibl = !pidf->opt.visibl;
                boxmnu->pcur->vloc = noyes [pidf->opt.visibl];
                mndisvri (boxmnu, boxmnu->pcur);
                break;
            case 9:
                if (pidf->opt.wrap)
                    { utalarm (); break; }
                wnstredt (pwin, 4, 29, pwin->attr.afld,
                                     pwin->attr.afld, boxmnu->pcur->vloc, 1);
                pidf->sym = * boxmnu->pcur->vloc;
                mndisvri (boxmnu, boxmnu->pcur);
                break;
            case 10:
                pidf->opt.noedit = !pidf->opt.noedit;
                boxmnu->pcur->vloc = noyes [pidf->opt.noedit];
                mndisvri (boxmnu, boxmnu->pcur);
                break;
            case 11: mnprnopt (&pidf->prnopt, NULL, &Att_Mnu2);  break;
            case 12:
                pidf->opt.wrap = !pidf->opt.wrap;
                utfree (&pidf->inival.s.p);
                pidf->inival.s.len = 0;
                pidf->prnopt.full = NO;
clrwrap:
                txtwrap->vloc = noyes [pidf->opt.wrap];
                mndisvri (boxmnu, txtwrap);
                if (!pidf->opt.wrap) { ijst = 0; goto setjst; }
                break;
            case 13:
                inputname (boxmnu, puser->docname,  12, MAX_NAM_LEN);
                break;
            case 14:
                inputname (boxmnu, puser->fldup,    13, MAX_NAM_LEN);
                break;
            case 15:
                inputname (boxmnu, puser->flddown,  14, MAX_NAM_LEN);
                break;
            case 16:
                inputname (boxmnu, puser->taskname, 15, 8);
                break;
            default :   done = YES; break;
        }
    }

ret:
    mndstroy (boxmnu);
    wndstroy (fonwin);
    mndstroy (typmnu);
    mndstroy (fmtmnu);
    mndstroy (inimnu);
    mndstroy (datmnu);
    treebrd.type ^= BORD_TCT;
    return;
}

/****
                10        20        30        40        50        60
       0123456789 123456789 123456789 123456789 123456789 123456789 1234
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                       ³
   ³  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ·  ³
   ³ 0³  ’¨¯ §­ ç¥­¨ï   „¢®©­®¥ æ¥«®¥  |  Œ­®¦¥áâ¢¥­­®¥            ¥â  º  ³
   ³ 1³  „¨ ¯ §®­                         ‡ é¨é¥­­®¥               ¥â  º  ³
   ³ 2³  ˆ­¨æ¨ «¨§¨àã¥¬®¥                 ˆ­¤¥ªá¨àã¥¬®¥            ¥â  º  ³
   ³ 3³  ‚ëà ¢­¨¢ ­¨¥                     ‚¨¤¨¬®¥                   „   º  ³
   ³ 4³  ‘¨¬¢®« ¤«ï ¢ëà ¢­¨¢ ­¨ï    _     ¥¤ ªâ¨àã¥¬®¥             „   º  ³
   ³ 5³  ‚ë¡®à èà¨äâ  ¤«ï ¯à¨­â¥à         €¢â®¯¥à¥­®á­®¥           ¥â  º  ³
   ³ 6ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶  ³
   ³ 7³  à¨¬¥à:    __________________________________________________  º  ³
   ³ 8³  ‡­ ç¥­¨¥:  __________________________________________________  º  ³
   ³ 9³  „¨ ¯ §®­: ®â ______________________ ¤® ______________________  º  ³
   ³10³  ‘¯®á®¡ ¨­¨æ¨ «¨§ æ¨¨:                         ® ª®¤¨ä¨ª â®àã  º  ³
   ³11ÆÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹  ³
   ³12³  „®ªã¬¥­â:                                                      º  ³
   ³13³  ‡ ¢¨á¨¬®¥ ¯®«¥ á¢¥àåã:                                         º  ³
   ³14³  ‡ ¢¨á¨¬®¥ ¯®«¥ á­¨§ã:                                          º  ³
   ³15³  ‡ ¤ ç :                                                        º  ³
   ³  ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼  ³
   ³                                                                       ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

                1
      01234567890123456
     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ·                 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ·
     ³  –¥«®¥          º0                ³  Š®­áâ ­â         º0
     ³  „¢®©­®¥ æ¥«®¥  º1                ³  ‘ã¬¬             º1
     ³  ‚¥é¥áâ¢¥­­®¥   º2                ³  ‘à¥¤­¥¥          º2
     ³  ’¥ªáâ®¢®¥      º3                ³  Š®«¨ç¥áâ¢®       º3
     ³  à®áâ®¥        º4                ³  Šà®áá¯®¨áª®¢ë©   º4
     ³  „¥­ì£¨         º5                ³  ® á¯à ¢®ç­¨ªã   º5
     ³  „ â            º6                ³  ® ª®¤¨ä¨ª â®àã  º6
     ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼                 ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼


                1         2         3
      0123456789012345678901234567890
     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ·
     ³  ‚¥ª ¯® ã¬®«ç ­¨î (0-99)  20  º0
     ³            (â¥ªãé¨© ¢¥ª - 0)  º1
     ³   §¤¥«¨â¥«ì               /  º2
     ³  ‚¥ª                          º3
     ³  Œ¥áïæ                        º4
     ³  ‚ëà ¢­¨¢ ­¨¥                 º5
     ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶6
     ³                               º7
     ³  à¨¬¥à:   25 September 1993  º8
     ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
*******/

static int  jstindx [] = { 0, 64, 192 };
static int  monindx [] = { 0, 2, 6, 10, 14 };

static ITEM defcen = { " ‚¥ª ¯® ã¬®«ç ­¨î (0-99)  @@ ", "“ãEe", "   " };
static ITEM sep    = { "  §¤¥«¨â¥«ì               @ ", "àHh", "  "  };
static ITEM cen    = { " ‚¥ª                         ", "‚¢Dd", NULL  };
static ITEM month  = { " Œ¥áïæ                       ", "Œ¬Vv", NULL  };
static ITEM jstrt  = { " ‚ëà ¢­¨¢ ­¨¥                ", "¯Gg", NULL  };


static void  optdate  (void) {
    MENU    * datemnu;
    VALOPT    valopt;
    WINDOW  * pwin;
    int       i, j, cent;
    register  imon = 0, ijst = 0;
    bool      done = NO, centure = NO;
    char    * p;
    long      date;


    if ((datemnu = mncreate (9, 31, 0, 0, &vmnubrd,&Att_Mnu2))== NULL) goto ret;

    if (pidf->valtype != SL_DATE) {
        valopt.date.centure  = 0;   valopt.date.separ    = '/';
    } else {
        valopt = pidf->valopt;
        i = (valopt.date.opt & 192);
        for (ijst = 2; ijst > 0 && i != jstindx [ijst]; ijst--);
        i = (valopt.date.opt & 14);
        for (imon = 4; imon > 0 && i != monindx [imon]; imon--);
        centure = (valopt.date.opt & CV_CENTURE);
    }
    utl2crdx ((long) valopt.date.centure, defcen.vars, 2, 10);
    sep.vars [0] = valopt.date.separ;

    if (!mnitmadd (datemnu, 1, 0, 1, &defcen)) goto ret;
    if (!mnitmadd (datemnu, 2, 2, 1, &sep))    goto ret;
    if (!mnitmadd (datemnu, 3, 3, 1, &cen))    goto ret;
    if (!mnitmadd (datemnu, 4, 4, 1, &month))  goto ret;
    if (!mnitmadd (datemnu, 5, 5, 1, &jstrt))  goto ret;

    wnsplitw (datemnu->pwin,  6, WN_HSPLIT|WN_SINGLE);
    mndsplay (datemnu, WN_CURMNU, WN_CURMNU);
    pwin = datemnu->pwin;
    wnwrtbuf (pwin, 1, 12, 17, "(â¥ªãé¨© ¢¥ª - 0)", pwin->attr.text);
    wnwrtbuf (pwin, 7,  2,  7, "à¨¬¥à:",           pwin->attr.text);

    while (1) {
        i = 0;
        if (centure) i |= CV_CENTURE;
        i |= jstindx [ijst];
        i |= monindx [imon];
        if (done) { valopt.date.opt = i;  break; }

        cent = valopt.date.centure;
        if (cent == 0) cent = utcurcen ();
        date = utmkdate (25, 9, (cent - 1) * 100 + 93);
        j = utdate2c (date, ptmp, valopt.date.separ, i);
        if (j < 17) utmovsc (ptmp + j, ' ', 17 - j);
        wnwrtbuf (pwin, 7, 12, 17, ptmp, pwin->attr.text);

        date = utmkdate (2, 4, (cent - 1) * 100 + 5);
        j = utdate2c (date, ptmp, valopt.date.separ, i);
        if (j < 17) utmovsc (ptmp + j, ' ', 17 - j);
        wnwrtbuf (pwin, 8, 12, 17, ptmp, pwin->attr.text);

        switch (mnreadop (datemnu, 0)) {
            case 1:
lab:
                wnstredt (pwin, 0, 27, pwin->attr.afld,
                                    pwin->attr.afld, datemnu->pcur->vloc, 2);
                p = datemnu->pcur->vloc;
                if (!utc2lrdx (&p, &date, 10) || !utisempt ((byte *) p) ||
                                                     utrange (date, 0L, 99L))
                    { utalarm (); goto lab; }
                utl2crdx (date, datemnu->pcur->vloc, 2, 10);
                valopt.date.centure = (int) date;
                mndisvri (datemnu, datemnu->pcur);
                break;
            case 2:
                wnstredt (pwin, 2, 28, pwin->attr.afld,
                                     pwin->attr.afld, datemnu->pcur->vloc, 1);
                valopt.date.separ = * datemnu->pcur->vloc;
                mndisvri (datemnu, datemnu->pcur);
                break;
            case 3:  centure = !centure;  break;
            case 4:
                imon++;
                if (imon > 4) imon = 0;
                break;
            case 5:
                ijst++;
                if (ijst > 2) ijst = 0;
                break;
            default :   done = YES; break;
        }
    }
    pidf->valopt = valopt;
ret:
    mndstroy (datemnu);
    return;
}

 /******************************
 SL_EXP              512  ªá¯®­¥­æ¨ «ì­ ï ä®à¬ 

 CV_RANGE1           1    Œ ­â¨áá  ®â 0 ¤® 9.999999999
 CV_BLANK_BP         2    ¥ ¢ë¢®¤¨âì 0 ¯¥à¥¤ â®çª®©
 CV_M_PLUS           4    ‚ë¢®¤¨âì '+' ¢ ¬ ­â¨áá¥
 CV_M_PLUS_BLANK     8    ‚ëà ¢­¨¢ âì ¯®¤ §­ ª ¢ ¬ ­â¨áá¥

 CV_E_PLUS           16   ‚ë¢®¤¨âì '+' ¢ íªá¯®­¥­â¥
 CV_E_PLUS_BLANK     32   ‚ëà ¢­¨¢ âì ¯®¤ §­ ª ¢ íªá¯®­¥­â¥
 CV_JST              64   ‚ëà ¢­¨¢ âì ¢¯à ¢® (íªá¯®­¥­âã)
 CV_ZERO             128  ‚ë¢®¤¨âì ¢¥¤ãé¨¥ '0'



                              1         2         3
                    01234567890123456789012345678901234
                   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
                   ³  „«¨­  ¬ ­â¨ááë  (2-8)        10  ³ 0
                   ³  Œ ­â¨áá                          ³ 1
                   ³  ªá¯®­¥­â                        ³ 2
                   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´ 3
                   ³  à¨¬¥à:  -2.45678987653421e-128  ³ 4
                   ³            5.64389578450743e   5  ³ 5
                   ³            8.36095432769318e  18  ³ 6
                   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*******************/

static int  manindx [] = { 0, 2, 4, 6, 8, 10, 1, 5, 9 };
static int  expindx [] = { 0, 16, 32, 64, 80, 96, 192, 208, 224 };

static ITEM lenman = { " „«¨­  ¬ ­â¨ááë  (2-8)        @@ ", "„¤Ll", "   " };
static ITEM man    = { " Œ ­â¨áá                         ", "Œ¬Vv", NULL  };
static ITEM expon  = { " ªá¯®­¥­â                       ", "í]",  NULL  };


static void  optdble  (bool dbl) {
    VALOPT    valopt;
    MENU    * dblmnu;
    WINDOW  * pwin;
    int       i, j;
    register  iexp = 0, iman = 0;
    bool      done = NO;
    char    * p;
    long      len, maxlen;
    double    x1, x2 = 5.64389578450743e5, x3 = 8.36095432769318e18;


    if (dbl) {
        x1 = -2.45678987653421e-128;
        i = SL_DOUBLE;
        maxlen = 16L;
        utmovabs (&lenman.text [20], "16)", 3, 0);
    } else {
        x1 = -2.45678987653421e-28;
        i = SL_FLOAT;
        maxlen = 8L;
        utmovabs (&lenman.text [20], "8) ", 3, 0);
    }

    if ((dblmnu = mncreate (7, 35, 0, 0, &vmnubrd, &Att_Mnu2))== NULL) goto ret;

    if (pidf->valtype != i || !(pidf->valopt.d.opt & SL_EXP)) {
        valopt.d.opt = 0;
        valopt.d.len = (dbl) ? 12 : 6;
    } else {
        valopt = pidf->valopt;
        i = (valopt.d.opt & 15);
        for (iman = 8; iman > 0 && i != manindx [iman]; iman--);
        i = (valopt.d.opt & 240);
        for (iexp = 8; iexp > 0 && i != expindx [iexp]; iexp--);
    }

    utl2crdx ((long) valopt.d.len, lenman.vars, 2, 10);

    if (!mnitmadd (dblmnu, 1, 0, 1, &lenman)) goto ret;
    if (!mnitmadd (dblmnu, 2, 1, 1, &man))    goto ret;
    if (!mnitmadd (dblmnu, 3, 2, 1, &expon))  goto ret;

    wnsplitw (dblmnu->pwin, 3, WN_HSPLIT|WN_SINGLE);
    mndsplay (dblmnu, WN_CURMNU, WN_CURMNU);
    pwin = dblmnu->pwin;

    wnwrtbuf (pwin, 4, 2, 7, "à¨¬¥à:", pwin->attr.text);

    while (1) {
        i = 0;
        i |= manindx [iman];
        i |= expindx [iexp];
        if (!dbl) i |= CV_FLOAT;
        if (done) { valopt.d.opt = i;  break; }

        j = utdbl2ce (x1, ptmp, valopt.d.len, i);
        if (j < 23) utmovsc (ptmp + j, ' ', 23 - j);
        wnwrtbuf (pwin, 4, 10, 23, ptmp, pwin->attr.text);


        j = utdbl2ce (x2, ptmp, valopt.d.len, i);
        if (j < 23) utmovsc (ptmp + j, ' ', 23 - j);
        wnwrtbuf (pwin, 5, 10, 23, ptmp, pwin->attr.text);

        j = utdbl2ce (x3, ptmp, valopt.d.len, i);
        if (j < 23) utmovsc (ptmp + j, ' ', 23 - j);
        wnwrtbuf (pwin, 6, 10, 23, ptmp, pwin->attr.text);

        switch (mnreadop (dblmnu, 0)) {
            case 1:
lab:
                wnstredt (pwin, 0, 31, pwin->attr.afld,
                                    pwin->attr.afld, dblmnu->pcur->vloc, 2);
                p = dblmnu->pcur->vloc;
                if (!utc2lrdx (&p, &len, 10) || !utisempt ((byte *) p) ||
                                utrange (len, 2L, maxlen))
                    { utalarm (); goto lab; }
                utl2crdx (len, dblmnu->pcur->vloc, 2, 10);
                valopt.d.len = (int) len;
                mndisvri (dblmnu, dblmnu->pcur);
                break;
            case 2:
                iman++;
                if (iman > 8) iman = 0;
                break;
            case 3:
                iexp++;
                if (iexp > 8) iexp = 0;
                break;
            default :   done = YES; break;
        }
    }
    valopt.d.opt |= SL_EXP;
    pidf->valopt = valopt;
ret:
    mndstroy (dblmnu);
    return;

}

/*******************
    CV_E_PLUS           16    ‚ë¢®¤¨âì '+'
    CV_E_PLUS_BLANK     32    ‚ëà ¢­¨¢ âì ¯®¤ §­ ª

                              1         2         3
                    01234567890123456789012345678901234
                   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
                   ³  „«¨­  ¤à®¡­®© ç áâ¨  (0-16)  10  ³ 0
                   ³  ‚ëà ¢­¨¢ ­¨¥                     ³ 1
                   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´ 2
                   ³  à¨¬¥à:  -245.678987653421128    ³ 3
                   ³            5643.895784507435764   ³ 4
                   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ



*********************/

static int  fjstindx [] = { 0, 16, 32 };

static ITEM lenprec = { " „«¨­  ¤à®¡­®© ç áâ¨  (0-16)  @@ ", "„¤Ll", "   " };
static ITEM fjst    = { " ‚ëà ¢­¨¢ ­¨¥                    ", "‚¢Dd", NULL  };


static void  optdblf  (bool dbl) {
    VALOPT    valopt;
    MENU    * dblmnu;
    WINDOW  * pwin;
    int       i, j;
    register  ijst = 0;
    bool      done = NO;
    char    * p;
    long      len, maxlen;
    double    x1 = -245.678987653421128, x2 = 5643.895784507435764;


    if (dbl) {
        i = SL_DOUBLE;
        maxlen = 16L;
        utmovabs (&lenprec.text [25], "16)", 3, 0);
    } else {
        i = SL_FLOAT;
        maxlen = 8L;
        utmovabs (&lenprec.text [25], "8) ", 3, 0);
    }

    if ((dblmnu = mncreate (5, 35, 0, 0, &vmnubrd, &Att_Mnu2))== NULL) goto ret;

    if (pidf->valtype != i || (pidf->valopt.d.opt & SL_EXP)) {
        valopt.d.opt = 0;
        valopt.d.len = (dbl) ? 6 : 3;
    } else {
        valopt = pidf->valopt;
        i = (valopt.d.opt & 48);
        for (ijst = 2; ijst > 0 && i != fjstindx [ijst]; ijst--);
    }

    utl2crdx ((long) valopt.d.len, lenprec.vars, 2, 10);

    if (!mnitmadd (dblmnu, 1, 0, 1, &lenprec)) goto ret;
    if (!mnitmadd (dblmnu, 2, 1, 1, &fjst))    goto ret;

    wnsplitw (dblmnu->pwin, 2, WN_HSPLIT|WN_SINGLE);
    mndsplay (dblmnu, WN_CURMNU, WN_CURMNU);
    pwin = dblmnu->pwin;

    wnwrtbuf (pwin, 3, 2, 7, "à¨¬¥à:", pwin->attr.text);

    while (1) {
        i = 0;
        i |= fjstindx [ijst];
        if (!dbl) i |= CV_FLOAT;
        if (done) { valopt.d.opt = i;  break; }

        j = utdbl2cf (x1, ptmp, 23, valopt.d.len, i);
        if (j < 23) utmovsc (ptmp + j, ' ', 23 - j);
        wnwrtbuf (pwin, 3, 10, 23, ptmp, pwin->attr.text);


        j = utdbl2cf (x2, ptmp, 23, valopt.d.len, i);
        if (j < 23) utmovsc (ptmp + j, ' ', 23 - j);
        wnwrtbuf (pwin, 4, 10, 23, ptmp, pwin->attr.text);

        switch (mnreadop (dblmnu, 0)) {
            case 1:
lab:
                wnstredt (pwin, 0, 31, pwin->attr.afld,
                                    pwin->attr.afld, dblmnu->pcur->vloc, 2);
                p = dblmnu->pcur->vloc;
                if (!utc2lrdx (&p, &len, 10) || !utisempt ((byte *) p) ||
                                utrange (len, 1L, maxlen))
                    { utalarm (); goto lab; }
                utl2crdx (len, dblmnu->pcur->vloc, 2, 10);
                valopt.d.len = (int) len;
                mndisvri (dblmnu, dblmnu->pcur);
                break;
            case 2:
                ijst++;
                if (ijst > 2) ijst = 0;
                break;
            default :   done = YES; break;
        }
    }
    pidf->valopt = valopt;
ret:
    mndstroy (dblmnu);
    return;

}

/*******************
    CV_E_PLUS           16    ‚ë¢®¤¨âì '+'
    CV_E_PLUS_BLANK     32    ‚ëà ¢­¨¢ âì ¯®¤ §­ ª

                              1
                    0123456789012345678
                   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
                   ³  ‚ëà ¢­¨¢ ­¨¥     ³ 0
                   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´ 1
                   ³  à¨¬¥à:  -14534  ³ 2
                   ³            26437  ³ 3
                   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ



*********************/

static ITEM ljst    = { " ‚ëà ¢­¨¢ ­¨¥    ", "‚¢Dd", NULL  };

static void  optlong  (void) {
    VALOPT    valopt;
    MENU    * lngmnu;
    WINDOW  * pwin;
    int       i, j;
    register  ijst = 0;
    bool      done = NO;
    long      x1 = -14534L, x2 = 26437L;



    if ((lngmnu = mncreate (4, 19, 0, 0, &vmnubrd, &Att_Mnu2))== NULL) goto ret;

    if (pidf->valtype != SL_INT && pidf->valtype != SL_LONG) valopt.d.opt = 0;
    else {
        valopt = pidf->valopt;
        i = (valopt.d.opt & 48);
        for (ijst = 2; ijst > 0 && i != fjstindx [ijst]; ijst--);
    }

    if (!mnitmadd (lngmnu, 1, 0, 1, &ljst))    goto ret;

    wnsplitw (lngmnu->pwin, 1, WN_HSPLIT|WN_SINGLE);
    mndsplay (lngmnu, WN_CURMNU, WN_CURMNU);
    pwin = lngmnu->pwin;

    wnwrtbuf (pwin, 2, 2, 7, "à¨¬¥à:", pwin->attr.text);

    while (1) {
        i = fjstindx [ijst];
        if (done) { valopt.d.opt = i;  break; }

        j = utl2c (x1, ptmp, i);
        if (j < 10) utmovsc (ptmp + j, ' ', 10 - j);
        wnwrtbuf (pwin, 2, 10, 10, ptmp, pwin->attr.text);

        j = utl2c (x2, ptmp, i);
        if (j < 10) utmovsc (ptmp + j, ' ', 10 - j);
        wnwrtbuf (pwin, 3, 10, 10, ptmp, pwin->attr.text);

        switch (mnreadop (lngmnu, 0)) {
            case 1:
                ijst++;
                if (ijst > 2) ijst = 0;
                break;
            default :   done = YES; break;
        }
    }
    pidf->valopt = valopt;
ret:
    mndstroy (lngmnu);
    return;

}

static  void  getexample (WINDOW * pwin) {
    register    i = 0, j = 0;
    byte        attr = pwin->attr.nice;

    switch (pidf->valtype) {
        case SL_TEXT:
            j = 14;
            utmovabs (ptmp, "’¥ªáâ®¢®¥ ¯®«¥", j, 0);
            i = pidf->inival.s.len;
            i = min (i, 50);
            utmovabs (ptmp2, pidf->inival.s.p, i, 0);
            break;
        case SL_FLOAT:
            if (pidf->valopt.d.opt & SL_EXP) {
                j = utdbl2ce (-2.34572, ptmp, pidf->valopt.d.len,
                                                        pidf->valopt.d.opt);
                i = utdbl2ce (pidf->inival.f, ptmp2, pidf->valopt.d.len,
                                                        pidf->valopt.d.opt);
            } else {
                j = utdbl2cf (-2.345783, ptmp, 50, pidf->valopt.d.len,
                                                        pidf->valopt.d.opt);
                i = utdbl2cf (pidf->inival.f, ptmp2, 50, pidf->valopt.d.len,
                                                        pidf->valopt.d.opt);
            }
            break;
        case SL_MONEY:
        case SL_DOUBLE:
            if (pidf->valopt.d.opt & SL_EXP) {
                j = utdbl2ce (-2.34572, ptmp, pidf->valopt.d.len,
                                                        pidf->valopt.d.opt);
                i = utdbl2ce (pidf->inival.d, ptmp2, pidf->valopt.d.len,
                                                        pidf->valopt.d.opt);
            } else {
                j = utdbl2cf (-2.345783, ptmp, 50, pidf->valopt.d.len,
                                                        pidf->valopt.d.opt);
                i = utdbl2cf (pidf->inival.d, ptmp2, 50, pidf->valopt.d.len,
                                                        pidf->valopt.d.opt);
            }
            break;
        case SL_DATE:
            j = utdate2c (utmkdate (3, 4, 1987), ptmp,
                                pidf->valopt.date.separ, pidf->valopt.date.opt);
            i = utdate2c (pidf->inival.l, ptmp2,
                                pidf->valopt.date.separ, pidf->valopt.date.opt);
            break;
        case SL_INT:
            j = utl2c (-12675L, ptmp, pidf->valopt.d.opt);
            i = utl2c ((long) pidf->inival.i, ptmp2, pidf->valopt.d.opt);
            break;
        case SL_LONG:
            j = utl2c (-12675L, ptmp, pidf->valopt.d.opt);
            i = utl2c (pidf->inival.l, ptmp2, pidf->valopt.d.opt);
            break;
        default :                           break;
    }
    utstrjst (pout, ptmp, j, pidf->sym, 50, pidf->prnopt.jst);
    wnwrtbuf (pwin, 7, 13, 50, pout, attr);
    if (pidf->prnopt.full) utstrjst (pout, ptmp2, i, pidf->sym, 50,
                                                            pidf->prnopt.jst);
    else                utmovsc (pout, ' ', 50);
    wnwrtbuf (pwin, 8, 13, 50, pout, attr);
    return;
}

static  int  val2c (VAL * pval) {
    register    j = 0, i;
    char      * pbuf;

    switch (pidf->valtype) {
        case SL_TEXT:
            if (pidf->opt.wrap) pbuf = Ed_ppl + 3;
            else                pbuf = pout;
            if (pval->s.p != NULL) {
                j = pval->s.len;
                utmovabs (pbuf, pval->s.p, j, 0);
            }
            break;
        case SL_FLOAT:
            j = utdbl2ce ((double) pval->f, pout, 7, CV_FLOAT|CV_RANGE1);
            break;
        case SL_MONEY:
        case SL_DOUBLE:
            j = utdbl2ce (pval->d, pout, 14, CV_RANGE1);
            break;
        case SL_DATE:
            j = utdate2c (pval->l, pout, '/', CV_CENTURE|CV_JST|CV_ZERO);
            break;
        case SL_INT:
            j = utl2crdx ((long) pval->i, pout, 0, 10);
            break;
        case SL_LONG:
            j = utl2crdx (pval->l, pout, 0, 10);
            break;
        default :                           break;
    }
    if (pidf->opt.wrap || pidf->valtype == SL_CDF) i = 0;
    else                                           i = j;
    utmovsc (pout + i, ' ', 50 - i);
    return j;
}

static bool c2val (VAL * pval) {
    register len;
    char   * pbuf = pout;
    bool     ret = NO, inival = NO;
    long     ltmpnum = 0L;
    double   dtmpnum = 0.0;

    if (pval == &pidf->inival)  { inival = YES; pidf->prnopt.full = NO; }
    switch (pidf->valtype) {
        case SL_TEXT:
            free (pval->s.p);
            if (pidf->opt.wrap) {
                pbuf = Ed_ppl + 3;
                len = pval->s.len;
            } else {
                len = strlen (pbuf);
                while (pbuf [len - 1] == ' ' && len > 0) len--;
            }
            if (len != 0) {
                pval->s.p = malloc (len + 1);
                utmovabs (pval->s.p, pbuf, len, 0);
                pval->s.p [len] = '\0';
                if (inival) pidf->prnopt.full = YES;
            } else pval->s.p = NULL;
            pval->s.len = len;
            ret = YES;
            break;
        case SL_FLOAT:
            if (!utisempt (pbuf)) {
                if (!utc2dbl (&pbuf, &dtmpnum, NO)) break;
                if (!utisempt (pbuf)) break;
                if (inival) pidf->prnopt.full = YES;
            }
            pval->f = (float) dtmpnum;
            ret = YES;
            break;
        case SL_MONEY:
        case SL_DOUBLE:
            if (!utisempt (pbuf)) {
                if (!utc2dbl (&pbuf, &dtmpnum, YES)) break;
                if (!utisempt (pbuf)) break;
                if (inival) pidf->prnopt.full = YES;
            }
            pval->d = dtmpnum;
            ret = YES;
            break;
        case SL_DATE:
            if (!utisempt (pbuf)) {
                if ((ltmpnum = utc2date (&pbuf, 20)) == -1L) break;
                if (!utisempt (pbuf)) break;
                if (inival) pidf->prnopt.full = YES;
            }
            pval->l = ltmpnum;
            ret = YES;
            break;
        case SL_LONG:
            if (!utisempt (pbuf)) {
                if (!utc2lrdx (&pbuf, &ltmpnum, 10)) break;
                if (!utisempt (pbuf)) break;
                if (inival) pidf->prnopt.full = YES;
            }
            pval->l = ltmpnum;
            ret = YES;
            break;
        case SL_INT:
            if (!utisempt (pbuf)) {
                if (!utc2lrdx (&pbuf, &ltmpnum, 10)) break;
                if (!utisempt (pbuf)) break;
                if (inival) pidf->prnopt.full = YES;
            }
            pval->i = (int) ltmpnum;
            ret = YES;
            break;
        default :    ret = YES;                       break;
    }
    return ret;
}

static bool vfyval (VAL * pval, VAL * pdnval, VAL * pupval) {

    if (!pidf->prnopt.full) return YES;
    switch (pidf->valtype) {
        case SL_FLOAT:
            if (utrange (pval->f, pdnval->f, pupval->f)) return NO;
            break;
        case SL_MONEY:
        case SL_DOUBLE:
            if (utrange (pval->d, pdnval->d, pupval->d)) return NO;
            break;
        case SL_DATE:
        case SL_LONG:
            if (utrange (pval->l, pdnval->l, pupval->l)) return NO;
            break;
        case SL_INT:
            if (utrange (pval->i, pdnval->i, pupval->i)) return NO;
            break;
        default :                           break;
    }
    return YES;
}

static void inputname (MENU * boxmnu, char * pname, int row, int len) {

    if (wnstredt (boxmnu->pwin, row, 43, boxmnu->pwin->attr.afld,
                                        boxmnu->pwin->attr.afld, pname, len)) {
        len = strlen (pname);
        while (pname [len - 1] == ' ' && len > 0) len--;
        pname [len] = EOS;
        mndisvri (boxmnu, boxmnu->pcur);
    }
    return;
}

