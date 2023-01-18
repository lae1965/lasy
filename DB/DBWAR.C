#include <sldbas.h>
#include <slmenu.h>


void    dbwar (char * pn, char * pm, bool disp) {
    char    *p;

    p = dbwarerr ("ndbas.err", "listing", pn, pm);
    if (disp && p != NULL) mnwarnin (NULL, 0, 1, p);
    return;
}

