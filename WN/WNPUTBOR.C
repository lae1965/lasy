/*********************************************************************
    wnputbor - Display a border surrounding a rectangular region.

bool wnputbor (WINDOW *pwin);

WINDOW *pwin    Pointer to WINDOW structure to surround.

Results     YES
            NO

**********************************************************************/

#include <slutil.h>
#include <slwind.h>

static void wnputbox (int boxtype, int att);

#define ATT_SH  Att_User.shdw
#define ATT     (wnscattr(wnscback (tmp_buf[i]),wnscback (ATT_SH)))

static  char  * tmp_buf;
static  int     W, H, WROW, WCOL;

bool wnputbor (WINDOW *pwin) {
    JOINTS    * pjoint;
    BORDER    * pborder = &pwin->bord;
    int         tl_type  = pborder->type & BORD_TITLE;
    int         tlen, col;
    register    i, lim;
    char far  * pbuf, * pscreen;

    tmp_buf = calloc (205, 1);  /*  */

    W = pwin->img.dim.w;             H = pwin->img.dim.h;
    WROW = pwin->where_shown.row;    WCOL = pwin->where_shown.col;

    if ((pborder->type & BORD_TYPE)  != 0       &&
        WROW > 0 && WCOL > 0 && WROW + H < NUM_ROWS && WCOL + W < NUM_COLS) {

        wnputbox ((pborder->type & BORD_TYPE) - 1, pwin->attr.bord);

        for (pjoint = pborder->pjoints; pjoint != NULL; pjoint = pjoint->next){
            pbuf    = (char far *) &(pjoint->jtype);
            pscreen = wnviptrl (pjoint->row + WROW - 1, pjoint->col + WCOL - 1);
            wnvicopy (&pbuf,&pscreen, 1,1,NUM_COLS*2, pwin->attr.bord, Cmd[2]);
        }
        if ((tl_type & (BORD_TLT|BORD_TCT|BORD_TRT))    &&
            (pborder->pttitle != NULL)                  &&
            (tlen = min (W, strlen (pborder->pttitle))) > 0) {

            if (tl_type & BORD_TLT) col = WCOL;
            if (tl_type & BORD_TCT) col = WCOL + (W + 1 - tlen)/2;
            if (tl_type & BORD_TRT) col = WCOL + W - tlen;
            pbuf    = (char far *) pborder->pttitle;
            pscreen = wnviptrl (WROW - 1, col);
            wnvicopy(&pbuf,&pscreen,1,tlen,NUM_COLS*2,pwin->attr.titl, Cmd[2]);
        }
        if ((tl_type & (BORD_BLT|BORD_BCT|BORD_BRT))    &&
            (pborder->pbtitle != NULL)                  &&
            (tlen = min (W, strlen (pborder->pbtitle))) > 0) {

            if (tl_type & BORD_BLT) col = WCOL;
            if (tl_type & BORD_BCT) col = WCOL + (W + 1 - tlen)/2;
            if (tl_type & BORD_BRT) col = WCOL + W - tlen;
            pbuf    = (char far *) pborder->pbtitle;
            pscreen = wnviptrl (WROW + H, col);
            wnvicopy(&pbuf,&pscreen,1,tlen,NUM_COLS*2,pwin->attr.titl, Cmd[2]);
        }
        H += 2; W += 2; WROW--; WCOL--;
    }
    if ((pborder->type & BORD_SBLOCK) == BORD_SBLOCK) {
        if (WROW + H < NUM_ROWS && WCOL + W < NUM_COLS) {
            pscreen = wnviptrl (WROW, WCOL + W);
            wnvicopy (&pscreen, &tmp_buf, H, 1, NUM_COLS * 2, 0, Cmd[6]);
            lim = H * 2;
            for (i = 0; i < lim; i++)
                tmp_buf[i]=(i&0x0001) ? ATT:((i==0 && tmp_buf[i]!='ß') ? 'Ü':'Û');
            wnvicopy (&tmp_buf,&pscreen, H, 1, NUM_COLS * 2, 0, Cmd[1]);
            pscreen = wnviptrl (WROW + H, WCOL + 1);
            wnvicopy (&pscreen, &tmp_buf, 1, W, NUM_COLS * 2, 0, Cmd[6]);
            lim = W * 2;
            for (i = 0; i < lim; i++) tmp_buf[i] = (i & 0x0001) ? ATT : 'ß';
            wnvicopy (&tmp_buf,&pscreen, 1, W, NUM_COLS * 2, 0, Cmd[1]);
        }
    } else if (pborder->type & BORD_SHADOW) {
        if (WROW + H < NUM_ROWS && WCOL + W + 1 < NUM_COLS) {
            if (H == 1) { WROW--; H++; }
            pscreen = wnviptrl (WROW + 1, WCOL + W);
            wnvicopy (NULL, &pscreen, H - 1, 2, NUM_COLS * 2,
                      utnybbyt (wnscback (ATT_SH), wnscfore (ATT_SH)), Cmd[4]);
            pscreen = wnviptrl (WROW + H, WCOL + 2);
            wnvicopy (NULL, &pscreen, 1, W, NUM_COLS * 2,
                      utnybbyt (wnscback (ATT_SH), wnscfore (ATT_SH)), Cmd[4]);
        }
    }
ret:
    free (tmp_buf);
    return (YES);
}

static void wnputbox (int boxtype, int att) {
    static char uplf[] = {'\332', '\326', '\325', '\311'},
                uprt[] = {'\277', '\270', '\267', '\273'},
                lolf[] = {'\300', '\323', '\324', '\310'},
                lort[] = {'\331', '\275', '\276', '\274'},
                horz[] = {'\304', '\315'},
                vert[] = {'\263', '\272'};
    char far  * pscreen;

    tmp_buf[0] = uplf [ boxtype & 3 ];
    utmovsc (&tmp_buf[1], horz [(boxtype >> 1) & 1], W);
    tmp_buf[W + 1] = uprt [ (boxtype >> 1) & 3 ];
    pscreen = wnviptrl (WROW - 1, WCOL - 1);
    wnvicopy (&tmp_buf, &pscreen, 1, W + 2, NUM_COLS * 2, att, Cmd[2]);

    utmovsc  (tmp_buf, vert [boxtype & 1], H);
    pscreen = wnviptrl (WROW, WCOL - 1);
    wnvicopy (&tmp_buf, &pscreen, H, 1, NUM_COLS * 2, att, Cmd[2]);

    utmovsc  (tmp_buf, vert [(boxtype >> 2) & 1], H);
    pscreen = wnviptrl (WROW, WCOL + W);
    wnvicopy (&tmp_buf, &pscreen, H, 1, NUM_COLS * 2, att, Cmd[2]);

    tmp_buf[0] = lolf [((boxtype >> 2) & 2) | (boxtype & 1) ];
    utmovsc (&tmp_buf[1],horz [(boxtype >> 3) & 1], W);
    tmp_buf[W + 1] = lort [ (boxtype >> 2) & 3 ];
    pscreen = wnviptrl (WROW + H, WCOL - 1);
    wnvicopy (&tmp_buf,&pscreen, 1, W + 2, NUM_COLS * 2, att, Cmd[2]);
}

