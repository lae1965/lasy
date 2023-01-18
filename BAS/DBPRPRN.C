#include    <sldbas.h>

static char   tmpprn [] = "tmpprn.tmp";

void dbprprn (void (*preview) (void)) {
    DOC         * doc = Cur_Edt;
    bool          change = doc->opt.change;
    register      hand;

    if (Cur_Edt->cur != NULL && !dbsetfld ()) return;

    doc->opt.for_prn = YES;
    hand = utcreate (tmpprn);

    dbsavdoc (doc, doc->crtroot, hand);
    utlseek (hand, 0L, UT_BEG);
    dbloddoc (doc, hand);

    if (!dboutprn (doc, NULL, preview)) Db_Err = NO_MEM;

    doc->opt.for_prn = NO;
    utlseek (hand, 0L, UT_BEG);
    dbloddoc (doc, hand);

    doc->opt.change = change;
    doc->opt.for_prn = NO;
    utclose (hand);
    utremove (tmpprn);
    return;
}

