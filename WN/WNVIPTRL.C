/***************************************************************************
    wnviptrl - Convert screen location into memory address

char far *wnviptrl (int row, int col);

int  row;       Row    (0 = top of screen)
int  col;       Column (0 = left edge)

Description This function computes the physical address in video
            memory of the specified location on the current display
            page.  (The current display page is defined by the
            global variable b_curpage.)

            An error will occur and NIL will be returned as the
            value of the function if the screen is not in a standard
            text mode (0, 1, 2, 3, or 7), or if row or col is beyond
            the edge of the screen.

Returns     The computed address, or NIL if an error occurred.

***************************************************************************/

#include <dos.h>
#include <slwind.h>

#define CRT_LEN_SEG     0x0000          /* Address of BIOS variable     */
#define CRT_LEN_OFFSET  0x044c          /* CRT_LEN which contains       */
#define pg_len	(*uttofar(CRT_LEN_SEG,CRT_LEN_OFFSET,unsigned))

char far *wnviptrl (int row, int col) {

    /* Construct address:  segment is beginning of screen buffer,       */
    /* offset is based on page length, page number, row, and column.    */
    /* Note:  pg_len may be incorrect for Monochrome Adapter but that   */
    /* should have no effect since b_curpage should be 0.               */

    return (uttofar( ((N_V_STATE.mode == 7) ? 0xb000 : 0xb800),
            ((row * NUM_COLS) + col) * 2 + n_w_envr.n_a_page * pg_len,
            char));
}

