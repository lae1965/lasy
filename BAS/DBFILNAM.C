/*******************************************************************************
    dbfilnam - возвращает имя файла документа с мнемоничным именем pname.               работа.

  char * dbfilnam (char * pname, char * pext);
        char * pname   - мнемоничное имя документа;
        char * pext    - расширение документа, по которому определяется его тип.

*******************************************************************************/

#include    <sldbas.h>

static char      pfilnam [MAX_NAM_LEN + 1];

char * dbfilnam (char * pname, char * pext) {
    int       hand, nfiles, lenofstr = MAX_NAM_LEN + 14;
    register  i;
    bool      retcode = NO;
    long      offset;

    strcpy (pfilnam, pext);
    strcat (pfilnam, ".ini");
    if ((hand = utopen (pfilnam, UT_R)) == -1) return NULL;
    utread (hand, &nfiles, sizeof (int));
    if (nfiles == 0) goto ret;
    for (i = 0, offset = 2L; i < nfiles; i++, offset += lenofstr) {
        utlseek (hand, offset, UT_BEG);
        utread (hand, pfilnam, MAX_NAM_LEN + 1);
        if (strcmp (pfilnam, pname) == 0) { retcode = YES; break; }
    }
    if (retcode) utread (hand, pfilnam, 13);
ret:
    utclose (hand);
    return ((retcode) ? pfilnam : NULL);
}

