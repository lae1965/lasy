/**********************************************************************/
/*                                                                    */
/*    SLDBAS.H     Header file for Turbo C TOOLS SLBase  Functions    */
/*                                                                    */
/**********************************************************************/
#ifndef DEF_SLDBAS
#define DEF_SLDBAS

#include <slmenu.h>

/********************************************************************/
/* Definitions of data types.                                       */
/********************************************************************/

#define NUM_OFF             256
#define PWRK_LEN            4096
#define PAGE_SIZE           256
#define MAX_NAM_LEN         20
#define SL_NUMPN            /*NUM_ROWS*4*/200
#define WORK                utworkin ()
#define ishide(v,w,b,l)     ((b + l <= v || b >= w + v) == YES)

/********************************************************************/
/* Структуры общего пользования                                     */
/********************************************************************/

typedef struct preview {
    int               pos;
    int               row;
    long              line;
} PREVIEW;

typedef union {
    struct {
        int     opt;
        byte    separ;           /*    Разделитель м-у ДД/ММ/ГГ     */
        byte    centure;         /*    Век                          */
    } date;
    struct {
        unsigned    opt    :  10;
        unsigned    len    :   6;
    } d;
} VALOPT;

typedef union {
    int     i;                   /*    Целое                        */
    long    l;                   /*    Двойное целое                */
    float   f;                   /*    Вещественное                 */
    double  d;                   /*    Двойное вещественное         */
    struct {                     /*    Текстовое                    */
        byte  * p;               /*    Ук. на строку                */
        int     len;             /*    Длина строки                 */
        byte    word_1;          /*    (Кол-во слов в строке) - 1   */
        byte    blank;           /*    Кол-во лишних пробелов       */
                                 /*    Для inival - кол-во абзацев  */
    } s;
    struct {                     /*    Для поля spec в LISTING'е    */
        long    numrec;          /*    Номер записи                 */
        long    offset;          /*    Смещение до numrec в lst.tmp */
    } lst;
    void        * q;
} VAL;

typedef union {
    int     docnam;
    struct {
        struct doc  * pdoc;
        int     idnup;
        int     idndown;
        int     tasknam;
    } link;
} FLDLINK;

typedef struct idlin {               /* Описывает строку, содержащую поля    */
    int             npn;             /* Физический номер строки              */
    byte            numdark;         /* Кол-во dark'ов в строке              */
    byte            places;          /* Кол-во посадочных мест под поля      */
    byte            num;             /* Кол-во шаблонных строк  (ifnot 0)    */
} IDLIN;

typedef struct name {
    byte            nam [MAX_NAM_LEN + 1];
} NAME;

typedef struct taskname {
    byte            nam [9];
} TASKNAME;

typedef struct {                 /* Параметры поля                       */
    unsigned visibl        : 1;  /* Видимое поле                         */
    unsigned multy         : 1;  /* Множественное поле                   */
    unsigned protect       : 1;  /* Защищенное поле                      */
    unsigned indx          : 1;  /* Индексируемое поле                   */

    unsigned sum           : 1;  /* Способ инициализации - сумма         */
    unsigned midl          : 1;  /* Способ инициализации - среднее       */
    unsigned numb          : 1;  /* Способ инициализации - количество    */

    unsigned first         : 1;  /* 1-е поле в строке                    */
    unsigned par           : 1;  /* Начало параграфа                     */
    unsigned wrap          : 1;  /* Текстовое поле автопереносное        */
    unsigned done          : 1;  /* Поле обработано                      */
    unsigned change        : 1;  /* В поле внесены изменения             */
    unsigned noedit        : 1;  /* Нередактируемое поле                 */
    unsigned dummy         : 3;  /* Резерв                               */
} FLDOPT;

typedef struct {                 /* Параметры кортежа                        */
    unsigned visibl        : 1;  /* Видимый кортеж                           */
    unsigned dark          : 1;  /* Видимый кортеж самого низкого уровня     */
    unsigned mark          : 1;  /* Кортеж помечен                           */
    unsigned done          : 1;  /* Кортеж обработан                         */
    unsigned multy         : 1;  /* Кортеж из множественного поля (или wrap) */
    unsigned dummy         :11;  /* Резерв                                   */
} CRTOPT;

typedef struct idfld {
    VAL             inival;          /* Значение инициализируемого поля      */
    VAL             dnval;           /*  гр.диап.знач./имя CDF              */
    VAL             upval;           /*  гр.диап.знач.                      */
    byte            valtype;         /* Тип значения поля                    */
    byte            initype;         /* Способ инициализации поля            */
    byte            sym;             /* Символ для выравнивания              */
    int             idc;             /* ID кортежа - хозяина поля            */
    int             idl;             /* ID оригинальной строки               */
    int             idn;             /* Номер имени поля в массиве doc->fnam */
    byte            instr;           /* Номер поля в строке                  */
    FLDOPT          opt;             /* Параметры поля                       */
    PRNOPT          prnopt;          /* Параметры для печати                 */
    VALOPT          valopt;          /* Параметры значения поля              */
    FLDLINK         fldlink;         /* Связь поля с другими полями и док-ми */
} IDFLD;

typedef struct fld {
    int             idf;             /* ID  поля                             */
    int             idc;             /* ID  кортежа - владельца шаблона      */
    int             idn;             /* Номер имени поля в массиве doc->fnam */
    struct crt  *   curcrt;          /* Ук. на текущий  кортеж документа     */
    struct fld  *   prev;            /* Ук. на предыдущее поле документа     */
    struct fld  *   next;            /* Ук. на следующее поле документа      */
    byte            instr;           /* Номер поля в строке                  */
    FLDOPT          opt;
    PRNOPT          prnopt;
    VAL             val;             /* Внутреннее значение поля             */
} FLD;

typedef struct {
    int             idf_first;       /* Номер первого id поля отн. к кортежу */
    int             idf_last;        /* Номер послед. id поля отн. к кортежу */
    int             idc_last;        /* Номер послед. IDCRT дочки (ifnot -1) */
    CRTOPT          opt;
    IDLIN           idd;             /* ID шаблонных строк                   */
} IDCRT;

typedef struct crt {
    int             idc;             /* ID кортежа                           */
    FLD         *   fld_first;       /* Ук. на первое поле кортежа           */
    FLD         *   fld_last;        /* Ук. на последнее свое поле кортежа   */
    FLD         *   fld_end;         /* Ук. на последнее поле отн. к кортежу */
    struct crt  *   highcrt;         /* Ук. на родительский экземпляр  crt   */
    struct crt  *   lowcrt;          /* Ук. на первую дочку данного crt      */
    struct crt  *   prevsis;         /* Ук. на предыдущую сестру    кортежа  */
    struct crt  *   nextsis;         /* Ук. на следующую сестру     кортежа  */
    struct crt  *   prev;            /* Ук. на предыдущий экземпляр кортежа  */
    struct crt  *   next;            /* Ук. на следующий  экземпляр кортежа  */
    CRTOPT          opt;
} CRT;

typedef struct docedt {
    int             visicol;         /* Колонка начала просмотра документа   */
    int             row;             /* Строка с текущим полем на экране     */
    int             beg;             /* Строка с текущим полем на экране     */
    int             len;             /* Строка с текущим полем на экране     */
    long            line;            /* Физический номер строки              */
    FLD         *   pfld;            /* Ук. на рабочую FLD                   */
} DOCEDT;

typedef struct pnswap {
    int             total;           /* Количество  строк                    */
    byte        *   ptxt;            /* Буфер строк                          */
    int         *   pn;              /* Массив смещений до строк             */
} PNSWAP;


typedef struct doc {
    char            sign[4];
    char            name_doc  [9];   /* Имя файла документа (без пути)       */
    char            name_data [9];   /* Имя файла данных                     */
    int             numidc;          /* Количество IDC в массиве idc         */
    int             numidf;          /* Количество IDF в массиве idf         */
    long            numrec;          /* N текущей записи (0 - новая запись)  */
    PNSWAP          o;               /* Структура оригинальных строк         */
    PNSWAP          d;               /* Структура шаблонных строк            */
    int             pos;             /* Позиция маркера в текущем поле       */
    int             lastline;        /* Последняя строка с полем             */
    int             total;           /* Общее количество строк в документе   */
    DOCEDT      *   old;             /* Ук.на структуру DOCEDT для пред.поля */
    DOCEDT      *   cur;             /* Ук.на структуру DOCEDT для тек. поля */
    byte        *   pwrk;            /* Ук. на рабочий буфер                 */
    TASKNAME    *   tnam;            /* Массив имен задач                    */
    NAME        *   dnam;            /* Массив имен документов               */
    NAME        *   fnam;            /* Массив имен полей                    */
    NAME        *   cnam;            /* Массив имен кортежей                 */
    IDLIN       *   idl;             /* Массив id оригинальных строк         */
    IDCRT       *   idc;             /* Массив описаний кортежей документа   */
    IDFLD       *   idf;             /* Массив описаний полей документа      */
    CRT         *   crtroot;         /* Ук. на корневой кортеж документа     */
    FLD         *   curfld;          /* Ук. на текущее поле документа        */
    WINDOW      *   pwin;            /* Ук. на окно документа                */
    PRNOPT          prnopt;
    PRNPAG          prnpag;
    PREVIEW         view;
    struct {
        unsigned    visibl   :  1;   /* 0 - Документ без вид. части          */
        unsigned    move     :  1;   /* 1 - Режим скролинга, 0-псевдографика */
        unsigned    cur_on   :  1;   /* 1 - Маркер включен                   */
        unsigned    hide     :  1;   /* 1 - Полей на экране не видно         */
        unsigned    scroll   :  1;   /* 1 - Скролинг документа               */
        unsigned    needout  :  1;   /* 1 - Необходим перевывод экрана       */
        unsigned    changpos :  1;   /* 1 - При перех.на нов. поле pos = 0   */
        unsigned    insert   :  1;   /* 1 - insert, 0 - ovewrite             */
        unsigned    autojump :  1;   /* 1 - Автоперескок на следующее поле   */
        unsigned    single   :  2;   /* 0 - Double, 1 - Single, 2 - Erase    */
        unsigned    change   :  1;   /* 1 - В документ вносились изменения   */
        unsigned    for_prn  :  1;   /* 1 - Режим печатания документа        */
        unsigned    nulpos   :  1;   /* 1 - При попадании в поле pos = 0     */
        unsigned    needfmt  :  1;   /* setnewfld необходимо вызывать fmtcol */
        unsigned    maxwin   :  1;   /* 1 - Окно создается и пок-ся с max H  */
        unsigned    _global  :  1;   /* 1 - База нах-ся в глобальном каталоге*/
        unsigned    for_lst  :  1;   /* 1 - Режим listing'а                  */
        unsigned    noflush  :  1;   /* 1 - Не вызывать kbflushb ()          */
        unsigned    dummy    : 13;   /* Резерв                               */

    } opt;
    byte    doctype;                 /* Тип документа                        */
} DOC;

typedef struct dcb {
    char            valsld [3];
    char            name_data [9];
    BORDER          bor;
    ATTRIB          att;
    int             h;               /* Высота окна документа                */
    int             w;               /* Ширина окна документа                */
    int             row;             /* Верхняя строка окна на экране        */
    int             col;             /* Левая колонка  окна на экране        */
    word            cnamlen;         /* Длина массива имен кортежей в байтах */
    word            fnamlen;         /* Длина массива имен полей в байтах    */
    word            tnamlen;         /* Длина массива имен задач в байтах    */
    word            dnamlen;         /* Длина массива имен док-тов в байтах  */
    word            idclen;          /* Длина массива IDCRT в байтах         */
    word            idflen;          /* Длина массива IDFLD в байтах         */
    word            idllen;          /* Длина массива IDLIN в байтах         */
    word            pndlen;          /* Длина массива PND в байтах           */
    word            dtxtlen;         /* Длина текста шаблонных строк         */
    word            pnolen;          /* Длина массива PNO в байтах           */
    word            otxtlen;         /* Длина текста оригинальных строк      */
    long            inioff;          /* Смещение до значений простых полей   */
    PRNOPT          prnopt;          /* Шрифт текста документа               */
    PRNOPT          fld_prnopt;      /* Шрифт полей по умолчанию             */
    PRNPAG          prnpag;          /* Параметры для печатания документа    */
    struct {
        unsigned    fatherbas  :  1; /* Документ - родитель базы             */
        unsigned    _global    :  1; /* 1 - база в общем каталоге, 0 - в тек.*/
        unsigned    dummy      : 14;
    } opt;
} DCB;

typedef struct crtwrk {
    DOC     * doc;                   /* Ук.на рабочий документ               */
    CRT     * crtmain;               /* Ук.на выдираемый из связки кортеж    */
    CRT     * firstdark;             /* Ук.на 1-й dark, вложенный в crtmain  */
    FLD     * fld_first;             /* Ук.на 1-е поле crtmain               */
    FLD     * firstmid;              /* Ук.на 1-е чужое поле в 1-й строке    */
    FLD     * lastmid;               /* Ук.на послед. чужое поле в 1 строке  */
    IDLIN   * pidl;                  /* ID выдираемой строки                 */
    byte      beginstr;              /* instr 1-го выдираемого поля crtmain  */
    int       nline;                 /* Количество строк в crtmain'ой группе */
    int       idc;                   /* ID кортежа - владельца строки        */
    bool      optfirst;
    bool      visi;
} CRTWRK;

/********************************************************************/
/* Структуры, используемые для работы с записями в базе             */
/********************************************************************/

typedef struct slhd {
    byte            sign [4];
    long            date;            /* Дата последней модификации           */
    long            len;             /* Общая длина записей в базе (in bytes)*/
    long            total;           /* Количество записей в базе            */
    int             treelen;         /* Длина дерева в базе (in bytes)       */
} SLHD;

typedef struct idbase {
    int     handb;                   /* handle базового файла                */
    int     handh;                   /* handle идентификатора базы           */
    long    lenh;                    /* Длина идентификатора базы            */
    SLHD    slhd;
    long    pboff [NUM_OFF];         /* Массив смещений до базовых записей   */
    long    firstboff;               /* Номер 1-го смещения в массиве pboff  */
} IDBASE;

typedef struct {
    long    firstrec;
    int     num_of_recs;
} IDGROUP;

/********************************************************************/
/* Структуры, используемые для работы с автопереносными полями      */
/********************************************************************/


typedef struct txtval {
    VAL               val;           /* Содержимое, выдранное из поля       */
    PRNOPT            prnopt;        /* Параметры для печати                */
    struct txtval   * next;          /* Следующий TXTVAL                    */
    struct txtval   * prev;          /* Предыдущий TXTVAL                   */
    bool              par;           /* Начало нового абзаца                */
    bool              newfld;        /* Содержимое VAL'а нач-ся с нов. поля */
} TXTVAL;


typedef struct idcol {
    PRNOPT            prnopt;        /* Параметры для печати параграфа      */
    FLD             * first;         /* Первое поле колонки                 */
    FLD             * last;          /* Последнее поле колонки              */
    FLD             * mult;          /* Первое множественное поле колонки   */
    TXTVAL          * pval;          /* Первый TXTVAL в колонке             */
    TXTVAL          * savval;        /* Последний обработанный TXTVAL       */
    struct idcol    * next;          /* Указатель на следующую колонку      */
} IDCOL;

typedef struct idedt {
    void          (*go2firstfld)(void);/* Выз-ся для уст-ки начального поля */

    void          (*prewait) (void); /* Вызывается до ожидания клавиши      */
                                     /* Стандартная - dbedstat              */

    int           (*aftergetkey)(void); /* Вызывается после получения клавиши  */

    int           (*postwait)(void); /* Вызывается после получения клавиши  */

    void          (*fldwrk)  (void); /* Вызывается перед dboutdoc           */

    int           (*leavefld)(void); /* Вызывается по сходу с поля          */

    void          (*updscreen)(DOC * doc); /* Вызывается из dboutdoc        */

    void          (*updfld)(DOC * doc, FLD * pfld, byte * att);
                                     /* Вызывается из dboutfld              */

    char       ** funkey;            /* Указатель на строки клавишных подск.*/
    char          row;               /* Строка,  где выводить caps          */
    char          col;               /* Колонка, где выводить caps          */
                                     /* if -1, не выводить                  */
} IDEDT;

typedef struct idcdf {               /* Идентифицирует открытый кодификатор */
    DOC             * doc;           /* Указатель на документ кодификатора  */
    int               num;           /* Кол-во полей, подключенных к doc    */
    struct idcdf    * next;
} IDCDF;

typedef struct idlst {          /* Идентифицирует LISTING                   */
    bool    lstfile;            /* YES - из списка найденных, NO - из базы  */
    long    first;              /* Номер 1го загруженного экз. для базы     */
    long    last;               /* Номер последнего загруж.экз.для базы     */
    long    total;              /* Общее кол-во экземпляров                 */
    int     H;                  /* Предполагаемое кол-во строк в порции     */
                                /* if -1 - порция загр-ся, пока есть память */
    long    wasrecs;            /* Общее кол-во пролистанных сверху экз-ров */
    int     currecs;            /* Кол-во экз-ров в текущей порции          */
    char    name_data [9];      /* Имя файла базы                           */
    bool    db_global;          /* YES-база в общем каталоге, NO-в текущем  */
    bool    more;
} IDLST;

typedef struct {
    bool    lstfile;            /* YES - из списка найденных, NO - из базы    */
    char    filename [13];      /* Имя файла списка найденных. if EOS - база  */
    long    firstnumrec;        /* Numrec экземпляра, с кот.нач-ся листинг    */
    long    first;              /* Номер 1го загруженного экземпляра          */
    long    total;              /* Общее кол-во экземпляров                   */
    int     H_page;             /* Предполагаемое кол-во строк в порции       */
                                /* if -1 - порция загр-ся, пока есть память   */
    int     currecs;            /* Кол-во экз-ров в текущей порции            */

    char    crtnam [MAX_NAM_LEN]; /* Имя листингового кортежа                 */
    CRT   * (*getlstcrt) (void);  /* Получает 1й листинговый кортеж в Cur_Lst */
    void    (*open)      (void);  /* Открывает (инициализирует) все необходим.*/
                                  /* для листинга базы, документы и файлы     */
    void    (*ini)       (bool down); /* Определяет первонач.для текущ.порции */
                                      /* базу, lstfile и numrec               */
    void    (*load)      (bool down); /* Заполняет поля в тек.лист-вом кортеже*/
    bool    (*stop)      (void);  /* YES - конец текущей порции               */
    void    (*close)     (void);  /* Закрывает (разрушает) базы, докуменеты и */
                                  /* файлы, которыми пользовался листинг      */
    bool    (*makpage)   (long num, bool down); /* Загружает страницу в лист. */
    bool    (*makcrt)    (bool down);        /* Загружает одну запись в лист. */

    int     (*userfun)   (void);

    int     firstrow;
    int     duphei;
    int     row;
    int     hei;
    int     visicol;

    CRT   * curcrt;
    int     numcrt;

    DOC     * curbasedoc;
    IDBASE  * curpidb;
    CRT     * curcrtwrk;
    int       curhandle;
    long      newnum;
    long      curnum;

} IDLIST;

typedef struct {

    long  * pmark;                /* Массив помеченных записей                */
    int     nummark;              /* Количество помеченных записей            */
    bool    mark;
    void    (*markone)   (void);
    void    (*markall)   (void);
} IDMARKLST;

typedef struct {
    int         (*enterfn) (void);        /* Функция обработки Enter'а */
    int         (*ed_lastmenudoc) (void); /* Ф-ия обр-ки клавиш последн.док-та*/
    char    **  lastdockey;               /* Клавишн.подсказка последн.док-та */
    bool        maxwin;                   /* YES-Окно последн.док.max.размеров*/
    int         (*ed_lastmenulst) (void); /* Ф-ия обр-ки клавиш последн.док-та*/
    char    **  lastlstkey;               /* Клавишн.подсказка последн.док-та */
} IDMKMENU;

typedef struct {
    bool        (*crt2crt) (void);     /* Функция переноса из меню в док-т   */
    bool        (*enterfn) (void);     /* Обработка Enter'а в ed_lastmenulst */
    struct {
        unsigned  mark     : 1;        /* YES-разрешена пометка неск.записей */
        unsigned  maxwin   : 1;        /* YES-окно последн.док.max.размеров  */
        unsigned  go2doc   : 1;        /* YES-разрешен вход в последн.док-т  */
        unsigned  dummy    :13;
    } opt;
} IDMENU;

typedef struct {
    PRNPAG      prnpag;
    void        (*preview)   (void); /* Предварительный просмотр */
    bool        (*setprnpag) (void); /* Установка PRNPAG         */
    int         (*outprn) (void);    /* Функция печатания        */
    int         (*vfyfldstr) (FLD * pfld); /* Вызывается перед каждой строкой */
} IDPRN;


#define SL_INT          0        /*    Целое                        */
#define SL_LONG         1        /*    Двойное целое                */
#define SL_FLOAT        2        /*    Вещественное                 */
#define SL_DOUBLE       3        /*    Двойное вещественное         */
#define SL_DIGITS       3        /*    Максимальный цифровой тип    */
#define SL_TEXT         4        /*    Текстовое                    */
#define SL_MONEY        5        /*    Деньги                       */
#define SL_DATE         6        /*    Дата                         */
#define SL_CDF          7        /*    Кодификатор (справочник)     */
#define SL_USER         8        /*    Минимальный пользоват. тип   */

#define SL_CONST        0        /*    Константа                    */
#define SL_SUM          1        /*    Сумма                        */
#define SL_MIDL         2        /*    Среднее                      */
#define SL_NUM          3        /*    Количество                   */
#define SL_DICT         4        /*    Справочник                   */
#define SL_NOTINI       5        /*    Не изменяется редактированием*/
#define SL_FIND         6        /*    Поисковый                    */

#define SL_EXP          512      /*    Экспоненциальная форма       */


#define DC_SIGN         "DOC"

#define DB_VISI         2
#define DB_MAXWIN       4

#define DB_CENTER      -4

#define FLD_IS_VAL      1
#define BAD_VAL         2
#define DIRTY_VAL       3
#define NO_RANGE        4
#define NO_MEM          5

#define DB_DOC          0
#define DB_QRY          1
#define DB_LST          2
#define DB_DICT         3
#define DB_VOC          4
#define DB_BAS          5
#define DB_HEAD         6
#define DB_QRYWRK       7
#define DB_GROUP        8

#define PR_BEGNOTE      1
#define PR_ENDNOTE      2


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
/* Functions using for SLBASE TOOLS                                 */
/********************************************************************/
bool    dbinidb     (int mode, int rows, char * penv);
void    dbdsplay    (DOC * doc, int row, int col, char * pttl);
void    dbremove    (DOC * doc);
bool    dbmakidlst  (bool lstfile, bool page);
bool    dbmaklst    (bool lstfile, bool page);
bool    dblodlst    (int direct);
bool    dbreldlst   (int change);
void    dbcalsum    (DOC * doc, FLD * pfldfrom);
void    dbfindb     (void);
void    dbsl2nam    (char * pext, int num, ...);
bool    dbfndfrg    (byte far * pfrg, byte far * pstr);
int     dbcmpsmp    (void far * qstr, void far * bstr);
int     dbclcwrd    (void far * from);
int     dbclrbln    (void far * to, void far * from, bool first);
void    dboutdoc    (DOC * doc);
void    dboutfld    (DOC * doc, FLD * pfld, int row, int beg, int len,
                                                       bool mistake, bool visi);
void    dbkhlpat    (CELLSC far * to, void far * from, char tatt, char batt);
char  * dbwarerr    (char *file_in, char *file_out, char *name, char *mess);
void    dbwar       (char *pn,char *pm, bool disp);
int     dbfmtscr    (void far *to, void far *from, int above, int remabove);
VAL   * dbcdf2nam   (DOC * doc, int code);
int     dbcdf2cod   (DOC * doc, byte * pname);
bool    dboutprn    (DOC * doc, char * filename, void (*preview) (void));
int     dbprndoc    (DOC * doc);
bool    dbprnlst    (char * filename);
int     dbprnlstdoc (DOC * doc, int mode);
bool    dbclrdoc    (DOC * doc, bool init);
void    dbdstroy    (DOC * doc);
FLD   * dbcrtdel    (DOC * doc, CRT * crtmain);
FLD   * dbcrtmul    (DOC * doc, CRT * crtmain, bool below, bool ini);
FLD   * dbdelout    (DOC * pdoc, CRT * crtmain, CRTWRK * pcrtwrk);
FLD   * dbmulout    (DOC * doc, CRT * crt, CRTWRK * pcwrk, bool dn, bool ini);
bool    dbmkroot    (DOC * doc, bool init);
void   _crtdel      (CRT * crtmain);
void   _outcrt      (CRT * crtmain);
void   _inscrt      (CRT * crtnew, CRT * crtover, bool down);
CRT   *_crtmake     (DOC * doc, CRT * crtmain);
DOC   * dbinidoc    (char * pfilnam, int opt, int doctype, bool db_global);
bool    dbdocedt    (DOC * doc, IDEDT * pidedt, bool edit, bool top);
bool    dbcor_bl    (int visicol, int W, int * b, int * l);
int     dbfindbr    (void far *from, int * pbeg, int * plen, int instr);
void    dbbeglen    (DOC * doc, FLD * pfld, int * b, int * l);
bool    dblodone    (DOC * pdoc, long numrec, bool forward);
bool    dblodrec    (DOC * pdoc, IDBASE * pdb, long numrec, bool forward);
bool    dblodcrt    (DOC * pdoc, CRT * pcrt, IDBASE * pidb, long numrec);
bool    dbloddoc    (DOC * pdoc, int hand);
bool    dbsavrec    (DOC * pdoc, CRT * pcrt);
bool    dbsavtmp    (DOC * pdoc, CRT * pcrt, IDBASE * pidb);
void    dbsavdoc    (DOC * pdoc, CRT * pcrt, int hand);
bool    dbdelone    (DOC * pdoc);
bool    dbdelrec    (IDBASE * pdb, long numrec);
IDBASE *dbopendb    (char * name_data, bool db_global, int mode);
void    dbcreatmp   (char * basenam);
void    dbclosdb    (IDBASE * pidb);
int     dbvaltoc    (DOC * doc, FLD * pfld, byte ** pp, int wid);
int     dbctoval    (DOC * doc, FLD * pfld, void * pbuf, VAL * pval);
int     how_row     (DOC * doc, FLD * high, FLD * low);
byte  * dbgetstr    (DOC * doc, FLD * pfld);
int     dbgetpno    (DOC * doc, FLD * pfld);
int     dbgetpnd    (DOC * doc, FLD * pfld);
IDFLD * dbgetidf    (DOC * doc, FLD * pfld);
IDLIN * dbgetidl    (DOC * doc, FLD * pfld);
IDLIN * dbgetidd    (DOC * doc, FLD * pfld);
CRT   * dbgetpapa   (FLD * pfld);
int     dbfmtpar    (DOC * doc, FLD ** ppfld, int mode);
void    dboutcol    (DOC * pdoc, FLD * pfld, bool for_save);
int     dbfmtcol    (bool need);
void    fmtcol      (void);
void    dbprpval    (int fldlen);
void    dbsetjst    (FLD * pfld, int jst);
void    dbpr2par    (FLD * pfld);
long    dbfndrec    (void);
bool    dbtstrec    (DOC * pbasedoc, DOC * pqrydoc, CRT * bcrt, CRT * qcrt);
bool    dbq2qry     (DOC * pqdoc, DOC * pqrydoc);
void    dbclrfrg    (DOC * doc);
int     dbquery     (int handlst);
bool    dbcrtocr    (DOC * pdto, DOC * pdfr, CRT * pcto, CRT * pcfr, bool mov);
FLD   * prev        (FLD * pcur);
FLD   * next        (FLD * pcur);
FLD   * prevcont    (FLD * pfld, CRT * papa);
FLD   * nextcont    (FLD * pfld, CRT * papa);
FLD   * lastvisi    (FLD * pflast);
FLD   * firstvisi   (FLD * pffirst);
FLD   * dbgetfld    (DOC * doc, FLD * pfld, char * pname, bool forward);
CRT   * dbgetcrt    (DOC * doc, CRT * pcrt, char * pname);
int     getwidsym   (PRNOPT * prnopt);
void    dbedt_shor  (bool right);
int     otherfield  (bool down, bool newstr);
void    dbed_ent    (bool ctrl);
TXTVAL *valbreak    (TXTVAL * oldval, byte * p, int len);
void    dbedt_sver  (bool down);
void    dbed_sp1    (void);
void    dbed_rgt    (void);
void    dbed_lft    (void);
void    dbed_tab    (bool shift);
void    dbed_ver    (bool down);
void    dbed_pg     (bool down);
void    dbed_ceh    (bool down);
void    dbed_clb    (void);
void    dbed_sde    (void);
void    dbed_mov    (void);
void    dbed_par    (void);
void    dbed_jst    (void);
void    dbed_cdt    (void);
void    dbed_2fl    (DOC * doc, bool visi);
void    dbprprn     (void (*preview) (void));
void    dbed_prf    (void);
void    dbed_prd    (void);
void    dbed_ccr    (void);
void    dbed_mup    (CRT * pcrt);
void    dbed_mdn    (CRT * pcrt);
void    dbed_del    (CRT * pcrt, bool ask);
void    dbed_svr    (DOC * doc, bool visi);
void    dbed_ldr    (bool nxt);
void    dbed_cld    (DOC * doc);
void    dbed_dlr    (DOC * doc);
void    dbed_edt    (void);
bool    dbed_nav    (void);
bool    dbed_c2v    (void);
void    setnewfld   (void);
bool    dbsetfld    (void);
int     edt_doc     (void);
void    dbedkhlp    (void);
int     edt_query   (void);
bool    vfyfl2base  (int handini);
long    findrec     (IDBASE * pdb, long * pnumrec, bool forward);
CRTWRK *dbcrtout    (DOC * pdoc, CRT * crtmain);
void    dbcrtins    (CRTWRK * pcrtwrk);
int     dbcalcln    (DOC * pdoc, FLD * pfcur, FLD * pflast);
void    dbclrdone   (CRT * pcrt);
void    colfree     (void);
CRT   * getnextfrom (CRT * pcrtfrom, FLD * plastfld);
CRT   * get1stfrom  (DOC * basedoc, CRT * bpapacrt, char * pnam);
FLD   * dbsrtcrt    (DOC * doc, FLD * pfld, bool forward);
void    dbed_srt    (bool forward);
void    dbviewhor   (bool right);
void    dbviewver   (bool down);
void    dbpreview   (void);
int     dbopen      (char * pfilnam, int doctype, bool db_global, int mode);
void    outfields   (void);
void    jstmrk      (void);
void    cur2old     (void);
void    currestore  (void);
void    rowlinecorr (register int i);
long    getlastnumrec (void);
void    pos2visicol (void);
void    val2pwrk    (void);
bool    dbsetsym    (void);
void    bssym       (void);
void    delsym      (void);
bool    dbktab      (bool shift);
void    dbedstat    (void);
void    dbstatdb    (void);
bool far _Cdecl dbedmous (int ev, int bs, int xp, int yp, int xd, int yd);
int     dbrolnam    (char *pn, char *pext, bool(*fn)(int h), char *pdocnam);
char  * dbfilnam    (char *pn, char *pext);
void    dbsethei    (DOC * doc);
void    dbrenambas  (char * old, bool oldgl, char * new, bool newgl, bool grp);
void    dbdelbas    (char * name, bool _global, bool grp);
bool    dbcopybas   (char * old, bool oldgl, char * new, bool newgl, bool grp);
bool    dbmovbas    (char * old, bool oldgl, char * new, bool newgl, bool grp);
IDBASE *dbcreatbas  (char * docfile, bool docglob, int doctype, char * basfile,
                                                                  bool basglob);
bool    dbsqzbas    (char * pfilnam, bool _glob, bool savdlrc, bool updw, bool grp);
void    dbmkname    (char * fullname, char * name, int doctype, bool glb);
bool    dbnewdoc    (void);
bool    dbunirec    (DOC * docfrom, char * basenam, bool _global);
char  * dbtxtini    (char * docnam, char * fldnam, int doctype);
void    dbmenulv    (char * menunam, bool _glob, IDMENU * pidmenu);
void    dbmkmnlv    (char * menunam, bool _glob, IDMKMENU * pidmenu);


IDMARKLST * lsinimark       (bool mark);
void        lsdstroymark    (void);
int         lstscrnv        (void);

void        lsremakpg       (void);
void        lsmarkone       (void);
void        lsmarkall       (void);
void        lsspec1         (void);
void        lshome          (void);
void        lsend           (void);
void        lsdown          (void);
void        lsup            (void);
void        lspgdn          (void);
void        lspgup          (void);
int         lsdelrec        (void);
void        crt2screen      (void);
void        updscreen       (DOC * doc);


bool        lsinilst        (bool lstfile);
void        lsprnlst        (void);
int         dbbase2grp      (char * lstfn, char * bn, bool glb, long numpapa);
long        dbgrp2base      (char * lstfn, char * bn, bool glb, long numpapa);
long        dbinsrec2grp    (int numitm);
void        dbdelrec2grp    (int numitm);
void        dbinsgrp        (char * basename, bool _global, long numpapa);
void        dbdelgrp        (char * basename, bool _global, long numpapa);
long        dbdelrec2lst    (char * lstfilename, int numitm);

#ifdef __cplusplus
}
#endif

extern WINDOW  * Db_pstat, * Db_phlp, * Cdf_Wind;
extern IDEDT   * Db_curidedt;
extern IDCDF   * Db_topidcdf;
extern DOC     * Cur_Doc, * Cur_Lst, * Cur_Qry, * Cur_Edt;
extern byte    * Db_pwrk;
extern char    * Db_ext [], Db_Env [70], DBL_point;
extern int       Db_MousX, Db_MousY, Db_Err, Db_Fonch;
extern bool      Db_main_edit, Db_edit;
extern bool      Db_MousMove;
extern IDCOL   * firstcol;
extern TXTVAL  * curval;
extern IDLST     idlst;
extern byte      UPPERASCII [];

extern IDPRN      * Curidprn;
extern IDLIST     * Curlisting;
extern IDMARKLST  * Curmarklst;
extern int          H_page, Cur_string, Num_page;

extern int       (* Db_Fn_valtoc) (FLD * pfld, char * pbuf, int valtype);
                                    /* Выз.из dbvaltoc для пользоват.типов */
extern int       (* Db_Fn_ctoval) (FLD * pfld, char * pbuf, int valtype);
                                    /* Выз.из dbctoval для пользоват.типов */
extern bool      (* Db_Fn_q2qry)  (FLD * pfld, char ** pbuf, int valtype);
                                    /* Выз.из qval2qry для пользоват.типов */
extern double    (* Db_Fn_cmpval) (VAL * pv1, VAL * pv2, int valtype);
                         /* Выз.из dbtstrec и dbsrtcrt для пользоват.типов */

#endif
