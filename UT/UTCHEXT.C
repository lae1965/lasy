/*************************************************************************
                utchext - ?????? ?????७?? 䠩??

void    utchext      (char *buf, char *ext);

char    *buf        ?????⥫? ?? ??ப? ? ?????? 䠩??
char    *ext        ?????⥫? ?? ??ப? ? ?????७???

*************************************************************************/

#include <string.h>
#include <slutil.h>


void  utchext (char * buf, char * ext) {
    char    * p;

    if ((p = strchr (buf, '.')) != NULL)  * p = '\0';
    if ((p = strrchr (buf, '\\')) == NULL)  p = buf;
    else                                    p++;
    if (strlen (p) > 8) * (p + 8) = '\0';
    strcat (buf, ext);
    return;
}

