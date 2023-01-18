#include <sldbas.h>
#include <alloc.h>

static void   _dstroy (DOC * doc);


void   dbdstroy (DOC * doc) {
    IDCDF       * pidcdf, * previdcdf;
    register      i = 0;

    if (doc == NULL) return;
    if (doc->doctype != DB_BAS && doc->doctype != DB_QRYWRK)
        for (; i < doc->numidf; i++) {
            if (Db_topidcdf == NULL) break;
            if (doc->idf [i].initype == SL_DICT) {
                pidcdf    = Db_topidcdf;
                previdcdf = NULL;
                while (strcmp ((char *) pidcdf->doc->cnam [0].nam,
                  (char *) doc->idf[i].fldlink.link.pdoc->cnam [0].nam) != 0) {
                    previdcdf = pidcdf;
                    pidcdf    = pidcdf->next;
                }
                pidcdf->num--;
                if (pidcdf->num == 0) {
                    _dstroy (pidcdf->doc);
                    if (previdcdf == NULL) Db_topidcdf     = pidcdf->next;
                    else                   previdcdf->next = pidcdf->next;
                    free (pidcdf);
                    if (Db_topidcdf == NULL)
                        { wndstroy (Cdf_Wind); Cdf_Wind = NULL; }
                }
            }
        }
    _dstroy (doc);
    return;
}

static void   _dstroy (DOC * doc) {
    register i;

    if (doc == NULL) return;

    if (doc->crtroot != NULL) dbcrtdel (doc, doc->crtroot);

    for (i = 0; i < doc->numidf; i++)
        if (doc->idf [i].valtype == SL_TEXT) free (doc->idf [i].inival.s.p);

    free (doc->cnam);
    free (doc->fnam);
    free (doc->tnam);
    free (doc->dnam);
    free (doc->idl);
    free (doc->idc);
    free (doc->idf);
    free (doc->pwrk);
    mnfrebrk (&doc->prnpag.firstbrk);
    free (doc->o.ptxt);
    free (doc->o.pn);
    free (doc->d.ptxt);
    free (doc->d.pn);
    if (doc->doctype != DB_DICT) wndstroy (doc->pwin);
    free (doc);
}

