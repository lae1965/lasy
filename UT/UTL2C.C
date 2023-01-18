 /*****************************************************************************
       utl2c () - преобразует число типа long в строку

            int utl2c (long num, char * pbuf, int opt);

   Параметы:
         long    num  -  преобразуемое число типа long;
         char  * p    -  указатель на буфер;
         int     opt  -  опции, которые могут принимать следующие значения:

   CV_E_PLUS           16    Выводить '+'
   CV_E_PLUS_BLANK     32    Выравнивать под знак

   Возвращает:               длину строки;
 *****************************************************************************/


#include <slutil.h>

int utl2c (long num, char * pbuf, int opt) {
    char    * p = pbuf;
    register  i = 0;

    if (num >= 0L) {
        if (opt & CV_E_PLUS)            { * p++ = '+'; i++; }
        else if (opt & CV_E_PLUS_BLANK) { * p++ = ' '; i++; }
    }
    return (i + utl2crdx (num, p, 0, 10));
}

