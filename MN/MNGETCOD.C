/*********************************************************************
    mngetcod - Создание рулонного меню из файла

int mngetcod (int row, int col, char *file, char *name, int defcod);

int     row         Номер строки меню на экране
int     col         Номер колонки меню на экране
char    *file       Имя файла со строками меню кодификатора
char    *name       Имя кодификатора
int     defcod      Начальное значение кода

Выход:  -2          Ошибка
        -1          Выход по ESC
        0,1,...     Код выбранного элемента меню
**********************************************************************/


#include <alloc.h>
#include <slmenu.h>

int mngetcod (int row, int col, char *file, char *name, int defcod) {

    FILE *fcod;
    long pos;
    int  i, ns = 0, stline = 0;
    size_t total = 0;
    char buf[81];
    char **text = NULL;
    char *pool = NULL, *p, *q;
    int  *code = NULL;
    bool found = NO;
    long tmp;   

    if (*name == EOS || (fcod = fopen (file, "rt")) == NULL)  return (-1);
    while (fgets (buf, sizeof(buf), fcod) != NULL) {
        if (*buf == '@' && strncmp (buf+1, name, strlen (name)) == 0) {
            found = YES;
            break;
        }
    }
    if (!found) { i = -1; goto ret; }
    pos = ftell (fcod);
    while ((q = fgets (buf, sizeof(buf), fcod)) != NULL && *q != '@') {
        utc2lrdx (&q, &tmp, 10);
        if (tmp == (long) defcod)  stline = ns;
        ns++;
        q++;
        while (*q != '\n' && *q != EOS) { q++; total++; }
        total += 3;                    /* Для EOS и 2-х пробелов */
    }
    fseek (fcod, pos, SEEK_SET);
    if ((pool = malloc (total)) == NULL) { i = -1; goto ret; }
    if ((text = malloc (ns * sizeof(char *))) == NULL) { i = -1; goto ret; }
    if ((code = malloc (ns * sizeof(int))) == NULL) { i = -1; goto ret; }
    p = pool;
    for (i = 0; i < ns; i++) {
        q = fgets (buf, sizeof(buf), fcod);
        text[i] = p;
        utc2lrdx (&q, &tmp, 10);
        code[i] = (int) tmp;
        q++;
        *p++ = ' ';
        while (*q != '\n' && *q != EOS)
            *p++ = *q++;
        *p++ = ' ';
        *p++ = EOS;
    }
    if ((i = mnrolmnu (row, col, ns, stline, text, name, 15, 1)) >= 0)
        i = code[i];
ret:
    fclose (fcod);
    if (pool != NULL) free (pool);
    if (code != NULL) free (code);
    if (text != NULL) free (text);
	return (i);
}

