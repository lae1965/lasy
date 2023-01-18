/**************************** 5.03.1992 ***************************************/
#include    "makedoc.h"
#include    <dos.h>
#include    <dir.h>
#include    <io.h>
#include    <sledit.h>
#include    <slkeyb.h>
#include    <alloc.h>
#include    <slmous.h>
#include    <slsymb.def>

char        * mdtmpm  = "MDTMPM.▓▓▓";
char        * mdtmpd  = "MDTMPD.▓▓▓";
char        * mdwrk   = "MDWRK.▓▓▓";
char        * mdotxt  = "MDOTXT.▓▓▓";
char        * mddtxt  = "MDDTXT.▓▓▓";
char        * mdpno   = "MDPNO.▓▓▓";
char        * mdpnd   = "MDPND.▓▓▓";
char          mdini [80];
char        * Md_ext [] = { ".SLD", ".SLQ", ".SLL", ".SLC" };
bool          valtext, valtree, valdoc, mod_mono;
bool          chwnatt, chwnbor, chwnsize;
char          fonch;
word        * pno, * pnd;
int           firstpno, lastpno, totalpno;
int           firstpnd, lastpnd;
char        * potxt;
word          size;
SLTREE      * ptree;
WINDOW      * pwind;
DCB           dcbwrk;
MAKER         maker;
DCB           dcb = {
                    "sld", "",
                    { BORD_DDSS|BORD_SHADOW, NULL, NULL, NULL },
                    { 0, 0, 0, 0, 0, 0, 0, 0 },
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0L
};

BORDER       treebrd = { BORD_DDSS,             NULL, NULL, NULL };
BORDER       mnubrd  = { BORD_SHADOW,           NULL, NULL, NULL };
BORDER       vmnubrd = { BORD_DDSS|BORD_SHADOW, NULL, NULL, NULL };


static ITEM  in    =
{ " На входе @@@@@@@@ ",                                        "ДдLl", NULL };
static ITEM  out   =
{ " На выходе @@@@@@@@ ",                                       "ЫыSs", NULL };
static ITEM  src  =
{ " Путь: @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ ",  NULL,  NULL };
static ITEM  nam   =
{ "                 Имя: @@@@@@@@@@@@@@@@@@@@                ", "ИиBb", NULL };
static ITEM  base  =
{ " Имя базы, подключенной к документу: @@@@@@@@@@@@@@@@@@@@ ",  NULL,  NULL };
static ITEM  oper  =
{ "             Операция: @@@@@@@@@@@@@@@@@@@@@@             ", "ОоJj", NULL };
static ITEM  exec  =
{ "  Выполнение  ",                                             "НнYy", NULL };
static ITEM  end   =
{ "     Выход     ",                                            "ВвDd", NULL };

static ITEM  itm[] = {
                { "   Окно   ",      "ОоJj", NULL },
                { "  Дерево  ",      "РрHh", NULL },
                { "   Текст   ",     "ТтNn", NULL },
                { "   Поля   ",      "ПпGg", NULL },
                { "   Принтер   ",   "ИиBb", NULL },
                { "  Сохранить  ",   "СсCc", NULL },
                { "   Выход   ",     "ВвDd", NULL }
};

static  ITEM    noyes[] = {
                { " Нет ",          "НнYy",  NULL },
                { " Да ",           "ДдLl",  NULL }
};

static  ITEM    operitm [] = {
    { "Создание документа   ", "СсCc", NULL },
    { "Модификация документа", "МмVv", NULL },
    { "Удаление документа   ", "УуEe", NULL },
    { "Создание базы        ", "ЗзPp", NULL },
    { "Подключение к базе   ", "ПпGg", NULL },
    { "Отключение от базы   ", "ОоJj", NULL },
    { "Удаление базы        ", "ДдLl", NULL }
};

static  ITEM    where_bas [] = {
    { "Местная база   ", "МмVv", NULL },
    { "Глобальная база", "ГгUu", NULL }
};

static ITEM typitm [] = {
    { "DOCUMENT", "DdВв", NULL },
    { "QUERY   ", "QqЙй", NULL },
    { "LISTING ", "LlДд", NULL },
    { "CODIFIC ", "CcСс", NULL }
};

static ITEM loditm [] = {
    { "Выбор имени   ", "ВвDd", NULL },
    { "Новый документ", "НнYy", NULL }
};

static MNITEM * pitmnam, * pitmpath;

static SLTREE   tree;
static MENU   * mainmnu;
static WINDOW * fonwin;
static char     tmpname [81];
static char     Md_Env [70];
static int      hor, ver;
static PRNPAG   prnpag = { NULL, 297, 210, 22, 10, 20, 0, 1, 10, 8, 2 };
static char     docnam  [MAX_NAM_LEN + 1];
static char     basenam [MAX_NAM_LEN + 1];
static char     document [MAX_NAM_LEN + 10] = "Документ ";
static char     bas [MAX_NAM_LEN + 6] = "База ";
static bool     _global;

static  void    delbase     (void);
static  void    incbase2doc (void);
static  void    excbase2doc (void);
static  char  * getdocnam   (char * pfilnam);
static  void    getbasenam  (void);
static  bool    inpdocnam   (ATTRIB * pattr);
static  bool    creatbas    (void);
static  void    fprn        (void);
static  bool    savedoc     (void);
static  bool    doc2tmp     (char * to, int handfrom, long len, long offset);
int             edtrec      (WINDOW *pwn, char * inf);
void            loaddoc     (void);
void            ffield      (void);
void            sld2txt     (int handto, void far * pto, int tosize,
                  int handfrom, void far * pfrom, long lenfrom, word fromsize);
void            txt2sld     (int handt, void far *pto, int tosiz, int handfr,
                                 void far * pfrom, long lenfrom, word fromsize);
void makedoc (void);

void main (void) {
    WINDOW    * pwin;
    MENU      * pmenu;
    char      * p;
    int         hand, tag;
    register    i, j;
    long    hz;

    hz = coreleft ();
    if (!wniniwin (_TEXTC80, _MINROWS)) return;
    wnblinks (YES);
    Kb_fun = utoutdat;

    if ((p = getenv ("LASY")) == NULL) {
        mnwarnin (NULL, 0, 3, " ", "LASY имеет неправильную конфигурацию", " ");
        wnfinwin ();
        return;
    }
    tree.namlen = MAX_NAM_LEN;
    tree.tab    = TREE_TAB;
    ptree       = &tree;

    dcb.prnpag = prnpag;

    strcpy (Md_Env, p);
    i = strlen (Md_Env);
    if (Md_Env [i - 1] != '\\') Md_Env [i] = '\\';

    strcpy (mdini, Md_Env);
    strcat (mdini, "md.ini");

    dcb.att = Att_Wind;   fonch = 176;   hor = 16;  ver = 8;
    utmovabs (Pal_Cur, Pal_Stnd, 17, 0);
    if ((hand = utopen (mdini, UT_R)) != -1) {
        utread (hand, &mod_mono, sizeof (bool));
        utread (hand, &Att_Tree, sizeof (ATTRIB));
        utread (hand, &dcb.att,  sizeof (ATTRIB));
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
    ptree->tab = TREE_TAB;

    if ((fonwin = wncreate (17, 66, 0, 0, &mnubrd,  &Att_Mnu1))==NULL) goto ret;
    if ((mainmnu = mncreate(13, 58, 0, 0, &treebrd, &Att_Mnu1))==NULL) goto ret;

    pwin = mainmnu->pwin;

    getcwd (maker.path, 50);
    strcat (maker.path, "\\");

    in.vars    = out.vars = typitm [0].text;
    src.vars   = maker.path;
    nam.vars   = loditm [1].text;
    base.vars  = basenam;
    oper.vars  = operitm [0].text;


    if (!mnitmadd (mainmnu,  1,  2,  6, &in))      goto ret;
    if (!mnitmadd (mainmnu,  2,  2, 38, &out))     goto ret;
    if (!mnitmadd (mainmnu,  3,  4,  0, &src))     goto ret;
    if (!mnitmadd (mainmnu,  4,  6,  0, &nam))     goto ret;
    if (!mnitmadd (mainmnu,  5,  8,  0, &base))    goto ret;
    if (!mnitmadd (mainmnu,  6, 10,  0, &oper))    goto ret;
    if (!mnitmadd (mainmnu,  7, 12, 10, &exec))    goto ret;
    if (!mnitmadd (mainmnu,  8, 12, 34, &end))     goto ret;

    mnsetcur (mainmnu, 6);
    mnsetpro (mainmnu, 3, 1);
    mnsetpro (mainmnu, 5, 1);

    pitmpath = mnfnditm (mainmnu->pitems, 3);
    pitmnam  = pitmpath->next;

    wnsplitw (pwin,  9, WN_HSPLIT|WN_SINGLE);
    wnsplitw (pwin, 11, WN_HSPLIT|WN_SINGLE);

    wndsplay (fonwin,  WN_CENTER, WN_CENTER);
    mndsplay (mainmnu, WN_CENTER, WN_CENTER);
    wnwrtbuf (pwin, 0, 24, 8, "ДОКУМЕНТ", WN_NATIVE);
    wnwrtbuf (pwin, 2,  1, 4, "Тип:",     WN_NATIVE);

    while (1) {
        dcb.opt.fatherbas = dcb.opt._global = 0;
        getbasenam ();
        switch (tag = mnreadop (mainmnu, MN_NOREM)) {
            case 1:
            case 2:
                pmenu = mncrestd (typitm,4,MN_VERT,0,0,&vmnubrd,&Att_Mnu2);
                if ((i = mnreadop (pmenu, 0)) >= 0) {
                    mainmnu->pcur->vloc = typitm [i].text;
                    mndisvri (mainmnu, mainmnu->pcur);
                    if (tag == 1) {
                        mainmnu->pcur->next->vloc = typitm [i].text;
                        mndisvri (mainmnu, mainmnu->pcur->next);
                        maker.opt.entertyp = i;
                        pitmnam->vloc = loditm [1].text;
                        * maker.enterfile = EOS;
                        * docnam = EOS;
                        mndisvri (mainmnu, pitmnam);
                    }
                    maker.opt.exittyp  = i;
                }
                mndstroy (pmenu);
                break;
            case 4:
                if (maker.opt.oper == 0) {
                    pmenu = mncrestd (loditm,2,MN_VERT,0,0,&vmnubrd,&Att_Mnu2);
                    mndsplay (pmenu, WN_CURMNU, WN_CURMNU);
                    i = mnreadop (pmenu, 0);
                } else i = 0;
                switch (i) {
                    case  0:
                        p = Md_ext [maker.opt.entertyp] + 1;

                        if (!wnstrinp (WN_CURMNU, WN_CURMNU, "Введите путь",
                                                        maker.path, 50)) break;
                        utstrcvt (maker.path, ST_TOUP);
                        i = strlen (maker.path);
                        if (i > 0 && maker.path [i - 1] != '\\')
                            strcpy (&maker.path [i], "\\");
                        pwin->options.covered = 0;
                        mndisvri (mainmnu, pitmpath);
                        pwin->options.covered = 1;
                        dbsl2nam (p, 1, maker.path);
                        strcpy (maker.enterfile, maker.path);
                        if ((i = dbrolnam (maker.enterfile, p, NULL, docnam))
                                                                        == 0) {
                            mnwarnin (NULL, 0, 3, " ",
                            "Не могу найти файлы с документами указанного типа",
                                                                          " ");
                            goto clrnam;
                        } else  if (i == 1) {
                            mainmnu->pcur->vloc = docnam;
                            mndisvri (mainmnu, mainmnu->pcur);
                        }
                        break;
                    case  1:
clrnam:
                        mainmnu->pcur->vloc = loditm [1].text;
                        * maker.enterfile = EOS;
                        * docnam = EOS;
                        mndisvri (mainmnu, mainmnu->pcur);
                        break;
                    default:    break;
                }
                if (maker.opt.oper < 2) mndstroy (pmenu);
                break;
            case 6:
                pmenu = mncrestd (operitm,7,MN_VERT,0,0,&vmnubrd,&Att_Mnu2);
                mnsetcur (pmenu, maker.opt.oper);
                if ((i = mnreadop (pmenu, 0)) >= 0) {
                    mainmnu->pcur->vloc = operitm [i].text;
                    mndisvri (mainmnu, mainmnu->pcur);
                    maker.opt.oper = i;
                }
                mndstroy (pmenu);
                break;
            case 7:
                i = maker.opt.oper;
                if (i > 0 && i < 6 && * docnam == EOS) {
                    mnwarnin (NULL, 0, 3, " ", "Введите имя документа",  " ");
                    break;
                }
                if ((i == 2 || i == 5 || i == 6) &&
                       mnwarnin (noyes, 2, 2, "    Вы уверены?    ", " ") != 1)
                    break;
                if (i > 2 && i != 5) {
                    pmenu = mncrestd (where_bas, 2, MN_VERT, 0, 0,
                                                          &vmnubrd, &Att_Mnu2);
                    _global = mnreadop (pmenu, 0);
                    mndstroy (pmenu);
                    if (_global < 0) continue;
                }
                if (i == 4 || i == 6) {
                    p = "SLB";
                    if (_global) strcpy (tmpname, Md_Env);
                    else         strcpy (tmpname, maker.path);
                    dbsl2nam (p, 1, tmpname);
                    if ((j = dbrolnam (tmpname, p, NULL, NULL)) == 0) {
                        mnwarnin (NULL, 0, 3, " ",
                                            "Не могу найти файлы с базами", " ");
                        break;
                    }
                    else if (j == -1) { * maker.basename = EOS; break; }
                    else strcpy (maker.basename, &tmpname [strlen(tmpname)-12]);
                }
                switch (i) {
                    case 0:
                    case 1:
                        makedoc ();
                        break;
                    case 2:
                        if (maker.opt.entertyp == MD_CDF) {
                            hand = utopen (maker.enterfile, UT_R);
                            utread (hand, &dcbwrk, sizeof (DCB));
                            utclose (hand);
                            utremove (dcb.name_data);
                        }
                        utremove (maker.enterfile);
                        * docnam = EOS;
                        pitmnam->vloc = loditm [1].text;
                        mndisvri (mainmnu, pitmnam);
                        break;
                    case 3:
                        if (maker.opt.entertyp != 0) {
                            mnwarnin (NULL, 0, 3,
                                "База создается только для документов",
                                                        "типа DOCUMENT", " ");
                            break;
                        }
                        creatbas ();
                        break;
                    case 4:
                        incbase2doc ();
                        break;
                    case 5:
                        excbase2doc ();
                        break;
                    case 6:
                        delbase ();
                        break;
                    default :   break;
                }
                break;
            case 8:      goto ret;
            default:     break;
        }
    }
ret:
    utremove ("sld.ini");
    utremove ("slb.ini");
    utremove ("slq.ini");
    utremove ("sll.ini");
    utremove ("slc.ini");
    utremove (mdpnd);
    utremove (mddtxt);
    utremove (mdpno);
    utremove (mdotxt);
    utremove (mdtmpm);
    mndstroy (mainmnu);
    wndstroy (fonwin);
    wnfinwin ();
    hz -= coreleft ();
    if (hz != 0L) utalarm ();
    return;
}

void makedoc (void) {
    MENU      * headmnu;
    int         i;
    bool        done = NO, save = YES;

    mnremove (mainmnu);
    wnremove (fonwin);

    if (* docnam != EOS)  loaddoc ();

    headmnu = mncrestd (itm, 7, MN_HOR, 0, 0, &mnubrd, &Att_Mnu1);
    mndsplay (headmnu, 1, 0);
    while (!done) {
        i = mnreadop (headmnu, MN_NOREM);
        if (i < 5) save = NO;
        switch (i) {
            case 0: fwind ();                                 break;
            case 1: ftree ();                                 break;
            case 2: fedit ();                                 break;
            case 3:
                if (Kb_sym != Down)    ffield ();
                break;
            case 4: fprn ();                                  break;
            case 5:
                if (Kb_sym != Down)    savedoc ();
                save = YES;
                break;
            case 6:
                if (Kb_sym == Down)                           break;
                if (!save && mnwarnin (noyes, 2, 4, " ",
                       " Документ не сохранен! ",
                       " Выходить без сохранения? ", " ") != 1) break;
                trnodfre (ptree, delpuser);

                done = YES;
            default : break;
        }
    }
    mndstroy (headmnu);
    wndsplay (fonwin,  WN_NATIVE, WN_NATIVE);
    mndsplay (mainmnu, WN_NATIVE, WN_NATIVE);
    return;
}

void loaddoc (void) {
    int         handdoc;
    long        offset;

/************************* Инициализация дерева *******************************/
    Ed_lsize = /*32500;*/ 5000;
    if ((Ed_ppl = utallocb (&Ed_lsize)) == NULL) return;
    Ed_rsize = /*32500;*/ 5000;
    if ((Ed_ppr = utallocb (&Ed_rsize)) == NULL) goto ret;


    trinidoc (maker.enterfile, YES);

    wndiswrk (3L, 3, " ", "Загрузка документа", " ");
    dcb.bor = dcbwrk.bor;    dcb.att = dcbwrk.att;
    dcb.h   = dcbwrk.h;      dcb.w   = dcbwrk.w;
    dcb.row = dcbwrk.row;    dcb.col = dcbwrk.col;
    dcb.prnopt     = dcbwrk.prnopt;
    dcb.fld_prnopt = dcbwrk.fld_prnopt;
    dcb.prnpag     = dcbwrk.prnpag;
    dcb.opt        = dcbwrk.opt;
    utmovabs (dcb.name_data, dcbwrk.name_data, 60, 0);

    valtree = YES;

    if ((handdoc = utopen (maker.enterfile, UT_R)) == -1) goto ret;
    if (!valdcb (handdoc, maker.enterfile)) goto ret;
    wnupdwrk (1L);

/************************ Инициализация шаблонных строк ***********************/

    offset = sizeof (DCB) + dcbwrk.tnamlen + dcbwrk.dnamlen + dcbwrk.fnamlen +
           dcbwrk.cnamlen + dcbwrk.idclen +  dcbwrk.idflen  + dcbwrk.idllen  +
           dcbwrk.pndlen;
    if (!doc2tmp (mddtxt, handdoc, dcbwrk.dtxtlen, offset)) goto ret;

/********************* Инициализация оригинального текста *********************/

    offset += dcbwrk.dtxtlen + dcbwrk.pnolen;
    if (!doc2tmp (mdotxt, handdoc, dcbwrk.otxtlen, offset)) goto ret;

ret:
    utsleept (4);
    free (Ed_ppr);
    free (Ed_ppl);
    utclose (handdoc);
    wnremwrk ();
    return;
}

static bool     doc2tmp (char * to, int handfrom, long len, long offset) {
    int         handwrk;

    if ((handwrk = utcreate (to)) == -1) return NO;
    utlseek (handfrom, offset, UT_BEG);
    sld2txt (handwrk, Ed_ppr, Ed_rsize-10, handfrom, Ed_ppl, len, Ed_lsize-10);
    utclose (handwrk);
    wnupdwrk (1L);
    return YES;
}

void fedit (void) {
    static ITEM  itm1[] = {
        { "  Текущий    ", "ТтNn", NULL },
        { "  Новый      ", "НнYy", NULL },
        { "  Загрузить  ",  "ЗзPp", NULL }
    };

    static ITEM  itm2[] = {
        { "  Из документа            ",  "ДдLl", NULL },
        { "  Из произвольного файла  ",  "ФфAa", NULL }
    };
    MENU    * mnu1, * mnu2;
    bool    done = NO, edt = NO;


    mnu1 = mncrestd (itm1, 3, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu2);
    mnu2 = mncrestd (itm2, 2, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu1);
    mndsplay (mnu1, WN_CURMNU, WN_CURMNU);
    while (!done) {
        switch (mnreadop (mnu1, MN_HORINVER)) {
            case 0:
                if (access (mdotxt, 0) != 0) {
                    mnwarnin (NULL, 0, 3, " ",
                            "Активного текста документа не существует", " ");
                    break;
                }
                strcpy (tmpname, mdotxt);
                edt = done = YES;
                break;
            case 1:
                utremove (mdotxt);
                * tmpname = '\0';
                edt = done = YES;
                break;
            case 2:
                mndsplay (mnu2, WN_CURMNU, WN_CURMNU);
                switch (mnreadit (mnu2)) {
                    case 0:
                        if (inpdocnam (&Att_Mnu2))  edt = done = YES;
                        break;
                    case 1:
                        if (mnfnminp (WN_CURMNU, WN_CURMNU,
                        "Введите имя файла или нажмите ENTER для выбоpa имени",
                                                     tmpname, ".txt", YES, YES))
                            edt = done = YES;
                        break;
                    default : break;
                }
                mnremove (mnu2);
                break;
            default : done = YES; break;
        }
    }
    mndstroy (mnu2);
    mndstroy (mnu1);
    if (edt) {
        edtrec (pwind, tmpname);
        wndstroy (pwind);
    }
    return;
}

void ftree (void) {
    static ITEM  itm[] = {
        { "  Текущее    ", "ТтNn", NULL },
        { "  Новое      ", "НнYy", NULL },
        { "  Загрузить  ",  "ЗзPp", NULL }
    };
    MENU       * mnu;

    if ((ptree->pwin = wncreate (NUM_ROWS - 5, NUM_COLS - 2,
                    3, 1, &treebrd, &Att_Tree)) == NULL) return;
    mnu = mncrestd (itm, 3, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu2);
    mndsplay (mnu, WN_CURMNU, WN_CURMNU);
    switch (mnreadop (mnu, MN_HORINVER)) {
        case 0:
            if (ptree->root == NULL) {
                mnwarnin (NULL, 0, 3, " ",
                        "Активного дерева документа не существует", " ");
                break;
            }
            mnremove (mnu);
            wndsplay (ptree->pwin, WN_NATIVE, WN_NATIVE);
            edttree ();
            break;
        case 1:
            mnremove (mnu);
            trnodfre (ptree, delpuser);
            if (!trmkroot (ptree, "Имя документа", NULL, inipuser)) return;
            wndsplay (ptree->pwin, WN_NATIVE, WN_NATIVE);
            edttree ();
            break;
        case 2:
            if (!inpdocnam (&Att_Mnu1)) break;
            trnodfre (ptree, delpuser);
            trinidoc (tmpname, NO);
            mnremove (mnu);
            wndsplay (ptree->pwin, WN_NATIVE, WN_NATIVE);
            edttree ();
            break;
        default : break;
    }
    mndstroy (mnu);
    wndstroy (ptree->pwin);
    ptree->pwin = NULL;
    return;
}

static  bool    savedoc (void) {
    int             handto, handwrk, handfrom;
    register word   i;

    if (!valtree) {      /** valtree = YES  - Дерево можно сохранять **/
        mnwarnin (NULL, 0, 3, "Не могу сохранить документ !",
                                "Структура полей дерева некорректна.", " ");
        return NO;
    }

    if (!wnstrinp (WN_CURMNU, WN_CURMNU, "Введите путь", maker.path, 60))
        return NO;
    utuninam (tmpname, Md_ext [maker.opt.exittyp]);
    utstrcvt (maker.path, ST_TOUP);
    i = strlen (maker.path);
    if (i > 0) {
        if (maker.path [i - 1] != '\\')
            { strcpy (&maker.path [i], "\\"); i++; }
        utmovmem (tmpname + i, tmpname, 13);
        utmovabs (tmpname, maker.path, i, 0);
    }
    mndisvri (mainmnu, pitmpath);
    Ed_lsize = Ed_rsize = 32500;
    if ((Ed_ppl = utallocb (&Ed_lsize)) == NULL) return NO;

    if ((Ed_ppr = utallocb (&Ed_rsize)) == NULL) { free (Ed_ppl); return NO; }
    Ed_rsize = (Ed_rsize / 4) * 4;


    handto = utcreate (tmpname);

    wndiswrk (3L, 3, " ", "Сохранение документа", " ");

    /****************  Сохраняем  дерево *****************/

    savetree (handto, NO);
    wnupdwrk (1L);

    handwrk = utcreate (mdwrk);

    /**************** Сохраняем  шаблонные строки  ***************/
    if ((handfrom = utopen (mddtxt, UT_R)) != -1) {

        utlseek (handwrk, 0L, UT_BEG);
        utchsize (handwrk, 0L);
        txt2sld (handwrk, Ed_ppr, Ed_rsize, handfrom, Ed_ppl,
                                                 utfsize (handfrom), Ed_lsize);
        dcb.dtxtlen = (word) utlseek (handwrk, 0L, UT_END);
        utlseek (handwrk, 0L, UT_BEG);
        dcb.pndlen = makepn (handto, Ed_ppr, Ed_rsize, handwrk, Ed_ppl,
                                        dcb.dtxtlen, Ed_lsize) * sizeof (word);
        utlseek (handwrk, 0L, UT_BEG);
        while (1) {
            if ((i = utread (handwrk, Ed_ppl, Ed_lsize)) == 0) break;
            utwrite (handto, Ed_ppl, i);
        }
        utclose (handfrom);
    } else dcb.pndlen = dcb.dtxtlen = 0;
    wnupdwrk (1L);
    /**************** Сохраняем  оригинальные строки  ***************/
    if ((handfrom = utopen (mdotxt, UT_R)) != -1) {

        utlseek (handwrk, 0L, UT_BEG);
        utchsize (handwrk, 0L);
        txt2sld (handwrk, Ed_ppr, Ed_rsize, handfrom, Ed_ppl,
                                                 utfsize (handfrom), Ed_lsize);
        dcb.otxtlen = (word) utlseek (handwrk, 0L, UT_END);
        utlseek (handwrk, 0L, UT_BEG);
        dcb.pnolen = makepn (handto, Ed_ppr, Ed_rsize, handwrk, Ed_ppl,
                                        dcb.otxtlen, Ed_lsize) * sizeof (word);
        utlseek (handwrk, 0L, UT_BEG);
        while (1) {
            if ((i = utread (handwrk, Ed_ppl, Ed_lsize)) == 0) break;
            utwrite (handto, Ed_ppl, i);
        }
        utclose (handfrom);
    } else dcb.pnolen = dcb.otxtlen = 0;
    wnupdwrk (1L);

    dcb.inioff = utlseek (handto, 0L, UT_CUR);
    handfrom = utopen (mdtmpm, UT_R);
    while (1) {
        if ((i = utread (handfrom, Ed_ppl, Ed_lsize)) == 0) break;
        utwrite (handto, Ed_ppl, i);
    }
    utclose (handfrom);

    /****************** Прописываем итоговый DCB ***********************/
    if (maker.opt.oper == 1) utremove (maker.enterfile);
    else {
        if (maker.opt.exittyp == MD_CDF) utuninam (dcb.name_data, NULL);
        else                             * dcb.name_data = EOS;
        dcb.opt.fatherbas = dcb.opt._global = 0;
    }

    strcpy (maker.enterfile, tmpname);
    strcpy (docnam, ptree->root->name);
    pitmnam->vloc = docnam;
    mndisvri (mainmnu, pitmnam);
    maker.opt.entertyp = maker.opt.exittyp;
    mainmnu->pitems->next->vloc = typitm [maker.opt.exittyp].text;
    mndisvri (mainmnu, mainmnu->pitems->next);
    utlseek (handto, 0L, UT_BEG);
    utwrite (handto, &dcb, sizeof (DCB));

    free (Ed_ppl);
    free (Ed_ppr);
    utclose (handwrk);
    utclose (handto);
    utremove (mdwrk);
    wnremwrk ();
    return YES;
}

void    dcbcorrect (void) {

    if (!chwnsize) {
        dcb.h   = dcbwrk.h;
        dcb.w   = dcbwrk.w;
        dcb.row = dcbwrk.row;
        dcb.col = dcbwrk.col;
    }
    if (!chwnbor)  dcb.bor = dcbwrk.bor;
    dcb.att = dcbwrk.att;
    return;
}

void fwind (void) {
    static ITEM  itm1[] = {
        { "  Размеры  ", "РрHh", NULL },
        { "  Бордюр   ", "Бб`",  NULL },
        { "  Цвет     ", "ЦцWw", NULL }
    };

    static ITEM  itm2[] = {
        { " Загрузить из документа ", "ЗзPp", NULL },
        { " Стандартный            ", "СсCc", NULL },
        { " Установить             ", "УуEe", NULL }
    };

    MENU    * mnu1, * mnu2, * psav;
    int       hand;
    bool      done = NO;

    psav = Cur_Menu;
    mnu1 = mncrestd (itm1, 3, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu2);
    while (!done) {
        wnselect (psav->pwin);
        Cur_Menu = psav;
        mndsplay (mnu1, WN_CURMNU, WN_CURMNU);
        switch (mnreadop (mnu1, MN_HORINVER)) {
            case 0:
                mnremove (mnu1);
                resize ();
                break;
            case 1:
                mnremove (mnu1);
                bordtype ();
                break;
            case 2:
                mnu2 = mncrestd (itm2, 3, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu1);
                mndsplay (mnu2, WN_CURMNU, WN_CURMNU);
                switch (mnreadit (mnu2)) {
                    case  0:
                        if (!inpdocnam (&Att_Mnu2)) break;
                        hand = utopen (tmpname, UT_R);
                        utread (hand, &dcbwrk, sizeof (DCB));
                        dcb.att = dcbwrk.att;
                        utclose (hand);
                        break;
                    case  1:
                        if ((hand = utopen (mdini, UT_R)) != -1) {
                            utlseek (hand, sizeof (ATTRIB) + 2L, UT_BEG);
                            utread (hand, &dcb.att, sizeof (ATTRIB));
                            utclose (hand);
                        } else  dcb.att = Att_Wind;
                        break;
                    case  2:
                        doccolor ();
                    default : break;
                }
                mndstroy (mnu2);
                break;
            case -1:  done = YES; break;
            default : break;
        }
    }
    mndstroy (mnu1);
    return;
}

void zoomwin (void) {
    register    i, tmp;

    if (dcb.w == 0)  {
        pwind = wncreate (1, 2, 0, 0, &dcb.bor, &Att_Edit);
        wndsplay (pwind, 12, WN_CENTER);
        utspeakr (100);
        utsleept (2);
        for (i = 0, tmp = 150; i < 8; i++) {
            utspeakr (tmp -= 5);
            wnresize (pwind, 1, 4, -1, -4);
            if (i < 4) utsleept (1);
        }
        utspeakr (tmp -= 5);
        for (; tmp < 8000; tmp += 250)  {
            delay (5);
            utspeakr (tmp);
        }
        for (; tmp > 0; tmp -= 2000)  {
            delay (5);
            utspeakr (tmp);
        }
        dcb.h   = (int) pwind->img.dim.h;
        dcb.w   = (int) pwind->img.dim.w;
        dcb.row = (int) pwind->where_shown.row;
        dcb.col = (int) pwind->where_shown.col;
    } else  {
        pwind = wncreate (dcb.h, dcb.w, dcb.row, dcb.col, &dcb.bor, &Att_Edit);
        wndsplay (pwind, WN_NATIVE, WN_NATIVE);
    }
    return;
}

void    savecolor (ATTRIB * pattrib, int place) {
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
        utwrite (hand, &dcb.att,  sizeof (ATTRIB));
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
            "Не могу сохранить цвета: ошибка при открытии файла" , " ");
    return;
}

static bool  creatbas (void) {
    int            handslb, handslh, handsli = -1;
    int            hand, len = MAX_NAM_LEN + 14, nfiles;
    bool           indx, retcode = YES;
    char         * pname;
    static SLHD    slhd;

    trinidoc (maker.enterfile, NO);
    pname = (char *) ptree->root->name;
    dcb = dcbwrk;

    if (* dcb.name_data != EOS) {
        strcpy (tmpname, Md_Env);
        strcat (tmpname, dcb.name_data);
        strcat (tmpname, ".SLB");
        if (access (tmpname, 0) == 0) {
            utmovabs (document + 9, docnam, MAX_NAM_LEN, 0);
            if (mnwarnin (noyes, 2, 5, document, "подключен к базе",
                      getdocnam (dcb.name_data), "Создавать новую?", " ") != 1)
                {  retcode = NO; goto ret; }
        }
    }
    if (!wnstrinp (WN_CURMNU, WN_CURMNU, "Введите имя базы",
                                            pname, MAX_NAM_LEN))
        { retcode = NO; goto ret; }

    strcpy (tmpname, Md_Env);
    dbsl2nam ("SLB", 1, tmpname);
    hand = utopen ("SLB.INI", UT_R);
    utread (hand, &nfiles, sizeof (int));
    for (; nfiles > 0; nfiles--) {
        utread (hand, tmpname, len);
        if (strcmp (pname, getdocnam (&tmpname [MAX_NAM_LEN + 1])) == 0)
            { retcode = NO; break; }
    }
    utclose (hand);
    if (!retcode) {
        strcat (bas, pname);
        mnwarnin (NULL, 0, 5, " ", bas, "уже существует.",
                            "Каждая база должна иметь уникальное имя!", " ");
        goto ret;
    }

    utuninam (maker.basename, ".SLB");
    hand = utopen (maker.enterfile, UT_RW);
    utread  (hand, &dcbwrk, sizeof (DCB));
    dcbwrk.opt.fatherbas = 1;
    dcbwrk.opt._global = _global;
    utmovabs (dcbwrk.name_data, maker.basename, 8, 0);
    utlseek (hand, 0L, UT_BEG);
    utwrite (hand, &dcbwrk, sizeof (DCB));
    utclose (hand);

    strcpy (dcb.name_data, maker.basename);
    if (_global) strcpy (tmpname, Md_Env);
    else         strcpy (tmpname, maker.path);
    strcat (tmpname, maker.basename);
    handslb = utcreate (tmpname);
    indx = savetree (handslb, YES);
    utchext (tmpname, ".SLH");
    handslh = utcreate (tmpname);
    slhd.treelen = (int) utfsize (handslb);
    slhd.date = utdtoday ();
    utwrite (handslh, &slhd, sizeof (SLHD));
    if (indx) { utchext (tmpname, ".SLI"); handslh = utcreate (tmpname); }
    utclose (handslb);   utclose (handslh);    utclose (handsli);
    strcpy (basenam, pname);
    mndisvri (mainmnu, pitmnam->next);
ret:
    document [9] = bas [5] = EOS;
    trnodfre (ptree, delpuser);
    return retcode;
}

static ITEM prnitm [] = {
    { " Шрифт текста документа ", "ТтNn", NULL },
    { " Шрифт полей документа  ", "ПпGg", NULL },
    { " Расположение на листе  ", "РрHh", NULL }
};

static void fprn (void) {
    MENU      * pmnu;
    bool        done = NO;

    pmnu = mncrestd (prnitm, 3, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu2);
    mndsplay (pmnu, WN_CURMNU, WN_CURMNU);

    while (!done) {
        switch (mnreadop (pmnu, MN_HORINVER)) {
            case  0:
                mnprnopt (&dcb.prnopt, " Шрифт тектса документа ", &Att_Mnu1);
                break;
            case  1:
                mnprnopt (&dcb.fld_prnopt, " Шрифт полей документа ", &Att_Mnu1);
                break;
            case  2:
                mnprnpag (&dcb.prnpag, &Att_Mnu1, NULL, 0L, NO);
                dcb.prnpag.firstbrk = NULL;
                break;
            default:    done = YES;     break;
        }
    }
    mndstroy (pmnu);
    return;
}

static  bool    inpdocnam (ATTRIB * pattr) {
    MENU        * pmnu, * savmnu = Cur_Menu;
    char        * p;
    bool          retcode = YES;
    register      i;

    pmnu = mncrestd (typitm, 4, MN_VERT, 0, 0, &vmnubrd, pattr);
    if ((i = mnreadop (pmnu, 0)) < 0) { retcode = NO; goto ret; }
    Cur_Menu = savmnu;
    p = Md_ext [i] + 1;
    strcpy (tmpname, maker.path);
    if (!wnstrinp (WN_CURMNU, WN_CURMNU, "Введите путь", tmpname, 60))
        { retcode = NO; goto ret; }
    dbsl2nam (p, 1, tmpname);
    if ((i = dbrolnam (tmpname, p, NULL, NULL)) == 0)
        mnwarnin (NULL, 0, 3, " ",
                    "Не могу найти файлы с документами указанного типа", " ");
    if (i < 1) retcode = NO;
ret:
    mndstroy (pmnu);
    return retcode;
}

static char * getdocnam (char * pfilnam) {
    int             hand;
    static char     ptmp [MAX_NAM_LEN + 1];

    hand = utopen (pfilnam, UT_R);
    utread (hand, &dcbwrk, sizeof (DCB));
    utread (hand, ptmp, MAX_NAM_LEN);
    utclose (hand);
    return ptmp;
}

static void getbasenam (void) {
    int             hand;

    if (* maker.enterfile == EOS) { * basenam = EOS; goto ret; }
    hand = utopen (maker.enterfile, UT_R);
    utread (hand, &dcb, sizeof (DCB));
    utclose (hand);
    if (dcb.opt._global) strcpy (tmpname, Md_Env);
    else                 strcpy (tmpname, maker.path);
    strcat (tmpname, dcb.name_data);
    strcat (tmpname, ".SLB");
    if (access (tmpname, 0) != 0) * basenam = EOS;
    else  strcpy (basenam, getdocnam (tmpname));
ret:
    mndisvri (mainmnu, pitmnam->next);
    return;
}

static void incbase2doc (void) {
    int     hand;

    hand = utopen (maker.enterfile, UT_WR);
    utread (hand, &dcb, sizeof (DCB));
    if (* dcb.name_data != EOS) {
        if (dcb.opt._global) strcpy (tmpname, Md_Env);
        else                 strcpy (tmpname, maker.path);
        strcat (tmpname, dcb.name_data);
        strcat (tmpname, ".SLB");
        if (access (tmpname, 0) == 0) {
            utmovabs (document + 9, docnam, MAX_NAM_LEN, 0);
            if (mnwarnin (noyes, 2, 5, document, "подключен к базе",
                        getdocnam (tmpname), "Подключать к другой?", " ") != 1)
                goto ret;
        }
    }
    dcb.opt.fatherbas = 0;
    dcb.opt._global = _global;
    utmovabs (dcb.name_data, maker.basename, 8, 0);
    utlseek (hand, 0L, UT_BEG);
    utwrite (hand, &dcb, sizeof (DCB));
ret:
    document [9] = EOS;
    utclose (hand);
    return;
}

static void excbase2doc (void) {
    int     hand;

    hand = utopen (maker.enterfile, UT_RW);
    utread (hand, &dcb, sizeof (DCB));
    dcb.opt.fatherbas = dcb.opt._global = 0;
    * dcb.name_data = EOS;
    utlseek (hand, 0L, UT_BEG);
    utwrite (hand, &dcb, sizeof (DCB));
    utclose (hand);
    return;
}

static void delbase (void) {

    if (_global) strcpy (tmpname, Md_Env);
    else         strcpy (tmpname, maker.path);
    strcat (tmpname, maker.basename);
    utremove (tmpname);
    utchext (tmpname, ".SLH");
    utremove (tmpname);
    utchext (tmpname, ".SLI");
    utremove (tmpname);
    * basenam = EOS;
    mndisvri (mainmnu, pitmnam->next);
    return;
}

/******************************************************************************
                  1         2         3         4         5
        0123456789012345678901234567890123456789012345678901234567
       ┌──────────────────────────────────────────────────────────┐
       │                        ДОКУМЕНТ                          │0
       │                                                          │1
       │ Тип:  На входе ШШШШШШШШ               На выходе ШШШШШШШШ │2
       │                                                          │3
       │ Путь: ШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШШ │4
       │                                                          │5
       │                 Имя: ШШШШШШШШШШШШШШШШШШШШ                │6
       │                                                          │7
       │ Имя базы, подключенной к документу: ШШШШШШШШШШШШШШШШШШШШ │8
       ├──────────────────────────────────────────────────────────┤9
       │               Операция: ШШШШШШШШШШШШШШШШШШШ              │10
       ├──────────────────────────────────────────────────────────┤11
       │            Выполнение              ШШВыходШШШ            │12
       └──────────────────────────────────────────────────────────┘

******************************************************************************/

