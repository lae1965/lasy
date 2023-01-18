/*************************************************************************
    wndiswrk - Создание и вывод окна "РАБОТА"

WINDOW * wndiswrk (long wrk, int num[, char *msg1, char *msg2 ...]);

long    wrk         Максимальное количество "РАБОТЫ"
int     num         Число строк сообщений
char    *msg        Строки сообщений в окне

Выход:  Указатель на структуру созданного окна
        NULL        Ошибка
**************************************************************************/

#include "language.h"
#include <stdarg.h>
#include <slwind.h>

static int  rowlin;
static int  maxlen;
static int  curlen;
static long maxwrk;
static long curwrk;

static char strini = '░';
static char strwrk = '█';
static WINDOW *pmsg;
static WINDOW *pfon;
static WINDOW *psav;

static  BORDER  msbor = { BORD_DDDD, NULL, NULL, NULL };
static  BORDER  phbor = { BORD_NO_BORDER | BORD_SHADOW, NULL,NULL,NULL };

WINDOW *wndiswrk (long wrk, int num, ...) {
    va_list       arg;
    int           len, i, wid, hei;
    char        * str, attr = Att_Work.text;

    if (wrk == 0L) attr |= SL_MONOBLINK;

    va_start (arg, num);
    for (wid = 0, i = 0; i < num; i++) {
        len = strlen (va_arg (arg, char *));
        wid = max (len, wid);
    }
    va_end (arg);

    wid = wid + 2;
    hei = (wrk <= 0) ? num : num + 1;
    pmsg = wncreate (hei, wid, 0, 0, &msbor, &Att_Work);  /* БП */

    va_start (arg, num);
    for (i = 0; i < num; i++) {
        str = va_arg (arg, char *);
        len = strlen (str);
        wnwrtbuf (pmsg, i, (wid-len)/2, len, str, attr);
    }
	va_end (arg);

	if (wrk > 0) {
        rowlin = hei - 1;
		maxlen = wid - 2;
        curlen = 0;
        maxwrk = wrk;
        curwrk = 0;
        for (i = 0; i < maxlen; i++)
            wnwrtbuf (pmsg, rowlin, i+1, 1, &strini, WN_NATIVE);
    } else  maxwrk = 0;

    psav = Cur_Wind;
    pfon = wncreate (hei+4, wid+8, 0, 0, &phbor, &Att_Work);
    wndsplay (pfon, WN_CENTER, WN_CENTER);
    wndsplay (pmsg, WN_CENTER, WN_CENTER);
    return (pmsg);
}

/***************************************************************************
        wnupdwrk - "Движение" строки в окне "РАБОТА"

void wnupdwrk (long wrk);

long wrk;       Количество выполненной "РАБОТЫ" за такт

***************************************************************************/

void wnupdwrk (long wrk) {
    long len;
	int i;

    if (maxwrk == 0 || pmsg == NULL)  return;
    curwrk += wrk;
    len    = (curwrk * (long)maxlen) / maxwrk;
    if (len > maxlen)   len = maxlen;
	for (i = curlen; i < (int)len; i++)
        wnwrtbuf (pmsg, rowlin, i+1, 1, &strwrk, WN_NATIVE);
	curlen = (int)len;
}

/***************************************************************************
        wnremwrk - Удаление с экрана окна "РАБОТА"

void wnremwrk (void);

***************************************************************************/

void wnremwrk (void)  {

    wndstroy (pmsg); wndstroy (pfon); wnselect (psav);
    pmsg = pfon = NULL;
    return;
}

