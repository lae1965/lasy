#include <slwind.h>

void wndisfon (char ch, int at) {
    byte         at_tmp;
    char far   * pbuf, * pscreen;

    at_tmp  = Att_User.nice;
    at  = (at == WN_NATIVE) ? (int) at_tmp : at;
    pbuf = (char far *) &ch;
    pscreen = wnviptrl (0, 0);
    wnvicopy (&pbuf, &pscreen, NUM_ROWS, NUM_COLS, NUM_COLS*2, at, Cmd[3]);
    return;
}

