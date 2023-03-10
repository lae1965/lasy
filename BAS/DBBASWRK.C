#include    <sldbas.h>
#include    <dir.h>


void dbrenambas (char * old, bool oldglobal,
                                        char * new, bool newglobal, bool grp) {
    char    * oldname = (char *) Db_pwrk;
    char    * newname = (char *) Db_pwrk + 200;
                                      /* ??६?頥? (??२?????뢠??) SLB */
    dbmkname   (oldname, old, DB_BAS, oldglobal);
    dbmkname   (newname, new, DB_BAS, newglobal);
    utrename (oldname, newname);
                                      /* ??६?頥? (??२?????뢠??) SLH */
    utchext  (oldname, Db_ext [DB_HEAD]);
    utchext  (newname, Db_ext [DB_HEAD]);
    utrename (oldname, newname);

                                      /* ??६?頥? (??२?????뢠??) SLG */
    if (grp) {
        utchext  (oldname, Db_ext [DB_GROUP]);
        utchext  (newname, Db_ext [DB_GROUP]);
        utrename (oldname, newname);
    }
    return;
}

void dbdelbas (char * name, bool _global, bool grp) {
    char * pname = (char *) Db_pwrk;
                                            /* ????塞 SLB */
    dbmkname   (pname, name, DB_BAS, _global);
    utremove (pname);
                                            /* ????塞 SLH */
    utchext  (pname, Db_ext [DB_HEAD]);
    utremove (pname);

                                            /* ????塞 SLG */
    if (grp) {
        utchext  (pname, Db_ext [DB_GROUP]);
        utremove (pname);
    }
    return;
}

bool dbcopybas (char * old, bool oldglobal,
                                        char * new, bool newglobal, bool grp) {
    char      * oldname = (char *) Db_pwrk;
    char      * newname = (char *) Db_pwrk + 200;
    bool        success = YES;

                                            /* ?????㥬 SLB */
    dbmkname (oldname, old, DB_BAS, oldglobal);
    dbmkname (newname, new, DB_BAS, newglobal);
    if (utcopy (newname, oldname, NO) != 1) return NO;
                                            /* ?????㥬 SLH */
    utchext (oldname, Db_ext [DB_HEAD]);
    utchext (newname, Db_ext [DB_HEAD]);
    if (utcopy (newname, oldname, NO) != 1)
        { success = NO; goto ret; }
                                            /* ?????㥬 SLH */
    if (grp) {
        utchext (oldname, Db_ext [DB_GROUP]);
        utchext (newname, Db_ext [DB_GROUP]);
        if (utcopy (newname, oldname, NO) != 1) success = NO;
    }
ret:
    if (!success) dbdelbas (new, newglobal, grp);
    return success;
}

bool dbmovbas (char * old, bool oldglobal,
                                        char * new, bool newglobal, bool grp) {
    char        diskcur = 0, diskglob = 0;
    register    retcode = YES;

    if (oldglobal != newglobal) {
        diskcur = (char) (getdisk () + 'A');
        diskglob = * Db_Env;
        diskglob = (char) utcupper ((int) diskglob);
    }

    if (diskglob == diskcur) {    /* ???? ??६?頥??? ?? ??? ?? ???? */
        dbrenambas (old, oldglobal, new, newglobal, grp);
        return YES;
    }

    retcode = dbcopybas (old, oldglobal, new, newglobal, grp); /* ?????㥬 ???? */

    if (retcode) dbdelbas (old, oldglobal, grp); /* ? ??.㤠?? 㤠?塞 ??.???? */

    return retcode;
}

IDBASE * dbcreatbas (char * docfile, bool docglob, int doctype, char * basfile,
                                                                bool basglob) {
    DCB       * dcb = (DCB *) Db_pwrk;
    IDBASE    * pidb = NULL;
    char      * pname = (char *) Db_pwrk;
    bool        retcode = YES;
    register    len, hand;

    dbmkname (pname, basfile, DB_BAS, basglob);
    dbcreatmp (pname);

    if ((hand = dbopen (docfile, doctype, docglob, UT_R)) == -1)
        { retcode = NO; goto ret; }
    utread (hand, dcb, sizeof (DCB));

    if ((pidb = dbopendb (basfile, basglob, UT_RW)) == NULL)
        { retcode = NO; goto ret; }

    if (!utwrite (pidb->handb, dcb, sizeof (DCB)))
        { retcode = NO; goto ret; }

    len = dcb->cnamlen + dcb->fnamlen + dcb->tnamlen + dcb->dnamlen +
                                        dcb->idclen  + dcb->idflen;
    if (!uthndcopy (pidb->handb, hand, (long) len))
        { retcode = NO; goto ret; }
    pidb->slhd.treelen = (int) utlseek (pidb->handb, 0L, UT_END);
    utlseek (pidb->handh, 0L, UT_BEG);
    if (!utwrite (pidb->handh, &pidb->slhd, sizeof (SLHD)))
        { retcode = NO; goto ret; }

ret:
    utclose (hand);
    if (retcode) return pidb;

    dbclosdb (pidb);
    dbdelbas (basfile, basglob, NO);
    return NULL;
}

