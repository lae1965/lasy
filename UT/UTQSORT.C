#include <slutil.h>
#include <math.h>

static  byte * copyone  (byte * p, int width);
static  void   copytail (byte * p, int banksize);

static  byte * z, * pbeg, * endbase;

/* Быстрая сортировка. Количество сравнений пропорционально N * Log2 (N) */
/* Требует памяти, равной сортируемому массиву                           */

bool utqsort (void * pbase, int nelem, int width,
                                           int (* fcmp) (void * p, void * q)) {
    byte      * p, * q, * tmpbase;
    double      d;
    int         nlevels, banksize;
    register    capas, j;

    if (nelem <= 1) return YES;

    endbase = (byte *) pbase + width * nelem;

    if ((tmpbase = malloc (nelem * width)) == NULL) return NO;
    d = log ((double) nelem) / log ((double) 2);
    nlevels = (int) d;
    if ((double) nlevels < d) nlevels++;

    for (capas = 1; nlevels > 0; nlevels--, capas *= 2) {    /* по уровням */
        banksize = capas * width;
        pbeg = p = pbase;
        z = tmpbase;
        while (1) {                       /* по парам банок */
            q = p + banksize;

            while (1) {                /* из 2-х банок делаем одну */

                if (q >= endbase || q >= pbeg + 2 * banksize) {
                    copytail (p, banksize);
                    break;
                }
                if (p >= pbeg + banksize) {
                    copytail (q, banksize * 2);
                    break;
                }

                if ((j = (* fcmp) (p, q)) <= 0) {
                    p = copyone (p, width);
                    if (j == 0) goto lab;
                } else {
lab:
                    q = copyone (q, width);
                }
            }
            if (z >= tmpbase + width * nelem) break;
            pbeg = p = pbeg + 2 * banksize;
        }
        utmovabs (pbase, tmpbase, nelem * width, 0);
    }
    free (tmpbase);
    return YES;
}

static  byte * copyone (byte * p, int width) {
    utmovabs (z, p, width, 0);
    z += width;
    return (p + width);
}

static  void copytail (byte * p, int banksize) {
    register j;

    if (pbeg + banksize > endbase) j = (int) (endbase - p);
    else j = banksize - (int) (p - pbeg);
    utmovabs (z, p, j, 0);
    z += j;
    return;
}

