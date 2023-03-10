/**********************************************************************
    wndsplay - ?뢮??? ???? ?? ??࠭

bool wndsplay (WINDOW *pwin, int row, int col);

WINDOW  *pwin   ?????⥫? ?? ????????? ???? WINDOW
int     row     ??ப?  ?????? ???孥?? 㣫? ????
                WN_NATIVE   ??? ?뫮 㪠???? ??? ᮧ?????
                WN_CENTER   ??????஢???? ?? ???⨪???
                WN_CURMNU   ?ਢ離? ? ⥪?饬? ????
                >= 0        ???? ???祭?? row
int     col     ??????? ?????? ???孥?? 㣫? ????
                WN_NATIVE   ??? ?뫮 㪠???? ??? ᮧ?????
                WN_CENTER   ??????஢???? ?? ???⨪???
                WN_CURMNU   ?ਢ離? ? ⥪?饬? ????
                >= 0        ???? ???祭?? col

??室:   YES    ???? ?뢥????
         NO     ?訡??
**********************************************************************/

#include <slwind.h>

static REGION *bordim (REGION *pregion, DIMREC *pdim, BORDER *pborder,
                       LOCATE *pwhere);

bool wndsplay (WINDOW *pwin, int row, int col) {
    REGION occupied;

    if (pwin->options.shown)  return (NO);
    if (wncustom (pwin, row, col) == NO)  return (NO);
    bordim (&occupied, &pwin->img.dim, &pwin->bord, &pwin->where_shown);
    pwin->where_prev = occupied.ul;

    pwin->prev.dim.h = (byte)REGION_H(occupied);
    pwin->prev.dim.w = (byte)REGION_W(occupied);

    if (wnpagadd (pwin) == NULL)  return (NO);
    if (wnunhide (pwin) == NO) { wnpagrem (pwin); return (NO); }
    wnselect (pwin);
    return (YES);
}

/****************************************************************************
    bordim - ????????? ࠧ???? ???? ? ?????஬

REGION *bordim (REGION *pregion, DIMREC *pdim, BORDER *pborder, LOCATE *pwhere);

??室:  ?????⥫? ?? ????????? REGION

*****************************************************************************/

static REGION *bordim (REGION *pregion, DIMREC *pdim, BORDER *pborder,
                                                            LOCATE *pwhere) {
    int     bordertype = pborder->type & BORD_TYPE;
    int     sh_row2;
    int     sh_col2;

	pregion->ul.row = pwhere->row;
	pregion->ul.col = pwhere->col;
	pregion->lr.row = pwhere->row + pdim->h - 1;
	pregion->lr.col = pwhere->col + pdim->w - 1;
    sh_row2 = pregion->lr.row + 1;
    sh_col2 = pregion->lr.col + 2;

    if (bordertype != 0 && pregion->ul.row > 0 && pregion->ul.col > 0 &&
           pregion->lr.row + 1 < NUM_ROWS  && pregion->lr.col + 1 < NUM_COLS) {
        pregion->ul.row--;
        pregion->ul.col--;
        pregion->lr.row++;
        pregion->lr.col++;
        sh_row2++;
        sh_col2++;
	}
    if ((pborder->type&BORD_SHADOW) && sh_row2<NUM_ROWS && sh_col2<NUM_COLS) {
        pregion->lr.row += 1;
        pregion->lr.col += 2;
    }
    return (pregion);
}

