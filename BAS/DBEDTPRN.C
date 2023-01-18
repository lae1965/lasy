#include    <sldbas.h>
#include    <slmous.h>



void dbed_2fl (DOC * doc, bool visi) {

    Cur_Edt = doc;
    if (visi && !dbsetfld ()) return;
    * Db_pwrk = EOS;
    mshide ();
    if (!wnstrinp (WN_CENTER, WN_CENTER, " Имя файла ",
            (char *) Db_pwrk, 60) || strlen ((char *) Db_pwrk) == 0)
        return;
    if (!dboutprn (doc, (char *) Db_pwrk, NULL)) Db_Err = NO_MEM;
    msunhide ();
    return;
}

void dbed_prf (void) {
    DOC     * doc = Cur_Edt;
    DOCEDT  * cur = doc->cur;

    if (cur->pfld->opt.wrap) {
        mnprnopt (&cur->pfld->prnopt, " Шрифт параграфа ", &Att_Mnu1);
        dbpr2par (cur->pfld);
    } else   mnprnopt (&cur->pfld->prnopt, " Шрифт поля ", &Att_Mnu1);
    doc->opt.change = YES;
    return;
}

void dbed_prd (void) {

    mnprnopt (&Cur_Edt->prnopt, " Шрифт текста документа ", &Att_Mnu2);
    return;
}

