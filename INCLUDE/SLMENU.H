/********************************************************************/
/*                                                                  */
/*  SLMENU.H     Header file for Turbo C TOOLS Menu Functions       */
/*                                                                  */
/********************************************************************/

#ifndef DEF_SLMENU
#define DEF_SLMENU  1

#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif

#ifndef DEF_SLWIND
#include <slwind.h>
#endif

/********************************************************************/
/* Definitions of data types.                                       */
/********************************************************************/

typedef struct {                /* data structure for user's items  */
    char          * text;       /* Pointer to item string           */
    char          * keys;       /* Pointer to string of keycodes    */
    char          * vars;       /* Pointer to item's var part       */
}   ITEM;

typedef struct mitem {          /* structure for an item in a menu  */
    int             tag;        /* Item's tag recognized by the user*/
    int             prot;       /* 1 - Item is protected            */
    char            row;        /* Coordinates (relative to window) */
    char            col;        /* of menu item text.               */
    char            len;        /* Length of ASCII text of menu item*/
    char            bpos;       /* Position of BOLD char            */
    char          * keys;       /* Pointer to string of keycodes    */
    signed char     vpos;       /* Starting position of var part    */
    char            vlen;       /* Length of var part               */
    char          * vloc;       /* Pointer to the varied string     */
    struct mitem  * prev;       /* Pointer to previous item         */
    struct mitem  * next;       /* Pointer to next item             */
}   MNITEM;                     /* specification in linked list.    */

typedef struct {                /* Data structure of the menu       */
    char        sign[4];        /* Characteristic signature.        */
    byte        type;           /* Type of the menu                 */
    WINDOW    * pwin;           /* The window we use.               */
    MNITEM    * pitems;         /* The items to display in window   */
    MNITEM    * pcur;           /* Currently selected item          */
}   MENU;

typedef struct {
    unsigned nlq           :2;   /* 0 - draft, 1 - roman, 2 - sans serif */
    unsigned italic        :1;   /* 1 - курсив                           */
    unsigned under         :1;   /* 1 - подчеркивание                    */
    unsigned bold          :2;   /* 1 - двойной удар 2 - выделенный      */
/* ───────────────────────────────────────────────────────────────────── */
    unsigned elite         :1;   /* 0 - 10 сим. на дюйм, 1 - 12 сим.     */
    unsigned size          :2;   /* 1 - удвоенный,   2 - уплотненный     */
    unsigned script        :2;   /* 1 - подстрочный, 2 - надстрочный     */
/* ───────────────────────────────────────────────────────────────────── */
    unsigned jst           :3;   /* Выравнивание 0-l, 1-r, 2-c, 3-f      */
    unsigned full          :1;   /* 0 - Пустое поле                      */
    unsigned spec          :1;   /* 1 - команда для засоса               */
} PRNOPT;

typedef struct pagbrk {
    long    line;                /* Строка, с которой начинается страница    */
    bool    fix;                 /* YES - фиксированное начало страницы      */
    struct pagbrk   * prev;      /* Указатель на предыдущую страницу         */
    struct pagbrk   * next;      /* Указатель на следующую страницу          */
} PAGBRK;

typedef struct {
    PAGBRK * firstbrk;             /* Ук. на первый разрыв страницы          */
    int      hei;                  /* Высота листа                           */
    int      wid;                  /* Ширина листа                           */
    int      top;                  /* Расстояние от верха листа до текста    */
    int      bot;                  /* Расстояние от низа листа до текста     */
    int      left;                 /* Расстояние от лева листа до текста     */
    int      right;                /* Расстояние от права листа до текста    */
    int      numcop;               /* Количество копий                       */
    int      tonote;               /* Расстояние от края листа до заголовка  */
    int      linspac;              /* Расстояние между строками              */
    int      copspac;              /* Расстояние между экземплярами          */
    char     pnote [63];           /* Указатель на текст заголовка           */
    PRNOPT   prnopt;               /* Шрифт заголовка                        */
    struct {
        unsigned    note    : 2;   /* Заголовок: 0 - нет, 1-сверху, 2-снизу  */
        unsigned    note1st : 1;   /* 1 - Выводить заголовок на 1-й странице */
        unsigned    ff      : 1;   /* 1 - form feed в конце документа        */
        unsigned    mirror  : 1;   /* 1 - двусторонняя печать                */
        unsigned    number  : 1;   /* 1 - вводить в заголовок номер страницы */
        unsigned    full    : 1;   /* 1 - Документ полностью пом-ся на листе */
        unsigned    newpage : 1;   /* 1 - Каждый экземпляр с нового листа    */
        unsigned    eq      : 1;   /* 1 - Равномерное расп-е экз-в на листе  */
        unsigned    brk     : 1;   /* 1 - Размер страниц определен в brk     */
        unsigned    dummy   : 7;
    } opt;
} PRNPAG;

typedef struct specprn {
    int             autopull;        /* Величина автозаправки в мм           */
    bool            n_216;           /* Минимальный перевод строки n/216     */
} SPECPRN;

/********************************************************************/
/* Constants                                                        */
/********************************************************************/
                                /* Error constants for MENU         */
#define MN_BAD_MENU 100         /* Menu failed                      */
#define MN_NO_SEL   101         /* No selectable item on menu.      */
#define MN_NO_ITEM  102         /* No item exists for given tag     */
#define MN_DUP_ITEM 103         /* Tag already exists in menu       */
#define MN_OVR_ITEM 104         /* Item overlaps other items        */
#define MN_DUP_KEY  105         /* Key already exists in menu       */
#define MN_UNK_KEY  106         /* Key unknown and MN_TRANSMIT opt  */

                                /* Constants for standart MENU type */
#define MN_GRID     0x0000      /* Grid       MENU type             */
#define MN_VERT     0x0001      /* Vertical   MENU type             */
#define MN_HOR      0x00FA      /* Horisontal MENU type             */
                                /* Bit OR with number of items/row  */

#define MN_NOPR     0x0000      /* Noprotected item in the MENU     */
#define MN_PR       0x0001      /* Protected   item in the MENU     */

                                /* Options for use with mnreadop()  */
#define MN_REMOVE   0x0001      /* Remove MENU after select         */
#define MN_DSTROY   0x0002      /* Remove & destroy after select    */
#define MN_TRANSMIT 0x0004      /* Transmit unknown key             */
#define MN_DIS_ESC  0x0008      /* Disable ESC-exit (only select)   */
#define MN_HORINVER 0x0030      /* Horisontal moves in VERT menu    */
#define MN_NOREM    0x0040      /* No remove MENU after ESC-key     */
#define MN_TRAN_ALL 0x0084      /* Transmit all key                 */
#define MN_DIS_DN   0x0100      /* Disable Down-exit in HOR menu    */

#define MN_SIGN     "MNU"       /* Definitive signature for MENU    */
#define MN_VAR_CHAR '@'         /* Define char of var part of item  */

#define mnvalmnu(p)  (p != NULL && strcmp (p->sign, MN_SIGN) == 0)
/********************************************************************/
/* External variables                                               */
/********************************************************************/

extern MENU * Cur_Menu;

#define mnreadit(pmenu)         mnreadop((pmenu),0)

/********************************************************************/
/* User function declarations                                       */
/********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

MENU *  _Cdecl mncreate (int hei,int wid,int row,int col,
                        BORDER *pbor,ATTRIB *patt);
MENU *  _Cdecl mncrestd (ITEM *array, int count, int typ, int row, int col,
                        BORDER *pbor, ATTRIB *patt);
void    _Cdecl mndstroy (MENU *pmenu);
bool    _Cdecl mndsplay (MENU *pmenu, int row, int col);
bool    _Cdecl mnremove (MENU *pmenu);

bool    _Cdecl mnitmadd (MENU *pmenu, int tag, int row, int col, ITEM *pitm);
int     _Cdecl mnreadop (MENU *pmenu, int option);
bool    _Cdecl mndisvar (MENU *pmenu, int tag);
bool    _Cdecl mnsetpro (MENU *pmenu, int tag, int option);
bool    _Cdecl mnsetcur (MENU *pmenu, int tag);
bool    _Cdecl mnmoveit (MENU *pmenu);
void    _Cdecl mngetitm (char *buf, int len);
bool    _Cdecl mnpalett (char *palett);
void    _Cdecl mnupdmnu (ATTRIB *patt, int num, ... );
int     _Cdecl mnwarnin (ITEM *arr, int max, int num, ... );
bool    _Cdecl mndskinp (int row, int col, char *dsk, char *ttl, int num,... );
int     _Cdecl mnharder (ITEM * pitem, int nitem);
bool    _Cdecl mnfnminp (int row, int col, char *ttl, char *fnbuf,
                         char *ext, bool old, bool dir);
int     _Cdecl mnrolmnu (int row, int col,int max, int stl,
                         char **txt, char *ttl,int hei, int pos);
int     _Cdecl mnrolwin (WINDOW *pwin, int max, int stl, char **txt);
int     _Cdecl mngetcod (int row, int col, char *file, char *name, int def);
char *  _Cdecl mngettxt (char *, char *, int);
bool    _Cdecl mngetasc (char * ch, BORDER * bor, ATTRIB * attr);
void    _Cdecl mnprnopt (PRNOPT * pprnopt, char * ttl, ATTRIB * pattr);
bool    _Cdecl mnprnpag (PRNPAG * prnpg, ATTRIB * pattrib, void (* fn) (void),
                                                           long tot, bool prnt);
bool    _Cdecl mnprnpag1 (PRNPAG * prnpg, ATTRIB * pattrib);
void    _Cdecl mnbrkcor  (void);
void    _Cdecl mnmakebrk (long line);
void    _Cdecl mnfrebrk  (PAGBRK ** ppbrk);


/********************************************************************/
/* Internal function declarations                                   */
/********************************************************************/

MNITEM* _Cdecl mnfnditm (MNITEM *h, int tag);
void    _Cdecl mnitmatr (MENU *pmenu, MNITEM *p);
bool    _Cdecl mndisvri (MENU *pmenu, MNITEM *pitem);

#ifdef __cplusplus
}
#endif

extern SPECPRN   specprn;

#endif
