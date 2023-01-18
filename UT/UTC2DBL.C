/***************************************************************************
    utc2dbl - Переводит строку в вещественное число

bool utc2dbl (char ** pp, double * val, bool doub) {

char    ** pp;       Указатель на буфер строки
double   * val;      Указатель на возвращаемое значение
bool       doub;     YES - тип значения - double, NO - float

Выход:
        YES - Ok;
***************************************************************************/

#include <ctype.h>
#include <math.h>
#include <slutil.h>

static  char * storebuf (char * pto, char * pfrom, register int len);

static  int     buflen, numlen, posexp;
static  bool    waspoint;
extern char DBL_point;

bool utc2dbl (char ** pp, double * val, bool doub) {
    double          x = 0.0;
    register        limexp, i = 0;
    int             rewpow = 0, remain = 0, j;
    bool            neg = NO;
    long            tmp;
    char          * p = * pp, buf [9], * pbuf, * pbegdig;

    waspoint = NO;
    numlen = posexp = 0;

    if (doub) limexp = 307;
    else      limexp =  38;

    p += utrewchr (p, 32);         /* ' ' */

    if (*p == '-')       { neg = YES; p++; }
    else if (*p == '+')               p++;

    p += utrewchr (p, 32);         /* ' ' */
    pbegdig = p;
    p += utrewchr (p, 48);         /* '0' */

    if (* p == DBL_point) {
        waspoint = YES;
        p++;
        rewpow = utrewchr (p, 48);         /* '0' */
        p += rewpow;
    }
    p = storebuf (buf, p, 8);
    if (buflen > 0) {
        pbuf = buf;
        utc2lrdx (&pbuf, &tmp, 10);
        x = (double) tmp;
    }
    if (doub && buflen == 8) {
        p = storebuf (buf, p, 7);
        if (buflen > 0) {
            pbuf = buf;
            utc2lrdx (&pbuf, &tmp, 10);
            x *= pow10 (buflen);
            x += (double) tmp;
        }
    }
    if (waspoint) remain = numlen - posexp;
    while (isdigit ((byte) (* p))) { p++; i--; }

    if (!waspoint) {
        rewpow = i;
        if (* p == DBL_point) p++;
        while (isdigit ((byte) (* p))) p++;
    }
    p += utrewchr (p, 32);         /* ' ' */

    tmp = 0L;
    if (toupper (* p) == 'E' || toupper (* p) == 'D') {
        p++;
        if (!utc2lrdx (&p, &tmp, 10)) return NO;
    } else if (pbegdig == p) return NO;

    i = (int) tmp - remain - rewpow;
    if (waspoint && posexp == 0) j = (int) tmp - (rewpow + 1);
    else                         j = numlen + i;
    if (abs (j) > limexp) return NO;
    x *= pow10 (i);

    * pp  = p;
    * val = (neg) ? -x : x;
    return YES;
}

static  char * storebuf (char * pto, char * pfrom, register int len) {
    register    i = 0;

    while (i < len) {
        if (* pfrom == DBL_point) {
            if (waspoint) break;
            pfrom++; waspoint = YES;
            posexp = numlen + i;
        }
        if (isdigit ((byte) (* pfrom))) {
            pto [i] = * pfrom;
            i++;  pfrom++;
        } else break;
    }
    pto [i] = '\0';
    numlen += i;
    buflen = i;

    return pfrom;
}

