/**********************************************************************
    wndsplay - Выводит окно на экран

bool wndsplay (WINDOW *pwin, int row, int col);

WINDOW  *pwin   Указатель на структуру окно WINDOW
int     row     Строка  левого верхнего угла окна
                WN_NATIVE   Как было указано при создании
                WN_CENTER   Центрирование по вертикали
                WN_CURMNU   Привязка к текущему меню
                >= 0        Само значение row
int     col     Колонка левого верхнего угла окна
                WN_NATIVE   Как было указано при создании
                WN_CENTER   Центрирование по вертикали
                WN_CURMNU   Привязка к текущему меню
                >= 0        Само значение col

Выход:   YES    Окно выведено
         NO     Ошибка
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
    bordim - Вычисляет размеры окна с бордюром

REGION *bordim (REGION *pregion, DIMREC *pdim, BORDER *pborder, LOCATE *pwhere);

Выход:  Указатель на структуру REGION

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