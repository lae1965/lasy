/*****************************************************************************
    dbinidb - Определяет начальные установки для базовых функций.

int dbinidb (int mode, int rows, char * penv);

int     mode;       Видео режим
int     rows;       Число строк для дисплея
char  * penv;       Имя пути до глобального каталога в environment'е

*****************************************************************************/

#include    <sldbas.h>
#include    <slmous.h>
#include    <slkeyb.h>

IDLIST      * Curlisting = NULL;
IDMARKLST   * Curmarklst = NULL;
WINDOW      * Db_phlp, * Db_pstat, * Cdf_Wind;
IDEDT       * Db_curidedt;
IDCDF       * Db_topidcdf;
byte        * Db_pwrk;
IDLST         idlst;
int           Db_Err, Db_Fonch;
char          Db_Env [70], DBL_point = '.';
char        * Db_ext [] =
    { ".sld", ".slq", ".sll", ".slc", ".slv", ".slb", ".slh", ".slq", ".slg" };

byte  UPPERASCII [256] = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,
  29,30,31,32,33,34,35,36,37,38,39,'(',')','*','+',',','-','.',47,'0','1','2',
  '3','4','5','6','7','8','9',':',';','<','=','>','?','@','A','B','C','D','E',
  'F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X',
  'Y','Z','[',92,']','^','_','`','A','B','C','D','E','F','G','H','I','J','K',
  'L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','{','|','}','~',
  '','А','Б','В','Г','Д','Е','Ж','З','И','Й','К','Л','М','Н','О','П','Р','С',
  'Т','У','Ф','Х','Ц','Ч','Ш','Щ','Ъ','Ы','Ь','Э','Ю','Я','А','Б','В','Г','Д',
  'Е','Ж','З','И','Й','К','Л','М','Н','О','П','░','▒','▓','│','┤','╡','╢','╖',
  '╕','╣','║','╗','╝','╜','╛','┐','└','┴','┬','├','─','┼','╞','╟','╚','╔','╩',
  '╦','╠','═','╬','╧','╨','╤','╥','╙','╘','╒','╓','╫','╪','┘','┌','█','▄','▌',
  '▐','▀','Р','С','Т','У','Ф','Х','Ц','Ч','Ш','Щ','Ъ','Ы','Ь','Э','Ю','Я','Ё',
  'ё','Є','є','Ї','ї','Ў','ў','°','∙','·','√','№','¤','■',255
};

int       (* Db_Fn_valtoc) (FLD * pfld, char  * pbuf, int valtype);
int       (* Db_Fn_ctoval) (FLD * pfld, char  * pbuf, int valtype);
bool      (* Db_Fn_q2qry)  (FLD * pfld, char ** pbuf, int valtype);
double    (* Db_Fn_cmpval) (VAL * pv1, VAL * pv2, int valtype);

bool    dbinidb (int mode, int rows, char * penv) {
    char      * p;
    register    hand, i;
    int         hor, ver;
    bool        mod_mono;

    Db_Fn_valtoc = NULL; Db_Fn_ctoval = NULL;
    Db_Fn_q2qry  = NULL; Db_Fn_cmpval = NULL;

    Db_topidcdf = NULL;
    Db_curidedt = NULL;
    Db_Err = 0;
    Db_phlp = Db_pstat = NULL;
    if (!wniniwin (mode, rows)) return NO;

    if (penv == NULL) * Db_Env = EOS;
    else {
        if ((p = getenv (penv)) == NULL) {
            mnwarnin (NULL, 0, 3, " ", "Задача имеет неправильную конфигурацию",
                                                                           " ");
            goto ret;
        }
        strcpy (Db_Env, p);
        i = strlen (Db_Env);
        if (Db_Env [i - 1] != '\\') Db_Env [i] = '\\';
    }

    if ((Db_pwrk = malloc (PWRK_LEN)) == NULL) goto ret;
    wnblinks (YES);

    strcpy ((char *) Db_pwrk, Db_Env);
    strcat ((char *) Db_pwrk, "md.ini");

    Db_Fonch = 176;   hor = 16;  ver = 8;
    utmovabs (Pal_Cur, Pal_Stnd, 17, 0);
    if ((hand = utopen (Db_pwrk, UT_R)) != -1) {
        utread (hand, &mod_mono, sizeof (bool));
        utread (hand, &Att_Tree, sizeof (ATTRIB));
        utread (hand, &Att_Mnu1, sizeof (ATTRIB));
        utread (hand, &Att_Mnu1, sizeof (ATTRIB));
        utread (hand, &Att_Mnu2, sizeof (ATTRIB));
        utread (hand, &Att_Khlp, sizeof (ATTRIB));
        utread (hand, &Att_Chlp, sizeof (ATTRIB));
        utread (hand, &Att_Edit, sizeof (ATTRIB));
        utread (hand, &Att_User, sizeof (ATTRIB));
        utread (hand, &Db_Fonch, 1);
        utread (hand, Pal_Cur, 17);
        utread (hand, &hor, sizeof (int));
        utread (hand, &ver, sizeof (int));
        utread (hand, &specprn, sizeof (SPECPRN));
        utclose (hand);
    }

    msspeed (hor, ver);
    wnsetpal (Pal_Cur);
    wndisfon (Db_Fonch, Att_User.nice);

    if ((Db_phlp = wncreate (1, NUM_COLS, NUM_ROWS - 1, 0, NULL, &Att_Khlp))
                                                             == NULL) goto ret;
    if ((Db_pstat = wncreate (2, NUM_COLS, 0, 0, NULL, &Att_Khlp))
                                                             == NULL) goto ret;

    wnmovpa (Db_pstat->img.pdata, NUM_COLS * 2, Att_Khlp.nfld);
    Kb_fun = utoutdat;
    return YES;

ret:
    dbfindb ();
    return NO;
}

void    dbfindb (void) {

    wndstroy (Db_pstat);
    wndstroy (Db_phlp);
    utfree   (&Db_pwrk);
    wnfinwin ();
}

