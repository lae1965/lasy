/*****************************************************************
    mndskinp - Ввод имени диска

bool mndskinp (row, col, dsk, ttl, num[,msg1,msg2,...] );

int     row;        Номер строки окна ввода на экране
int     col;        Номер колонки окна ввода на экране
char    *ttl;       Заголовок окна ввода (или NULL)
char    *dsk;       Буфер строки ввода имени диска ("C:")
int     num;        Число строк сообщений
char    *msg;       Строки сообщений в меню

Выход:  YES         Имя диска выбрано
        NO          Имя диска не выбрано
*****************************************************************/

#include <stdarg.h>
#include <dos.h>
#include <slmenu.h>

#define diskettes ((utpeekw( uttofaru( 0x40, 0x10) ) >> 6) & 3)
/* 0 - 1d, 1 - 2d*/

#define fantom     utpeekb( uttofaru( 0x50, 0x04) )

char    Cur_Disk_Name = 0;
static  ITEM    item_Disk = { NULL, NULL, NULL };
static  BORDER  bord_Disk = { BORD_DDDD|BORD_TCT, NULL, NULL, NULL };
static  BORDER  phbor     = { BORD_NO_BORDER | BORD_SHADOW, NULL,NULL,NULL };

bool cdecl mndskinp (int row, int col, char *dsk, char *ttl, int num,... ) {
#define len_keys 3

	union  REGS inreg, outreg;
	WINDOW *pfon, *pswin;
	MENU   *pmsg, *psmnu;
	va_list arg;
    int  drivers, len, pos, i, j, wid, hei, lenmnu, curdsk = 0, citm = -1;
    bool ret = NO;
	char *str, *keys, text[6];
	int len_item = 3;

    inreg.h.ah = 0x19; /* Get current disk */
    int86( 0x21, &inreg, &outreg);

    inreg.h.ah = 0x0E; /* Set current disk */
	curdsk = inreg.h.dl = outreg.h.al;
    int86( 0x21, &inreg, &outreg);
    drivers = (int)outreg.h.al ;            /* Number of system drivers */
                                         /* Check for drive availability.*/
    for (i = 2; i < drivers; i++) {
        inreg.h.ah = 0x0E;                      /* Set current disk */
        inreg.h.dl = (unsigned char)i;
        int86( 0x21, &inreg, &outreg);

        inreg.h.ah = 0x19;                      /* Get current disk */
        int86( 0x21, &inreg, &outreg);

        if (outreg.h.al != inreg.h.dl)  break;  /* Drive not available. */
    }
    inreg.h.ah = 0x0E;                          /* Set current disk */
    inreg.h.dl =(unsigned char) curdsk;
	int86( 0x21, &inreg, &outreg);

	drivers = i;
	if ( i < 6 ) len_item = 5;
	else if ( i > 10 ) len_item = 1;

	memset (text, BLANK, len_item);
	text[len_item] = 0;

    keys = calloc ( i, len_keys );  /* БП */

    if (!diskettes)  drivers--;


    va_start (arg, num);
    for (wid = 0, i = 0; i < num; i++) {
		len = strlen (va_arg (arg, char *));
		if (len > wid)
			wid = len;
	}
	va_end (arg);

	lenmnu = len_item * drivers;
    wid    = max (wid, lenmnu) + 2;
    hei    = num + 1;
	bord_Disk.pttitle = ttl;
    pmsg   = mncreate (hei, wid, 0, 0 , &bord_Disk, &Att_Disk);

	va_start (arg, num);
    for (i = 0; i < num; i++) {
		str = va_arg (arg, char *);
		len = strlen (str);
        wnwrtbuf (pmsg->pwin, i, (wid-len)/2, len, str, WN_NATIVE);
	}
	va_end (arg);

	for (pos = (wid - lenmnu)/2, i = 0, item_Disk.text = &text[0];
                                                    i < drivers;  i++)  {
		if (i==0 && diskettes == 0 && fantom) {
			i++; drivers++;
		}
		j = i*len_keys;
		text[len_item/2] = keys[j] = (char)('A' + i);
		if (Cur_Disk_Name == text[len_item/2])
			citm = i;
		keys[j+1] = text[len_item/2] | 0x20; /* Lower keys letter */
		keys[j+2] = 0;
		item_Disk.keys = &keys[j];

        mnitmadd (pmsg, i, num , pos, &item_Disk);  /*БП*/
        if (text[len_item/2] == 'A' && !diskettes)  { i++; drivers++; }
		pos += len_item;
	}
    pfon = wncreate (hei+4, wid+8, row, col, &phbor, &Att_Disk);

    pswin = Cur_Wind;
    psmnu = Cur_Menu;
    pmsg->pcur = mnfnditm (pmsg->pitems, citm == -1 ? curdsk : citm);
	wndsplay (pfon, row, col );
	mndsplay (pmsg, pfon->where_shown.row+2, pfon->where_shown.col+4);
    if ((i = mnreadit (pmsg)) >= 0) {
        dsk[0] = (char)i + 'A';
        dsk[1] = ':';
        dsk[2] = EOS;
        ret = YES;
    }
	mndstroy (pmsg);
	wndstroy (pfon);
    free (keys);
    Cur_Menu = psmnu;
	wnselect (pswin);
    Cur_Disk_Name = 0;
	return (ret);
}

