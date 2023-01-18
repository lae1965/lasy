/*****************************************************************************/
/*    Функция utmony2c () конвертирует строку суммы цифрами в строку         */
/*    суммы прописью в различных валютах (по умолчанию в рублях)             */
/*                                                                           */
/*    int utmony2c (byte * pin, byte * pout,                                 */
/*                                 VALUTA * pbig, VALUTA * psmall);          */
/*                                                                           */
/*    byte    * pin    - указатель на строку с суммой цифрами;               */
/*    byte    * pout   - указатель на строку с суммой прописью.              */
/*                       Если pout == NULL - вычисляется длина строки,       */
/*                       а значение в нее не заносится;                      */
/*    VALUTA  * pbig   - указатель на структуру наименований крупной валюты  */
/*                                                      (если NULL - рубли)  */
/*    VALUTA  * psmall - указатель на структуру наименований мелкой валюты   */
/*                                                    (если NULL - копейки)  */
/*                                                                           */
/*    Возвращает длину строки с суммой прописью (если pout == NULL).         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#include <slutil.h>
#include <ctype.h>


typedef struct {
    byte     digs [3];
    VALUTA * names;
} DIGBIT;

static VALUTA pname [6] = {
    { "триллион ", "триллиона ", "триллионов ", 1 },
    { "миллиард ", "миллиарда ", "миллиардов ", 1 },
    { "миллион ",  "миллиона ",  "миллионов ",  1 },
    { "тысяча ",   "тысячи ",    "тысяч ",      0 },
    { "рубль ",    "рубля ",     "рублей ",     1 },
    { "копейка",   "копейки",    "копеек",      0 }
};

static DIGBIT pdig [6];

static char * edin_g [2] = { "одна ", "две " };
static char * edin [9] = {
"один ", "два ", "три ", "четыре ", "пять ",
"шесть ", "семь ", "восемь ", "девять "
};
static char * teen [9] = {
"одиннадцать ", "двенадцать ", "тринадцать ", "четырнадцать ", "пятнадцать ",
"шестнадцать ", "семнадцать ", "восемнадцать ", "девятнадцать "
};
static char * desyat [9] = {
"десять ", "двадцать ", "тридцать ", "сорок ", "пятьдесят ",
"шестьдесят ", "семьдесят ", "восемьдесят ", "девяносто "
};
static char * sto [9] = {
"сто ", "двести ", "триста ", "четыреста ", "пятьсот ",
"шестьсот ", "семьсот ", "восемьсот ", "девятьсот "
};
static char coop [] = "   ";
static char pnul [] = "ноль ";
static char   * po;
static int      len;
static bool     stor;


static void dig2c (char * pfrom);

int utmony2c (byte * pin, byte * pout, VALUTA * pbig, VALUTA * psmall) {
    char        * p, * q;
    register    i, j;
    bool        was_bit, was_dig = NO;

    stor = (pout != NULL);
    po = (char *) pout;
    len = 0;

    for (i = 0; i < 6; i++)  {
        pdig [i].names = &pname [i];
        utmovsc (pdig [i].digs, EOS, 3);
    }

    if (pbig != NULL) { pdig [4].names = pbig; pdig [5].names = psmall; }

    while (* pin == ' ') pin++;      /* Промотали ведушие пробелы */

    p = (char *) pin;

    while (isdigit (* p)) p++;   /* Промотали крупную валюту */
    q = p;

    for (i = 4; i >= 0; i--) {
        for (j = 2; j >= 0; j--) {
            if (p == (char *) pin) goto small;
            p--;
            pdig [i].digs [j] = * p - '0';
        }
    }

small:
    while (* q == ' ') q++;
    if (* q == EOS) goto cvt;
    if (!isdigit (* q)) {
        q++;
        while (* q == ' ') q++;
    }
    if (* q == EOS || !isdigit (* q)) goto cvt;
    pdig [5].digs [1] = * q - '0';
    q++;
    if (* q == EOS || !isdigit (* q)) {
        pdig [5].digs [2] = pdig [5].digs [1];
        pdig [5].digs [1] = 0;
    } else  pdig [5].digs [2] = * q - '0';

cvt:
    for (i = 0; i < 6; i++) {
        if (i < 5) {
            was_bit = NO;
            if (pdig [i].digs [0] > 0) {
                dig2c (sto [pdig [i].digs [0] - 1]);
                was_bit = YES;
            }
            if (pdig [i].digs [1] > 0) {
                was_bit = YES;
                if (pdig [i].digs [1] == 1 && pdig [i].digs [2] > 0) {
                    dig2c (teen [pdig [i].digs [2] - 1]);
                    goto lab;
                }
                dig2c (desyat [pdig [i].digs [1] - 1]);
            }

            if (pdig [i].digs [2] > 0) {
                was_bit = YES;
                if (!pdig [i].names->sex && pdig [i].digs [2] < 3)
                    dig2c (edin_g [pdig [i].digs [2] - 1]);
                else
                    dig2c (edin [pdig [i].digs [2] - 1]);
            }
        } else {
            was_bit = YES;
            coop [0] = pdig [i].digs [1] + '0';
            coop [1] = pdig [i].digs [2] + '0';
            dig2c (coop);
            if (coop [0] == '1') goto lab;
        }
        if (was_bit || i == 4) {
            if (!was_bit && !was_dig) {
                dig2c (pnul);
                goto lab;
            }
            if (pdig [i].digs [2] == 1) dig2c (pdig [i].names->for_1);
            else if (pdig [i].digs[2] >= 5 || pdig [i].digs[2] == 0) {
lab:
                dig2c (pdig [i].names->for_other);
            } else  dig2c (pdig [i].names->for_2);
        }
        if (was_bit) was_dig = YES;
    }
    if (stor) * pout = (byte) utcupper (* pout);
    return len;
}

static void dig2c (char * pfrom) {

    if (stor) strcat (po, pfrom);
    else      len += strlen (pfrom);
    return;
}

