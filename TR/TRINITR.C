#include <sltree.h>


SLTREE  *  trcreate (int namlen, int H, int W, ATTRIB * att, BORDER * bor) {
    SLTREE    * ptree;

    if ((ptree = utalloc (SLTREE)) == NULL) return NULL;
    if ((ptree->pwin = wncreate (H, W, 0, 0, bor, att)) == NULL)
        { free (ptree); return NULL; }
    ptree->namlen = namlen;
    ptree->tab    = TREE_TAB;
    return ptree;
}

void trdstroy (SLTREE * ptree, bool (* fn) (SLNODE * pnode)) {

    trnodfre (ptree, fn);
    wndstroy (ptree->pwin);
    free (ptree);
    return;
}

void trnodfre (SLTREE * ptree, bool (* fn) (SLNODE * pnode)) {
    SLNODE   * pnode = ptree->root, * pnext;

    while (pnode != NULL) {
        pnext = pnode->next;
        if (fn != NULL) (* fn) (pnode);
        free (pnode->name);
        free (pnode);
        pnode = pnext;
    }
    ptree->root = ptree->ice   = NULL;
    ptree->row  = ptree->total = 0;

    return;
}

