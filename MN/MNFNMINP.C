/**********************************************************************
    mnfnminp - Ввод строки имени файла в окне

bool mnfnminp (int row, int col, char *ttl, char *fnbuf, char *ext,
               bool verify, bool dir);

int     row         Номер строки окна ввода на экране
int     col         Номер колонки окна ввода на экране
char    *ttl        Заголовок окна ввода (или NULL)
char    *fnmbuf     Буфер строки ввода имени файла
char    *ext        Расширение по умолчанию
bool    verify      YES Проверить введеное имя на существование
                    NO  Не проверять
bool    dir         YES Показ поддиректориев
                    NO  Выбор в текущем директории

Выход:  YES         Имя файла введено или выбрано
        NO          Имя файла не введено или введенный файл не существует
**************************************************************************/

#include <alloc.h>
#include <dir.h>
#include <ctype.h>
#include <io.h>
#include <dos.h>
#include <slmenu.h>

#define MAXSELECT   100

bool cdecl get_wild (int, int, char *, bool);

bool mnfnminp (int row, int col, char *ttl, char *fnbuf, char *ext,
                                                    bool verify, bool dir) {
    static char tmpbuf[MAXPATH];
    bool        wild;
    char       *p;

    strcpy (tmpbuf, fnbuf);
    while (1) {
        if (!wnstrinp (row, col, ttl, tmpbuf, dir ? 60 : 12))  return (NO);

        if (strlen (tmpbuf) == 0)  { strcat (tmpbuf, "*"); verify = YES; }

        if ((p = strchr (tmpbuf, '.')) == NULL && verify) {
            if (ext == NULL)                 strcat (tmpbuf, ".*");
            else                             strcat (tmpbuf, ext);
        }
        strupr (tmpbuf);
        wild = strchr (tmpbuf, '?') != NULL || strchr (tmpbuf, '*') != NULL;

        if (!verify && !wild) {
            if (p == tmpbuf && *p == '.') { utalarm (); return (NO);  }
            else              { strcpy (fnbuf, tmpbuf); return (YES); }
        }

        if (wild) {
            if (get_wild (row, col, tmpbuf, dir)) {
                strcpy (fnbuf, tmpbuf);                 return (YES);
            } else                                      continue;
        }
        if (access (tmpbuf, 0) == 0) { strcpy (fnbuf, tmpbuf); return (YES); }
        mnwarnin (NULL, 0, 4, " ", tmpbuf, "не существует", " ");
    }
}

bool get_wild (int row, int col, char buf[MAXPATH], bool dir) {
#define  diskettes ((utpeekw( uttofaru( 0x40, 0x10) ) >> 6) & 3) /*Кол-во ГМД*/
#define  fantom     utpeekb( uttofaru( 0x50, 0x04) )

    char            hom_dir[MAXDRIVE+MAXDIR-1];
    static char    *files[MAXSELECT];
    char            cur_dir[MAXDRIVE+MAXDIR-1];
    char            drive[MAXDRIVE], ext[MAXEXT];
    char            fname[MAXFILE+MAXEXT-1];
    char           *p;
    struct ffblk    ffound;
    int             flags, sel, nfiles;
    bool            ret, done = NO;
    register int    i;

	getcwd (hom_dir, MAXDRIVE+MAXDIR-1);
    flags = fnsplit(buf, drive, cur_dir, fname, ext);
    strcat (fname, ext);
    if (flags & DRIVE) {
        if (!diskettes && (*drive & 0xDF) < 'C' && (*drive & 1) == (char)fantom)
            return NO;    /*Введен не существующий ГМД*/
		setdisk (toupper(*drive) - 'A');
    }
    if (flags & DIRECTORY) {
		i = strlen (cur_dir) - 1;
        if (i > 0)  cur_dir[i] = EOS;
        if (chdir (cur_dir) != 0)
            { setdisk (toupper (* hom_dir) - 'A'); return (NO); }
    }

    do {
        getcwd (cur_dir, MAXDRIVE+MAXDIR-1);
		nfiles = 0;

                    /*** Get all subdirectories ***/
        if (dir) {
            if (findfirst ("*", &ffound, FA_DIREC) == 0) {
                do {
					if ((ffound.ff_attrib & FA_DIREC) == 0 ||
						strcmp (ffound.ff_name, ".") == 0)
                            continue;              /* Нашли файл */
					if (nfiles >= MAXSELECT) break;
					files[nfiles] = malloc (strlen (ffound.ff_name) + 4);
                    strcpy (files[nfiles], " ");
					strcat (files[nfiles], ffound.ff_name);
                    strcat (files[nfiles], "\\ ");
                    nfiles++;
                } while (findnext (&ffound) == 0);
            }
        }
                    /*** Get all archive files ***/

        if (findfirst (fname, &ffound, FA_ARCH) == 0) {
            do {
                if (nfiles >= MAXSELECT) break;
				files[nfiles] = malloc (strlen (ffound.ff_name) + 3);
                strcpy (files[nfiles], " ");
				strcat (files[nfiles], ffound.ff_name);
                strcat (files[nfiles], " ");
                strlwr (files[nfiles]);
                nfiles++;
            } while (findnext (&ffound) == 0);
        }
        if (nfiles <= 0) { ret = NO; break; }

        sel = mnrolmnu (row, col, nfiles, 0, files, cur_dir, 15, 1);

        if (sel < 0) { done = YES; ret = NO; }        /* ESC pressed */
        else {                                       /* ENTER pressed */
            p = files[sel] + 1;             /* Leading blank deleted */
            i = strlen (p) - 1;             /* Last symbol position */
            p[i--] = EOS;                   /* Trailing blank deleted */
            if (p[i] == '\\') { p[i--] = EOS; chdir (p); } /*Change dir & continue */
            else {                          /* Return full path */
                strcpy (buf, cur_dir);
                if (buf[strlen(buf)-1] != '\\')  strcat (buf, "\\");
                strcat (buf, strupr (p));
				done = ret = YES;
			}
        }
        for (i = 0; i < nfiles; i++)  free (files[i]);
    } while (!done);

            /* Restore current drive & directory and return */
	setdisk (toupper(*hom_dir) - 'A');
	chdir (hom_dir);
	return (ret);
}

