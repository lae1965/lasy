#include <slutil.h>
#include <string.h>
#include "makedoc.h"


bool    valdcb     (int handle, char * name) {
    long    savoff;
    char   * p;
    char    buf[3];
    bool    ret = NO;

    utstrcvt (name,   ST_TOUP);
    if ((p = strchr (name, '.')) == NULL) return NO;
    if (strcmp (p + 1, Md_ext [maker.opt.entertyp] + 1) != 0) return NO;
    savoff = utlseek (handle, 0L, UT_CUR);
    utlseek (handle, 0L, UT_BEG);
    if (utread (handle, buf, 3) == 3 && strncmp (buf, "sld", 3) == 0) ret = YES;
    utlseek (handle, savoff, UT_BEG);
    return ret;
}

