/****************************************************************************
    utdbl2cf - Переводит вещественное число в строку в f - формате

int utdbl2cf (double x, char * buf, int wid, int prec, int opt) {

double  x;          Число
char  * buf;        Буфер под число
int     wid;        Длина буфера buf
int     prec;       Количество позиций под дробную часть
int     opt;        Может принимать следующие значения:

    CV_E_PLUS           0x10    Выводить '+'
    CV_E_PLUS_BLANK     0x20    Выравнивать под знак
    CV_JST              0x40    Выравнивать вправо
    CV_ZERO             0x80    Выводить ведущие '0'
    CV_FLOAT            0x100   Тип переменной - float

            Значения можно комбинировать операцией
            побитового ИЛИ (|).

        В случае, если число не убирается в буфер в f - формате, сначала
      отбрасывается дробная часть, затем происходит попытка конвертации в
      e - формате, а в случае неудачи число вообще не выводится в буфер.
Выход:          Длина полученного буфера buf;
                -1 - буфер слишком мал

****************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <slutil.h>

static int jstdouble (char * pbuf, int prec, int dec, int limprec);
extern char DBL_point;

int utdbl2cf (double x, char * buf, int wid, int prec, int opt) {
    int         dec, sign, option = CV_BLANK_BP;
    bool        needprec = NO;
    int         i = 0, limprec;
    double      delta;
    char far  * pbuf, * p = (char far *) buf;
    char        sym;

    if (opt & CV_FLOAT)  { limprec =  8; option |= CV_FLOAT; }
    else                   limprec = 15;

    delta = 5.1 * pow10 ( -(prec + 1));
    if (x >= 0.0) x += delta;
    else          x -= delta;

    pbuf = ecvt (x, limprec, &dec, &sign);

    for (i = prec + dec; i > 0 && i < limprec; i++)
        pbuf [i] = '0';


    if (sign || (opt & CV_E_PLUS) || (opt & CV_E_PLUS_BLANK)) i = 1;
    else                                                      i = 0;
    if (dec > 0) {
        i += dec + 1;
        if (i > wid) {
            wid -= 2 + sign;                   /* '.', 'd', '-' */
            if (dec < 10)       wid--;
            else if (dec < 100) wid -= 2;
            else                wid -= 3;
            if (wid <= 0) return -1;
            wid = min (wid, limprec);
            return utdbl2ce (x, buf, wid, option);
        }
        if (i + prec > wid) prec = wid - i;
    } else if (i + prec + 2 > wid) prec = wid - i - 2;

    if (sign)                       { * p++ = '-'; wid--; }
    else if (opt & CV_E_PLUS)       { * p++ = '+'; wid--; }
    else if (opt & CV_E_PLUS_BLANK) { * p++ = ' '; wid--; }
    if (opt & CV_JST) {
        if (opt & CV_ZERO) sym = '0';
        else               sym = ' ';
        if (dec > 0)  i = wid - (dec + prec + 1);
        else          i = wid - (prec + 2);
        utmovsc (p, sym, i);
        p += i;
    }
    if (dec > 0) {
        if (dec > limprec) {
            utmovabs (p, pbuf, limprec, 0);
            p += limprec; dec -= limprec;
            utmovsc (p, '0', dec);
            p += dec; * p++ = DBL_point;
            utmovsc (p, '0', prec);
        } else {
            utmovabs (p, pbuf, dec, 0);
            p += dec;    pbuf += dec;
            * p++ = DBL_point; limprec -= dec;
            needprec = YES;
        }
    } else {
        * p++ = '0'; * p++ = DBL_point; dec = -dec;
        if (dec > prec)  utmovsc (p, '0', prec);
        else {
            utmovsc (p, '0', dec);
            p += dec; prec -= dec;
            needprec = YES;
        }
    }
    if (needprec) {
        if (prec > limprec) {
            utmovabs (p, pbuf, limprec, 0);
            p += limprec; prec -= limprec;
            utmovsc (p, '0', prec);
        } else  utmovabs (p, pbuf, prec, 0);
    }
    p += prec;
    * p = '\0';
    return (int) (p - buf);
}

double utdblcorr (double x, int prec, bool doub) {
    int         dec, sign, limprec, i;
    char far  * pbuf, * p;

    if (doub) limprec =  8;
    else      limprec = 15;
    x += 5.0 * pow10 ( -(prec + 1));

    pbuf = ecvt (x, limprec, &dec, &sign);

    for (i = prec + dec; i > 0 && i < limprec; i++)
        pbuf [i] = '0';

    p = pbuf;
    utc2dbl (&p, &x, doub);
    if (sign) x = -x;
    dec -= limprec;
    x *= pow10 (dec);

    return x;
}

