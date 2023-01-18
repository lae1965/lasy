#include <slmous.h>
#include <slkeyb.h>
#include <slchsc.def>

static void far  msfunc (int ev, int bs, int xp, int yp, int xd, int yd);
static void      msterm (void);

#define ROWS    25
#define COLS    80

static int left_sc, left_ch;
static int righ_sc, righ_ch;
static int cent_sc, cent_ch;

bool msemukbd (int l_sc, int l_ch, int r_sc, int r_ch, int c_sc, int c_ch)
{
    int mask = MS_MOVE|MS_LEFT_PRESS|MS_RIGHT_PRESS|MS_CENTER_PRESS;

    left_sc = (l_sc == -1) ? KB_S_N_ENTER : l_sc;
    left_ch = (l_ch == -1) ? KB_C_N_ENTER : l_ch;
    righ_sc = (r_sc == -1) ? KB_S_N_ESC   : r_sc;
    righ_ch = (r_ch == -1) ? KB_C_N_ESC   : r_ch;
    cent_sc = (c_sc == -1) ? KB_S_A_X     : c_sc;
    cent_ch = (c_ch == -1) ? KB_C_A_X     : c_ch;
    atexit (msterm);
    mssetpos (10, 10);
    mshandlr (&mask, msfunc);
    return (YES);
}

static void msterm (void)
{
    int mask = MS_NO_EVENT;
    mshandlr (&mask, msfunc);
}
#pragma warn -par
static void far msfunc (int event, int buttons, int col, int row, int xd, int yd)
{
    char sc, ch = (char) buttons;
    static int orow, ocol;

    kbflushb ();
    switch (event) {
        case MS_MOVE:
            if (orow != row) {
                if ((orow - row) < 0) {
                    sc = KB_S_N_DOWN;
                    ch = KB_C_N_DOWN;
                }
                else {
                    sc = KB_S_N_UP;
                    ch = KB_C_N_UP;
                }
                kbplace (KB_TAIL, (int)ch, (int)sc);
            }
            else if (ocol != col) {
                if ((ocol-col) < 0) {
                    sc = KB_S_N_RIGHT;
                    ch = KB_C_N_RIGHT;
                }
                else {
                    sc = KB_S_N_LEFT;
                    ch = KB_C_N_LEFT;
                }
                kbplace (KB_TAIL, (int)ch, (int)sc);
            }
            if (row > ROWS - 2 || row < 2  || col > COLS - 2 || col < 2) {
                    ocol = COLS / 2;
                    orow = ROWS / 2;
                    mssetpos (orow, ocol);
            }
            else {
                ocol = col;
                orow = row;
            }
            break;
        case MS_LEFT_PRESS:
            if (!msfunkey (col, row))
                kbplace (KB_TAIL, left_ch, left_sc);
            break;
        case MS_RIGHT_PRESS:
            kbplace (KB_TAIL, righ_ch, righ_sc);
            break;
        case MS_CENTER_PRESS:
            kbplace (KB_TAIL, cent_ch, cent_sc);
            break;
    }
}
#pragma warn +par
