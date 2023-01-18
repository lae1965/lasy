/****************************************************************************
    wnfunhlp - Создание и вывод окна HELP-а по функциональным клавишам

WINDOW * wnfunhlp (char **txt);

char    **txt;      Указатель на массив из 10 строк

Выход:   Указатель на структуру созданного окна
         NULL    Ошибка
*****************************************************************************/

#include <slwind.h>

WINDOW * wnfunhlp (char ** txt) {
    static char *key[] = { "1"," 2"," 3"," 4"," 5"," 6"," 7"," 8"," 9"," 10" };
    char buf[7];
    WINDOW *pw, *ps;
    register int i;
    int pos;

    if ((pw = wncreate (1, 80, N_V_STATE.numtextrows-1, 0, NULL, &Att_Khlp))
        == NULL)  return (NULL);
    for (i = 0, pos = 0; i < 10; pos += (i==0) ? 7 : 8, i++)
        wnwrtbuf (pw, 0, pos, 0, key[i], Att_Khlp.bold);
    for (i = 0, pos = 1; i < 10 && txt[i] != NULL; pos += (i==8) ? 9 : 8, i++) {
        strncpy (buf, txt[i], 6);
        buf[6] = EOS;
        wnwrtbuf (pw, 0, pos, 0, buf, Att_Khlp.text);
    }
    ps = Cur_Wind;
    if (!wndsplay (pw, WN_NATIVE, WN_NATIVE)) return (NULL);
    wnselect (ps);
    return (pw);
}
/*        10        20        30        40        50        60        70
0123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
1xxxxxx 2xxxxxx 3xxxxxx 4xxxxxx 5xxxxxx 6xxxxxx 7xxxxxx 8xxxxxx 9xxxxxx 10xxxxxx
*/

