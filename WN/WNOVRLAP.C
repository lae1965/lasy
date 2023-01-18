/********************************************************************
    wnovrlap - Report whether two rectangular regions overlap.

bool wnovrlap (LOCATE *pcorner1,DIMREC *pdim1,LOCATE *pcorner2,DIMREC *pdim2);

LOCATE *pcorner1;      Pointers to structures designating
LOCATE *pcorner2;      upper left corners of the two regions.
DIMREC *pdim1;         Pointers to structures designating
DIMREC *pdim2;         the dimensions of the two regions.

Description This function compares two rectangular regions (assumed
            to lie on the same display screen) and reports whether
            they overlap.

Returns     YES         if they overlap,
            NO          if not.
*********************************************************************/

#include <slwind.h>

bool wnovrlap (LOCATE *pcorner1, DIMREC *pdim1, LOCATE *pcorner2, DIMREC *pdim2)
{
    return ((pcorner1->row + pdim1->h - 1 >= pcorner2->row &&
             pcorner2->row + pdim2->h - 1 >= pcorner1->row &&
             pcorner1->col + pdim1->w - 1 >= pcorner2->col &&
             pcorner2->col + pdim2->w - 1 >= pcorner1->col) ? YES : NO);
}

