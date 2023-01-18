/*************************************************************************
    utdbl2ce - Переводит вещественное число в строку в e - формате

int utdbl2ce (double x, char * buf, int prec, int opt);

double  x;          Число
char  * buf;        Буфер под число
int     prec;       Количество значимых цифер под мантиссу
int     opt;        Может принимать следующие значения:

    CV_RANGE1           0x01    Мантисса от 0 до 9.999999999
    CV_BLANK_BP         0x02    Не выводить 0 перед точкой
    CV_M_PLUS           0x04    Выводить '+' в мантиссе
    CV_M_PLUS_BLANK     0x08    Выравнивать под знак в мантиссе
    CV_E_PLUS           0x10    Выводить '+' в экспоненте
    CV_E_PLUS_BLANK     0x20    Выравнивать под знак в экспоненте
    CV_JST              0x40    Выравнивать экспоненту вправо
    CV_ZERO             0x80    Выводить ведущие '0' в экспоненте
    CV_FLOAT            0x100   Тип переменной - float

            Значения можно комбинировать операцией
            побитового ИЛИ (|).

Выход:          Длина полученного буфера buf

*************************************************************************/

#include <stdlib.h>
#include <slutil.h>


int utdbl2ce (double x, char * buf, int prec, int opt) {
    int         dec, sign;
    register    i, limprec;
    char far  * pbuf, * p = (char far *) buf;
    char        pdec [5];

    limprec = min (prec, 15);
    pbuf = ecvt (x, limprec, &dec, &sign);
    if (sign)                       * p++ = '-';
    else if (opt & CV_M_PLUS)       * p++ = '+';
    else if (opt & CV_M_PLUS_BLANK) * p++ = ' ';
    if (opt & CV_RANGE1) { * p++ = * pbuf++; prec--; limprec--; dec--; }
    else if (!(opt & CV_BLANK_BP)) * p++ = '0';
    * p++ = '.';
    utmovabs (p, pbuf, limprec, 0);
    p += limprec; i = prec - limprec;
    utmovsc (p, '0', i);   p += i;

    if (dec < 0) { sign = 1; dec = -dec; }
    else           sign = 0;
    if (opt & CV_FLOAT)  { * p++ = 'e'; i = 2; }
    else                 { * p++ = 'd'; i = 3; }
    if (opt & CV_JST) {
        if (opt & CV_ZERO) i = -i;
    } else i = 0;
    i = utl2crdx ((long) dec, pdec, i, 10);
    if (sign)                       * p++ = '-';
    else if (opt & CV_E_PLUS)       * p++ = '+';
    else if (opt & CV_E_PLUS_BLANK) * p++ = ' ';
    utmovabs (p, pdec, i, 0);
    p += i;  * p = '\0';
    return (int) (p - buf);
}

