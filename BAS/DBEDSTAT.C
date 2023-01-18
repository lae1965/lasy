#include    <sldbas.h>
#include    <alloc.h>

static char * jmp [] = { "Fix", "Jmp" };
static char * ins [] = { "Ovr", "Ins" };
static char * ldr [] = { "Double", "Single", "Erase " };
static char * mov [] = { "Edit", "Move" };
static char * jst [] = { " Left ", "Right ", "Centre", "Format" };
static char * typ [] = {
"Int   ", "Long  ", "Float ", "Double", "Text  ", "Money ", "Date  ", "Code  "
};

static char pstat [] =
"L:     C:                      LaSy 1.00                     ";

void dbedstat (void) {   /*** Вызывается перед ожиданием клавиши ***/
/*           P:
"L:     C:   Jmp  Ins   Centre LaSy 1.00  Single    Edit     253k 22/07/92 12:15";
 Текущая запись:                 WORKING  Symple   From 1000000000  To 1000000000
 01234567890123456789012345678901234567890123456789012345678901234567890123456789
		   1         2         3         4         5         6         7
*/

    PAGBRK      * curbrk;
    CELLSC      * pdata;
    DOC         * doc = Cur_Edt;
    DOCEDT      * cur = doc->cur;
    IDFLD       * pidf;
    char        * p = (char *) Db_pwrk, * p2 = (char *) Db_pwrk + 100;
    char far    * pbuf,   * pscreen;
    register      i, j;
    int           pos = doc->pos;
    long          line = cur->line;
    byte          nfld = Att_Khlp.nfld, afld = Att_Khlp.afld;

    pdata = Db_pstat->img.pdata;
    wnmovpsa (pdata,      pstat, 61, nfld);
    wnmovpca (pdata + 80,    ' ', 80, nfld);

    if (doc->opt.for_prn) {
        line = doc->view.line;
        pos = doc->view.pos + cur->visicol;
        wnmovps  (pdata + 12,  "P:", 2);
        for (curbrk = doc->prnpag.firstbrk, i = 0; line >= curbrk->line; i++)
            curbrk = curbrk->next;
        j = utl2crdx ((long) i, p, 0, 10);
        wnmovpsa (pdata + 14, p, j, afld);
    }

    j = utl2crdx (line, p, 0, 10);
    wnmovpsa (pdata + 2, p, j, afld);

    j = utl2crdx ((long) pos, p, 0, 10);
    wnmovpsa (pdata + 9, p, j, afld);
    if (doc->opt.for_prn) goto output;

    wnmovpsa (pdata + 12, jmp [doc->opt.autojump], 3, afld);
    wnmovpsa (pdata + 17, ins [doc->opt.insert], 3, afld);

    if (cur->pfld->prnopt.jst == UT_FORMAT) i = 3;
    else {
        i = cur->pfld->prnopt.jst;
        i = utclrbit (i, UT_SGN_BEG);
    }
    wnmovpsa (pdata + 24, jst [i], 6, afld);

    if (!doc->opt.move) wnmovpsa (pdata + 42, ldr [doc->opt.single], 6, afld);
    wnmovpsa (pdata + 52, mov [doc->opt.move], 4, afld);

    if (doc->doctype == DB_DICT) goto output;

    pidf = dbgetidf (doc, cur->pfld);
    if (pidf->valtype < SL_USER) {
        wnmovpsa (pdata + 121, typ [pidf->valtype], 6, afld);
        if (pidf->valtype == SL_TEXT && pidf->initype == SL_DICT)
            wnmovpsa (pdata + 127, "+  Code", 7, afld);
    }
    switch (pidf->valtype) {
        case SL_INT:
            i = utl2crdx ((long) pidf->dnval.i, p,  0, 10);
            j = utl2crdx ((long) pidf->upval.i, p2, 0, 10);
            break;
        case SL_LONG:
            i = utl2crdx (pidf->dnval.l, p,  0, 10);
            j = utl2crdx (pidf->upval.l, p2, 0, 10);
            break;
        case SL_FLOAT:
            i = utdbl2ce ((double) pidf->dnval.f, p,  5, CV_RANGE1|CV_FLOAT);
            j = utdbl2ce ((double) pidf->upval.f, p2, 5, CV_RANGE1|CV_FLOAT);
            break;
        case SL_DOUBLE:
            i = utdbl2ce (pidf->dnval.d, p,  5, CV_RANGE1);
            j = utdbl2ce (pidf->upval.d, p2, 5, CV_RANGE1);
            break;
        case SL_DATE:
            i = utdate2c (pidf->dnval.l, p,  '/', CV_CENTURE);
            j = utdate2c (pidf->upval.l, p2, '/', CV_CENTURE);
            break;
        default :  i = 0; break;
    }
    if (i > 0) {
        wnmovps (pdata + 130, "From", 4);
        wnmovpsa (pdata + 135, p, i, afld);

        wnmovps (pdata + 147, "To", 2);
        wnmovpsa (pdata + 150, p2, j, afld);
    }

output:
    if (doc->numrec > 0L) {
        i = utl2crdx (doc->numrec, p, 0, 10);
        wnmovps  (pdata + 80, "Текущая запись:", 15);
        wnmovpsa (pdata + 96, p, i, afld);
    }
    pbuf =  (char far *) (Db_pstat->img.pdata);
    pscreen = wnviptrl (0, 0);
    wnvicopy(&pbuf, &pscreen, 1, 61, NUM_COLS*2, 0, Cmd[1]);
    pbuf =  (char far *) (Db_pstat->img.pdata + NUM_COLS);
    pscreen = wnviptrl (1, 0);
    wnvicopy(&pbuf, &pscreen, 1, NUM_COLS, NUM_COLS*2, 0, Cmd[1]);
    return;
}

