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


typedef struct slnode {         /* Структура узла дерева иерархии   */
    char          * name;       /* Указатель на строку названия     */
    word            nsons;      /* Число   сыновей     узла         */
    int             level;      /* Уровень вложенности в дереве     */
	dword           pmask;      /* Маска активных предыдущих уровней*/
    struct slnode * father;     /* Указатель на родителя            */
    struct slnode * prev;       /* Указатель на пред. узел          */
    struct slnode * next;       /* Указатель на след. узел          */

    struct  {
        unsigned last    :  1;  /* Послед.узел в списке у родителя  */
        unsigned lock    :  1;  /* Помеченный узел                  */
        unsigned fld     :  1;  /* YES если текущий узел - поле     */
        unsigned dummy   : 13;  /* Резерв                           */
    } opt;

    void          * puser;      /* Ук. на внешнюю структуру         */
} SLNODE;

typedef struct {                /* Представление дерева иерархии    */
    int             namlen;     /* Длина имени узла                 */
    int             total;      /* Количество узлов дерева          */
    int             topnum;     /* Номер верхнего узла экрана       */
    int             row;        /* Номер текущей строки в окне      */
    int             col;        /* Номер текущей колонки в окне     */
    int             tab;        /* Размер отступа уровней           */
    int             minlev;     /* Минимальный уровень видимой части*/
    WINDOW        * pwin;       /* Окно, в котором дерево показано  */
    SLNODE        * root;       /* Указатель на корневой узел       */
    SLNODE        * cur;        /* Указатель на текущий узел        */
    SLNODE        * ice;        /* Указатель на замороженный узел   */
    SLNODE        * top;        /* Указатель на верхний узел экрана */
    SLNODE        * prevtop;    /* Указатель на верхний узел экрана */
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