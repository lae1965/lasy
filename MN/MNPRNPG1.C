#include   <slmenu.h>


static BORDER  mnubrd = { BORD_DDSS|BORD_SHADOW, NULL, NULL, NULL };

static bool     vfypag  (void);


static char * noyes [] = { "¥â", " „ " };
static char * wnote [] = { "   ¥â", "‘¢¥àåã", " ‘­¨§ã" };

static ITEM mirror = { " „¢ãáâ®à®­­ïï ¯¥ç âì     @@@      ", "„¤Ll", NULL   };
static ITEM linspac= { " Œ¥¦áâà®ç­ë© ¨­â¥à¢ «     @@      ", "Œ¬Vv", "   "  };
static ITEM wid    = { " ˜¨à¨­                  @@@@ (¬¬) ", "˜èIi", "     "};
static ITEM hei    = { " ‚ëá®â                  @@@@ (¬¬) ", "‚¢Dd", "     "};
static ITEM left   = { " ‘«¥¢                   @@@@ (¬¬) ", "‹«Kk", "     "};
static ITEM right  = { " ‘¯à ¢                  @@@@ (¬¬) ", "¯Gg", "     "};
static ITEM top    = { " ‘¢¥àåã                 @@@@ (¬¬) ", "‘áCc", "     "};
static ITEM bot    = { " ‘­¨§ã                  @@@@ (¬¬) ", "­Yy", "     "};
static ITEM style  = { " ˜à¨äâ                            ", "”äAa", NULL   };
static ITEM note   = { " ‚ë¢®¤¨âì             @@@@@@      ", "›ëSs", NULL   };
static ITEM number = { " ã¬¥à æ¨ï áâà ­¨æ       @@@      ", "“ãEe", NULL   };
static ITEM tonote = { " Žâáâã¯                 @@@@ (¬¬) ", "Ž®Jj", "     "};
static ITEM prn    = { "             ¥ç â âì             ", "—çXx", NULL   };
static ITEM note1  = {
    " ‚ë¢®¤¨âì ­  ¯¥à¢®© áâà ­¨æ¥                                 @@@      ",
                                                                "…¥Tt", NULL  };
static ITEM text   = {
    " ’¥ªáâ: @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
                                                                "’âNn", NULL  };

static PRNPAG * prnpag;

bool    mnprnpag1 (PRNPAG * prnpg, ATTRIB * pattrib) {
    MENU      * pmenu;
    char      * p;
    bool        done, retcode;
    long        i;
    int         row, col, len, * pi;

    pmenu = mncreate (13, 72, 0, 0, &mnubrd, pattrib);

    prnpag = prnpg;
    prnpag->opt.ff = YES;
    prnpag->numcop = 1;

    mirror.vars  = noyes [prnpag->opt.mirror];
    number.vars  = noyes [prnpag->opt.number];
    note.vars    = wnote [prnpag->opt.note];
    note1.vars   = noyes [prnpag->opt.note1st];
    utl2crdx ((long) prnpag->linspac, linspac.vars, 2, 10);
    if (prnpag->wid < 1) prnpag->wid = 210;
    utl2crdx ((long) prnpag->wid, wid.vars, 4, 10);
    if (prnpag->hei < 1) prnpag->hei = 270;
    utl2crdx ((long) prnpag->hei, hei.vars, 4, 10);
    utl2crdx ((long) prnpag->left, left.vars, 4, 10);
    utl2crdx ((long) prnpag->right, right.vars, 4, 10);
    utl2crdx ((long) prnpag->top, top.vars, 4, 10);
    utl2crdx ((long) prnpag->bot, bot.vars, 4, 10);
    utl2crdx ((long) prnpag->tonote, tonote.vars, 4, 10);
    text.vars = prnpag->pnote;

    mnitmadd (pmenu,  1,  0,  1, &mirror);
    mnitmadd (pmenu,  4,  0, 37, &linspac);
    mnitmadd (pmenu,  9,  2,  1, &wid);
    mnitmadd (pmenu, 10,  2, 37, &hei);
    mnitmadd (pmenu, 11,  4,  1, &left);
    mnitmadd (pmenu, 12,  4, 37, &right);
    mnitmadd (pmenu, 13,  5,  1, &top);
    mnitmadd (pmenu, 14,  5, 37, &bot);
    mnitmadd (pmenu, 15,  7,  1, &style);
    mnitmadd (pmenu, 16,  7, 37, &note);
    mnitmadd (pmenu, 17,  8,  1, &number);
    mnitmadd (pmenu, 18,  8, 37, &tonote);
    mnitmadd (pmenu, 19,  9,  1, &note1);
    mnitmadd (pmenu, 20, 10,  1, &text);
    mnitmadd (pmenu, 22, 12, 18, &prn);

    wnsplitw (pmenu->pwin,  1, WN_HSPLIT|WN_SINGLE);
    wnsplitw (pmenu->pwin,  3, WN_HSPLIT|WN_SINGLE);
    wnsplitw (pmenu->pwin,  6, WN_HSPLIT|WN_SINGLE);
    wnsplitw (pmenu->pwin, 11, WN_HSPLIT|WN_SINGLE);
    mnsetcur (pmenu, 22);

    mndsplay (pmenu, WN_CENTER, WN_CENTER);

    wnwrtbuf (pmenu->pwin,  1, 26, 15, "  §¬¥àë «¨áâ  ", WN_NATIVE);
    wnwrtbuf (pmenu->pwin,  3, 30,  8, " Žâáâã¯ ",        WN_NATIVE);
    wnwrtbuf (pmenu->pwin,  6, 28, 11, " ‡ £®«®¢®ª ",     WN_NATIVE);

lab:
    done = retcode = NO;
    while (!done) {
        pi = NULL;
        switch (mnreadop (pmenu, 0)) {
            case  1:
                prnpag->opt.mirror = !prnpag->opt.mirror;
                pmenu->pcur->vloc = noyes [prnpag->opt.mirror];
                break;
            case  4: row = 0; col = 63; len = 2; pi = &prnpag->linspac; break;
            case  9: row = 2; col = 25; len = 4; pi = &prnpag->wid;     break;
            case 10: row = 2; col = 61; len = 4; pi = &prnpag->hei;     break;
            case 11: row = 4; col = 25; len = 4; pi = &prnpag->left;    break;
            case 12: row = 4; col = 61; len = 4; pi = &prnpag->right;   break;
            case 13: row = 5; col = 25; len = 4; pi = &prnpag->top;     break;
            case 14: row = 5; col = 61; len = 4; pi = &prnpag->bot;     break;
            case 15:
                mnprnopt (&prnpag->prnopt, " ˜à¨äâ § £®«®¢ª  ", &Att_Mnu2);
                continue;
            case 16:
                if (prnpag->opt.note < 2) prnpag->opt.note++;
                else                      prnpag->opt.note = 0;
                pmenu->pcur->vloc = wnote [prnpag->opt.note];
                break;
            case 17:
                prnpag->opt.number = !prnpag->opt.number;
                pmenu->pcur->vloc = noyes [prnpag->opt.number];
                break;
            case 18: row = 8; col = 61; len = 4; pi = &prnpag->tonote;   break;
            case 19:
                prnpag->opt.note1st = !prnpag->opt.note1st;
                pmenu->pcur->vloc = noyes [prnpag->opt.note1st];
                break;
            case 20:
                wnstredt (pmenu->pwin, 10, 9, pmenu->pwin->attr.afld,
                              pmenu->pwin->attr.afld, pmenu->pcur->vloc, 62);
                break;
            case 22:
                retcode = YES;
            case -1: done = YES; continue;
            default :            continue;
        }
        if (pi != NULL) {
            while (1) {
                wnstredt (pmenu->pwin, row, col, pmenu->pwin->attr.afld,
                            pmenu->pwin->attr.afld, pmenu->pcur->vloc, len);
                p = pmenu->pcur->vloc;
                if (utc2lrdx (&p, &i, 10) && utisempt ((byte *) p) && i >= 0L)
                    break;
                utalarm ();
            }
            utl2crdx (i, pmenu->pcur->vloc, len, 10);
            * pi = (int) i;
        }
        mndisvri (pmenu, pmenu->pcur);
    }
    if (retcode && !vfypag ()) {
        mnwarnin (NULL, 0, 3, " ", "¥á®®â¢¥âáâ¢¨¥ à §¬¥à®¢", " ");
        goto lab;
    }
ret:
    mndstroy (pmenu);
    return retcode;
}

/***************************************************************************
  0         1         2         3         4         5         6         7
  012345678901234567890123456789012345678901234567890123456789012345678901
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³  „¢ãáâ®à®­­ïï ¯¥ç âì     ˜˜˜         Œ¥¦áâà®ç­ë© ¨­â¥à¢ «     ˜˜       ³0
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ  §¬¥àë «¨áâ  ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´1
 ³  ˜¨à¨­                  ˜˜˜˜ (¬¬)    ‚ëá®â                  ˜˜˜˜ (¬¬)  ³2
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Žâáâã¯ ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´3
 ³  ‘«¥¢                   ˜˜˜˜ (¬¬)    ‘¯à ¢                  ˜˜˜˜ (¬¬)  ³4
 ³  ‘¢¥àåã                 ˜˜˜˜ (¬¬)    ‘­¨§ã                  ˜˜˜˜ (¬¬)  ³5
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ‡ £®«®¢®ª ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´6
 ³  ˜à¨äâ                               ‚ë¢®¤¨âì             ˜˜˜˜˜˜       ³7
 ³  ã¬¥à æ¨ï áâà ­¨æ       ˜˜˜         Žâáâã¯                 ˜˜˜˜ (¬¬)  ³8
 ³  ‚ë¢®¤¨âì ­  ¯¥à¢®© áâà ­¨æ¥                                 ˜˜˜       ³9
 ³  ’¥ªáâ: ˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜ ³10
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´11
 ³   §¡¨¢ª  ¯® áâà ­¨æ ¬               ¥ç â âì                          ³12
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
******************************************************************************/

static bool   vfypag (void) {

    if (prnpag->hei < 50 || prnpag->wid < 50) return NO;
    if (prnpag->top + prnpag->bot + 10 > prnpag->hei) return NO;
    if (prnpag->left + prnpag->right + 10 > prnpag->wid) return NO;
    if (prnpag->opt.note == 1 && prnpag->tonote >= prnpag->top) return NO;
    if (prnpag->opt.note == 2 && prnpag->tonote >= prnpag->bot) return NO;
    return YES;
}

