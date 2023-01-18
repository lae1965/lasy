#include <ctype.h>
#include <dos.h>
#include <slutil.h>

static  int str2month (char * p);


static byte days [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static byte monthlen [] = { 6, 7, 5, 6, 3, 4, 4, 7, 8, 7, 6, 7,
                            7, 8, 5, 5, 3, 4, 4, 6, 9, 7, 8, 8 };
static char * monthstr [] = {
    "Января",  "Февраля",  "Марта",     "Апреля",  "Мая",      "Июня",
    "Июля",    "Августа",  "Сентября",  "Октября", "Ноября",   "Декабря",
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"
};
/************************************************************************
    utmkdate - Переводит день, месяц и год в дату

long utmkdate (int day, int month, int year);

int     day;        День  (1 - 31)
int     month;      Месяц (1 - 12)
int     year;       Год   (0 - 99, 100 ...)
Выход:  Полученная дата

**************************************************************************/

long utmkdate (int day, int month, int year) {
    long         date = 0L;
    register     i;

    if (month < 1 || month > 12 || year < 0)  return (-1L);
    i = days [month - 1];
    if (month == 2) i += utisleap (year);
    if (day < 1 || day > i)  return (-1L);

    if (year > 0)
        date = (long) year * 365L + year / 4 - year / 100 + year / 400 + 1;

    for (i = 1; i < month; i++) {
        date += days [i - 1];
        if (i == 2) date += utisleap (year);
    }

    date += day - 1;
    return (date);
}

/**************************************************************************
    utspdate - Переводит дату в день, месяц и год

void utspdate (long date, int * pday, int * pmonth, int * pyear);

long      date;       Дата
int     * pday;      Буфер под день  (1 - 31)
int     * pmonth;    Буфер под месяц (1 - 12)
int     * pyear;     Буфер под год   (1900 ... )

**************************************************************************/

void utspdate (long date, int * pd, int * pm, int * py) {
    register    i, dif;
    long        tmp;

    i = (int) (date / 365L);
    while (i > 0) {
        tmp = i * 365L + i / 4 - i / 100 + i / 400 + 1;
        if (tmp <= date) { date -= tmp; break; }
        i--;
    }
    * py = i;
    for (i = 1; ; i++) {
        dif = days [i - 1];
        if (i == 2)         dif += utisleap (* py);
        if (date < dif)     break;
        else                date -= dif;
    }
    * pm = i;
    * pd = (int) date + 1;
}

/**************************************************************************

    utc2date - Переводит строку типа ДД.ММ.ГГГГ в дату

long utc2date (char ** pp, int centure);

char    ** pp;       Указатель на буфер строки
int        centure;  Век
Выход:  Полученная дата
        ** pp        Указатель на конец переведенной строки

**************************************************************************/

long utc2date (char ** pp, int centure) {
    int     day = 0, month = 0, year = 0;
    long    tmp;
    char    separ = ' ', * p = * pp;

    p += utrewchr (p, 32);          /* ' ' */
    if (!utc2lrdx (&p, &tmp, 10)) return (-1L);
    day = (int) tmp;
    p += utrewchr (p, 32);          /* ' ' */
    if (isdigit ((byte) (* p))) {
        if (!utc2lrdx (&p, &tmp, 10)) return (-1L);
        month = (int) tmp;
    } else {
        if ((month = str2month (p)) == -1) {
            separ = * p;
            p++;
            p += utrewchr (p, 32);          /* ' ' */
            if (isdigit ((byte) (* p))) {
                if (!utc2lrdx (&p, &tmp, 10)) return (-1L);
                month = (int) tmp;
            } else if ((month = str2month (p)) == -1) return (-1L);
        }
    }
    if ((p = strchr (p, separ)) == NULL) return (-1L);
    p++;
    p += utrewchr (p, 32);          /* ' ' */
    if (!utc2lrdx (&p, &tmp, 10)) return (-1L);
    year = (int) tmp;
    if (year < 100) {
        if (centure == 0) centure = utcurcen ();
        year += (centure - 1) * 100;
    }
    * pp = p;
    return (utmkdate (day, month, year));
}

/**************************************************************************

    utdate2c - Переводит дату в строку типа ДД.ММ.ГГГГ

int utdate2c (long date, char * buf, char separ, int opt);

long      date;      Дата
char    * buf;       Буфер под дату
char      separ;     Разделитель между ДД, ММ, ГГ
int       opt;       Может принимать следующие значения:

    CV_JST              0x40    Выравнивать вправо
    CV_ZERO             0x80    Выводить ведущие '0'
    CV_CENTURE          0x01    Выводить век
    CV_STRMON           0x02    Выводить месяц строкой
    CV_LAT              0x04    Выводить месяц по английски
    CV_FULL             0x08    Выводить месяц полностью

            Значения можно комбинировать операцией
            побитового ИЛИ (|).

Выход:  Длина полученного буфера buf

**************************************************************************/

int utdate2c (long date, char * buf, char separ, int opt) {
    register    i = 0, j = 8;
    int         day, month, year, len = 3;
    char      * p = buf;

    if (opt & CV_JST) {
        if (opt & CV_ZERO) i--;
        else               i++;
    }
    utspdate (date, &day, &month, &year);
    p += utl2crdx ((long) day, p, i * 2, 10);
    * p++ = separ;
    if (opt & CV_STRMON) {
        if (opt & CV_LAT) { month += 12; j++; }
        month--;
        if (opt & CV_FULL) {
            len = monthlen [month];
            if (i != 0) { j -= len; utmovsc (p, ' ', j); p += j; }
        }
        utmovabs (p, monthstr [month], len, 0);
        p += len;
    } else p += utl2crdx ((long) month, p, i * 2, 10);
    * p++ = separ;
    if (opt & CV_CENTURE) i *= 2;
    else                  year %= 100;
    p += utl2crdx ((long) year, p, i * 2, 10);
    * p = '\0';
    return (int) (p - buf);
}

/**************************************************************************

    utdtoday - Возвращает текущую дату

long utdtoday (void);

Выход:  Текущая дата

**************************************************************************/

long utdtoday (void) {
    struct date  pdate;

    getdate (&pdate);
    return (utmkdate (pdate.da_day, pdate.da_mon, pdate.da_year));
}

/**************************************************************************
    utcurcen - Возвращает текущий век;

int utcurcen (void);

Выход:  Текущий век.

**************************************************************************/

int utcurcen (void) {
    struct date  pdate;

    getdate (&pdate);
    return (pdate.da_year / 100 + 1);
}

/**************************************************************************
    utbegmon - Возвращает дату первого дня месяца

long utbegmon (long date);

long    date;       Дата

Выход:  Полученная дата

**************************************************************************/

long utbegmon (long date) {
    int     day, month, year;

    utspdate (date, &day, &month, &year);
    return (utmkdate (1, month, year));
}

/**************************************************************************
    utendmon - Возвращает дату последнего дня месяца

long utendmon (long date);

long    date;       Дата

Выход:  Полученная дата

**************************************************************************/

long utendmon (long date) {
    int day, month, year;

    utspdate (date, &day, &month, &year);
    return (utmkdate (utmndays (month, year), month, year));
}

/**************************************************************************
    utprvmon - Возвращает дату первого дня пред.месяца

long utprvmon (long date);

long    date;       Дата

Выход:  Полученная дата

**************************************************************************/

long utprvmon (long date) {

    return (utbegmon (utbegmon (date) - 1));
}

/**************************************************************************
    utnxtmon - Возвращает дату первого дня след.месяца

long utnxtmon (long date);

long    date;       Дата
Выход:  Полученная дата

**************************************************************************/

long utnxtmon (long date) {
    return (utendmon (date) + 1);
}

/**************************************************************************
    utmndays - Возвращает число дней в месяце

int utmndays (int month, int year);

int     month;      Месяц (1 - 12)
int     year;       Год   (0 - 99, 100 ...)
Выход:  День  (1 - 31)
        -1    Ошибка

**************************************************************************/

int utmndays (int month, int year) {
    int day;

    if (month < 1 || month > 12 || year < 0)   return (-1);
    day = days [month - 1];
    if (month == 2) day += utisleap (year);
    return (day);
}

/**************************************************************************
    utisleap - Определяет високосность года

bool utisleap (int year);

int     year;     Определяемый год

Выход:
    YES - Год високосный
    NO  - Год не високосный
**************************************************************************/

bool utisleap (register int year) {
    return ((year & 0x0003) == 0 && year % 100 != 0 || year % 400 == 0);
}

static  int str2month (char * p) {
    char        * ptmp;
    register      i, j;

    for (i = 0; i < 24; i++) {
        ptmp = monthstr [i];
        for (j = 0; j < 3; j++)
            if (utcupper (p [j]) != utcupper (ptmp [j])) break;
        if (j == 3) {
            if (i >= 12) i -= 12;
            return (i + 1);
        }
    }
    return -1;
}

