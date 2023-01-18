/*                                                                    */
/*    SLWIND.H     Header file for Turbo C TOOLS Window Functions     */
/*                                                                    */
/**********************************************************************/
#ifndef DEF_SLWIND
#define DEF_SLWIND   1

#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif

#include <slutil.h>

/********************************************************************/
/* Definitions of data types.                                       */
/********************************************************************/

typedef struct {                /* Filled by wniniwin() function    */
    short   numtextcols;        /* number of text columns available */
    short   numtextrows;        /* number of text rows available    */
    short   numcolors;          /* number of actual colors          */
    short   numvideopages;      /* number of available video pages  */
    short   mode;               /* current video mode               */
    short   adapter;            /* active display adapter           */
    short   monitor;            /* active display monitor           */
    short   memory;             /* adapter video memory in K bytes  */
    short   byte_char;          /* Bytes per character              */
}   VIDEO_STATE;

typedef struct {
    short   known;              /* 1 - fill VIDEO_STATE             */
    short   n_a_page;           /* currently ACTIVE video page      */
    short   n_v_page;           /* currently VISUAL video page      */
}   CURNT_STATE;

typedef struct {                /* CURTYP structure:                */
    byte    high;               /* high scan line in a cursor       */
    byte    low;                /* low  scan line in a cursor       */
}   CURTYP;

typedef struct {                /* LOCATE structure                 */
    byte    row;                /* row and column relative to a     */
    byte    col;                /* rectangular region.              */
}   LOCATE;

typedef struct {                /* REGION structure                 */
    LOCATE  ul;                 /* Upper left  corner.              */
    LOCATE  lr;                 /* Lower right corner.              */
}   REGION;

typedef struct {                /* CELLSC structure                 */
    char    ch;                 /* character on a PC screen         */
    char    at;                 /* attribute of char on a PC screen */
}   CELLSC;

typedef struct {                /* DIMREC structure                 */
    byte    h;                  /* height rectangular region.       */
    byte    w;                  /* width  rectangular region.       */
}   DIMREC;

typedef struct {                /* PIMAGE structure                 */
    DIMREC  dim;                /* Dimensions of the rectangle.     */
    CELLSC *pdata;              /* Pointer to array of cells        */
}   PIMAGE;

typedef struct joints {         /* JOINTS structure                 */
    byte    row;                /* Row and column of joint relative */
    byte    col;                /* to upper-left of border.  (0,0). */
    char    jtype;              /* IBM graphic char for joint.      */
    char    _dummy;             /* Structure alignment.             */
    struct  joints  *next;      /* Pointer to next joint in linked  */
}   JOINTS;                     /* list of joints.                 */

typedef struct {                /* BORDER structure                 */
    int     type;               /* Type of border (see below)       */
    char   *pttitle;            /* Title on top border.             */
    char   *pbtitle;            /* Title on bottom border.          */
    JOINTS *pjoints;            /* Pointer to linked list of joints.*/
}   BORDER;

typedef struct {                /* ATTRIB structure                 */
    byte    text;               /* normal text attribute            */
    byte    bord;               /* border attribute                 */
    byte    titl;               /* border title attribute           */
    byte    high;               /* highlight attribute       (MENU) */
    byte    prot;               /* protection item attribute (MENU) */
    byte    bold;               /* bold char attribute       (MENU) */
    byte    err;                /* error attribute                  */
    byte    shdw;               /* shadow attribute                 */
    byte    blck;               /* block attribute                  */
    byte    afld;               /* active field attribute           */
    byte    nfld;               /* not active field attribute       */
    byte    nice;               /* reserve attribute                */
}   ATTRIB;

typedef struct window {         /* WINDOW structure                 */
    char    sign[4];            /* Identifying signature.           */
    LOCATE  cur_loc;            /* Relative location of own cursor  */
    CURTYP  cur_typ;            /* Cursor type.                     */
    LOCATE  where_shown;        /* Where window is currently shown  */
    PIMAGE  img;                /* Contents of data area.           */
    LOCATE  where_prev;         /* Region occupied by window        */
    PIMAGE  prev;               /* Previous contents of screen      */
    BORDER  bord;               /* Border description.              */
    ATTRIB  attr;               /* Default attributes               */
    struct {                    /* "options" substructure:          */
        unsigned shown      :1; /* Window is shown on the screen    */
        unsigned delayed    :1; /* Output postponed until update    */
        unsigned removable  :1; /* Removable window                 */
        unsigned cur_on     :1; /* Cursor visible.                  */
        unsigned dirty      :1; /* Output request waits update      */
        unsigned covered    :1; /* Some of data area is covered     */
        unsigned linked     :1; /* Window is a member of linked list*/
        unsigned temp_hid   :1; /* Window is a temporary hidden     */
        unsigned reserved   :8; /* Reserved for future use.         */
    } options;
    struct window * above;      /* Window above this (top -- NULL)  */
    struct window * below;      /* Window below this (bot -- NULL)  */
}   WINDOW;

/********************************************************************/
/* Constants for attributes                                         */
/********************************************************************/

enum ATTCOL {
    SL_BLACK       = 0x00,
    SL_BLUE        = 0x01,
    SL_GREEN       = 0x02,
    SL_CYAN        = 0x03,
    SL_RED         = 0x04,
    SL_MAGENTA     = 0x05,
    SL_BROWN       = 0x06,
    SL_WHITE       = 0x07,
    SL_INTENSITY   = 0x08,
    SL_UNDERLINE   = 0x01,
    SL_NORMAL      = 0x07,
    SL_REVERSE     = 0x70,
    SL_MONOBLINK   = 0x80
};

/********************************************************************/
/* Constants for wniniwin()     Text modes                          */
/********************************************************************/

#define _PREVIOS   -1   /* Restor previos state of adapter  */
#define _TEXTBW40   0   /* 40-column text, 16 grey          */
#define _TEXTC40    1   /* 40-column text, 16/8 color       */
#define _TEXTBW80   2   /* 80-column text, 16 grey          */
#define _TEXTC80    3   /* 80-column text, 16/8 color       */
#define _TEXTMONO   7   /* 80-column text, BW               */

/********************************************************************/
/* Constants for wniniwin()     Graphics modes for future used      */
/********************************************************************/

#define _MRES4COLOR     4   /* 320 x 200, 4 color           */
#define _MRESNOCOLOR    5   /* 320 x 200, 4 grey            */
#define _HRESBW         6   /* 640 x 200, BW                */
#define _HERCMONO       8   /* 720 x 348, BW for HGC        */
#define _MRES16COLOR    13  /* 320 x 200, 16 color          */
#define _HRES16COLOR    14  /* 640 x 200, 16 color          */
#define _ERESNOCOLOR    15  /* 640 x 350, BW                */
#define _ERESCOLOR      16  /* 640 x 350, 4 or 16 color     */
#define _VRES2COLOR     17  /* 640 x 480, BW                */
#define _VRES16COLOR    18  /* 640 x 480, 16 color          */
#define _MRES256COLOR   19  /* 320 x 200, 256 color         */
#define _ORESCOLOR      64  /* 640 x 400, 1 of 16 colors (Olivetti) */

/********************************************************************/
/* Constants for wniniwin()     Number of rows                      */
/********************************************************************/

#define _MINROWS        25  /* 25-rows text for all adapters*/
#define _MIDROWS        43  /* 43-rows text for EGA/VGA     */
#define _MAXROWS        50  /* 50-rows for VGA              */

/********************************************************************/
/* Constants for wniniwin()     Active adapter                      */
/********************************************************************/

#define _CGA            0x0002  /* Color Graphics Adapter             (CGA)  */
#define _EGA            0x0004  /* Enhanced Graphics Adapter          (EGA)  */
#define _VGA            0x0008  /* Video Graphics Array               (VGA)  */

/********************************************************************/
/* Constants for wniniwin()     Monitor values                      */
/********************************************************************/

#define _MONO           0x0001  /* Monochrome */
#define _COLOR          0x0002  /* Color (or Enhanced emulating color) */
#define _ENHCOLOR       0x0004  /* Enhanced Color */
#define _ANALOGMONO     0x0008  /* Analog Monochrome only */
#define _ANALOGCOLOR    0x0010  /* Analog Color only */
#define _ANALOG         0x0018  /* Analog Monochrome and Color modes */

/********************************************************************/
/* Other constants                                                  */
/********************************************************************/

#define WN_SIGN         "WIN"
#define WN_BUFSIZ       1024
#define WN_V_INT        0x10

/********************************************************************/
/* Constants (type of borders and title(s)) for wnputbor()          */
/********************************************************************/

#define BORD_NO         0x0000
#define BORD_NO_BORDER  0x0000
#define BORD_SHADOW     0x0800
#define BORD_SBLOCK     0x1800

#define BORD_TITLE      0x07E0
#define BORD_TLT        0x0020
#define BORD_TCT        0x0040
#define BORD_TRT        0x0080
#define BORD_BLT        0x0100
#define BORD_BCT        0x0200
#define BORD_BRT        0x0400

#define BORD_TYPE       0x001F
#define BORD_SSSS       0x0001
#define BORD_SSSD       0x0002
#define BORD_SSDS       0x0003
#define BORD_SSDD       0x0004
#define BORD_SDSS       0x0005
#define BORD_SDSD       0x0006
#define BORD_SDDS       0x0007
#define BORD_SDDD       0x0008
#define BORD_DSSS       0x0009
#define BORD_DSSD       0x000A
#define BORD_DSDS       0x000B
#define BORD_DSDD       0x000C
#define BORD_DDSS       0x000D
#define BORD_DDSD       0x000E
#define BORD_DDDS       0x000F
#define BORD_DDDD       0x0010
        /*   BRTL */

/********************************************************************/
/* Constants for wnsplitw()                                         */
/********************************************************************/

#define WN_HSPLIT   0x0000
#define WN_VSPLIT   0x0002
#define WN_SINGLE   0x0000
#define WN_DOUBLE   0x0001

/********************************************************************/
/* Constants (direction of scroll) for wnscrblk()                   */
/********************************************************************/

#define WN_SCR_UP       0
#define WN_SCR_DOWN     1
#define WN_SCR_RIGHT    2
#define WN_SCR_LEFT     3

/********************************************************************/
/* Constants for cursor size                                        */
/********************************************************************/

#define WN_CUR_NAD      0       /* Use requested cursor scan lines  */
#define WN_CUR_ADJ      1       /* Adjust cursor size into normal   */

/********************************************************************/
/* Constants for wndsplay()                                         */
/********************************************************************/

#define WN_NATIVE       -1
#define WN_CENTER       -2
#define WN_CURMNU       -3

#define WN_UPDATE       0x00
#define WN_NO_UPDATE    0x08

/********************************************************************/
/* Global variables declarated in wniniwin() function               */
/********************************************************************/

extern  VIDEO_STATE N_V_STATE;      /* Characteristics of adapter   */
extern  CURNT_STATE n_w_envr;       /* Current window environment   */
extern  WINDOW    * Win_List;       /* Pointer to WINDOW list       */
extern  WINDOW    * Cur_Wind;       /* Window selected for I/O      */
extern  WINDOW    * Lst_Wind;       /* Window last selecterd        */
extern  int         Cmd[13];        /* Commands for wnwicopy ()     */
extern  int         Err_Val;
extern  int         Err_Disk;
extern  int         Err_Hard;
extern  char        Pal_Stnd [17], Pal_Cur [17];

extern  ATTRIB      Att_Wind, Att_User, Att_Work, Att_Chlp, Att_Roll, Att_Edit;
extern  ATTRIB      Att_Khlp, Att_Mnu1, Att_Mnu2, Att_Disk, Att_Warn, Att_Tree;

/********************************************************************/
/* Windows Macros                                                   */
/********************************************************************/

#define REGION_H(region)    ((region).lr.row - (region).ul.row + 1)
#define REGION_W(region)    ((region).lr.col - (region).ul.col + 1)
#define NUM_COLS            (N_V_STATE.numtextcols)
#define NUM_ROWS            (N_V_STATE.numtextrows)

#define wnislink(pwin)      ((pwin)->options.linked)
#define wnis_ega()          (N_V_STATE.adapter & _EGA)
#define wnis_vga()          (N_V_STATE.adapter & _VGA)
#define wnishigh()          (N_V_STATE.adapter > _CGA)
#define wnscfore(attr)      ((attr) & 0x000F)
#define wnscback(attr)      (((attr) >> 4) & 0x000F)
#define wnscattr(back,fore) ((((back) & 0x000F) << 4) | ((fore) & 0x000F))
#define wncurpos(r,c)       {r=Cur_Wind->cur_loc.row;c=Cur_Wind->cur_loc.col;}
#define wnvalwin(p)         (p != NULL && strcmp (p->sign,WN_SIGN) == 0)
#define wnclrmsg(p,r,c,l,a) (wnmovpca((p)->img.pdata+(r)*(p)->img.dim.w+c,' ',l,a)

/********************************************************************/
/* User functions declarations by category                          */
/********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

        /* Initializing, Creation, Destruction & Displaying/Removing */

int     _Cdecl wniniwin (int mode, int rows);
void    _Cdecl wnfinwin (void);
WINDOW *_Cdecl wncreate (int hei,int wid,int row,int col,BORDER *pb,ATTRIB *pa);
void    _Cdecl wndstroy (WINDOW *pwin);
bool    _Cdecl wnremove (WINDOW *pwin);
bool    _Cdecl wndsplay (WINDOW *pwin, int row, int col);

        /* Border manipulation */

bool    _Cdecl wnputbor (WINDOW *pwin);
bool    _Cdecl wnsplitw (WINDOW *pwin, int pos, int option);
void    _Cdecl wnaddjoi (WINDOW *pwin, int row, int col, int sym);
void    _Cdecl wnundjoi (WINDOW *pwin);

        /* WORKING manipulation */

WINDOW *_Cdecl wndiswrk (long wrk, int num, ...);
void    _Cdecl wnupdwrk (long);
void    _Cdecl wnremwrk (void);

        /* Cursor action */

bool    _Cdecl wncursor (WINDOW *pwin);
void    _Cdecl wncurmov (int row, int col);
void    _Cdecl wntogcur (bool insert, bool visi);

        /* HELP manipulation */

void    _Cdecl wngethlp (char *fname, char *topic);
void    _Cdecl wnsethlp (char *fname);
void    _Cdecl wncurhlp (char *topic);
WINDOW *_Cdecl wnkeyhlp (char **txt);
WINDOW *_Cdecl wnfunhlp (char **txt);
WINDOW *_Cdecl  wnfunkey (void (* fn)(void), bool show);

        /* Attribute manipulation. */

void    _Cdecl wnatrblk (WINDOW *pwin,int r1,int c1,int h,int w,int at,int op);
void    _Cdecl wnsetpal (char *ptable);
void    _Cdecl wnblinks (int option);
bool    _Cdecl wncolors (byte *pcol, char *titl, void (* fn)(void));

        /* Window scrolling. */

bool    _Cdecl wnscrblk (WINDOW *pwin,int r1,int c1,int h,int w,int at,int dir,int cnt,int op);

        /* Window input / output */

int     _Cdecl wnwrtbuf (WINDOW *pwin,int r,int c,int num,char *buf,int at);
bool    _Cdecl wnstrinp (int row, int col, char *ttl, char *buf, int max);
bool    _Cdecl wnstredt (WINDOW *pw,int r,int c,int ta,int ha,char *p,int max);
bool    _Cdecl wnnuminp (int row,int col,char *ttl,long *pnum,long min,long max);
bool    _Cdecl wndblinp (int row,int col,char *ttl,double *pnum,double min,double max);
WINDOW *_Cdecl wndistxt (int row,int col,int hei,int wid,char **txt,char *ttl,ATTRIB *patt);
WINDOW *_Cdecl wndismrk (int row,int col,char **txt,int opt);
void    _Cdecl wndisfon (char ch,int at);
void    _Cdecl wnmovpsa (CELLSC far * to, void far * from, int count, byte at);
void    _Cdecl wnmovpca (CELLSC far * to, char ch, int count, byte at);
void    _Cdecl wnmovpa  (CELLSC far * to, int count, byte at);
void    _Cdecl wnmovpc  (CELLSC far * to, char ch, int count);
void    _Cdecl wnmovps  (CELLSC far * to, void far * from, int count);
void    _Cdecl wnmovsp  (void far * to, CELLSC far * from, int count);

        /* Other functions */

void    _Cdecl wnselect (WINDOW *pwin);
bool    _Cdecl wnresize (WINDOW *pwin, int dn, int rt, int up, int lf);
bool    _Cdecl wnmovwin (WINDOW *pwin, int rowu, int coll, int rowd,int colr);
bool    _Cdecl wnredraw (void);
bool    _Cdecl wnupdate (WINDOW *pwin);

/********************************************************************/
/* Internal functions                                               */
/********************************************************************/

void    _Cdecl wnloafnt (int video_card);
void    _Cdecl wncoverw (WINDOW *pwind);
bool    _Cdecl wncustom (WINDOW *pwin, int row, int col);
bool    _Cdecl wnhidewn (WINDOW *pwin);
bool    _Cdecl wnunhide (WINDOW *pwin);
bool    _Cdecl wnpagrem (WINDOW *pwin);
WINDOW *_Cdecl wnpagadd (WINDOW *pwin);
bool    _Cdecl wnupdblk (WINDOW *pwin, int r1, int c1, int hei, int wid);
bool    _Cdecl wnovrlap (LOCATE *pcorner1,DIMREC *pdim1,LOCATE *pcorner2,DIMREC *pdim2);
int     _Cdecl wngetcur (int *prow,int *pcol,int *phigh,int *plow);
int     _Cdecl wnsetcur (int row,int col);
int     _Cdecl wnsizcur (int on, int high, int low, int adjust);
bool    _Cdecl wnvicopy (char far **ppfrom,char far **ppto,int num_rows,
                         int row_length,int scn_width,int attr,int opt);
int     _Cdecl wnviroll (int num_rows,int attr,int u_row,int u_col,int l_row,int l_col,int dir);
int     _Cdecl wnvihorz (int num_cols,int attr,int u_row,int u_col,int l_row,int l_col,int dir);
char    _Cdecl wnscread (int *pfore, int *pback);
void    _Cdecl wnscrbar (WINDOW * pwin, bool right, long cur, long total);
char far *_Cdecl wnviptrl (int row, int col);

#ifdef __cplusplus
}
#endif

#endif
