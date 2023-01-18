/**********************************************************************
    wnselect - Set window for future I/O.

void wnselect (WINDOW *pwin);

WINDOW *pwin    Pointer to WINDOW structure to select.

Description This function designates a window to be "current".
            Subsequent Blaise C TOOLS window I/O requests will be
            directed to this window.  (Such I/O will not necessarily
            have visible effects if the window is designated
            "delayed" or is overlapped by another window.)

            The window need not be currently displayed.

            The window's cursor is not activated.  Another window on
            this display page may have been designated to have an
            active cursor via WNCURSOR.

            An error occurs if pwin does not point to a valid window
            structure.

***********************************************************************/

#include <slwind.h>

void wnselect (WINDOW *pwin) {

    Lst_Wind = Cur_Wind;
	if (pwin->options.shown) wncursor (pwin);
    Cur_Wind = pwin;
    return;
}

