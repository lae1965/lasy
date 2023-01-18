/**********************************************************************
   wnwrtbuf - Write buffer to the window.

int wnwrtbuf (WINDOW *pwin, int row, int col, int len, char *buf, int attr);

WINDOW *pwin    Pointer to window structure
int  num_writ   Number of character cells actually written
int  row, col   Row and column at which to begin writing
int  len        Number of character cells to write.
                If len == 0 indicates that
                the buf is terminated by a NUL ('\0')
                character.
char * buf      Data to write
int  attr       WN_NATIVE if window's native attribute is to be used;
 *
*************************************************************************/

#include <slwind.h>


int wnwrtbuf (WINDOW *pwin, int row, int col, int len, char *buf, int attr) {
    register     H = pwin->img.dim.h, W  = pwin->img.dim.w;
    int          offset, last, last_row, area  = H * W;
    CELLSC     * pdata;

    if (row == WN_NATIVE) row = pwin->cur_loc.row;
    if (col == WN_NATIVE) col = pwin->cur_loc.col;


    if (len == 0) {
        len = (int) strlen (buf);
        if (len == 0)  return 0;
    }
    if (row == WN_CENTER) row = H / 2;
    if (col == WN_CENTER) col = (W - len) / 2;

    offset = (row * W) + col;
    pdata  = pwin->img.pdata + offset;
    utuplim (len, (area - offset));
    last   = offset + len - 1;
    utuplim (last, area - 1);
    last_row = last / W;

    if (attr == WN_NATIVE)  wnmovps  (pdata, buf, len);
    else                    wnmovpsa (pdata, buf, len, (byte) attr);


    if (row == last_row) 
        pwin->options.dirty = !wnupdblk (pwin, row, col, 1, len);
    else                 
        pwin->options.dirty = !wnupdblk (pwin, row, 0, last_row - row + 1, W);
    return (len);
}

