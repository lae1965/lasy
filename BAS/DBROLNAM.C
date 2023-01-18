/*******************************************************************************
    dbrolnam - выбирает документ, с которым будет производиться дальнейшая
               работа.

  int dbrolnam (char * pname, char * pext, bool (* fn) (int hand),
                                                               char * pdocnam);
        char * pname   - на входе  - путь до нужного каталога;
                         на выходе - имя файла выбранного документа (с путем);
        char * pext    - расширение документа, по которому определяется его тип;
        bool (* fn)    - указатель на фильтрующую функцию;
        char * pdocnam - указатель на строку, в которую заносится имя выбранного
                         документа (if pdocnam != NULL).

*******************************************************************************/

#include    <sldbas.h>
#include    <slmenu.h>


int dbrolnam (char * pname, char * pext, bool (* fn) (int hand),
                                                              char * pdocnam) {
    char   ** txt = NULL, * p;
    int       hand, nfiles, lenofstr = MAX_NAM_LEN + 14, * pind = NULL;
    register  i, j = 0;
    int       retcode = 1;
    long      offset;

    p = &pname [strlen (pname)];
    strcpy (p, pext);
    strcat (p, ".ini");
    if ((hand = utopen (p, UT_R)) == -1) return 0;
    utread (hand, &nfiles, sizeof (int));
    if (nfiles == 0) { retcode = 0; goto ret; }
    if ((txt = calloc (nfiles, sizeof (char *))) == NULL)
        { retcode = 0; goto ret; }
    if ((pind = calloc (nfiles, sizeof (int))) == NULL)
        { retcode = 0; goto ret; }
    for (i = 0, offset = 2L; i < nfiles; i++, offset += lenofstr) {
        utlseek (hand, offset, UT_BEG);
        utread (hand, p, MAX_NAM_LEN + 1);
        if (fn != NULL && !(* fn) (hand)) continue;
        if ((txt [j] = calloc (strlen (p) + 1, sizeof (char))) == NULL)
            { retcode = 0; goto ret; }
        strcpy (txt [j], p);
        pind [j] = i;
        j++;
    }
    if ((i = mnrolmnu (WN_CURMNU, WN_CURMNU, j, 0, txt, NULL, 15, 0)) < 0)
        { retcode = -1; goto ret; }
    if (pdocnam != NULL) strcpy (pdocnam, txt [i]);
    utlseek (hand, (long) pind [i] * (long) lenofstr + MAX_NAM_LEN + 3, UT_BEG);
    utread (hand, p, 13);
ret:
    utclose (hand);
    for (i = 0; i < j; i++) free (txt [i]);
    free (txt);
    free (pind);
    return retcode;
}

