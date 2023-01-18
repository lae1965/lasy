/************************************************************************
    wnviroll - Vertically scroll lines of text

int wnviroll (int num_lines, int attr, int u_row, int u_col,
              int l_row, int l_col, int dir);

int num_lines;      Number of lines to scroll.
                    A value of 0 specifies that all lines within the
                    region should be scrolled (thus clearing the region)
int attr;           This is the attribute to be used on the vacant lines
int u_row, u_col;   Upper left corner of region.
int l_row, l_col;   Lower right corner of region.
int dir             Scrolling direction
                    WN_SCR_UP
                    WN_SCR_DOWN

Description This function moves lines of characters (with their
            attributes) up or down within a defined rectangular
            region.  The vacant lines are filled with blanks and a
            specified attribute.

Limitation  This function cannot perform a scroll on an inactive
            page in a graphics mode.

Returns     Number of lines actually scrolled
            0   if inactive page in graphics mode
***********************************************************************/

#include <dos.h>
#include <slwind.h>

int wnviroll (int num_lines, int attr, int u_row, int u_col,
                                            int l_row, int l_col, int dir) {
    int         height, width, command;
    union REGS  inregs,outregs;
    char  far * pfrom;
    char  far * pto;
    char        blank   = ' ';
    char  far * pblank  = &blank;

    height = l_row - u_row + 1;
    if (num_lines <= 0 || num_lines > height)   num_lines = height;

    if (n_w_envr.n_v_page == n_w_envr.n_a_page ) {
        inregs.h.ah = (unsigned char) ((dir == WN_SCR_UP) ? 6 : 7);
        inregs.h.al = (unsigned char) num_lines;
        inregs.h.bh = (unsigned char) attr;
        inregs.h.ch = (unsigned char) u_row;
        inregs.h.cl = (unsigned char) u_col;
        inregs.h.dh = (unsigned char) l_row;
        inregs.h.dl = (unsigned char) l_col;
        int86 (16,&inregs,&outregs);
    } else {
        width  = l_col - u_col + 1;

        if (num_lines < height) {
            if (dir == WN_SCR_DOWN) {
                command = Cmd[9];
                pfrom   = wnviptrl (u_row, u_col);
                pto = wnviptrl (u_row + num_lines,u_col);
            } else {
                command = Cmd[7];
                pfrom   = wnviptrl (u_row + num_lines,u_col);
                pto = wnviptrl (u_row, u_col);
            }
            wnvicopy (&pfrom, &pto, height - num_lines, width,
                                                    NUM_COLS * 2, 0, command);
        } else   dir = WN_SCR_UP;

        if (dir == WN_SCR_DOWN) pto = pfrom;
        else  pto = wnviptrl (u_row + height - num_lines,u_col);
        wnvicopy (&pblank, &pto, num_lines, width, NUM_COLS * 2, attr, Cmd[3]);
    }
    return (num_lines);
}

