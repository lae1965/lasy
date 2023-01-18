#include <sltree.h>
#include <string.h>

static char     sym[] = "³ÄÍÀÃÔÆ";
                       /*01234567*/
static char     buf [40];

void trouttr (SLTREE * ptree, bool hide) {
    register   i, j;
    int        level, maxlev, k, widname;
    long       b;
    SLNODE   * pnode;
    WINDOW   * win = ptree->pwin;
    CELLSC   * pdata, *pto;
    byte       tattr  = win->attr.text;
    byte       cattr  = win->attr.high;
    byte       iattr  = win->attr.bold;
    byte       lattr  = win->attr.prot;
    byte       clattr = win->attr.nice, attr;
    int        W = win->img.dim.w, H = win->img.dim.h;
    char     * p, * pname;
    char far * pbuf, * pscreen;

    if (win == NULL) return;

    pdata   = win->img.pdata;
    pbuf    = (char far *) pdata;
    pscreen = wnviptrl (win->where_shown.row, win->where_shown.col);

    if (!hide) {
        ptree->prevtop = ptree->top;
        i = (W - ptree->namlen - ptree->tab) / ptree->tab;
        ptree->minlev = (i < ptree->cur->level) ? ptree->cur->level - i : 0;
    }
    pnode = ptree->prevtop;
    maxlev = W / ptree->tab + ptree->minlev;
    wnmovpca (pdata, ' ', H * W, tattr);

    for (i = 0; i < H; i++, pdata += W) {
        if (pnode != NULL) {
            widname = strlen (pnode->name);
            level = pnode->level - ptree->minlev;
            for (j = level - 1, b = 1L; j >= 0; j--, b = (b << 1)) {
                k = j * ptree->tab;
                if (k >= W) continue;
                if (pnode->pmask & b) pdata[k].ch = sym[0];
            }
            if (level < 0) {
                k = ((!pnode->opt.fld) ?
                          - (level + 1) * ptree->tab : -level * ptree->tab - 2);
                if (k < widname)  {
                    pname   = pnode->name + k;
                    widname -= k;
                    pto     = pdata;
                } else widname = 0;
                k = j = 0;
            } else if (pnode->level <= maxlev) {
                p = buf;
                if (pnode->opt.fld) {
                    if (pnode->opt.last) *p++ = sym[3];
                    else                 *p++ = sym[4];
                    *p++ = sym[1];
                    *p++ = ' ';
                } else {
                    if (pnode->opt.last) *p++ = sym[5];
                    else                 *p++ = sym[6];
                    for (j = 0; j < ptree->tab - 2; j++) *p++ = sym[2];
                    *p++ = sym[7];
                }
                j = ptree->tab * level;
                k = (int) (p - buf);

                if (j + k >= W) { k = W - j; widname = 0; }
                else {
                    pname = pnode->name;
                    if (j + k + widname >= W) widname = W - j - k;
                    pto = pdata + j + k;
                }
                wnmovps (pdata + j, buf, k);
            }
            if (pnode->opt.lock)       attr = lattr;
            else                       attr = tattr;
            if (pnode == ptree->ice) attr = iattr;
            if (pnode == ptree->cur) {
                ptree->col = j + k;
                if (pnode->opt.lock) attr = clattr;
                else                 attr = cattr;
            }
            if (pnode == ptree->ice) attr |= SL_MONOBLINK;

            wnmovpsa (pto, pname, widname, attr);

            pnode = pnode->next;
        }
    }
    wnvicopy (&pbuf, &pscreen, H, W, NUM_COLS * 2, 0, Cmd[1]);
    return;
}

