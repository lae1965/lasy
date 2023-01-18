#include    <sldbas.h>
#include    <slkeyb.h>
#include    <slsymb.def>
#include    <slmous.h>
#include    <alloc.h>
#include    <io.h>
#include    <conio.h>


static bool     setprnpag   (void);
static int      outlstprn   (void);
static bool     prnlststop  (void);

void    lsprnlst (void) {
    long    first   = Curlisting->first;
    int     h_page  = Curlisting->H_page;
    bool    (* oldstop) (void) = Curlisting->stop;
    int     hand;

    if ((hand = utopen ("lstpag.ini", UT_R)) != -1) {
        utread (hand, &Cur_Lst->prnpag, sizeof (PRNPAG));
        utclose (hand);
    }

    Cur_Lst->opt.for_prn = YES;

    Curidprn = utalloc (IDPRN);
    Curidprn->prnpag = Cur_Lst->prnpag;
    Curidprn->setprnpag = setprnpag;
    Curidprn->outprn = outlstprn;
    dbprnlst (NULL);
    hand = utcreate ("lstpag.ini");
    utwrite (hand, &Curidprn->prnpag, sizeof (PRNPAG));
    utclose (hand);
    utfree (&Curidprn);

    Curlisting->H_page   = h_page;
    Curlisting->stop     = oldstop;
    Cur_Lst->opt.for_prn = NO;

    if (Curmarklst == NULL) return;  /* ??????????????????*/
    Curlisting->makpage (first, YES);
    Curlisting->curcrt = Curlisting->getlstcrt ();
    Curlisting->row = Curlisting->firstrow;
    Curlisting->visicol = 0;
    Curlisting->hei =
        dbcalcln (Cur_Lst, firstvisi (Curlisting->curcrt->fld_first),
                                    lastvisi (Curlisting->curcrt->fld_end)) + 1;

    return;
}

static bool   setprnpag (void) {

    return mnprnpag1 (&Curidprn->prnpag, &Att_Mnu1);
}

static int  outlstprn (void) {

    Curlisting->first   = 1L;
    Curlisting->H_page  = H_page;
    Curlisting->stop    = prnlststop;
    Curlisting->currecs = 0;

    while (Curlisting->first + Curlisting->currecs <= Curlisting->total) {
        Curlisting->makpage (Curlisting->first + Curlisting->currecs, YES);
        if (!dbprnlstdoc (Cur_Lst, PR_BEGNOTE|PR_ENDNOTE)) return NO;
    }

    return YES;
}

static bool prnlststop (void) {

    if (Cur_Lst->total > Curlisting->H_page) {
        if (Curlisting->currecs > 1) {
            dbcrtdel (Cur_Lst, Curlisting->curcrtwrk);
            Curlisting->currecs--;
        }
        return YES;
    }
    return NO;
}

