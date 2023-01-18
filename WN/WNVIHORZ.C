/*********************************************************************
    wnvihorz - Horizontally scroll columns of text

int wnvihorz (int num_cols, int attr, int u_row, int u_col,
              int l_row, int l_col, int dir);

int num_cols;       Number of columns to scroll.
                    A value of 0 specifies that all columns
                    within the region should be scrolled
                    (thus clearing the region).
int attr;           This is the attribute to be used on the
                    vacant columns.
int u_row,u_col;    Upper left corner of region.
int l_row,l_col;    Lower right corner of region.
int dir;            Scrolling direction
                    WN_SCR_RIGHT
                    WN_SCR_LEFT

Description This function moves columns of characters (with their
            attributes) within a defined rectangular region to the
            left or right.  The vacant columns are filled with
            blanks and a specified attribute.

            This function works only for standard text modes (0, 1,
            2, 3, and 7).

Returns     Number of columns actually scrolled
            0   if graphics mode
**************************************************************************/

#include <slwind.h>

int wnvihorz (int num_cols, int attr, int u_row, int u_col,
                                            int l_row, int l_col, int dir) {
    int  height,width;
    int  command;
    char far *pfrom;
    char far *pto;
    char     blank   = ' ';
    char far *pblank = &blank;

    height = l_row - u_row + 1;
    width  = l_col - u_col + 1;
    if (num_cols <= 0 || num_cols > width)  num_cols = width;
     /* Do not scroll more columns  than there are in the region.*/

    if (num_cols < width) {
        if (dir == WN_SCR_LEFT) {
            command = Cmd[7];
            pfrom = wnviptrl (u_row, u_col + num_cols);
            pto   = wnviptrl (u_row, u_col);
        } else {
            command = Cmd[9];
            pfrom = wnviptrl (u_row,u_col);
            pto   = wnviptrl (u_row,u_col + num_cols);
        }
        wnvicopy (&pfrom,&pto,height,width - num_cols,
                                                    NUM_COLS * 2, 0, command);
    } else dir = WN_SCR_LEFT;

    if (dir == WN_SCR_LEFT) pto = wnviptrl (u_row,u_col + width - num_cols);
    else pto = pfrom;
    wnvicopy (&pblank, &pto, height, num_cols, NUM_COLS * 2, attr, Cmd[3]);
    return (num_cols);
}

