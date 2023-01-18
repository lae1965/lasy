/*************************************************************************
    utstrcvt - Преобразование строки

Прототип:

char * utstrcvt (char *psource, int options);

Параметры:

char *  psource;    Указатель на исходную строку.
int     options;    Параметры преобразования.

Описание:   Функция utstrcvt() преобразует исходную строку psource
            в соответствии с параметром options и возвращает указатель
            на измененную строку. Возможные коды преобразования:

            ST_RWHITE       Удаляет все пробелы.
            ST_RLWHITE      Удаляет все ведущие пробелы.
            ST_RTWHITE      Удаляет все хвостовые пробелы.
            ST_REDUCE       Сжимает все неодиночные пробелы до одного.
            ST_NOQUOTE      Символы в ' или " не преобразуются.
            ST_TOUP         Преобразует буквы в верхний регистр.
            ST_TOLOW        Преобразует буквы в нижний регистр.
            ST_RCONTROL     Удаляет все управляющие символы.

            Коды преобразования можно комбинировать операцией
            побитового ИЛИ (|).

Возвращает: Указатель на преобразованную строку.

****************************************************************************/

#include <ctype.h>
#include <slutil.h>


char * utstrcvt (char * from, int conv) {
    char            * pfrom = from;
    register char   * pto   = from;
    register char     c;
    char              quote_char = EOS;

    int               rlwhite      = conv & ST_RLWHITE;
    int               rwhite       = conv & ST_RWHITE;
    int               reduce       = (!rwhite) && (conv & ST_REDUCE);
    int               ckquotes     = conv & ST_NOQUOTE;
    int               to_up        = conv & ST_TOUP;
    int               to_low       = conv & ST_TOLOW;
    int               rcontrol     = conv & ST_RCONTROL;
    int               in_white     = NO;
    int               hit_nonwhite = NO;
    int               quote_on     = NO;

    while ((c = *pfrom++) != EOS) {
        if (quote_on) {
            *pto++ = c;
            if (c == quote_char) quote_on = NO;
        } else if (ckquotes && ((c == '"') || (c == '\''))) {
            *pto++ = c;
            in_white     = NO;
            hit_nonwhite = YES;
            quote_on     = YES;
            quote_char   = c;
        } else if (isspace((byte) c) && isascii((byte) c)) {
            if (rwhite)
                ;
            else if (rlwhite && !hit_nonwhite)
                ;
            else if (reduce) {
                if (in_white)
                    ;
                else  {
                    *pto++ = BLANK;
                    in_white = YES;
                }
            } else if (rcontrol && iscntrl ((byte) c))
                ;
            else
                *pto++ = c;
        } else if (iscntrl((byte) c) && isascii((byte) c)) {
            in_white     = NO;
            hit_nonwhite = YES;

            if (rcontrol)
                ;
            else  *pto++ = c;
        }
        else {
            in_white     = NO;
            hit_nonwhite = YES;

            if (isascii((byte) c)) {
                if (to_up)   c = (char)toupper((int)c);
                if (to_low)  c = (char)tolower((int)c);
            }
            *pto++ = c;
        }
    }
    *pto = EOS;

    if (conv & ST_RTWHITE)
        for (c = *--pto; isspace((byte)c)&&isascii((byte)c) && (pto>=from);
                                                                   c = *--pto)
            *pto = EOS;
    return (from);
}

