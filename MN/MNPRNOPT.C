#include <slmenu.h>

static BORDER  mnubrd = { BORD_DDSS|BORD_TCT, NULL, NULL, NULL };
static BORDER  fonbrd  = { BORD_SHADOW, NULL, NULL, NULL };


static char   * noyes  [] = { "???", " ??" };
static char   * pnlq   [] = { "     Draft", "     Roman",  "Sans serif" };
static char   * pelite [] = { " Pica", "Elite" };
static char   * psize  [] = { " ??ଠ?????", "  ?????????", "????⭥????" };
static char   * pscr   [] = { " ??ଠ?쭠?", "???????筠?", "???????筠?" };
static char   * pbold  [] = { "  ??ଠ?????", "??????? 㤠?", "  ?뤥??????" };

static ITEM    nlq    = { "  ?????                 @@@@@@@@@@ ", "??Ii", NULL };
static ITEM    italic = { "  ???ᨢ                       @@@ ", "??Rr", NULL };
static ITEM    under  = { "  ?????ન?????                @@@ ", "??Gg", NULL };
static ITEM    bold   = { "  ????? ??????        @@@@@@@@@@@@ ", "??Ll", NULL };
static ITEM    elite  = { "  ???                        @@@@@ ", "??Uu", NULL };
static ITEM    size   = { "  ??????               @@@@@@@@@@@ ", "??Hh", NULL };
static ITEM    script = { "  ??????? ? ??ப?     @@@@@@@@@@@ ", "??Cc", NULL };

void   mnprnopt (PRNOPT * pprnopt, char * ttl, ATTRIB * pattr) {
    PRNOPT    prnopt = * pprnopt;
    WINDOW  * fonwin;
    MENU    * mnu;
    bool      done = NO;

    mnubrd.pttitle = ttl;
    mnu    = mncreate (12, 38, 0, 0, &mnubrd, pattr);
    fonwin = wncreate (16, 46, 0, 0, &fonbrd, pattr);

    nlq.vars    = pnlq   [prnopt.nlq];
    italic.vars = noyes  [prnopt.italic];
    under.vars  = noyes  [prnopt.under];
    bold.vars   = pbold  [prnopt.bold];
    elite.vars  = pelite [prnopt.elite];
    size.vars   = psize  [prnopt.size];
    script.vars = pscr   [prnopt.script];

    if (!mnitmadd (mnu, 1,  1,  1, &nlq))    goto ret;
    if (!mnitmadd (mnu, 2,  2,  1, &italic)) goto ret;
    if (!mnitmadd (mnu, 3,  3,  1, &under))  goto ret;
    if (!mnitmadd (mnu, 4,  4,  1, &bold))   goto ret;
    if (!mnitmadd (mnu, 5,  8,  1, &elite))  goto ret;
    if (!mnitmadd (mnu, 6,  9,  1, &size))   goto ret;
    if (!mnitmadd (mnu, 7, 10,  1, &script)) goto ret;

    wnsplitw (mnu->pwin,  6, WN_HSPLIT|WN_SINGLE);

    wndsplay (fonwin, WN_CENTER, WN_CENTER);
    mndsplay (mnu,    WN_CENTER, WN_CENTER);
    while (!done) {
        switch (mnreadop (mnu, 0)) {
            case 1:
                if (prnopt.nlq < 2) prnopt.nlq++;
                else                prnopt.nlq = 0;
                if (prnopt.nlq != 0 && (prnopt.bold == 1 || prnopt.size == 2))
                    prnopt.nlq = 0;
                mnu->pcur->vloc = pnlq [prnopt.nlq];
                mndisvri (mnu, mnu->pcur);
                break;
            case 2:
                prnopt.italic = !prnopt.italic;
                mnu->pcur->vloc = noyes [prnopt.italic];
                mndisvri (mnu, mnu->pcur);
                break;
            case 3:
                prnopt.under = !prnopt.under;
                mnu->pcur->vloc = noyes [prnopt.under];
                mndisvri (mnu, mnu->pcur);
                break;
            case 4:
                if (prnopt.bold < 2) prnopt.bold++;
                else                 prnopt.bold = 0;
                if (prnopt.bold == 1 && prnopt.nlq != 0) prnopt.bold++;
                if (prnopt.bold == 2 && (prnopt.size == 2 || prnopt.elite))
                    prnopt.bold = 0;
                mnu->pcur->vloc = pbold [prnopt.bold];
                mndisvri (mnu, mnu->pcur);
                break;
            case 5:
                prnopt.elite = !prnopt.elite;
                if (prnopt.elite && (prnopt.size == 2 || prnopt.bold == 2))
                    prnopt.elite = 0;
                mnu->pcur->vloc = pelite [prnopt.elite];
                mndisvri (mnu, mnu->pcur);
                break;
            case 6:
                if (prnopt.size < 2) prnopt.size++;
                else                 prnopt.size = 0;
                if (prnopt.size == 2 &&
                        (prnopt.nlq != 0 || prnopt.elite || prnopt.bold == 2))
                    prnopt.size = 0;

                mnu->pcur->vloc = psize [prnopt.size];
                mndisvri (mnu, mnu->pcur);
                break;
            case 7:
                if (prnopt.script < 2) prnopt.script++;
                else                   prnopt.script = 0;
                mnu->pcur->vloc = pscr [prnopt.script];
                mndisvri (mnu, mnu->pcur);
                break;
            default :   done = YES; break;
        }
    }

ret:
    mndstroy (mnu);
    wndstroy (fonwin);
    * pprnopt = prnopt;
    return;
}

/******************************************************************************

                          1         2         3
                01234567890123456789012345678901234567
               ??????????????????????????????????????ķ
               ?                                      ?0
               ?   ?????                Sans serif    ?1
               ?   ???ᨢ               ???           ?2
               ?   ?????ન?????        ???           ?3
               ?   ??????? 㤠?         ???           ?4
               ?                                      ?5
               ??????????????????????????????????????Ķ6
               ?                                      ?7
               ?   ???                  Pica (Elite)  ?8
               ?   ??????               ????⭥????   ?9
               ?   ??????? ? ??ப?     ???????筠?   ?10
               ?                                      ?11
               ??????????????????????????????????????ͼ

******************************************************************************/
