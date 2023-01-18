/**********************************************************************/
/*                                                                    */
/*    SLMOUS.H     Header file for       C TOOLS Mouse  Functions     */
/*                                                                    */
/**********************************************************************/

#ifndef DEF_SLMOUS
#define DEF_SLMOUS

#if __STDC__
#define _Cdecl
#else
#define _Cdecl  cdecl
#endif

#ifndef DEF_SLUTIL
#include <slutil.h>
#endif

/*************************************************************/
/*  Definitions for msptsmsk() (opt)                         */
/*************************************************************/
#define      MS_AND                  0x00
#define      MS_XOR                  0x01

/*************************************************************/
/*  Button status define.   (buttons)                        */
/*************************************************************/
#define      MS_NOT_PRESSED          0x00
#define      MS_LEFT                 0x01
#define      MS_RIGHT                0x02
#define      MS_CENTER               0x04


/*************************************************************/
/*  Definitions for mshandlr() .   (event) mask.             */
/*************************************************************/
#define      MS_NO_EVENT             0x00
#define      MS_MOVE                 0x01
#define      MS_LEFT_PRESS           0x02
#define      MS_LEFT_RELEAS          0x04
#define      MS_RIGHT_PRESS          0x08
#define      MS_RIGHT_RELEAS         0x10
#define      MS_CENTER_PRESS         0x20
#define      MS_CENTER_RELEAS        0x40

/*************************************************************/
/*  Library function prototypes.                             */
/*************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

int     _Cdecl   msinst(void);    /* Returns number of mouse buttons and
                                   * reset driver for text (graphics) use
                                   */

bool    _Cdecl   mshide(void);   /*
                                  * Hide text (graphics) mouse pointer
                                  */

bool    _Cdecl   msunhide(void); /*
                                  * Unhide text (graphics) mouse
                                  * pointer.
                                  */

bool    _Cdecl   msgetpos( int *row, int *col, int *buttons );
                                 /* Returns row, col ( Y, X) mouse
                                  * position and button status, i.e.
                                  * witch buttons pressed at the time
                                  * (see button status define).
                                  */

bool    _Cdecl   mssetpos( int row, int col);
                                 /* Set mouse pointer to
                                  * row, col ( Y, X) position.
                                  */

bool    _Cdecl   msbound( int r1, int c1, int r2, int c2 );
                                 /* Define region for mouse pointer
                                  * motion.
                                  * r1, c1 ( Y1, X1 ) - upper left
                                  * corner of the screen;
                                  * r2, c2 ( Y2, X2 ) - down right
                                  * corner of the screen;
                                  */

bool    _Cdecl   msptsmsk( int bg, int  fg, int symb, int opt );
                                /* Set software mask for mouse pointer
                                 * in TEXT mode only.
                                 * bg - background color mouse marker;
                                 * fg - foreground color mouse marker;
                                 * symb - ascii character that always
                                 * visible, with same color as <fg>, on
                                 * a <bg> color field.
                                 * If symb == 0 than character in a pointer
                                 * will be the same as on the screen at a
                                 * given place and a given time.
                                 * Set mouse pointer to AND or XOR style
                                 * with parameter <opt>, that may be equals
                                 * MS_AND or MS_XOR values.
                                 */

bool    _Cdecl   mspthmsk( int start, int  end);
                                /* Set hardware mask for mouse pointer
                                 * in TEXT mode only.
                                 * start, end - define start, end  scan
                                 * lines of standart text cursor.
                                 */

void (_Cdecl far * _Cdecl mshandlr (int *event, void (far * _Cdecl func)()))();
/*
 * This function sets a mouse interrupt routine <func>, witch
 * called directly from mouse driver ( for example: mouse.com )
 * as far procedure after manipulator used (i.e. move, buttons
 * pressed or released). You may set different condition for
 * <func> call (see event Definitions for mshandlr).
 * Term 'event' is the same as 'manipulator using'.
 *
 * Use:
 *
 * #pragma check_stack( off )
 * #pragma check_pointer( off )
 * void far func ( int event, int butstat,
 *                  int xpos,  int ypos,
 *                  int xdist, int ydist );
 *
 * Mouse driver tells <func>:
 *  -witch event has occured at a time - <event>;
 *  -mouse button status - <butstat>;
 *  -mouse pointer coordinates - row(ypos), col(xpos);
 *  -distances of motion (in mickeys value) since last interrupt
 *   may be negative or pozitive sign.
 * Note: 1 mickey = 1/200th of an inch.
 * Returns:
 *       - old_event_mask to event address.
 *       - old_interrupt_routine_address.
 */

bool (_Cdecl far * _Cdecl msshadow( int *event, bool (far *_Cdecl ShadowFunc)()))();
/*
 * This function adds alternate (shadow) mouse handler
 *  routine which will work with natural (setted by mshandlr())
 *  handler together. It called just before the mshandlr's function.
 *  if ( event == 0 ) the ShadowFunc will be no more called.
 *  The original handler not be called if Shadow function
 *  returns  YES (non zero value).
 * Returns:
 *       - old_event_mask to event address.
 *       - old_interrupt_routine_address.
 */

bool    _Cdecl   msspeed (int horiz, int vert);
                                /* Set determine beetwin display
                                 * pointer motion speed and
                                 * manipulator motion ones.
                                 * Default settings are <horis> = 8,
                                 * <vert> = 16;
                                 */

bool    _Cdecl   msexclus( int r1, int c1, int r2, int c2 );
                                /* Set exclucion area for mouse
                                 * pointer in the motion region
                                 * (see msbound).
                                 */

bool    _Cdecl   msptgmsk( int xhot, int  yhot, int *mask );
/*
 * int mask[32]; - 64 bytes buffer for AND/XOR graphics pointer
 *                  define.
 * First 16 word defines 16x16 bit "AND mask", that is, the background
 *  will shown through werever there is a 1-bit in that data.
 *
 * The second 32 byte set define "XOR mask"; that is after the "AND mask"
 *  has cleared out some of the background, the pixels matching the 1-bit
 *  in this data set are toggled.
 */

bool    _Cdecl  msemukbd (int l_sc, int l_ch,
                          int r_sc, int r_ch,
                          int c_sc, int c_ch);

bool    _Cdecl  msfunkey (int xp, int yp);

#ifdef __cplusplus
}
#endif

#endif
