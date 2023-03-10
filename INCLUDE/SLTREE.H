/**********************************************************************/
/*                                                                    */
/*    SLTREE.H     Header file for Turbo C TOOLS SLTree  Functions    */
/*                                                                    */
/**********************************************************************/
#ifndef DEF_SLTREE
#define DEF_SLTREE

#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif

#include <slmenu.h>

/********************************************************************/
/* Definitions of data types.                                       */
/********************************************************************/

#define TREE_TAB              5


typedef struct slnode {         /* ????????? 㧫? ??ॢ? ?????娨   */
    char          * name;       /* ?????⥫? ?? ??ப? ????????     */
    word            nsons;      /* ??᫮   ?뭮???     㧫?         */
    int             level;      /* ?஢??? ??????????? ? ??ॢ?     */
	dword           pmask;      /* ??᪠ ??⨢??? ?।?????? ?஢???*/
    struct slnode * father;     /* ?????⥫? ?? த?⥫?            */
    struct slnode * prev;       /* ?????⥫? ?? ?।. 㧥?          */
    struct slnode * next;       /* ?????⥫? ?? ᫥?. 㧥?          */

    struct  {
        unsigned last    :  1;  /* ??᫥?.㧥? ? ᯨ᪥ ? த?⥫?  */
        unsigned lock    :  1;  /* ????祭??? 㧥?                  */
        unsigned fld     :  1;  /* YES ?᫨ ⥪?騩 㧥? - ????     */
        unsigned dummy   : 13;  /* ??????                           */
    } opt;

    void          * puser;      /* ??. ?? ??????? ?????????         */
} SLNODE;

typedef struct {                /* ?।?⠢????? ??ॢ? ?????娨    */
    int             namlen;     /* ????? ????? 㧫?                 */
    int             total;      /* ???????⢮ 㧫?? ??ॢ?          */
    int             topnum;     /* ????? ???孥?? 㧫? ??࠭?       */
    int             row;        /* ????? ⥪?饩 ??ப? ? ????      */
    int             col;        /* ????? ⥪?饩 ??????? ? ????     */
    int             tab;        /* ?????? ????㯠 ?஢???           */
    int             minlev;     /* ??????????? ?஢??? ??????? ?????*/
    WINDOW        * pwin;       /* ????, ? ????஬ ??ॢ? ????????  */
    SLNODE        * root;       /* ?????⥫? ?? ??୥??? 㧥?       */
    SLNODE        * cur;        /* ?????⥫? ?? ⥪?騩 㧥?        */
    SLNODE        * ice;        /* ?????⥫? ?? ????஦????? 㧥?   */
    SLNODE        * top;        /* ?????⥫? ?? ???孨? 㧥? ??࠭? */
    SLNODE        * prevtop;    /* ?????⥫? ?? ???孨? 㧥? ??࠭? */
} SLTREE;


#define killice(ptree)           (ptree)->ice = NULL
#define makeice(ptree)           (ptree)->ice = (ptree)->cur


/********************************************************************/
/* User functions declarations by category                          */
/********************************************************************/

#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************************/
/* Functions using for SLTREE                                       */
/********************************************************************/
SLTREE * _Cdecl trcreate (int namlen, int H, int W, ATTRIB * att, BORDER * bor);
void     _Cdecl trdstroy (SLTREE * ptree, bool (* fn) (SLNODE * pnode));
void     _Cdecl trnodfre (SLTREE * ptree, bool (* fn) (SLNODE * pnode));
void     _Cdecl trouttr  (SLTREE * ptree, bool hide);
void     _Cdecl troutprn (SLTREE * ptree);
bool     _Cdecl tricenxt (SLTREE * ptree);
void     _Cdecl trrename (SLTREE * ptree);
void     _Cdecl trtbleft (SLTREE * ptree);
void     _Cdecl trtbrght (SLTREE * ptree);
bool     _Cdecl trcend   (SLTREE * ptree);
bool     _Cdecl trchome  (SLTREE * ptree);
bool     _Cdecl trcpgup  (SLTREE * ptree);
bool     _Cdecl trcpgdn  (SLTREE * ptree);
bool     _Cdecl trsup    (SLTREE * ptree);
bool     _Cdecl trsleft  (SLTREE * ptree);
bool     _Cdecl trsright (SLTREE * ptree);
bool     _Cdecl trsdown  (SLTREE * ptree);
bool     _Cdecl trhome   (SLTREE * ptree);
bool     _Cdecl trpgup   (SLTREE * ptree);
bool     _Cdecl trend    (SLTREE * ptree);
bool     _Cdecl trpgdn   (SLTREE * ptree);
bool     _Cdecl trup     (SLTREE * ptree);
bool     _Cdecl trdown   (SLTREE * ptree);
void     _Cdecl trpush   (SLTREE * ptree);
void     _Cdecl trpop    (SLTREE * ptree);
SLNODE * _Cdecl trprvbrt (SLNODE * pnode);
SLNODE * _Cdecl trnxtbrt (SLNODE * pnode);
void     _Cdecl trgo2nod (SLTREE * ptree, SLNODE * pnode);
bool     _Cdecl trishigh (SLTREE * ptree, SLNODE * pfind);
void     _Cdecl trupdmsk (SLTREE * ptree, SLNODE * cur);
void     _Cdecl trdelnod (SLTREE * ptree, bool (* fn) (SLNODE * pnode));
int      _Cdecl trinshig (SLTREE * ptree, bool fld, char * name,
                                             bool (* fn) (SLNODE * pnode));
int      _Cdecl trinslow (SLTREE * ptree, bool fld, int where, char * name,
                                             bool (* fn) (SLNODE * pnode));
bool     _Cdecl trmkroot (SLTREE * ptree, char * titl, char * name,
                                             bool (* fn) (SLNODE * pnode));


#ifdef __cplusplus
}
#endif

#endif
