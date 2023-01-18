/***************************************************************************
    wnpagrem - Remove window from linked list of windows

bool wnpagrem (WINDOW *pwindow);

WINDOW  *pwindow;   Window to remove from list.

Description This function removes a window from the linked list
            which governs the windows displayed on its video device
            and display page.

Returns     YES         Ok
            NO          Error
***************************************************************************/

#include <slwind.h>


bool wnpagrem (WINDOW *pwindow) {

    if (!wnislink (pwindow))   return (NO);
    if (Win_List == pwindow)  Win_List = pwindow->below;
    if (pwindow->above != NULL) pwindow->above->below = pwindow->below;
    if (pwindow->below != NULL) pwindow->below->above = pwindow->above;
    pwindow->options.linked = 0;
    pwindow->above   = NULL;
	pwindow->below   = NULL;
    return (YES);
}

