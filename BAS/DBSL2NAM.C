/*******************************************************************************
    ?㭪??? dbsl2nam ?ந?????? ????? 䠩??? ???㬥?⮢ ⨯? sl ? ?????७???
    pext ? ??⠫??? path. ????????? ???㬥??? ????????? ? 䠩? ? ᫥???饬
    ??ଠ??:

         ??? ????????? (21b) - ??? ?????, ? ??????? ????????? ???????? (13b).

    ??? 䠩?? ?? ᯨ᪮? ????????? ???㬥?⮢ ??ନ?????? ᫥???騬 ??ࠧ??:

         ? ?????⢥ ????? 䠩?? ??????? pext, ? ????஬? ???????????
         ?????७?? "ini".

            void dbsl2nam (char * pext, int num,
                                          char * path1[, char * path2, ...]);

    char * pext - ?????७?? ?᪮??? 䠩??? ???㬥?⮢;
    char * path - ???? ?? ??⠫???, ? ????஬ ?ந???????? ?????.


*******************************************************************************/

#include <dir.h>
#include <dos.h>
#include <conio.h>
#include <stdarg.h>
#include <sldbas.h>

#define FILE_NAM_LEN      13


void dbsl2nam (char * pext, int num, ...) {
    char          * p, * filename, * docname;
    register        handto, handfrom;
    struct ffblk    ffound;
    int             nfiles = 0, i, j;
    va_list         arg;
    char          * path;

    filename = malloc (80);
    docname  = malloc (MAX_NAM_LEN + 1);
    strcpy (filename, pext);
    strcat (filename, ".ini");
    handto = utcreate (filename);
    utwrite (handto, &nfiles, sizeof (int));

    va_start (arg, num);

    for (; num > 0; num--) {

        path = va_arg (arg, char *);

        i = 0;
        if (path != NULL && (i = strlen (path)) > 0) {
            if (path [i - 1] != '\\') path [i++] = '\\';
            utmovabs (filename, path, i, 0);
        }
        p = &filename [i];
        j = strlen (pext);
        utmovabs (p,     "*.", 2, 0); i += 2;
        utmovabs (p + 2, pext, j, 0); i += j;
        filename [i] = EOS;
        if (findfirst (filename, &ffound, FA_ARCH) == 0) {
            docname [MAX_NAM_LEN] = EOS;
            do {
                nfiles++;
                strcpy (p, ffound.ff_name);
                handfrom = utopen (filename, UT_R);
                utlseek (handfrom, (long) sizeof (DCB), UT_BEG);
                utread  (handfrom, docname, MAX_NAM_LEN);
                utwrite (handto, docname, MAX_NAM_LEN + 1);
                utwrite (handto, ffound.ff_name, FILE_NAM_LEN);
                utclose (handfrom);
            } while (findnext (&ffound) == 0);
        }
    }

    va_end (arg);

    utlseek (handto, 0L, UT_BEG);
    utwrite (handto, &nfiles, sizeof (int));
    utclose (handto);
    free    (docname);
    free    (filename);
    return;
}

