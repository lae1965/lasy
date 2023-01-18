#include <sltree.h>
#include <string.h>

static char     sym[] = "³ÄÍÀÃÔÆ>";
                       /*01234567*/
static char     buf [150];
static ITEM       pitem [] = {
    { " à®¤®«¦¨âì ", "¯Gg", NULL },
    { " ®¢â®à¨âì ",  "’âNn", NULL },
    { " à¥ªà â¨âì ", "àHh", NULL }
};

static  int  str2prn (char * p, int len);

void troutprn (SLTREE * ptree) {
    register   j;
    int        k, widname, ret;
    long       b;
    SLNODE   * pnode = ptree->root;
    char     * p;

beg:
    pnode = ptree->root;
    while (pnode != NULL) {
        utmovsc (buf, ' ', 150);
        p = buf;
        for (j = pnode->level - 1, b = 1L; j >= 0; j--, b = (b << 1)) {
            k = j * ptree->tab;
            if (pnode->pmask & b) buf [k] = sym [0];
        }

        p = buf;
        if (pnode->level > 0) p += pnode->level * ptree->tab;
        if (pnode->level != -1) {
            if (pnode->opt.fld) {
                if (pnode->opt.last) * p++ = sym [3];
                else                 * p++ = sym [4];
                * p++ = sym [1];
                * p++ = ' ';
            } else {
                if (pnode->opt.last) * p++ = sym [5];
                else                 * p++ = sym [6];
                for (j = 0; j < ptree->tab - 2; j++) * p++ = sym [2];
                * p++ = sym [7];
            }
        }
        widname = strlen (pnode->name);
        utmovabs (p, pnode->name, widname, 0); p += widname;
        utmovabs (p, "\r\n", 2, 0);

        if ((ret = str2prn (buf, (int) (p - buf) + 2)) == 1) goto beg;
        else if (ret == 2) return;

        pnode = pnode->next;
    }
    return;
}

static  int  str2prn (char * p, int len) {
    register    i, ret;

    for (i = 0; i < len; i++) {
cont:
        utch2prn (p [i]);
        if ((ret = mnharder (pitem, 3)) == -1) continue;
        if (ret == 0) goto cont;
        return ret;
    }
    return 0;
}

