#include    "makedoc.h"

static void           upddoc  (void);

static char         * ttl = "Заголовок";
static WINDOW       * tstwind;
static BORDER         tstbor = { BORD_DDSS|BORD_SHADOW,NULL,NULL,NULL };

void doccolor (void) {
    static ITEM  itm[] = {
        { "  Текст              ",     "ТтNn", NULL },     /* TEXT */
        { "  Бордюр             ",     "Бб`" , NULL },     /* BORD */
        { "  Заголовок          ",     "ГгUu", NULL },     /* TITL */
        { "  Активное поле      ",     "АаFf", NULL },     /* AFLD */
        { "  Неактивное поле    ",     "НнYy", NULL },     /* NFLD */
        { "  Запрещенное поле   ",     "ЗзPp", NULL },     /* PROT */
        { "  Ошибочное поле     ",     "ОоJj", NULL },     /* ERR  */
        { "  Абзац              ",     "ЦцWw", NULL },     /* NICE */
        { "  Активное на абзаце ",     "КкRr", NULL },     /* BOLD */
        { "",                           NULL,  NULL },
        { "  Сохранить          ",     "СсCc", NULL }
    };
    static char   * txt[] = { "         Текст      ",
                              "  Текст  Активное   ",
                              "     Текст          ",
                              "   Абзац            ",
                              "   Неактивное       ",
                              "   Активное + абзац ",
                              "           Текст    ",
                              " Текст Запрещенное  ",
                              "        Текст       ",
                              "   Ошибочное  Текст ",
                              " Текст              " };

    MENU          * mnu, * psav;
    CELLSC        * pdata;
    byte          * att;
    bool            done = NO;
    int             i;
    static int      lastitem;

    psav = Cur_Menu;

    tstbor.pttitle = ttl;
    tstbor.type |= BORD_TCT;
    if ((tstwind = wncreate (11, 20, 0, 0, &tstbor, &dcb.att)) == NULL) return;
    mnu = mncrestd (itm, 11, MN_VERT, 0, 0, &vmnubrd, &Att_Mnu2);
    mnsetpro (mnu, 9, 1);
    mnsetcur (mnu, lastitem);
    wnsplitw (mnu->pwin, 9, WN_HSPLIT|WN_SINGLE);
    pdata = tstwind->img.pdata;
    for (i = 0; i < 11; i++, pdata += 20)
        wnmovpsa (pdata, txt[i], 20, dcb.att.text);
    while (!done) {
        wnselect (psav->pwin);
        Cur_Menu = psav;
        mndsplay (mnu, WN_CURMNU, WN_CURMNU);
        i = mnreadit (mnu);
        if (i >= 0 && i != 10) { wndsplay (tstwind,WN_CENTER, 57); lastitem = i;}
        switch (i) {
            case  0:   att = &dcb.att.text;         break;
            case  1:   att = &dcb.att.bord;         break;
            case  2:   att = &dcb.att.titl;         break;
            case  3:   att = &dcb.att.afld;         break;
            case  4:   att = &dcb.att.nfld;         break;
            case  5:   att = &dcb.att.prot;         break;
            case  6:   att = &dcb.att.err;          break;
            case  7:   att = &dcb.att.nice;         break;
            case  8:   att = &dcb.att.bold;         break;
            case 10:   savecolor (&dcb.att, 2);
            case -1:   done = YES;
            default :                            continue;
        }
        wncolors (att, itm[i].text, upddoc);
lab:
        wnremove (tstwind);
    }
    tstbor.type ^= BORD_TCT;
    mndstroy (mnu);
    wndstroy (tstwind);
    return;
}

static void     upddoc (void) {
    CELLSC      * pdata = tstwind->img.pdata;
    char far    * pbuf, * pscreen;
    register    W = tstwind->img.dim.w;

    wnmovpa (pdata,         11 * W, dcb.att.text);         /* Текст       */
    wnmovpa (pdata + W + 9,      8, dcb.att.afld);         /* Активное    */
    wnmovpa (pdata + 3 * W,      W, dcb.att.nice);         /* Абзац       */
    wnmovpa (pdata + 4 * W,      W, dcb.att.nfld);         /* Неактивное  */
    wnmovpa (pdata + 5 * W,      W, dcb.att.bold);         /* Акт. + абз. */
    wnmovpa (pdata + 7 * W + 7, 11, dcb.att.prot);         /* Запрещенное */
    wnmovpa (pdata + 9 * W + 3,  9, dcb.att.err);          /* Ошибочное   */

    pbuf = (char far *) tstwind->img.pdata;
    pscreen = wnviptrl (tstwind->where_shown.row, tstwind->where_shown.col);
    wnvicopy (&pbuf, &pscreen, 11, W, NUM_COLS*2, 0, Cmd[1]);
    tstwind->attr = dcb.att;
    wnputbor (tstwind);
    return;
}

/*******************************************************************************
                            О К Н О
 57
          10
 0123456789 123456789
┌──────Заголовок─────╖
│         Текст      ║░░0
│  Текст  Активное   ║░░1
│     Текст          ║░░2
│   Абзац            ║░░3
│   Неактивное       ║░░4
│   Активное + абзац ║░░5
│           Текст    ║░░6
│ Текст Запрещенное  ║░░7
│        Текст       ║░░8
│   Ошибочное  Текст ║░░9
│ Текст              ║░░10
╘════════════════════╝░░
 ░░░░░░░░░░░░░░░░░░░░░░░
*******************************************************************************/

