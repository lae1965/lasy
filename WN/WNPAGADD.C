/*************************************************************************
    wnpagadd - Add window to linked list of windows

WINDOW *wnpagadd (WINDOW *pwindow);

WINDOW  *pwindow;   Window to add to list.

Description This function adds a window to the linked list which
            governs the windows displayed on a given device and
            video page.  The window is stored as the topmost (i.e.,
            most recently displayed).

Returns     Pointer to newly-linked WINDOW structure
            NULL        if failure.
***************************************************************************/

#include <slwind.h>

WINDOW *wnpagadd (WINDOW *pwindow) {

    if (wnislink (pwindow))   return (NULL);
    pwindow->below = Win_List;
    pwindow->above = NULL;
    if (pwindow->below != NULL) pwindow->below->above = pwindow;
    pwindow->options.linked = 1;
    return (Win_List = pwindow);
}

