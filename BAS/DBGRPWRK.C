#include    <sldbas.h>


int dbbase2grp (char * lstfilename, char * basename,
                                                  bool _global, long numpapa) {
    IDGROUP   idgroup;
    long    * pnum;
    word      buflen;
    int       hand, i;

    hand = dbopen (basename, DB_GROUP, _global, UT_R);
    utlseek (hand, (numpapa - 1) * sizeof (IDGROUP), UT_BEG);
    utread  (hand, &idgroup, sizeof (IDGROUP));
    utclose (hand);

    buflen = (word) idgroup.num_of_recs * sizeof (long);
    pnum = malloc (buflen);
    for (i = 0; i < idgroup.num_of_recs; i++) pnum [i] = idgroup.firstrec + i;

    if ((hand = utopen (lstfilename, UT_W)) == -1)
        hand = utcreate (lstfilename);
    utwrite (hand, pnum, buflen);
    utclose (hand);

    free (pnum);
    return idgroup.num_of_recs;
}

long dbgrp2base (char * lstfilename, char * basename,
                                                  bool _global, long numpapa) {
    IDGROUP   idgroup;
    long      numrec, offset, l;
    long    * pnum;
    word      buflen;
    int       hand, i;

    hand  = utopen (lstfilename, UT_R);
    buflen = (word) utfsize (hand);
    pnum = malloc (buflen);
    utread (hand, pnum, buflen);
    utclose (hand);

    hand = dbopen (basename, DB_HEAD, _global, UT_RW);
    idgroup.num_of_recs = (int) (buflen / sizeof (long));

    for (i = 0; i < idgroup.num_of_recs; i++) {
        numrec = pnum [i];
        utlseek (hand, (numrec - 1) * sizeof (long) + sizeof (SLHD), UT_BEG);
        utread  (hand, &offset, sizeof (long));
        utlseek (hand, -((long) sizeof (long)), UT_CUR);
        l = -1L;
        utwrite (hand, &l, sizeof (long));
        pnum [i] = offset;
    }
    l = utlseek (hand, 0L, UT_END);
    utwrite (hand, pnum, buflen);
    free (pnum);
    idgroup.firstrec = (l - sizeof (SLHD)) / sizeof (long) + 1;
    utclose (hand);

    hand = dbopen (basename, DB_GROUP, _global, UT_W);
    utlseek (hand, (numpapa - 1) * sizeof (IDGROUP), UT_BEG);
    utwrite (hand, &idgroup, sizeof (IDGROUP));
    utclose (hand);

    return idgroup.firstrec;
}

long dbinsrec2grp (int numitm) {
    DOC     * basedoc;
    long    * pnum, numrec, offset;
    int       hand, remain;

    basedoc = dbinidoc (Cur_Lst->name_data, 0, DB_BAS, Cur_Lst->opt._global);
    dbsavrec (basedoc, basedoc->crtroot);
    numrec = basedoc->numrec;
    dbdstroy (basedoc);

    hand = utopen (Curlisting->filename, UT_RW);
    remain = (int) utfsize (hand) / sizeof (long) - numitm + 1;
    pnum = malloc ((remain + 1) * sizeof (long));
    * pnum = numrec;
    offset = (numitm - 1) * sizeof (long);
    utlseek (hand, offset, UT_BEG);
    utread  (hand, pnum + 1, remain * sizeof (long));
    utlseek (hand, offset, UT_BEG);
    utwrite (hand, pnum, (remain + 1) * sizeof (long));
    free (pnum);
    utclose (hand);

    Curlisting->total++;

    return numrec;
}

void dbdelrec2grp (int numitm) {
    DOC     * basedoc;
    long      numrec;

    numrec = dbdelrec2lst (Curlisting->filename, numitm);
    Curlisting->total--;
    basedoc = dbinidoc (Cur_Lst->name_data, 0, DB_BAS, Cur_Lst->opt._global);
    basedoc->numrec = numrec;
    dbdelone (basedoc);
    dbdstroy (basedoc);

    return;
}

long dbdelrec2lst (char * lstfilename, int numitm) {
    long    * pnum, numrec, offset;
    int       hand, remain;

    hand = utopen (lstfilename, UT_RW);
    remain = (int) utfsize (hand) / sizeof (long) - numitm + 1;
    pnum = malloc (remain * sizeof (long));
    offset = (numitm - 1) * sizeof (long);
    utlseek (hand, offset, UT_BEG);
    utread  (hand, pnum, remain * sizeof (long));
    numrec = * pnum;
    utlseek (hand, offset, UT_BEG);
    utwrite (hand, pnum + 1, (remain - 1) * sizeof (long));
    free (pnum);
    utchsize (hand, utfsize (hand) - sizeof (long));
    utclose (hand);

    return numrec;
}

void dbinsgrp (char * basename, bool _global, long numpapa) {
    IDGROUP   * pgroup;
    long        offset;
    int         remain;
    int         hand;
    char      * pname = (char *) Db_pwrk;

    dbmkname (pname, basename, DB_GROUP, _global);
    if ((hand = utopen (pname, UT_RW)) == -1) hand = utcreate (pname);

    remain = (int) (utfsize (hand) / sizeof (IDGROUP) - numpapa) + 1;
    pgroup = calloc (remain + 1, sizeof (IDGROUP));
    offset = (numpapa - 1) * sizeof (IDGROUP);
    utlseek (hand, offset, UT_BEG);
    utread  (hand, pgroup + 1, remain * sizeof (IDGROUP));
    utlseek (hand, offset, UT_BEG);
    utwrite (hand, pgroup, (remain + 1) * sizeof (IDGROUP));
    free (pgroup);
    utclose (hand);
}

void dbdelgrp (char * basename, bool _global, long numpapa) {
    IDBASE    * pdb;
    IDGROUP     idgroup;
    long        offset;
    int         i, hand;

    hand = dbopen (basename, DB_GROUP, _global, UT_RW);
    offset = (numpapa - 1) * sizeof (IDGROUP);
    utlseek (hand, offset, UT_BEG);
    utread  (hand, &idgroup, sizeof (IDGROUP));

    pdb = dbopendb (basename, _global, UT_RW);
    for (i = 0; i < idgroup.num_of_recs; i++)
        dbdelrec (pdb, idgroup.firstrec + i);
    dbclosdb (pdb);

    utmovsc (&idgroup, 0, sizeof (IDGROUP));
    utlseek (hand, offset, UT_BEG);
    utwrite (hand, &idgroup, sizeof (IDGROUP));
    utclose (hand);

}

