#ifndef DEF_SLEDIT
#define DEF_SLEDIT     1

#include <slwind.h>
#include <slkeyb.h>
#include <slldraw.h>

#define     BUF_WRK     9216
#define     TAB_SIZE    4
#define     MOVLEFT(i)  movleft(pe,&pe->line,(i));
#define     MOVRIGHT(i) movright(pe,&pe->line,(i));
#define     WORKING     utworkin ()

#define     NEED_OUTSCR 1
#define     NEED_OUTLIN 2

typedef  struct {            /**       ?????????         ?????              **/
    long     start;          /* ????? ??ࢮ? ??ப? ?????                    */
    long     nline;          /* ???????⢮  ??ப ? ?????                    */
    long     lsize;          /* ?????? ????? ? ????? ᢮?. 䠩??             */
    long     loff;           /* ???饭?? ?? ??砫? ????? ? ???.䠩??         */
    long     rsize;          /* ?????? ????? ? ?ࠢ?? ᢮?. 䠩??            */
    int      nbank;          /* ????? ???樨, ? ???.????? ????? ? ?ࠢ.䠩?? */
    int      roff;           /* ???饭?? ?? ????? ????? ? ???樨             */
} BLK;

typedef struct edt {
    bool         infempty;       /* NO  - ????, ??? ?????? ?? ?ਣ?????   */
    int          handinf;        /* Handle ?ਣ?????                      */
    long         infoff;         /* ???????? 㪠??⥫? ?室???? 䠩??     */

    int          handleft;       /* Handle 䠩?? ᢮????? "????"          */
    long         leftoff;        /* ???????? 㪠??⥫? ᫥??              */

    int          handright;      /* Handle 䠩?? ᢮????? "?ࠢ?"         */
    int          nbank;          /* ???????⢮ ?????? ? 䠩?? ??ࠢ?      */
    int          rightoff;       /* ???????? 㪠??⥫? ? ????? ??ࠢ?     */

    bool         block;          /* YES - ???? ????祭                    */
    bool         blkmark;        /* YES - ???? ?⬥砥???                 */

    BLK          cur;            /* ????騩  ????                         */
    BLK          old;            /* ??????  ????                          */

    bool         insert;         /* YES - ??⠢??                         */
    int          visicol;        /* ??????? ? ????ன ??ப? ????? ? ???? */
    int          row;            /* ??????? ??ப? ? ????                 */
    int          col;            /* ??????? ??????? ? ????                */
    int          blank;          /* ???????⢮ ?????? ?஡????            */
    int          lf;             /* ???????⢮ ?????? ??ப               */
    long         line;
    long         total;
    bool         single;
    bool         changed;

    struct edt  *next;
    struct edt  *prev;

    int          needout;
    int          numwind;
    char         fname[60];
} EDT;


extern WINDOW     * Ed_pstat, * Ed_pwin;
extern char       * Ed_ppl;         /* ??. ?? ????? "᫥??"                  */
extern char       * Ed_pl;          /* ??. ?? ????? "᫥??"                  */
extern word       * Ed_pnl;         /* ??. ?? ?????? ⥪?饩 ??ப?          */
extern word       * Ed_pnllast;     /* ??. ?? ?????? ??ࢮ? ??ப? ? ??????  */
extern char       * Ed_pleft;       /* ??. ?? ᨬ??? ᫥?? ?? ??થ??        */
extern word         Ed_lsize;       /* ?????? ?????? ??????                  */
extern char       * Ed_ppr;         /* ??. ?? ????? "??ࠢ?"                 */
extern char       * Ed_pr;          /* ??. ?? ????? "??ࠢ?"                 */
extern word       * Ed_pnr;         /* ??. ?? ?????? ⥪?饩 ??ப?          */
extern word       * Ed_pnrlast;     /* ??. ?? ?????? ??ࢮ? ??ப? ? ??????  */
extern char       * Ed_pright;      /* ??. ?? ᨬ??? ??ࠢ? ?? ??થ??       */
extern word         Ed_rsize;       /* ?????? ?ࠢ??? ??????                 */
extern char       * Ed_ppw;         /* ??. ?? ࠡ?稩 ?????                  */
extern char       * Ed_pw;          /* ??. ?? ࠡ?稩 ?????                  */
extern bool         Ed_lbufempty;
extern bool         Ed_rbufempty;

int     edprewrk    (int ht,void far *t,int b,int hf,void far *f,long l,
                                                    word fs, long * total);
void    statupd     (EDT *pe);
void    viroll      (int dir);
int     edtrec      (WINDOW *pwin, char * inf);
void    paintlin    (EDT *pe);
void    outlin      (EDT *pe);
void    setsym      (EDT *pe, char sym, int count);
EDT    *iniedt      (EDT *pe, char *inf);
EDT    *kcw         (EDT *pe);
EDT    *otherwind   (EDT *pe, bool next, bool altf1);
void    swapright   (EDT *pe, bool all);
void    swapleft    (EDT *pe, bool all);
bool    loadright   (EDT *pe);
bool    loadleft    (EDT *pe);
void    kcy         (EDT *pe);
void    kright      (EDT *pe);
void    kcright     (EDT *pe);
void    ksright     (EDT *pe);
void    kleft       (EDT *pe);
void    kcleft      (EDT *pe);
void    ksleft      (EDT *pe);
void    ksdown      (EDT *pe);
void    kdown       (EDT *pe);
void    kpgdn       (EDT *pe);
void    kcpgdn      (EDT *pe);
void    kup         (EDT *pe);
void    ksup        (EDT *pe);
void    kpgup       (EDT *pe);
void    kcpgup      (EDT *pe);
void    khome       (EDT *pe);
void    kchome      (EDT *pe, bool total);
void    kdel        (EDT *pe);
void    kcend       (EDT *pe);
void    kend        (EDT *pe);
bool    kaltx       (EDT *pe);
void    kck         (EDT *pe);
void    kf7         (EDT *pe);
void    kbs         (EDT *pe);
void    kenter      (EDT *pe);
void    outscr      (EDT *pe);
void    kcf9        (EDT *pe);
void    kf10        (EDT *pe);
void    kcf10       (EDT *pe, BLK *blk, bool move, bool interwin);
int     kf9         (EDT *pef, EDT *pet, bool move);
int     foundblk    (EDT *pe, BLK *blk);
int     pnllen      (word i);
int     pnrlen      (word i);
char    setld       (EDT *pe, bool need);
void    blkcorrect  (EDT *pe);
void    blankclr    (EDT *pe);
void    wrtort      (EDT *pe, char *from, char *pcur, word count);
bool    movleft     (EDT *pe, long far *line,   long count);
bool    movright    (EDT *pe, long far *line,   long count);
void    kf2         (EDT *pe);
int     scanlf      (char far *p, unsigned count);
void    outpim      (char far *to, char far *from, int attr, int num, int w);
void    outcur      (char far *to, char attr, int visicol, int w);
int     loadlt      (char far **p, char far *pl, char far **q, long leftoff,
                            long far *offset, char far **savplf, char far *pw,
                                            word num, word frsize, int first);

int     loadrt      (char far **p, char far *pr, char far **q, int far *nbank,
                            int far *rightoff, char far **savplf, char far *pw,
                            int num, unsigned frsize, int newbank, int newoff);
void    mkstatln    (void);

#endif
