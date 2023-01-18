/************************************************************************/
/*                                                                      */
/*             Создание рулонного меню, выбор элемента                  */
/*                                                                      */
/* result = mnrollwin (pwin, max, stl, txt);                            */
/*                                                                      */
/* Вход:    WINDOW  *pwin       Указатель на окно вывода.               */
/*          int     max         Максимальное число элементов меню.      */
/*          int     stl         Начальный элемент при входе в меню      */
/*          char    **txt       Указатель на массив строк меню          */
/*                                                                      */
/* Выход:   int     result      -2      Ошибка                          */
/*                              -1      Выход по ESC                    */
/*                              0,1,... Номер выбранного элемента меню  */
/*                                                                      */
/************************************************************************/

#include <slkeyb.h>
#include <slsymb.def>
#include <slwind.h>
#include <string.h>
#include <slmous.h>

/************************************************************/
/* Macro define.                                            */
/************************************************************/
#define DIS_ITEM(itm)                                         \
    wnwrtbuf(pwin, itm - top, 0, (int)strlen(txt[itm]) > W ? W : 0, \
    txt[itm], WN_NATIVE)

#define ATT_ITEM(itm, attr)                                   \
    wnatrblk(pwin, itm - top, 0, 1, W, attr, WN_UPDATE)

/************************************************************/
/* Static  globals.                                         */
/************************************************************/
static int cur = 0;     /* Current item.                    */
static int top = 0;     /* Item's number at a top of window.*/
static int last = 0;    /* Total item. */
static REGION for_UP;   /* Region where mouse marker shown with up arrow. */
static REGION for_DOWN; /* Region where mouse marker shown with down arrow.*/
static REGION for_WINDOW;/* Region where rolling menu is placed. */
/************************************************************/
/* Static prototypes.                                       */
/************************************************************/
static  bool m_in_rect (byte row, byte col, REGION *preg);
static  bool change_cur (WINDOW *pwin, int stl, char **txt);
static  int  find_item (int ch, char *find, char **txt, int max);
static  int  input (WINDOW *pwin,char **txt);

int _Cdecl mnrolwin (WINDOW *pwin, int max, int stl, char **txt) {
    int             tmp;
    int             H = pwin->img.dim.h, W = pwin->img.dim.w;
    bool            done = NO;
    char            find [79];
    register int    i;
    int             event;
    bool    (far * _Cdecl old_msfunc)();

    *find = EOS;
    event = MS_NO_EVENT;
    old_msfunc = msshadow (&event, NULL);
    if (stl != cur || last != (max - 1) || !stl) {
        cur = top = 0;
        last  = max - 1;
        msptsmsk( SL_WHITE, SL_WHITE, 0, MS_XOR );
        /*
        * Set up  regions.
        */
        for_WINDOW.ul.row = pwin->where_shown.row;
        for_WINDOW.ul.col = pwin->where_shown.col;
        for_WINDOW.lr.row = pwin->where_shown.row+pwin->img.dim.h-(byte)1;
        for_WINDOW.lr.col = pwin->where_shown.col+pwin->img.dim.w-(byte)1;

        for_UP.ul.row = for_WINDOW.ul.row-(byte)1;
        for_UP.ul.col = for_WINDOW.ul.col;
        for_UP.lr.row = for_WINDOW.ul.row-(byte)1;
        for_UP.lr.col = for_WINDOW.lr.col;

        for_DOWN.ul.row = for_WINDOW.lr.row+(byte)1;
        for_DOWN.ul.col = for_WINDOW.ul.col;
        for_DOWN.lr.row = for_WINDOW.lr.row+(byte)1;
        for_DOWN.lr.col = for_WINDOW.lr.col;

        pwin->options.delayed = 1;
        wnscrblk (pwin, 0, 0, H, W, WN_NATIVE, WN_SCR_UP, 0, 0);  /* Clear */
        /* Fill the window with items. */
        for (i = top; i <= H && i < max ; i++) DIS_ITEM (i);
        /*  Highlighte a first item. */
        ATT_ITEM (cur, pwin->attr.high);
        mshide();
        wnupdate (pwin);
        msptsmsk (SL_WHITE, SL_WHITE, 0, MS_XOR);
        msunhide();
        /* Correction of starting line(item). */
        if (utrange (stl, 0, max)) stl = 0;
        change_cur (pwin, stl, txt);
    }

    while (!done) {
        switch (input (pwin, txt)) {
            case Home:  change_cur (pwin, 0, txt);                  break;
            case End:   change_cur (pwin, last, txt);               break;
            case Up:    if (cur > 0) change_cur (pwin, cur - 1, txt); break;
            case Down:
            case Tab: if (cur < last) change_cur (pwin, cur + 1, txt); break;
            case PgUp:
                if (cur > 0) {
                    tmp = cur - H + 1;
                    if (tmp < 0) tmp = 0;
                    change_cur (pwin, tmp, txt);
                }
                break;
            case PgDn:
                if (cur < max - 1) {
                    tmp = cur + H - 1;
                    if (tmp > last) tmp = last;
                    change_cur (pwin, tmp, txt);
                }
                break;
            case Esc:   cur = -1;
            case GrayEnter:
            case Enter: done = YES;    break;

            default:
                if ((i = find_item (Kb_ch, find, txt, max )) == -1) utalarm();
                else  change_cur (pwin, i, txt);
                break;
        }
        /** Destroy a find template. */
        *find = 0;
    }
    msshadow (&event, old_msfunc);
    return (cur);
}

static  bool change_cur (WINDOW *pwin, int new, char **txt) {
    int     off, i;
    int     H = pwin->img.dim.h, W = pwin->img.dim.w;


    if ( new == cur ) return NO;
    ATT_ITEM (cur, pwin->attr.text);

    if ((off = top - new) > 0) {   /** Back move. */
        top -= off;
        off = max(off, H);
        wnscrblk (pwin, 0, 0, H, W, WN_NATIVE, WN_SCR_DOWN, off, 0);
        for (i = new; i < new + off; i++)
            DIS_ITEM (i);
    }  else  if ((off = new - H + 1 - top ) > 0)  {
        top += off;
        off = min (H - 1, off);
        wnscrblk (pwin, 0, 0, H, W, WN_NATIVE, WN_SCR_UP, off, 0);
        for (i = new; i > new - off; i--)  DIS_ITEM (i);
    }
    cur = new;
    ATT_ITEM (cur, pwin->attr.high);
    mshide();
    wnupdate (pwin);
    msunhide();
    return YES;
}

static  int  find_item (int ch, char *find, char **txt, int max) {
    register i;

    if (*find == 0) {
        for (i = 0; i < max; i++) {                     /* First time. */
            if ((char)ch == *txt[i]) {
                *((int *)find) = ch;
                return i;
            }
        }
    } else {
        /*
         * Find buffer does not empty. Attempt to find
         * old template with a new last letter from
         * a current position.
         */
        i = cur;
        if (ch) strcat (find, (char *) &ch);
        else  i++;

        for (; i < max; i++) {
            if (!strncmp (find, txt[i], strlen (find)))  return i;
        }
        if (ch)  find[strlen(find)-1] = 0;     /* Delete last character. */
    }
    /* Find failed. */
    return -1;
}

static int input (WINDOW *pwin, char **txt) {
#define UP_ARROW    0x18
#define DOWN_ARROW  0x19
#define TIMER_TICK()  *(long far *)0x0000046CL
#define TIMEOUT     5       /* Timeout between mouse clicks. */

extern int MS_buttons;
static  long    Last_46C;
    int row, col, buttons, ms_char = 0, fg, bg;
    static int old_ms_char;
    char mouse_state = 0;

    if (MS_buttons)  {
        while (!kbready ())  {
            msgetpos(&row, &col, &buttons);
            /*
             * Adjust software mask of a mouse pointer.
             */
            if (m_in_rect ((byte) row, (byte) col, &for_UP) == YES)
                ms_char = UP_ARROW;
            else if (m_in_rect ((byte) row, (byte) col, &for_DOWN) == YES)
                ms_char = DOWN_ARROW;
            else    ms_char = 0;
            if (old_ms_char != ms_char) {
                old_ms_char = ms_char;
                if (ms_char) {
                    fg = SL_WHITE;
                    bg = SL_BROWN;
                }
                else    bg = fg = SL_WHITE;
                msptsmsk (bg, fg , ms_char, MS_XOR);
            }

            /*
             * Mouse cliks analyzing.
             */
            if (buttons == MS_LEFT || buttons == MS_RIGHT) {
                mouse_state = 1;
                if (ms_char == UP_ARROW)
                        kbsetsym (Up);
                else  if ( ms_char == DOWN_ARROW )
                        kbsetsym (Down);
                else
                if ( m_in_rect( (byte)row, (byte)col, &for_WINDOW) == YES ) {
                    if (change_cur (pwin, row-(int)for_WINDOW.ul.row+top, txt)
                            == NO && (TIMER_TICK() - Last_46C) < TIMEOUT) {
                        if( buttons == MS_LEFT ) kbsetsym (Enter);
                        else                     kbsetsym (Esc);

                        Last_46C = 0L;
                    }
                }
            } /* end if ( buttons == MS_LEFT || buttons == MS_RIGHT) */
            else
                if( mouse_state ) {
                    mouse_state = 0;
                    Last_46C = TIMER_TICK();
                }
        } /* end of while ( !kbready() ) */
    } /* end of if( MS_buttons ) */
    return (kbgetkey (NULL));
}
/*
 * static  bool  m_in_rect( byte row, byte col, REGION *preg);
 *
 *  Check coordinates (row,col) for range.
 *
 * Returns:     NO -    if out of rectangular range,
 *              YES -   if in range.
 */

static  bool m_in_rect( byte row, byte col, REGION *preg) {
    return( (!utrange( row, preg->ul.row, preg->lr.row) &&
             !utrange( col, preg->ul.col, preg->lr.col) ) ? YES : NO );
}
