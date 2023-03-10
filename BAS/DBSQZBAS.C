#include    <sldbas.h>
#include    <slkeyb.h>


bool    dbsqzbas (char * pfilnam, bool _global,
                                       bool savdelrec, bool updwrk, bool grp) {
    IDBASE  * pidbfrom = NULL, * pidbto = NULL;
    IDGROUP   idgroup;
    char      _tmpbas [] = "_tmpbas.slg";
    char    * tmpbas = _tmpbas + 1;
    long      total, l, curnumrec, firstnumrec;
    bool      success = YES;
    int       handgrpfrom, handgrpto, totalgrp;

    if ((pidbto = dbcreatbas (pfilnam, _global, DB_BAS, tmpbas, NO)) == NULL)
        { success = NO; goto ret; }

    if ((pidbfrom = dbopendb (pfilnam, _global, UT_R)) == NULL)
        { success = NO; goto ret; }

    utlseek (pidbto->handh,   sizeof (SLHD), UT_BEG);

    if (grp) {
        handgrpfrom = dbopen (pfilnam, DB_GROUP, _global, UT_R);
        handgrpto   = utcreate (tmpbas);
        totalgrp = (int) (utfsize (handgrpfrom) / sizeof (IDGROUP));
        curnumrec = 1L;
    } else      totalgrp = 1;

    for (; totalgrp > 0; totalgrp--) {
        if (grp) {
            utread (handgrpfrom, &idgroup, sizeof (IDGROUP));
            total = idgroup.num_of_recs;
            if (total == 0) goto writegrp;
            firstnumrec = idgroup.firstrec;
        } else {
            total = pidbfrom->slhd.total;
            firstnumrec = 1L;
        }

        utlseek (pidbfrom->handh,
                    (firstnumrec - 1) * sizeof (long) + sizeof (SLHD), UT_BEG);

        for (; total > 0L; total--) {

            while (1) {
                if (kbvfysym (1)) { success = NO; goto ret; }  /* Esc */
                utread (pidbfrom->handh, &l, sizeof (long));
                if (l != -1L) break;               /* ??諨 ???稬?? ?????? */
                if (savdelrec)
                    if (!utwrite (pidbto->handh, &l, sizeof (long)))
                        { success = NO; goto ret; }
            }
            if (updwrk) wnupdwrk (1L);
            utlseek (pidbfrom->handb, l, UT_BEG);    /* ??⠫? ?? ?? ??砫? */
            l = utlseek (pidbto->handb, 0L, UT_END);
            if (!utwrite (pidbto->handh, &l, sizeof (long)))
                { success = NO; goto ret; }
                            /* ??।????? ? ?ய?ᠫ? ᬥ饭?? ?? ?????? ? to */
            utread  (pidbfrom->handb, &l, sizeof (long));
            if (!utwrite (pidbto->handb, &l, sizeof (long)))
                { success = NO; goto ret; }
            l -= sizeof (long);

            if (!uthndcopy (pidbto->handb, pidbfrom->handb, l))
                { success = NO; goto ret; }
        }
        if (grp) {
            idgroup.firstrec = curnumrec;
writegrp:
            if (!utwrite (handgrpto, &idgroup, sizeof (IDGROUP)))
                { success = NO; goto ret; }
            curnumrec += idgroup.num_of_recs;
        }
    }

    pidbto->slhd.date  = utdtoday ();
    pidbto->slhd.len   = pidbfrom->slhd.len;
    pidbto->slhd.total = pidbfrom->slhd.total;

    utlseek (pidbto->handh, 0L, UT_BEG);
    if (!utwrite (pidbto->handh, &pidbto->slhd, sizeof (SLHD)))
        success = NO;
ret:
    if (grp) {
        utclose (handgrpto);
        utclose (handgrpfrom);
    }
    dbclosdb (pidbto);
    dbclosdb (pidbfrom);

    if (success) {
                /* ??८??뢠?? ?????? ???? ?६????? ?????? */
        dbrenambas (pfilnam, _global, _tmpbas, _global, grp);

             /* ?᫨ ????? ???? ??८????????? ? ??????, 㤠?塞 ?????? */
        if (dbmovbas (tmpbas, NO, pfilnam, _global, grp))
            dbdelbas (_tmpbas, _global, grp);
        else {
                /* ????? ??????頥? ???ன ???? ?? ??? */
            dbrenambas (_tmpbas, _global, pfilnam, _global, grp);
            success = NO;
        }
    }
           /* ?᫨ ?訡?? - 㤠?塞 ????? ???? */
    if (!success) dbdelbas (tmpbas, NO, grp);

    return success;
}

