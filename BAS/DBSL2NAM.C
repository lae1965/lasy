/*******************************************************************************
    Функция dbsl2nam производит поиск файлов документов типа sl с расширением
    pext в каталоге path. Найденные документы заносятся в файл в следующем
    формате:

         ИМЯ ДОКУМЕНТА (21b) - ИМЯ ФАЙЛА, В КОТОРОМ НАХОДИТСЯ ДОКУМЕНТ (13b).

    Имя файла со списком найденных документов формируется следующим образом:

         В качестве имени файла берется pext, к которому добавляется
         расширение "ini".

            void dbsl2nam (char * pext, int num,
                                          char * path1[, char * path2, ...]);

    char * pext - расширение искомых файлов документов;
    char * path - путь до каталога, в котором производится поиск.


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