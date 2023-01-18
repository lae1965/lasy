#include <slmenu.h>
#include <slkeyb.h>
#include <sldbas.h>

static ITEM       pitem [] = {
    { " Продолжить ", "ПпGg", NULL },
    { " Повторить ",  "ТтNn", NULL },
    { " Прекратить ", "РрHh", NULL }
};
static BORDER     brd = { BORD_DDSS|BORD_SHADOW, NULL, NULL, NULL };
static WINDOW   * pwin;
static PRNPAG     prnpag = { 297, 203, 20, 20, 20, 7, 1, 10, 8, 2 };
static PRNOPT     prnopt;
static SPECPRN    specprn = { 20, YES };
static float      li, pt;
static int        numpage, n, curstr, widsym;
static char       filename [80], pbuf [80];
static byte     * Tmp_buf, * For_prnopt;
static byte       inibuf [] = { 27, '@', 27, 'x', 0, 27, 'P', 27, '5',
                                27, '-', 0,  27, 'W', 0, 18,  27, 'T',
                                27, 'F', 27, 'H' };
static byte       interval [] = { 27, 51, 0 };
static byte       rn       [] = { 13, 10 };
static byte       ff       [] = { 12 };

static int      str2prn         (byte * p, int len);
static int      outnote         (int mode);
static int      note2prn        (void);
static int      setprnopt       (PRNOPT * prnopt);
static int      offprnopt       (PRNOPT * prnopt);
static int      outleft         (void);
static int      outlf           (int numlf);
static int      outrn           (int num);

void main (int argc, char ** argv) {
    char            * pbuf = NULL, * p;
    char            * penv;
    int               num_fl_str = 0;
    bool              mod_mono;
    int               hand = -1, fonch = 176;
    word              len  = (word) 65500L, savlen;
    register          i, j;
    int               ret, numstr, numcop, curcop, total, space, copspac = 1;
    bool              neednote = YES;
    int               _4 = 4, _216 = 216;


    if (!wniniwin (_PREVIOS, _PREVIOS)) return;

    pbuf = utallocb (&len);
    Tmp_buf = malloc (1024);
    For_prnopt = malloc (1024);
    savlen = len;

    if (pbuf == NULL || Tmp_buf == NULL || For_prnopt == NULL) {
        mnwarnin (NULL, 0, 3, " ", "Не хватает памяти", " ");
        goto end;
    }

    prnpag.opt.ff = YES;
    if ((penv = getenv ("LASY")) != NULL) {
        strcat (penv, "\\");
        strcpy (pbuf, penv);
        strcat (pbuf, "md.ini");
        if ((hand = utopen (pbuf, UT_R)) != -1) {
            utread (hand, &mod_mono, sizeof (bool));
            utlseek (hand, 2L * sizeof (ATTRIB), UT_CUR);
            utread  (hand, &Att_Mnu1, sizeof (ATTRIB));
            utlseek (hand, 4L * sizeof (ATTRIB), UT_CUR);
            utread  (hand, &Att_User, sizeof (ATTRIB));
            utread  (hand, &fonch, 1);
            utlseek (hand, 21L, UT_CUR);
            utread  (hand, &specprn, sizeof (SPECPRN));
            utclose (hand);
        }
    }
    wndisfon (fonch, Att_User.nice);

    if (!specprn.n_216) { _4 = 2; _216 = 180; }
    li = 25.4 / (float) _216;
    pt = 25.4 / 120;

    pwin = wncreate (7, 43, 0, 0, &brd, &Att_Mnu1);
    wnwrtbuf (pwin, 1, 10,  5, "Копия", WN_NATIVE);
    wnwrtbuf (pwin, 2, 10,  8, "Страница", WN_NATIVE);
    wnwrtbuf (pwin, 3, 10, 13, "Всего страниц", WN_NATIVE);
    wnwrtbuf (pwin, 5,  3, 37, "Для прекращения печатания нажмите ESC",
                                                                  WN_NATIVE);

    if (argc > 1) strcpy (filename, argv [1]);
    else
newfile:
        if (!mnfnminp (WN_CENTER, WN_CENTER, " Имя файла ", filename,
                                                    NULL, YES, YES)) goto end;

    if ((hand = utopen (filename, UT_R)) == -1) {
        mnwarnin (NULL, 0, 3, " ", "Не могу открыть файл", argv [1], " ");
        goto end;
    }
    while ((len = utread (hand, pbuf, len)) > 0)
        for (i = 0; i < len; i++)
            if (pbuf [i] == '\n') num_fl_str++;
    if (pbuf [i - 1] != '\n') num_fl_str++;
    len = savlen;

beg:
    numpage = 0;
    mnprnopt (&prnopt, "Шрифт текста", &Att_Mnu1);
    if (!mnprnpag (&prnpag, &Att_Mnu1, YES)) goto end;
    if (prnpag.prnopt.elite) widsym = 10;
    else                     widsym = 12;
    if (prnpag.prnopt.size == 1) widsym *= 2;
    else if (prnpag.prnopt.size == 2 && prnpag.prnopt.nlq == 0)
        widsym = widsym / 2 + 1;
    space = n = min ((_4 + prnpag.linspac) * 3, _216);
    numstr = (int) ((float) (prnpag.hei - prnpag.top - prnpag.bot) /
            ((float) n * li));
    numcop  = prnpag.numcop;
    curstr  = 0;
    if (prnpag.opt.eq) {
        i = (int) ((float) prnpag.hei / li);
        j = (int) (((float) ((int) ((float) (num_fl_str * n) * li) +
                                prnpag.top + prnpag.bot) * numcop) / li);
        if (i < j) {
            mnwarnin (NULL, 0, 4, " ", "Заказанное количество экземпляров",
                                            "не умещается на лист", " ");
            goto beg;
        }
        space = (int) ((float) (prnpag.top + prnpag.bot) / li)
                                                        + (i - j) / numcop;
        copspac = 0;
    } else if (prnpag.copspac > 0)
        { copspac = prnpag.copspac; space *= copspac; }

    wndsplay (pwin, WN_CENTER, WN_CENTER);

    i = utl2crdx ((long) (num_fl_str / numstr + (num_fl_str % numstr > 0)),
                                                                  pbuf, 0, 10);
    wnwrtbuf (pwin, 3, 25, i, pbuf, WN_NATIVE);

    for (curcop = 1; curcop <= numcop; curcop++) {
        i = utl2crdx ((long) curcop, pbuf, 0, 10);
        wnwrtbuf (pwin, 1, 25, i, pbuf, WN_NATIVE);
        utlseek (hand, 0L, UT_BEG);
        total = 0;
        if (prnpag.opt.newpage) numpage = 0;
        if (neednote) {
            if ((ret = outnote (0)) > 0) goto vfyret;
        } else neednote = YES;
        if ((ret = outleft ()) > 0) goto vfyret;
        while ((len = utread (hand, pbuf, len)) > 0) {
            for (i = 0; i < len; i++) {
cont:
                if (kbvfysym (1)) goto end;
                if (pbuf [i] == '\r' || pbuf [i] == '\n') {
prnrn:
                    i++;
                    if ((ret = outrn (n)) > 0) goto vfyret;
                    total++;
                    curstr++;
                    if (total == num_fl_str) goto endfile;
                    if (curstr == numstr &&
                                   (total != num_fl_str || curcop < numcop)) {
                        if ((ret = outnote (1)) > 0) goto vfyret;
                        curstr = 0;
                    }
                    if ((ret = outleft ()) > 0) goto vfyret;
                    continue;
                }
                utch2prn (pbuf [i]);
                if ((ret = mnharder (pitem, 3)) != -1) {
                    if (ret == 0) goto cont;
                    goto vfyret;
                }
            }
        }
endfile:
        if (total < num_fl_str) {
            if ((ret = outrn (n)) > 0) goto vfyret;
            goto prnrn;
        }
        if (prnpag.opt.eq || !prnpag.opt.newpage &&
                (!prnpag.opt.full || curstr+num_fl_str+copspac <= numstr)) {
            curstr += copspac;
            if (curcop < numcop) {
                if ((ret = outlf (space)) > 0) goto vfyret;
                if (curstr < numstr)  neednote = NO;
            }
        }
        if (neednote) {
            if ((ret = outnote (2)) > 0) goto vfyret;
            curstr = 0;
        }
    }
    if (!neednote && (ret = outnote (2)) > 0) goto vfyret;
    if (argc < 2) {
        p = strrchr (filename, '\\');
        utmovabs (p + 1, "*.*", 4, 0);
        wnremove (pwin);
        goto newfile;
    }
end:
    wndstroy (pwin);
    utclose  (hand);
    free     (pbuf);
    utfree   (&For_prnopt);
    utfree   (&Tmp_buf);
    wnfinwin ();
    return;
vfyret:
    if (ret == 1) goto beg;
    if (ret == 2) goto end;
}

static  int  str2prn (byte * p, int len) {
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

static  int outnote (int mode) {
    int         sizetxt, i, ret, tonote = 0;
    bool        note;

    if (mode > 0) {
        note = (numpage > 1 || prnpag.opt.note1st);
        if (prnpag.opt.note == 2 && note) {
            sizetxt = (int) ((float) (curstr *  n) * li);
            i = prnpag.hei - prnpag.top - sizetxt - prnpag.tonote;
            if ((ret = outlf ((int) ((float) i / li))) > 0) return ret;
            if ((ret = note2prn ()) > 0) return ret;
        }
        if (prnpag.opt.ff && (ret = str2prn (ff, 1)) > 0) return ret;
    }
    if (mode < 2) {
        if (mnwarnin (NULL, 0, 3, " ", "Вставьте бумагу", " ") == -1)
            return 2;
        numpage++;
        if (mode == 0) {
            if ((ret = str2prn (inibuf, 22))  > 0) return ret;
            if ((ret = setprnopt (&prnopt)) > 0)   return ret;
        }
        note = (numpage > 1 || prnpag.opt.note1st);
        if (prnpag.opt.note == 1 && note) {
            tonote = max (prnpag.tonote - specprn.autopull, 0);
            if ((ret = outlf ((int) ((float) tonote / li))) > 0) return ret;
            if ((ret = note2prn ()) > 0) return ret;
        }
        if ((ret = outlf ((int) ((float) (max (prnpag.top - tonote -
                                specprn.autopull, 0)) / li))) > 0) return ret;
    }
    i = utl2crdx ((long) numpage, pbuf, 0, 10);
    wnwrtbuf (pwin, 2, 25, i, pbuf, WN_NATIVE);
    return 0;
}

static int note2prn (void) {
    int     ret, jst = UT_LEFT, len, numlen, vallen;
    byte    pnum [7];

    len = (int) ((float) (prnpag.wid - prnpag.right - prnpag.left) /pt)/widsym;
    vallen = strlen (prnpag.pnote);
    numlen =  utl2crdx ((long) numpage, pnum, 0, 10);
    if (len < vallen + numlen + 1) vallen = len - numlen - 1;

    if (prnpag.opt.mirror && numpage % 2 == 0) jst = UT_RIGHT;

    if ((ret = outleft ()) > 0) return ret;
    utstrjst ((char *) Tmp_buf, (char *) prnpag.pnote, vallen, ' ', len, jst);

    if (prnpag.opt.number) {
        if (jst == UT_RIGHT) utmovabs (Tmp_buf, pnum, numlen, 0);
        else  utmovabs (Tmp_buf + len - numlen, pnum, numlen, 0);
    }
    if ((ret = offprnopt (&prnopt)) > 0)        return ret;
    if ((ret = setprnopt (&prnpag.prnopt)) > 0) return ret;
    if ((ret = str2prn (Tmp_buf, len)) > 0)    return ret;
    if ((ret = offprnopt (&prnpag.prnopt)) > 0) return ret;
    if ((ret = setprnopt (&prnopt)) > 0)        return ret;
    return (outrn ((prnpag.opt.note == 1) ? 1 : n));
}

static  int  setprnopt (PRNOPT * prnopt) {
    byte   * q = For_prnopt;

    if (prnopt->nlq != 0) {
        * q++ = 27; * q++ = 'x'; * q++ = 1;
        * q++ = 27; * q++ = 'k'; * q++ = prnopt->nlq - 1;
    }
    if (prnopt->size != 0) {
        if (prnopt->size == 1)  { * q++ = 27; * q++ = 'W'; * q++ = 1; }
        else                      * q++ = 15;                /* SI */
    }
    if (prnopt->elite)  { * q++ = 27;  * q++ = 'M'; }     /* Esc M */
    if (prnopt->italic) { * q++ = 27;  * q++ = '4'; }     /* Esc 4 */
    if (prnopt->under)  { * q++ = 27;  * q++ = 45; * q++ = 1; }   /* Esc - 1 */


    if (prnopt->script != 0)
        { * q++ = 27; * q++ = 'S'; * q++ = prnopt->script - 1; }

    if (prnopt->bold > 0) {
        * q++ = 27;
        if (prnopt->bold == 1) * q++ = 'G';
        else                   * q++ = 'E';
    }
    return (str2prn (For_prnopt, (int) (q - For_prnopt)));
}

static  int  offprnopt (PRNOPT * prnopt) {
    byte   * q = For_prnopt;

    if (prnopt->nlq != 0) { * q++ = 27; * q++ = 'x'; * q++ = 0; }
    if (prnopt->size != 0) {
        if (prnopt->size == 1)  { * q++ = 27; * q++ = 'W'; * q++ = 0; }
        else                      * q++ = 18;                /* DC2 */
    }
    if (prnopt->elite)  { * q++ = 27;  * q++ = 'P'; }     /* Esc P */
    if (prnopt->italic) { * q++ = 27;  * q++ = '5'; }     /* Esc 5 */
    if (prnopt->under)  { * q++ = 27; * q++ = 45; * q++ = 0; }   /* Esc - 0 */
    if (prnopt->script != 0) { * q++ = 27;  * q++ = 'T'; }
    if (prnopt->bold > 0) {
        * q++ = 27;
        if (prnopt->bold == 1) * q++ = 'H';
        else                   * q++ = 'F';
    }

    return (str2prn (For_prnopt, (int) (q - For_prnopt)));
}

static int  outleft (void) {
    int         left = prnpag.left, num;
    byte      * p = Tmp_buf;
    register    ret;

    if (prnpag.opt.mirror && numpage % 2 == 0) left = prnpag.right;

    if ((num = (int) ((float) left / pt)) == 0) return 0;
    * p++ = 27; * p++ = 'Y'; * p++ = (byte) num % 256; * p++ = num / 256;
    if ((ret = str2prn (Tmp_buf, 4)) > 0) return ret;
    for (; num > 0; num--) {
cont:
        utch2prn (0);
        if ((ret = mnharder (pitem, 3)) == -1) continue;
        if (ret == 0) goto cont;
        return ret;
    }
    return 0;
}

static  int  outlf (int numlf) {
    register ret, remain = numlf % n;

    for (numlf = numlf / n; numlf > 0; numlf--)
        if ((ret = outrn (n)) > 0) return ret;
    if (remain > 0) return (outrn (remain));
    return 0;
}

static  int  outrn (int num) {
    int     ret;

    interval [2] = num;
    if ((ret = str2prn (interval, 3)) > 0) return ret;
    return (str2prn (rn, 2));
}

/*******************************************************************************

                       1         2         3         4
             0123456789012345678901234567890123456789012
            ┌───────────────────────────────────────────┐
            │                                           │0
            │          Копия          333               │1
            │          Страница       555               │2
            │          Всего сраниц   1000              │3
            │                                           │4
            │   Для прекращения печатания нажмите ESC   │5
            │                                           │6
            └───────────────────────────────────────────┘

*******************************************************************************/

