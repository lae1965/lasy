#include "language.h"
#include <slwind.h>
#include <slkeyb.h>
#include <slsymb.def>
#include <slmous.h>

static bool far _Cdecl msmnhand (int ev, int bs, int xp, int yp, int xd, int yd);
static  bool        get_line   (char *, FILE *, byte, byte);
static  void        change_top (int);
static  void        move_high  (int);

#define REPT    0xFF
#define BOLD    '^'
#define UP      "\030"
#define DN      "\031"

struct index_entry {
    char     name[32];          /* Идентификатор темы   */
    long     fpos;              /* Позиция темы в файле */
    int      len;               /* Число строк в теме   */
};

static  BORDER  bor = { BORD_DDDD|BORD_SHADOW|BORD_TCT|BORD_BRT,NULL,NULL,NULL };

#define MAX_TOPS 16
static  char Hlp_File[80] = "";
static  char topname[32]  = "";
static  char topbuff[MAX_TOPS][32];
static  int  indbuff;
static  int  indcurr;
static  char *buftop      = NULL;
static  int  hei, wid, top, toplen;
static  char *phigh;
static  int  lhigh;
static  int  mou_flag;
#ifdef  ENGLISH
static  char *keyhelp[] = { "Index","Back","","","","","","","","Quit" };
#else
static  char *keyhelp[] = { "Индекс","Предыд","","","","","","","","Выход" };
#endif

void wnsethlp (char *fname) {

    strncpy (Hlp_File, fname, 78);
}

void wncurhlp (char *topic) {

    if (Hlp_File[0] != EOS)
        wngethlp (Hlp_File, topic);
}

void wngethlp (char *fname, char *topic) {
    FILE    * fhlp;
    WINDOW  * pwin, * psav, * phlp;
    CELLSC  * pcell;
    int       nent, i, tmp, ntyp, btyp = 0;
    char    * p;
    struct    index_entry ent;
    bool      done = NO;
    char      more[4];
    bool      next = NO, found;
    void (*fnF1)(void),(*fnF2)(void),(*fnF10)(void);
    int       event;
	bool (far * _Cdecl old_msfunc)();

    mou_flag = 0;
    if (topic != NULL) {
        strncpy (topname, topic, 31);
        topname[31] = EOS;
    }
    if ((fhlp = fopen (fname, "rb")) == NULL) return;
    hei = getw (fhlp);
    wid = getw (fhlp);
    fread (&ent, sizeof(ent), 1, fhlp);
	nent = ((int)ent.fpos - 4) / (int)sizeof(ent);

	bor.pttitle = topname;
	bor.pbtitle = more;
    if ((pwin = wncreate (hei, wid, 0, 0, &bor, &Att_Chlp)) == NULL) {
        fclose (fhlp);
        return;
	}
    fnF1  = kbhotkey (F1,  NULL);
    fnF2  = kbhotkey (F2,  NULL);
    fnF10 = kbhotkey (F10, NULL);

    pcell = pwin->img.pdata;
    pwin->options.delayed = 1;
    psav = Cur_Wind;
    phlp = wnfunhlp (keyhelp);
    indbuff = indcurr = 0;
    strcpy (topbuff [indbuff], topname);

    msptsmsk (SL_WHITE, SL_WHITE, 0, MS_XOR);
    msunhide ();
    event = MS_LEFT_PRESS|MS_LEFT_RELEAS|MS_RIGHT_PRESS;
    old_msfunc = msshadow (&event, msmnhand);
    while (!done) {
		*more = EOS;
		fseek (fhlp, 4L, SEEK_SET);
		tmp = nent;
		found = NO;
        while (tmp-- > 0) {
            fread (&ent, sizeof(ent), 1, fhlp);
            if ((strcmp (topname, ent.name)) == 0 ) {
				found = YES;
				break;
            }
        }
        if (!found) {
			fseek (fhlp, 4L, SEEK_SET);
            fread (&ent, sizeof(ent), 1, fhlp);
            strcpy (topname, ent.name);
        }
		fseek (fhlp, ent.fpos, SEEK_SET);
		toplen = ent.len;

		tmp = max (toplen, hei) * wid * 2;
        if ((p = buftop = malloc (tmp)) == NULL)  break;
        while (tmp > 0)
            { *p++ = ' '; *p++ = Att_Chlp.text; tmp -= 2; }
		for (tmp = toplen, p = buftop; tmp > 0; tmp--, p += wid * 2)
            if (!get_line (p, fhlp, Att_Chlp.text, Att_Chlp.bold)) goto EXIT;
		phigh = buftop;
		lhigh = 0;
        if (!next && !wndsplay (pwin, WN_CENTER, WN_CENTER)) goto EXIT;
		change_top (0);
		next = NO;

        while (!done && !next) {
            *more = EOS;
            ntyp = 0;
            if (top > 0) { strcat (more, UP); ntyp |= 0x02; }
            if (top < toplen - hei) { strcat (more, DN); ntyp |= 0x01; }
            if (ntyp != btyp) { wnputbor (pwin); btyp = ntyp; }
            if (mou_flag == -1) {
                kbsetsym (Up);
                utsleept (4);
            }
            else if (mou_flag == 1) {
                kbsetsym (Down);
                utsleept (4);
            }
            switch (kbgetkey (NULL)) {
                case Tab:        move_high (1);                    break;
                case ShiftTab:   move_high (-1);                   break;
                case Up:         change_top (top - 1);             break;
                case Down:       change_top (top + 1);             break;
                case PgUp:       change_top (top - hei + 1);       break;
                case PgDn:       change_top (top + hei - 1);       break;
                case Home:       change_top (0);                   break;
                case End:        change_top (toplen - hei);        break;
                case GrayEnter:
                case Enter:
                    if (lhigh > 0) {
                        for (i = 0; i < lhigh && i < 31; i++) {
                            topname[i] = *phigh;
                            phigh += 2;
                        }
                        topname[i] = EOS;
						next = YES;
						btyp = -1;
                        if (++indbuff == MAX_TOPS) indbuff = 0;
                        strcpy (topbuff [indbuff], topname);
                        indcurr = indbuff;
					}
                    break;
                case F1:
                    *topname = EOS;
                    next = YES;
                    btyp = -1;
                    if (++indbuff == MAX_TOPS) indbuff = 0;
                    strcpy (topbuff [indbuff], topname);
                    indcurr = indbuff;
                    break;

                case F2:
                    if (--indcurr < 0) indcurr = indbuff;
                    strcpy (topname, topbuff [indcurr]);
                    next = YES;
                    btyp = -1;
                    break;

                case Esc:
                case F10:        done = YES;                       break;
                default:                                           break;
            }
        }
        utfree (&buftop);
    }
EXIT:
    msshadow (&event, old_msfunc);
    mshide ();

    free (buftop);
	pwin->img.pdata = pcell;
    wndstroy (pwin);
    wndstroy (phlp);
    wnselect (psav);
    fclose (fhlp);
    kbhotkey (F1,  fnF1 );
    kbhotkey (F2,  fnF2 );
    kbhotkey (F10, fnF10);
    return;
}

static  bool get_line (char *str, FILE *fhlp, byte att_norm, byte att_bold) {
    bool            is_bold = NO;
    char          * p;
    int             len = 0;
    register int    c, count;

    p = str;
	while ((c = fgetc (fhlp)) != EOS) {
        if (feof (fhlp))  return (NO);
        if (len >= wid)   continue;
		if (c == BOLD) {
			if ((c = fgetc (fhlp)) != BOLD) {
				is_bold = !is_bold;
                if (c == EOS) break;
			}
		}
        if (c == REPT ) {
            count = fgetc (fhlp);
            c = fgetc (fhlp);
			while (count-- > 0 && len < wid) {
				*p++ = (char)c;
				*p++ = is_bold ? att_bold : att_norm;
                len++;
            }
        } else {
			*p++ = (char)c;
			*p++ = is_bold ? att_bold : att_norm;
            len++;
        }
    }
	while (len++ < wid) {
        *p++ = ' ';
		*p++ = is_bold ? att_bold : att_norm;
	}
    return (YES);
}

static void change_top (int newtop) {
    char    * pstart, * pend, * p;

    if (newtop > toplen - hei)  newtop = toplen - hei;
    if (newtop < 0)             newtop = 0;
    pstart = buftop + newtop * wid * 2;
    pend   = pstart + hei * wid * 2 - 2;
    Cur_Wind->img.pdata = (CELLSC *) pstart;
    Cur_Wind->options.dirty = 1;

	if (lhigh == 0 || phigh < pstart) {
		while (lhigh > 0) {
            *(phigh + 1) = Cur_Wind->attr.bold;
			phigh += 2;
			lhigh--;
		}
        for (p = pstart; p <= pend; p += 2) {
            if (*(p+1) == Cur_Wind->attr.bold) {
				phigh = p;
                while (p <= pend && *(p+1) == Cur_Wind->attr.bold) {
                    *(p+1) = Cur_Wind->attr.high;
					lhigh++;
					p += 2;
				}
				break;
			}
		}
    } else if (phigh > pend){
		while (lhigh > 0) {
            *(phigh + 1) = Cur_Wind->attr.bold;
			phigh += 2;
			lhigh--;
		}
		for (p = pend; p >= pstart; p-=2) {
            if (*(p+1) == Cur_Wind->attr.bold) {
                while (p >= pstart && *(p+1) == Cur_Wind->attr.bold) {
                    *(p+1) = Cur_Wind->attr.high;
					lhigh++;
					p -= 2;
				}
				phigh = p + 2;
				break;
			}
		}
	}
    wnupdate (Cur_Wind);
	top = newtop;
}

static void move_high (int dir) {
    char    * pstart, * pend, * p;

	pstart = buftop + top*wid*2;
	pend   = pstart + hei*wid*2 - 2;

    if (lhigh == 0)  return;
	if (dir > 0) {
		while (lhigh > 0) {
            *(phigh + 1) = Cur_Wind->attr.bold;
			phigh += 2;
			lhigh--;
		}
		p = phigh;
        while (1)  {
            if (p > pend)  p = pstart;
            if (*(p+1) == Cur_Wind->attr.bold) {
				phigh = p;
                while (p <= pend && *(p+1) == Cur_Wind->attr.bold) {
                    *(p+1) = Cur_Wind->attr.high;
					lhigh++;
					p += 2;
				}
				break;
            } else    p += 2;
		}
    } else {
		p = phigh - 2;
		while (lhigh > 0) {
            *(phigh + 1) = Cur_Wind->attr.bold;
			phigh += 2;
			lhigh--;
		}
		while (1) {
            if (p < pstart) p = pend;
            if (*(p+1) == Cur_Wind->attr.bold) {
                while (p >= pstart && *(p+1) == Cur_Wind->attr.bold) {
                    *(p+1) = Cur_Wind->attr.high;
					lhigh++;
					p -= 2;
				}
				phigh = p + 2;
				break;
            } else    p -= 2;
		}
	}
    Cur_Wind->options.dirty = 1;
    wnupdate (Cur_Wind);
}
#ifdef TC_lang
#pragma warn -par
#else
#pragma check_pointer( off )
#pragma check_stack( off )
#endif
static bool far _Cdecl msmnhand (int ev, int bs, int xp, int yp,
                                                            int xd, int yd) {
    int        wrow  = Cur_Wind->where_shown.row;
    int        wcol  = Cur_Wind->where_shown.col;
    int        whei  = Cur_Wind->img.dim.h - 1;
    int        wwid  = Cur_Wind->img.dim.w - 1;
    char       sym[4];
    bool       ret = NO;
    char far * pbuf, * pscreen;

    kbflushb ();
    mou_flag = 0;
    switch (ev) {
        case MS_LEFT_PRESS:
            pbuf = (char far *) sym;
            pscreen = wnviptrl (yp, xp);
            if (yp >= wrow  &&  yp <= wrow + whei  &&
                xp >= wcol  &&  xp <= wcol + wwid) {
                wnvicopy (&pscreen, &pbuf, 1, 1, NUM_COLS*2, 0, Cmd[6]);
                if (sym[1] == Cur_Wind->attr.high) {
                    kbsetsym (Enter);
                    return YES;
                }
                if (sym[1] == Cur_Wind->attr.bold) {
                    do {
                        move_high (1);
                        wnvicopy (&pscreen, &pbuf, 1, 1, NUM_COLS*2,0,Cmd[6]);
                    }
                    while (sym[1] != Cur_Wind->attr.high);
                    return YES;
                }
            }
            if (yp == wrow + whei + 1) {
                wnvicopy (&pscreen, &pbuf, 1, 1, NUM_COLS*2, 0, Cmd[5]);
                if (sym[0] == UP[0]) {
                    mou_flag = -1;
                    kbsetsym (Up);
                    ret = YES;
                }
                else if (sym[0] == DN[0]) {
                    mou_flag = 1;
                    kbsetsym (Down);
                    ret = YES;
                }
                return ret;
            }
            ret=msfunkey (xp, yp);
            break;
        case MS_LEFT_RELEAS:
            kbsetsym (Del);
            ret = YES;
            break;
        case MS_RIGHT_PRESS:
            kbsetsym (Esc);
            ret = YES;
            break;
    }
    return ret;
}
#ifdef TC_lang
#pragma warn +par
#endif

