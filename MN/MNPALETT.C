#include "language.h"
#include <slmenu.h>
#include <slkeyb.h>
#include <slsymb.def>
#include <slmous.h>

static bool far _Cdecl msmnhand (int ev, int bs, int xp, int yp, int xd, int yd);
static void pal_crea (char *palett);
static void pal_disp (char *palett, int ind, int cur);
static int  pal_foun (char *palett, int cur);
static int  mnread   (MENU *pmenu);


#define D_max 63
#define D_pal 16
#define W_pal (M_pal->pwin)
static  MENU  *M_pal = NULL;
static  BORDER B_pal = { BORD_DDSS|BORD_SHADOW,NULL,NULL,NULL };
static  ITEM   I_pal [D_pal] = {
#ifdef ENGLISH
    { " Black           ",    "",   NULL },
    { " Blue            ",    "",   NULL },
    { " Green           ",    "",   NULL },
    { " Cyan            ",    "",   NULL },
    { " Red             ",    "",   NULL },
    { " Magenta         ",    "",   NULL },
    { " Brown           ",    "",   NULL },
    { " White           ",    "",   NULL },
    { " Gray            ",    "",   NULL },
    { " Light Blue      ",    "",   NULL },
    { " Light Green     ",    "",   NULL },
    { " Light Cyan      ",    "",   NULL },
    { " Light Red       ",    "",   NULL },
    { " Light Magenta   ",    "",   NULL },
    { " Yellow          ",    "",   NULL },
    { " Light White     ",    "",   NULL }};
#else
    { " ó•‡≠Î©          ",    "",   NULL },
    { " ë®≠®©           ",    "",   NULL },
    { " á•´•≠Î©         ",    "",   NULL },
    { " àß„¨‡„§≠Î©      ",    "",   NULL },
    { " ä‡†·≠Î©         ",    "",   NULL },
    { " è„‡Ø„‡≠Î©       ",    "",   NULL },
    { " äÆ‡®Á≠•¢Î©      ",    "",   NULL },
    { " Å•´Î©           ",    "",   NULL },
    { " í•¨≠Æ-·•‡Î©     ",    "",   NULL },
    { " ü‡™Æ-£Æ´„°Æ©    ",    "",   NULL },
    { " ü‡™Æ-ß•´•≠Î©    ",    "",   NULL },
    { " ü‡™Æ-®ß„¨‡„§≠Î© ",    "",   NULL },
    { " ü‡™Æ-™‡†·≠Î©    ",    "",   NULL },
    { " ü‡™Æ-Ø„‡Ø„‡≠Î©  ",    "",   NULL },
    { " Ü•´‚Î©          ",    "",   NULL },
    { " ü‡™Æ-°•´Î©      ",    "",   NULL }};
#endif

static  WINDOW *W_lin = NULL;
static  BORDER  B_lin = { BORD_NO_BORDER|BORD_SHADOW,NULL,NULL,NULL };

static char Pal_Save [D_pal];

static char Pal_Move [D_max+1] = {
    0, 32, 16, 48,  8, 40, 24, 56,
    1, 33, 17, 49,  9, 41, 25, 57,
    2, 34, 18, 50, 10, 42, 26, 58,
    3, 35, 19, 51, 11, 43, 27, 59,
    4, 20, 36, 52, 12, 28, 44, 60,
    5, 21, 37, 53, 13, 29, 45, 61,
    6, 38, 22, 54, 14, 46, 30, 62,
    7, 39, 23, 55, 15, 47, 31, 63
};
static int Mou_Ind;
static  char * brick = "€€€€€€€€€€€€€€€€€€€€";

bool mnpalett (char * palett) {
    int       cur = 0,   ind = 0, j;
    bool      ret = YES, don = NO;
    int       event;
    bool      (far * _Cdecl old_msfunc)();
    void      (*fnF2)(void),(*fnF8)(void),(*fnF9)(void);

    fnF2  = kbhotkey (F2,  NULL);
    fnF8  = kbhotkey (F8,  NULL);
    fnF9  = kbhotkey (F9,  NULL);

    pal_crea (palett);
    ind = pal_foun (palett, cur);
    pal_disp (palett, ind, cur);

    msptsmsk (SL_WHITE, SL_WHITE, 0, MS_XOR);
    msunhide ();
    event = MS_LEFT_PRESS|MS_RIGHT_PRESS;
    old_msfunc = msshadow (&event, msmnhand);
    while (!don) {
        wnwrtbuf (W_lin, 1, ind, 1, "\36", WN_NATIVE);
        cur = mnread (M_pal);
        wnwrtbuf (W_lin, 1, ind, 1, " ", WN_NATIVE);
        switch (Kb_sym) {
            case F9:
                ind = Mou_Ind;
                palett[cur] = Pal_Move[ind];
                pal_disp (palett, ind, cur);
                wnsetpal (palett);
                break;
            case Esc:
                utmovabs (palett, Pal_Save, D_pal, 0);
                ret = NO;
            case GrayEnter:
            case Enter:   don = YES;       break;
            case Left:
            case Right:
                if (Kb_sym == Left) ind = (ind == 0) ? D_max : --ind;
                else ind = (ind == D_max) ? 0 : ++ind;
                palett[cur] = Pal_Move[ind];
                pal_disp (palett, ind, cur);
                wnsetpal (palett);
                break;
            case F2:
                utmovabs (palett, Pal_Stnd, D_pal, 0);
                wnsetpal (palett);
            case F8:
            case Up:
            case Down:
            case Home:
            case End:
                ind = pal_foun (palett, cur);
                pal_disp (palett, ind, cur);
                break;
            default :
                break;
        }
    }
    mshide ();
    msshadow (&event, old_msfunc);

    wnsetpal (palett);
    for (j = 0; j < 64; j++) wnresize (W_lin, 0, -1, 0, 1);
    wndstroy (W_lin);
    mndstroy (M_pal);
    kbhotkey (F2,  fnF2 );
    kbhotkey (F8,  fnF8 );
    kbhotkey (F9,  fnF9 );
    return (ret);
}

static void pal_crea (char * palett) {
    int  tmp, j;

    M_pal = mncreate (16, 50, 0, 0,  &B_pal, &Att_Roll);
    Cur_Menu = M_pal;
    wnsplitw (W_pal, 17, WN_SINGLE|WN_VSPLIT);
    wnsplitw (W_pal, 24, WN_SINGLE|WN_VSPLIT);
    wnsplitw (W_pal, 27, WN_SINGLE|WN_VSPLIT);
    wnaddjoi (W_pal,  7, 51, '∂');
    wnaddjoi (W_pal, 14, 51, '∂');
    wnwrtbuf (W_pal,  6, 27, 0, "√ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ", Att_Roll.bord);
    wnwrtbuf (W_pal, 13, 27, 0, "√ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ", Att_Roll.bord);
    for (j = 0; j < D_pal; j++)  mnitmadd (M_pal, j,  j, 0, &I_pal[j]);
#ifdef ENGLISH
    wnwrtbuf (W_pal,  0, 29, 0, "Use keys: ", WN_NATIVE);
    wnwrtbuf (W_pal,  1, 29, 0, "\030\031",         W_pal->attr.bold);
    wnwrtbuf (W_pal,  2, 29, 0, "\033\032",         W_pal->attr.bold);
    wnwrtbuf (W_pal,  3, 29, 0, "F2",               W_pal->attr.bold);
    wnwrtbuf (W_pal,  4, 29, 0, "Enter",            W_pal->attr.bold);
    wnwrtbuf (W_pal,  5, 29, 0, "Esc",              W_pal->attr.bold);
    wnwrtbuf (W_pal,  1, 36, 0, "Attribute     ",   WN_NATIVE);
    wnwrtbuf (W_pal,  2, 36, 0, "Palette       ",   WN_NATIVE);
    wnwrtbuf (W_pal,  3, 36, 0, "Normal palette",   WN_NATIVE);
    wnwrtbuf (W_pal,  4, 36, 0, "Quit & update ",   WN_NATIVE);
    wnwrtbuf (W_pal,  5, 36, 0, "Don't update  ",   WN_NATIVE);
    wnwrtbuf (W_pal, 14, 29, 0, "Selected     00  00h ", WN_NATIVE);
    wnwrtbuf (W_pal, 15, 29, 0, "Normal       00  00h ", WN_NATIVE);
#else
    wnwrtbuf (W_pal,  0, 29, 0, "à·ØÆ´Ïß„©‚• ™´†¢®Ë®: ", WN_NATIVE);
    wnwrtbuf (W_pal,  1, 29, 0, "\030\031",         W_pal->attr.bold);
    wnwrtbuf (W_pal,  2, 29, 0, "\033\032",         W_pal->attr.bold);
    wnwrtbuf (W_pal,  3, 29, 0, "F2",               W_pal->attr.bold);
    wnwrtbuf (W_pal,  4, 29, 0, "Enter",            W_pal->attr.bold);
    wnwrtbuf (W_pal,  5, 29, 0, "Esc",              W_pal->attr.bold);
    wnwrtbuf (W_pal,  1, 36, 0, "Ä‚‡®°„‚       ",   WN_NATIVE);
    wnwrtbuf (W_pal,  2, 36, 0, "è†´®‚‡†       ",   WN_NATIVE);
    wnwrtbuf (W_pal,  3, 36, 0, "ë‚†≠§.Ø†´®‚‡† ",   WN_NATIVE);
    wnwrtbuf (W_pal,  4, 36, 0, "ÇÎÂÆ§         ",   WN_NATIVE);
    wnwrtbuf (W_pal,  5, 36, 0, "é‚™†ß         ",   WN_NATIVE);
    wnwrtbuf (W_pal, 14, 29, 0, "ÇÎ°‡†≠≠Î©    00  00h ", WN_NATIVE);
    wnwrtbuf (W_pal, 15, 29, 0, "ë‚†≠§†‡‚     00  00h ", WN_NATIVE);
#endif

    utmovabs (Pal_Save, palett, D_pal, 0);
    wnsetpal (palett);
    mndsplay (M_pal, 3, WN_CENTER);

    W_lin = wncreate (2, 2, 0, 0,  &B_lin, &Att_Wind);
    wnwrtbuf (W_lin, 0, 0, 1, brick, Pal_Move[0]);
    wnwrtbuf (W_lin, 0, 1, 1, brick, Pal_Move[1]);

    wndsplay (W_lin, W_pal->where_shown.row + W_pal->img.dim.h + 2, WN_CENTER);
    for (j = 0; j < 31; j++) {
        wnresize (W_lin, 0, 1, 0, -1);
        tmp = 2 * j + 2;
        wnwrtbuf (W_lin, 0, tmp, 1, brick, Pal_Move[tmp]);
        wnwrtbuf (W_lin, 0, tmp + 1, 1, brick, Pal_Move[tmp + 1]);
    }
    wnatrblk (W_lin, 1, 0, 1, 64, SL_WHITE|SL_INTENSITY, WN_UPDATE);
}

static void pal_disp (char * palett, int ind, int cur) {
    int     i, j;
    char    buf[4];

    for (j = 0; j < D_pal; j++) {
        wnwrtbuf (W_pal, j, 18, 6, brick, j);
        utl2crdx ((long) palett [j], buf, -2, 10);
        wnwrtbuf (W_pal, j, 25, 2, buf, WN_NATIVE);
    }
    for (j = 7; j <= 12; j++) {
        i = (cur < 8)  ? cur * 8 : (cur - 8) * 8 + 4;
        wnwrtbuf (W_pal, j, 29, 20, brick, Pal_Move [i]);
    }
    utl2crdx ((long)Pal_Move[ind], buf, -2, 10);
    wnwrtbuf (W_pal, 14, 42, 2, buf, WN_NATIVE);
    utl2crdx ((long)Pal_Move[ind], buf, -2, 16);
    wnwrtbuf (W_pal, 14, 46, 2, buf, WN_NATIVE);
    utl2crdx ((long)Pal_Stnd[cur], buf, -2, 10);
    wnwrtbuf (W_pal, 15, 42, 2, buf, WN_NATIVE);
    utl2crdx ((long)Pal_Stnd[cur], buf, -2, 16);
    wnwrtbuf (W_pal, 15, 46, 2, buf, WN_NATIVE);
}

static int pal_foun (char * palett, int cur) {
    register j;
    for (j = 0; j <= D_max; j++)  if (palett [cur] == Pal_Move [j]) return (j);
    return (0);
}

static int mnread (MENU * pmenu) {
    MNITEM * pcur = pmenu->pcur;
    MNITEM * p = pcur;

    kbgetkey (NULL);

    switch (Kb_sym) {
        case Up:       pcur = pcur->prev;           break;
        case Down:     pcur = pcur->next;           break;
        case Home:     pcur = pmenu->pitems;        break;
        case End:      pcur = pmenu->pitems->prev;  break;
        default:       pcur = NULL;                 break;
    }
    if (pcur != NULL) {
        pmenu->pcur = pcur;
        mnitmatr (pmenu, pcur);
        mnitmatr (pmenu, p);
    }
    return pmenu->pcur->tag;
}

#ifdef TC_lang
#pragma warn -par
#else
#pragma check_pointer( off )
#pragma check_stack( off )
#endif
static bool far _Cdecl msmnhand (int ev, int bs, int xp, int yp, int xd, int yd)
{
    MNITEM *q;
    MNITEM *p = Cur_Menu->pitems;
    int wrow  = Cur_Menu->pwin->where_shown.row;
    int wcol  = Cur_Menu->pwin->where_shown.col;
    int lrow  = W_lin->where_shown.row;
    int lcol  = W_lin->where_shown.col;
    int prow  = W_pal->where_shown.row;
    int pcol  = W_pal->where_shown.col;
    bool ret  = NO;
    kbflushb ();
    switch (ev) {
        case MS_LEFT_PRESS:
            do {
                if (yp == p->row + wrow  &&
                    xp >= p->col + wcol  &&  xp <= p->col + wcol + p->len) {
                        q = Cur_Menu->pcur;
                        Cur_Menu->pcur = p;
                        mnitmatr (Cur_Menu, q);
                        mnitmatr (Cur_Menu, Cur_Menu->pcur);
                        kbsetsym (F8);
                        return YES;
                }
                p = p->next;
            } while (p != Cur_Menu->pitems);
            if (yp == lrow  &&  xp >= lcol  &&  xp <= lcol + 63) {
                Mou_Ind = xp - lcol;
                kbsetsym (F9);
                return YES;
            }
            if (yp == prow +  1) {
                if (xp == pcol + 29) kbsetsym (Up);
                if (xp == pcol + 30) kbsetsym (Down);
                return YES;
            }
            if (yp == prow +  2) {
                if (xp == pcol + 29) kbsetsym (Left);
                if (xp == pcol + 30) kbsetsym (Right);
                return YES;
            }
            break;
        case MS_RIGHT_PRESS:
            kbsetsym (Esc);
            return YES;
    }
    return ret;
}
#ifdef TC_lang
#pragma warn +par
#endif

/*

    ∞±≤∞±≤∞±≤∞±≤∞±≤∞±≤∞±≤±≤∞±≤∞±≤∞±≤∞±≤∞±≤∞±≤∞±≤∞±≤∞±≤∞±≤∞±≤∞±≤∞±≤∞

    0         10        20        30         40        50
     0123456789 123456789 123456789 123456789 123456789
    ⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ¬ƒƒƒƒƒƒ¬ƒƒ¬ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ∑
00  ≥ Black           ≥∞∞∞∞∞∞≥00≥ à·ØÆ´Ïß„©‚• ™´†¢®Ë®:  ∫
01  ≥ Blue            ≥±±±±±±≥01≥ ||     Ä‚‡®°„‚        ∫  \030\031
02  ≥ Green           ≥≤≤≤≤≤≤≥02≥ --     è†´®‚‡†        ∫  \033\032
03  ≥ Cyan            ≥∞∞∞∞∞∞≥03≥ F2     ë‚†≠§.Ø†´®‚‡†  ∫
04  ≥ Red             ≥±±±±±±≥04≥ Enter  ÇÎÂÆ§          ∫
05  ≥ Magenta         ≥≤≤≤≤≤≤≥05≥ Esc    é‚™†ß          ∫
06  ≥ Brown           ≥∞∞∞∞∞∞≥20√ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ∂
07  ≥ White           ≥±±±±±±≥07≥ ∞∞∞±±±≤≤≤∞∞∞±±±≤≤≤∞∞∞ ∫  \037
08  ≥ Dark Gray       ≥≤≤≤≤≤≤≥08≥ ∞∞∞±±±≤≤≤∞∞∞±±±≤≤≤∞∞∞ ∫
09  ≥ Light Blue      ≥∞∞∞∞∞∞≥09≥ ∞∞∞±±±≤≤≤∞∞∞±±±≤≤≤∞∞∞ ∫
10  ≥ Light Green     ≥±±±±±±≥10≥ ∞∞∞±±±≤≤≤∞∞∞±±±≤≤≤∞∞∞ ∫
11  ≥ Light Cyan      ≥≤≤≤≤≤≤≥11≥ ∞∞∞±±±≤≤≤∞∞∞±±±≤≤≤∞∞∞ ∫
12  ≥ Light Red       ≥∞∞∞∞∞∞≥12≥ ∞∞∞±±±≤≤≤∞∞∞±±±≤≤≤∞∞∞ ∫
13  ≥ Light Magenta   ≥±±±±±±≥13√ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ∂
14  ≥ Yellow          ≥≤≤≤≤≤≤≥14≥ ÇÎ°‡†≠≠Î©    00  00h  ∫  \036
15  ≥ Intense White   ≥±±±±±±≥15≥ ë‚†≠§†‡‚     00  00h  ∫
    ‘ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕœÕÕÕÕÕÕœÕÕœÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕº
     0123456789 123456789 123456789 123456789 123456789
    0         10        20        30         40        50
*/
