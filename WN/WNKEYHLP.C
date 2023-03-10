/*************************************************************************
    wnkeyhlp - ???????? ? ?뢮? ???? ??ࠢ?筮? ??ப?

WINDOW * wnkeyhlp (char **txt);

char    **txt;      ?????⥫? ?? ???ᨢ ??ࠢ????? ??ப
                    ??᫥???? ??ப? - NULL

??室:   ?????⥫? ?? ????????? ᮧ??????? ????
         NULL    ?訡??
**************************************************************************/

#include <slwind.h>


WINDOW *wnkeyhlp (char **txt) {
    WINDOW   * pw, * ps;
    register   i, att;
    int        pos = 0;

    if ((pw = wncreate (1, NUM_COLS, NUM_ROWS - 1, 0, NULL, &Att_Khlp)) == NULL)
        return (NULL);

    for (i=0; txt[i] != NULL; i++) {
        att = (i % 2) ? Att_Khlp.text : Att_Khlp.bold;
        wnwrtbuf (pw, 0, pos, 0, txt[i], att);
        pos += strlen (txt[i]);
    }
    ps = Cur_Wind;
    if (!wndsplay (pw, WN_NATIVE, WN_NATIVE)) { wndstroy (pw); return (NULL); }
    wnselect (ps);
    return (pw);
}

