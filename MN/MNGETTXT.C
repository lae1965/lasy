/**************************************************************************
    mngettxt - ????祭?? ⥪??? ????䨪????? ?? 䠩??

char * mngettxt (char *file, char *name, int code);

char    *file       ??? 䠩?? ?? ??ப??? ????䨪?????
char    *name       ??? ????䨪?????
int     code        ???祭?? ????

??室:  ?????⥫? ?? ??ப? ????䨪?????
        NULL    ?訡?? ??? ?????⢨? ????
***************************************************************************/

#include <slmenu.h>

char *mngettxt (char *file, char *name, int code) {
    char *buf;
    FILE *fcod;
    char *q;
    char *p = NULL;
    bool found = NO;
    long tmp;   

    if (*name == EOS || (fcod = fopen (file, "rt")) == NULL)  return (NULL);
    if ((buf = calloc (85, 1)) == NULL) goto ret;
    while (fgets (buf, sizeof(buf), fcod) != NULL) {
        if (*buf == '@' && strncmp (buf+1, name, strlen (name)) == 0) {
            found = YES;
            break;
        }
    }
    if (found) {
        while ((q = fgets (buf, sizeof(buf), fcod)) != NULL && *q != '@') {
            utc2lrdx (&q, &tmp, 10);
            if (tmp == (long) code) {
                p = q + 1;
				buf [strlen(buf) - 1] = EOS;
                break;
            }
        }
    }
    free (buf);
ret:
    fclose (fcod);
    return (p);
}

