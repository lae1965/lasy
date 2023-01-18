/**********************************************************************/
/*                                                                    */
/*    MAKEDOC.H    Header file for Turbo C TOOLS Makedoc  Functions   */
/*                                                                    */
/**********************************************************************/
#ifndef DEF_MAKEDOC
#define DEF_MAKEDOC

#include <sldbas.h>
#include <sltree.h>

/********************************************************************/
/* User functions declarations by category                          */
/********************************************************************/

#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif

typedef struct mdnode {         /* Структура узла дерева иерархии   */
    IDFLD           idf;
    CRTOPT          copt;       /* Опции  кортежа                   */
    IDLIN           idl;       /* Указатель на структ. ориг. IDLIN */
    IDLIN           idd;       /* Указатель на структ. шабл. IDLIN */
    char            docname  [MAX_NAM_LEN+1];
                                /* Имя док-та, по кот.иниц-ся поле  */
    char            fldup    [MAX_NAM_LEN+1];
                                /* Имя зависимого поля сверху       */
    char            flddown  [MAX_NAM_LEN+1];
                                /* Имя зависимого поля снизу        */
    char            taskname [9];
                                /* Имя задачи, вып-ой на данном поле*/
} MDNODE;

typedef struct maker {
    char            path [61];      /* Путь до источника             */
    char            enterfile [65]; /* Имя файла старого документа   */
    char            basename [13];  /* Имя файлов базы данных        */
    struct {
        unsigned    entertyp : 2;   /* Тип документа на входе        */
        unsigned    exittyp  : 2;   /* Тип документа на выходе       */
        unsigned    oper     : 3;   /* Вид операции                  */
        unsigned    dummy    : 9;
    } opt;
} MAKER;


#define    MD_DOC    0
#define    MD_QRY    1
#define    MD_LST    2
#define    MD_CDF    3

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* Functions using for MAKEDOC                                      */
/********************************************************************/
void         zoomwin     (void);
void         outcolomn   (CELLSC * pdata, int col, int width, int command);
void         freelin     (void);
void         ffield      (void);
void         fedit       (void);
void         ftree       (void);
void         fwind       (void);
void         bordtype    (void);
void         resize      (void);
void         edttree     (void);
void         trinidoc    (char *path, bool all);
void         dcbcorrect  (void);
bool         savetree    (int  handle, bool for_base);
void         outtext     (int row, int col, int visicol, int npno);
void         doccolor    (void);
void         savecolor   (ATTRIB *pattrib, int place);
void         optbox      (SLNODE * pnode);
bool         valdcb      (int handle, char * name);
int          vfytree     (void);
int          vfytrcrt    (SLNODE * curcrt);
int          makepn      (int handto, void far * pto, int tosize, int handfrom,
                                 void far * pfrom, long lenfrom, word fromsize);
int          txt2bin     (int handto, int handfrom, void far * pfrom, int len);
int          edtdupl     (int * len, int r, int c, int n, char * pt, bool dpl);
bool         delpuser    (SLNODE * pnode);
bool         delnode     (SLNODE * pnode);
bool         inipuser    (SLNODE * pnode);
int          whereins    (bool fld);
void         code2txt    (void far * pto, void far * pfrom, int len);
void         txt2code    (void far * pto, void far * pfrom, int len);

#ifdef __cplusplus
}
#endif

/********************************************************************/
/*              Extern        symbols                               */
/********************************************************************/
extern bool      valtext, valtree, valdoc;
extern bool      chwnatt, chwnbor, chwnsize;
extern char      fonch;
extern char    * potxt, * Md_ext [];
extern char    * mdotxt, * mddtxt, * mdmtxt, * mdpno;
extern char    * mdtmpm, * mdtmpd, * mdpnd,  * mdpnm, mdini [80];
extern word    * pno, * pnd;
extern word      size;
extern int       firstpno, lastpno, totalpno;
extern int       firstpnd, lastpnd;
extern MAKER     maker;
extern BORDER    vmnubrd, mnubrd, treebrd;
extern ATTRIB    treattr;
extern SLTREE  * ptree;
extern WINDOW  * pwind;
extern DCB       dcbwrk, dcb;

#endif
