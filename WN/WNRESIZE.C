#include "language.h"
#include <slwind.h>

#define     R   N_V_STATE.numtextrows - 1
#define     C   N_V_STATE.numtextcols - 1

static PIMAGE      im;
static LOCATE      loc;

static void     getimg   (void);
static void     coverner (void);
static void     putimg   (void);

bool cdecl wnresize (WINDOW *pwin, int dn, int rt, int up, int lf) {
    int         c1,  c2,  r1,  r2,  W,  H;
    int         c1_, c2_, r1_, r2_, W_, H_;
    int         c1__, c2__, r1__, r2__, W__, H__;
    CELLSC    * cell, * cell_, * pc, * pc_;

	/* BORD_SHADOW corectors */
	int		shadow_r, shadow_d;
    register i;
	bool ret;

    if (pwin->options.covered) { wnhidewn (pwin); wnunhide (pwin); }
	shadow_r = ((pwin->bord.type & BORD_SHADOW) ?  2 : 0);
	shadow_d = ((pwin->bord.type & BORD_SHADOW) ?  1 : 0);

	/* old window parameters (previos image) */
	c1  =   pwin->where_prev.col;
	r1  =   pwin->where_prev.row;
	W   =   pwin->prev.dim.w;
	H   =   pwin->prev.dim.h;
	c2  =   c1 + W - 1;
	r2  =   r1 + H - 1;

	/* new window parameters (previos image) */
	c1_ =   c1 + lf;
	r1_ =   r1 + up;
	c2_ =   c2 + rt;
	r2_ =   r2 + dn;

    /* corrections */
	utbound( c1_, 0, C );
	utbound( c2_, 0, C );
	utbound( r1_, 0, R );
	utbound( r2_, 0, R );

    if ((W_=c2_-c1_+1) < (shadow_r+((pwin->bord.type & BORD_TYPE) ? 3 : 1)) ||
         (H_=r2_- r1_+1) < (shadow_d+((pwin->bord.type & BORD_TYPE) ? 3 : 1)))
        return NO;

    /* Allocate memory for new image */
    cell = pwin->prev.pdata;
    cell_= (CELLSC *) calloc( sizeof(CELLSC), (size_t)W_* H_);

    /* Check window for overlap */
    if (r2  >= r1_  &&  r2_ >= r1   &&  c2  >= c1_  &&  c2_ >= c1) {
		/* Overlap */
		/* Copy overlaped part from old to new */
		r1__ = max ( r1, r1_);
		r2__ = min ( r2, r2_);
		c1__ = max ( c1, c1_);
		c2__ = min ( c2, c2_);
		H__ = r2__ - r1__ + 1;
		W__ = c2__ - c1__ + 1;

		pc = &cell[ (r1__- r1)*W + (c1__- c1) ];
		pc_ = &cell_[ (r1__- r1_)*W_ + (c1__- c1_) ];

        for (i = 0; i < H__; i++)
            utmovmem ((char far *) &pc_[i * W_], (char far *) &pc[i * W],
                                                        W__ * sizeof (CELLSC));

		/* Get parts of new image */
		im.dim.w = (byte)W_;
		loc.col  = (byte)c1_;
        if ( r1_ < r1 ) {
			loc.row  = (byte)r1_;
			im.dim.h = (byte)(r1-r1_);
			im.pdata = cell_;
            getimg ();
		}
        if ( r2_ > r2 ) {
			loc.row  = (byte)r2 + 1;
			im.dim.h = (byte)(r2_-r2);
			im.pdata = &cell_[(r2-r1_+ 1)*W_];
            getimg ();
		}
		im.dim.h = 1;
        if ( c1_ < c1 ) {
			im.dim.w = (byte)(c1 - c1_) ;
			im.pdata = &cell_[ ((r1 <= r1_ ) ? 0 : (r1-r1_)) * W_ ];
			loc.col = (byte)c1_;
			loc.row = (byte)r1__;
            for ( i = 0; i < H__; i++, loc.row++, im.pdata += W_ ) getimg ();
		}
        if ( c2_ > c2 ) {
			im.dim.w = (byte)(c2_ - c2 );
			im.pdata = &cell_[ W_ - (c2_ - c2 ) +
				((r1 <=  r1_ ) ? 0 : ((r1-r1_) * W_)) ];
			loc.col = (byte)c2 + 1;
			loc.row = (byte)r1__;
            for (i = 0; i < H__; i++, loc.row++, im.pdata += W_) getimg ();
		}

        /* Put parts of old image */
        if ( r1_ > r1 ) {
			im.dim.w = (byte)W;
			loc.col  = (byte)c1;
			loc.row  = (byte)r1;
			im.dim.h = (byte)(r1_ - r1);
			im.pdata = cell;
            putimg ();

            if ( (pwin->bord.type & BORD_SHADOW) && !rt ) {
				im.pdata = &cell[(int)(im.dim.h+1)*W - 2];
				im.dim.w = 2; im.dim.h = 1;
				loc.col  = (byte)(c2 - 1);
				loc.row	++;
                putimg ();
			}
		}
        if ( r2_ < r2 ) {
			im.dim.w = (byte)W;
			loc.col  = (byte)c1;
			loc.row  = (byte)(r2_+1-shadow_d);
			im.dim.h = (byte)(r2 - r2_ + shadow_d);
			im.pdata = &cell[(r2_-r1-shadow_d+1)*W];
            putimg ();
		}
		im.dim.h = 1;
        if ( c1_ > c1 ) {
			im.dim.w = (byte)(c1_ - c1);
			im.pdata = &cell[ ((r1_ < r1 ) ? 0 : (r1_-r1)) * W ];
			loc.col = (byte)c1;
			loc.row = (byte)r1__;
            for (i = 0; i < H__; i++, loc.row++, im.pdata += W) putimg ();

            if ( (pwin->bord.type & BORD_SHADOW) && !dn ) {
				im.dim.w = (byte)(2 + c1_ - c1);
				im.pdata = &cell[ (H-1) * W ];
				loc.row  = (byte)r2;
                putimg ();
			}
		}
        if ( c2_ < c2 ) {
			im.dim.w = (byte)(c2 - c2_) + shadow_r;
			im.pdata = &cell[ c2_- c1 + 1 - shadow_r +
				 ((r1_ <= r1 ) ? 0 : ((r1_- r1) * W)) ];
			loc.col = (byte)c2_ + 1 - shadow_r;
			loc.row = (byte)r1__;
            for (i = 0; i < H__; i++, loc.row++, im.pdata += W) putimg ();
		}
    } else {
		/* No overlap */
		im.dim.h    = (byte)H_;
		im.dim.w    = (byte)W_;
		im.pdata    = cell_;
		loc.row     = (byte)r1_;
		loc.col     = (byte)c1_;
        getimg ();

		im.dim.h    = (byte)H;
		im.dim.w    = (byte)W;
		im.pdata    = cell;
		loc.row     = (byte)r1;
		loc.col     = (byte)c1;
        putimg ();

	}
    free (cell);

	/* pwin->prev corection */
	pwin->prev.dim.h = (byte)H_;
	pwin->prev.dim.w = (byte)W_;
	pwin->prev.pdata = cell_;
	pwin->where_prev.row = (byte)r1_;
	pwin->where_prev.col = (byte)c1_;

	/* pwin->img and pwin->where_shown corection */

	i = (pwin->bord.type & BORD_TYPE) ?  1 : 0;
	r1__ = r1_ + i;
	c1__ = c1_ + i;
	r2__ = r2_ - shadow_d - i;
	c2__ = c2_ - shadow_r - i;

	H__ = r2__- r1__ + 1;
	W__ = c2__- c1__ + 1;

	cell = pwin->img.pdata;
    cell_= (CELLSC *) calloc (sizeof(CELLSC), (size_t)W__* H__);

    H  = min ((int)pwin->img.dim.h , H__);
	W_ = pwin->img.dim.w;
    W  = min (W_ , W__);

	pwin->img.dim.h = (byte)H__;
	pwin->img.dim.w = (byte)W__;
	pwin->img.pdata = cell_;
	pwin->where_shown.row = (byte)r1__;
	pwin->where_shown.col = (byte)c1__;

    wnmovpca (pwin->img.pdata, ' ', pwin->img.dim.h * pwin->img.dim.w,
                                                         pwin->attr.text);
	/* copy part of previos data*/
	for ( i = 0; i < H; i++ )
        utmovmem ((char far *) &cell_[i * W__], (char far *) &cell[i * W_],
                                                          W * sizeof (CELLSC));

	free(cell);

	loc  = pwin->where_prev;
	im   = pwin->prev;
	pwin->prev.pdata = NULL;
    pwin->options.shown     = 0;
    pwin->options.delayed   = 0;
	pwin->options.removable = 0;
    wnpagrem (pwin);
    ret = wndsplay (pwin, WN_NATIVE, WN_NATIVE);
	pwin->prev = im;
	pwin->where_prev = loc;
	pwin->options.removable = 1;
	wncurmov(0,0);
	coverner();
	return (ret);
}

static void     coverner (void) {
	WINDOW *pw, *pw1;

    if ( Win_List->below == NULL ) return;

	/* First :  uncover all linked windows */
    pw = Win_List;
    do   pw->options.covered = 0;
    while( (pw = pw->below) != NULL );

    /* begin from LAST window (Win_List) properly covered other ones*/
    for ( pw1 = Win_List; pw1->below != NULL; pw1 = pw1->below ) {
        for ( pw = pw1->below; ; pw = pw->below ) {
            if (wnovrlap (&pw1->where_prev, &pw1->prev.dim, &pw->where_prev,
                                          &pw->prev.dim) && pw->options.shown)
                pw->options.covered = 1;
            if (pw->below == NULL)  break;
		}
	}
    return;
}

static void     getimg (void) {
    char far  * pbuf, * pscreen;

    pbuf = (char far *) im.pdata;
    pscreen = wnviptrl (loc.row, loc.col);
    wnvicopy (&pscreen, &pbuf, im.dim.h, im.dim.w, NUM_COLS*2, 0, Cmd[6]);
    return;
}

static void     putimg (void) {
    char far  * pbuf, * pscreen;

    pbuf = (char far *) im.pdata;
    pscreen = wnviptrl (loc.row, loc.col);
    wnvicopy (&pbuf, &pscreen, im.dim.h, im.dim.w, NUM_COLS*2, 0, Cmd[1]);
    return;
}

