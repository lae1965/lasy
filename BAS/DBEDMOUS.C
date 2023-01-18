/*****************************************************************************
    Обрабатывает мышиные прерывания;

    Производит стандартное хождение по документу, помещая в буфер
    клавиатуры навигационные клавиши.
    По левой кнопке помещается клавиша SpecK1.

bool far _Cdecl dbedmous (int ev, int bs, int xp, int yp, int xd, int yd);

    Параметры и код возврата значения не имют;


******************************************************************************/

#include    <sldbas.h>
#include    <slkeyb.h>
#include    <slmous.h>
#include    <slsymb.def>

#pragma warn -par
bool far _Cdecl dbedmous (int ev, int bs, int xp, int yp, int xd, int yd) {
    int         up    = Cur_Edt->pwin->where_shown.row;
    int         down  = up + Cur_Edt->pwin->img.dim.h - 1;
    int         left  = Cur_Edt->pwin->where_shown.col;
    int         right = left + Cur_Edt->pwin->img.dim.w - 1;


    if (!Cur_Edt->opt.noflush) {
        kbflushb ();
        Kb_2_tail = Kb_2_head;
    }
    switch (ev) {
        case MS_LEFT_PRESS:
            Db_MousX = xp - left;
            Db_MousY = yp - up;
            kbsetsym (SpecK1);
            break;
        case MS_RIGHT_RELEAS:
            Db_MousMove = NO;
            break;
        case MS_RIGHT_PRESS:
            if (Cur_Edt->opt.move) {
                if (yp == up)               kbsetsym (ShiftUp);
                else if (yp == down)        kbsetsym (ShiftDown);
                if (xp == left)             kbsetsym (ShiftLeft);
                else if (xp == right)       kbsetsym (ShiftRight);
                Db_MousMove = YES;
            }
            break;
        default :
            Db_MousMove = NO;
            if (utrange (yp, up, down) || utrange (xp, left, right)) {
                utbound (yp, up, down);
                utbound (xp, left, right);
                mssetpos (yp, xp);
            }
            if (yp == up)            msptsmsk (SL_RED,   SL_BLACK, 24, MS_XOR);
            else if (yp == down)     msptsmsk (SL_RED,   SL_BLACK, 25, MS_XOR);
            else if (xp == left)     msptsmsk (SL_RED,   SL_BLACK, 27, MS_XOR);
            else if (xp == right)    msptsmsk (SL_RED,   SL_BLACK, 26, MS_XOR);
            else                     msptsmsk (SL_WHITE, SL_WHITE,  0, MS_XOR);
            break;
    }
    return NO;
}
#ifdef TC_lang
#pragma warn +par
#endif

