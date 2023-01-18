/*******   03/23/92 06:08pm   *************************************************/

#include "makedoc.h"
#include <stdio.h>

static SLNODE  * nodmake    (int ncrt, SLNODE * papa, SLNODE * prev);
static void      restortree (void);

static int           hand;
static int           curfld, curcrt;
static bool          pnmake;
static NAME        * dnam, * fnam, * cnam;
static TASKNAME    * tnam;
static IDFLD       * idf;
static IDCRT       * idc;
static IDLIN       * idl;

void     trinidoc (char *path, bool all) {
    word      ii = (word) 65536L;

    if (path == NULL) return;

    pnmake = all;

    idl    = NULL;

    if ((hand = utopen (path, UT_R)) == -1)                 goto badmem;

    if (!valdcb (hand, path))                               goto badmem;

    if (utread (hand, &dcbwrk, sizeof (DCB)) == ii)         goto badmem;

    if ((cnam = malloc (dcbwrk.cnamlen)) == NULL)           goto badmem;
    if (utread (hand, cnam, dcbwrk.cnamlen) == ii)          goto badmem;

    if ((fnam = malloc (dcbwrk.fnamlen)) == NULL)           goto badmem;
    if (utread (hand, fnam, dcbwrk.fnamlen) == ii)          goto badmem;

    if (dcbwrk.tnamlen > 0) {
        if ((tnam = malloc (dcbwrk.tnamlen)) == NULL)       goto badmem;
        if (utread (hand, tnam, dcbwrk.tnamlen) == ii)      goto badmem;
    }

    if (dcbwrk.dnamlen > 0) {
        if ((dnam = malloc (dcbwrk.dnamlen)) == NULL)       goto badmem;
        if (utread (hand, dnam, dcbwrk.dnamlen) == ii)      goto badmem;
    }

    if ((idc = malloc (dcbwrk.idclen)) == NULL)             goto badmem;
    if (utread (hand, idc, dcbwrk.idclen) == ii)            goto badmem;

    if ((idf = malloc (dcbwrk.idflen)) == NULL)             goto badmem;
    if (utread (hand, idf, dcbwrk.idflen) == ii)            goto badmem;

    if (pnmake) {

        if ((idl = malloc (dcbwrk.idllen))  == NULL)        goto badmem;
        if (utread (hand, idl, dcbwrk.idllen) == ii)        goto badmem;
    }

/*********************** Данные из файла зачитаны *****************************/

    curcrt = curfld = 0;
    utlseek (hand, dcbwrk.inioff, UT_BEG);
    if (nodmake (0, NULL, NULL) == NULL)
         { trnodfre (ptree, delpuser); ptree->root = NULL; }
    else { trupdmsk (ptree, ptree->root); restortree (); }

badmem:
    free (fnam);  free (cnam); free (idc);  free (idf);  free (idl);
    utfree (&dnam); utfree (&tnam);
    utclose (hand);
    return;
}

static SLNODE * nodmake (int ncrt, SLNODE * papa, SLNODE * prev) {
    int       nsons = 0, j;
    SLNODE  * fld, * crt;
    SLNODE  * last, * mylast;
    MDNODE  * puser;
    char    * ptxt;
    register  idclast, idflast;

    if ((crt = utalloc (SLNODE)) == NULL) return NULL;
    if ((crt->name = (char *) utalloc (NAME)) == NULL) return NULL;
    if ((crt->puser = utalloc (MDNODE)) == NULL) return NULL;
    puser = (MDNODE *) crt->puser;
    mylast = last = crt;
    strcpy (crt->name, (char *) &cnam[ncrt]);
    if (pnmake) {
        puser->idd      = idc [ncrt].idd;
        puser->copt     = idc [ncrt].opt;
    } else puser->copt.multy = idc [ncrt].opt.multy;
    curcrt++;
    ptree->total++;
    if (papa == NULL) {
        ptree->minlev = crt->level = -1;
        ptree->cur = ptree->top = ptree->root = crt;
        crt->father = crt;
        ptree->topnum = ptree->row = 0;
    } else {
        crt->level = papa->level + 1;
        crt->father = papa;
    }
    crt->prev = prev;
    if (prev != NULL) prev->next = crt;

    idclast = idc[ncrt].idc_last;
    idflast = idc[ncrt].idf_last;

    while (curfld <= idflast) {
        if (idclast >= curcrt && curfld == idc[curcrt].idf_first) {
            mylast = last;
            if ((last = nodmake (curcrt, crt, last)) == NULL) return NULL;
            nsons++;
            mylast = mylast->next;
        } else {
            if ((fld = utalloc (SLNODE)) == NULL) return NULL;
            if ((fld->name = (char *) utalloc (NAME)) == NULL) return NULL;
            if ((fld->puser = utalloc (MDNODE)) == NULL) return NULL;
            puser = (MDNODE *) fld->puser;
            strcpy (fld->name, (char *) &fnam [idf [curfld].idn]);
            fld->level = crt->level + 1;
            fld->father = crt;
            fld->prev = last;
            if (last != NULL) last->next = fld;
            fld->opt.fld = YES;
            mylast = last = fld;
/******************* Копирование опций из IDFLD в SLNODE **********************/
            puser->idf = idf [curfld];
            if (maker.opt.entertyp == maker.opt.exittyp) {
                if (idf [curfld].valtype == SL_TEXT &&
                                        (j = idf [curfld].inival.s.len) > 0) {
                    puser->idf.inival.s.p = calloc (j + 1, 1);
                    ptxt = malloc (j * 2);
                    utread (hand, ptxt, j * 2);
                    utdecode (puser->idf.inival.s.p, ptxt, j);
                    free (ptxt);
                }
            } else {
                puser->idf.inival.d = 0.0;
                puser->idf.prnopt.full = NO;
            }
            if (pnmake && puser->idf.opt.first)
                puser->idl = idl [idf [curfld].idl];
            if (idf [curfld].fldlink.docnam != -1)
                strcpy (puser->docname,
                              (char *) dnam [idf [curfld].fldlink.docnam].nam);

            if (idf [curfld].fldlink.link.tasknam != -1)
                strcpy (puser->taskname,
                        (char *) tnam [idf [curfld].fldlink.link.tasknam].nam);

            if (idf [curfld].fldlink.link.idndown != -1)
                strcpy (puser->flddown,
                       (char *) fnam [idf [curfld].fldlink.link.idndown].nam);

            if (idf [curfld].fldlink.link.idnup != -1)
                strcpy (puser->fldup,
                        (char *) fnam [idf [curfld].fldlink.link.idnup].nam);
/******************************************************************************/
            nsons++;
            curfld++;
            ptree->total++;
        }
    }
    crt->nsons = nsons;
    mylast->opt.last = YES;
    return last;
}

static void restortree (void) {
    SLNODE    * cur = ptree->root, * p;
    MDNODE    * puser;

    do {
        if (cur->opt.fld) {
            if (strcmp (cur->name, "spec") == 0) {
                p = cur->next;
                while (!p->opt.fld) p = p->next;
                ((MDNODE *) p->puser)->idl = ((MDNODE *) cur->puser)->idl;
                ((MDNODE *) p->puser)->idf.opt.first =
                                      ((MDNODE *) cur->puser)->idf.opt.first;
                ptree->cur  = cur;
                trdelnod (ptree, delpuser);
                cur = ptree->cur;
                continue;
            }
            if (maker.opt.exittyp == MD_QRY) {
                ((MDNODE *) cur->puser)->copt.multy = YES;
                ((MDNODE *) cur->puser)->idf.opt.wrap = YES;
            }
        } else {
            if (((MDNODE *) cur->puser)->copt.multy && cur->nsons == 1) {
                p     = cur;
                cur   = cur->next;
                puser = (MDNODE *) cur->puser;

                puser->idd = ((MDNODE *) p->puser)->idd;
                * ((MDNODE *) p->puser) = * puser;

                puser->idf.inival.s.p = NULL;

                p->opt.fld  = YES;
                ptree->cur  = cur;
                trdelnod (ptree, delpuser);
                cur = p;
            }
        }
        cur = cur->next;
    } while (cur != NULL);
    if (ptree->cur != ptree->root) trhome (ptree);
    return;
}

bool savetree (int  handle, bool for_base) {
    bool        indx = NO;
    register    i, j;
    int         nfld = 0, ncrt = 0, nidl = 0, nfldname = 0;
    int         ndoc = 0, ndocname = 0, ntask = 0, ntaskname = 0;
    int         c = -1, f = -1, l = -1, k, handtmp;
    SLNODE    * cur = ptree->root, * p;
    MDNODE    * puser;
    IDFLD     * pidf;
    IDCRT     * pidc;
    char      * ptxt;

    handtmp = utcreate (mdtmpm);
    do {
        if (cur->opt.fld) {
            nfld++;
            puser = (MDNODE *) cur->puser;
            if (puser->idf.opt.indx)  indx = YES;
            if (puser->idf.opt.first) nidl++;
            if (* puser->docname  != EOS) ndoc++;
            if (* puser->taskname != EOS) ntask++;

        /* образование поля spec для запросного документа */
            if (maker.opt.exittyp == MD_QRY) {
                ptree->cur = cur;
                trinshig (ptree, YES, "spec", inipuser);
                ((MDNODE *) (ptree->cur->puser))->idf.opt.first = YES;
                ((MDNODE *) (ptree->cur->puser))->idf.opt.visibl = NO;
                ((MDNODE *) (ptree->cur->puser))->idl = puser->idl;
                puser->idf.opt.first = NO;
                nfld++;
            } else if (puser->idf.valtype != SL_TEXT)
                puser->idf.opt.wrap = NO;
        /**************************************************/

        /****** замена множественного поля на кортеж ******/
            if (puser->idf.opt.multy) {
                ptree->cur = cur;
                cur->opt.fld = NO;
                puser->copt.multy = YES;
                puser->copt.visibl = puser->idf.opt.visibl;
                trinslow (ptree, YES, 1, cur->name, inipuser);
                * ((MDNODE *) ptree->cur->puser) = * puser;
                puser->idf.inival.s.p = NULL;
                ncrt++;
                cur = ptree->cur;
            }
        /**************************************************/

        } else {
            ncrt++;
            if (maker.opt.exittyp == MD_LST && cur->level == 0) {
                ptree->cur = p = cur;
                do    p = p->next;
                while (!p->opt.fld);
                puser = (MDNODE *) p->puser;
                trinslow (ptree, YES, 1, "spec", inipuser);
                ((MDNODE *) (ptree->cur->puser))->idf.opt.visibl = NO;
                ((MDNODE *) (ptree->cur->puser))->idf.opt.first =
                                                        puser->idf.opt.first;
                puser->idf.opt.first = NO;
                ((MDNODE *) (ptree->cur->puser))->idl = puser->idl;
            }
        }
        cur = cur->next;
    } while (cur != NULL);

    if ((idc =  calloc (ncrt,  sizeof(IDCRT)))     == NULL) goto badmem;
    if ((idf =  calloc (nfld,  sizeof(IDFLD)))     == NULL) goto badmem;
    if ((idl =  calloc (nidl,  sizeof(IDLIN)))     == NULL) goto badmem;
    if ((fnam = malloc (nfld  * sizeof(NAME)))     == NULL) goto badmem;
    if ((cnam = malloc (ncrt  * sizeof(NAME)))     == NULL) goto badmem;
    if (ndoc > 0 && (dnam = malloc (ndoc  * sizeof(NAME))) == NULL) goto badmem;
    if (ntask > 0 && (tnam = malloc (ntask * sizeof(TASKNAME))) == NULL)
        goto badmem;

    cur = ptree->root;
    for (i = 0; i < ptree->total; i++, cur = cur->next) {
        puser = (MDNODE *) cur->puser;
        if (cur->opt.fld) {
            f++;
            pidf = idf + f;
            * pidf = puser->idf;
            for (j = 0; j < nfldname; j++)
                if (strcmp ((char *) &fnam [j], cur->name) == 0) goto lab;
            strcpy ((char *) &fnam [j], cur->name);
            nfldname++;
lab:
            pidf->idn = j;

            j = -1;
            if (* puser->docname != EOS) {
                for (j = 0; j < ndocname; j++)
                    if (strcmp ((char *) &dnam [j], puser->docname) == 0)
                        goto lab1;
                strcpy ((char *) &dnam [j], puser->docname);
                ndocname++;
            }
lab1:
            pidf->fldlink.docnam = j;

            j = -1;
            if (* puser->taskname != EOS) {
                for (j = 0; j < ntaskname; j++)
                    if (strcmp ((char *) &tnam [j], puser->taskname) == 0)
                        goto lab2;
                strcpy ((char *) &tnam [j], puser->taskname);
                ntaskname++;
            }
lab2:
            pidf->fldlink.link.tasknam = j;

            pidf->fldlink.link.idndown = -1;
            if (* puser->flddown != EOS) {
                for (p = ptree->root, j = 0; p != NULL; p = p->next) {
                    if (!p->opt.fld) continue;
                    if (strcmp ((char *) p->name, puser->flddown) == 0) {
                        pidf->fldlink.link.idndown = j;
                                             /* idf поля с нужным именем */
                        break;
                    }
                    j++;
                }
            }

            pidf->fldlink.link.idnup = -1;
            if (* puser->fldup != EOS) {
                for (p = ptree->root, j = 0; p != NULL; p = p->next) {
                    if (!p->opt.fld) continue;
                    if (strcmp ((char *) p->name, puser->fldup) == 0) {
                        pidf->fldlink.link.idnup = j;
                                             /* idf поля с нужным именем */
                        break;
                    }
                    j++;
                }
            }
/****************** Копирование опций из в SLNODE  в IDFLD ********************/
            if (pidf->valtype == SL_TEXT && (j = pidf->inival.s.len) > 0) {
                ptxt = malloc (j * 2);
                utcode (ptxt, pidf->inival.s.p, j);
                utwrite (handtmp, ptxt, j * 2);
                free (ptxt);
            }
            pidf->idc = 0;
            if (puser->idf.opt.first) {
                if (for_base) pidf->opt.first = NO;
                l++;
                idl [l] = puser->idl;
            }
            pidf->idl = l;
/******************************************************************************/
        } else {
            c++;
            pidc = idc + c;
            strcpy ((char *) &cnam[c], cur->name);
            pidc->idd  = puser->idd;
            pidc->opt = puser->copt;
            pidc->idf_first = f + 1;
            j = f; k = c; p = cur;
            while (1) {
                p = p->next;
                if (p == NULL || p->level <= cur->level) break;
                if (p->opt.fld) j++;
                else            k++;
            }
            if (c == k) pidc->idc_last = -1;
            else        pidc->idc_last = k;
            pidc->idf_last = j;
        }
    }
    for (i = 0; i < nfld; i++) {      /* по idf поля находим его имя */
        if (idf [i].fldlink.link.idndown != -1)
            idf [i].fldlink.link.idndown =
                                        idf [idf [i].fldlink.link.idndown].idn;
        if (idf [i].fldlink.link.idnup != -1)
            idf [i].fldlink.link.idnup = idf [idf [i].fldlink.link.idnup].idn;
    }
    restortree ();
    dcb.idclen  = ncrt      * sizeof (IDCRT);
    dcb.idflen  = nfld      * sizeof (IDFLD);
    dcb.idllen  = nidl      * sizeof (IDLIN);
    dcb.fnamlen = nfldname  * sizeof  (NAME);
    dcb.cnamlen = ncrt      * sizeof  (NAME);
    dcb.dnamlen = ndocname  * sizeof  (NAME);
    dcb.tnamlen = ntaskname * sizeof  (TASKNAME);

    if (!utwrite (handle, &dcb, sizeof (DCB)))  goto badmem;
    if (!utwrite (handle, cnam, dcb.cnamlen))   goto badmem;
    if (!utwrite (handle, fnam, dcb.fnamlen))   goto badmem;
    if (ntaskname > 0 && !utwrite (handle, tnam, dcb.tnamlen)) goto badmem;
    if (ndocname  > 0 && !utwrite (handle, dnam, dcb.dnamlen)) goto badmem;
    if (!utwrite (handle, idc,  dcb.idclen))    goto badmem;
    if (!utwrite (handle, idf,  dcb.idflen))    goto badmem;
    if (!utwrite (handle, idl,  dcb.idllen))    goto badmem;
badmem:
    utclose (handtmp);
    free (idc);   free (idf);   free (idl);
    free (fnam);  free (cnam);  utfree (&dnam); utfree (&tnam);
    return indx;
}

