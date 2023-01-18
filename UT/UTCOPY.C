#include <slutil.h>
#include <io.h>

/*
     0  - Файл с данным именем уже существует;
    -1  - Ошибка открытия файла to;
    -2  - Ошибка открытия файла from;
    -3  - Ошибка копирования;
     1  - Ok!
*/

int    utcopy (char * to, char * from, bool overwrite) {
    char      * pbuf = NULL;
    int         handto = -1, handfrom = -1;
    register    len, retcode = 1;

    if (!overwrite && access (to, 0) == 0) return 0;

    if ((handto   = utcreate (to)) == -1) return -1;
    if ((handfrom = utopen (from, UT_R)) == -1) { retcode = -2; goto ret; }
    if ((pbuf = malloc (512)) == NULL)          { retcode = -3; goto ret; }

    do {
        if ((len = utread (handfrom, pbuf, 512)) == 0) break;
        if (!utwrite (handto, pbuf, len)) { retcode = -3; break; }
    } while (len == 512);
ret:
    free (pbuf);
    utclose (handfrom);
    utclose (handto);
    if (retcode < -1) utremove (to);
    return 1;
}

