/*****************************************************************************
	Функция
       char * dbwarerr (char *file_in, char *file_out, char *name, char *mess)
осуществляет двухуровневый поиск сообщений, которые прописываются в выходной
файл.
	ПАРАМЕТРЫ:

			  char *file_in  - имя  входного файла,
			  char *file_out - имя выходного файла,
			  char *name     - имя    темы,
			  char *mess     - имя подтемы.
    ВОЗВРАЩАЕТ:              - указатель на первую строку сообщения
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <alloc.h>

typedef struct {
	char name[32];
	long offset;
	unsigned int len;
} TEMA;


char *dbwarerr (char *file_in, char *file_out, char *name, char *mess) {
    static FILE   * fin, * fout;
    static char     first;
    static char     pret[75];
    char          * pbuf = NULL, * p;
    TEMA            tema;
    int             i, found = 0, nname, nmess;

    pret[0] = '\0';
    if (first == 0) {
        if ((fout = fopen (file_out, "wb")) == NULL)  return NULL;
		first = 1;
    } else  if ((fout = fopen (file_out, "ab")) == NULL)  return NULL;

	if (fin == NULL)
        if ((fin = fopen (file_in, "rb")) == NULL) {
            fprintf (fout, "Функция dbwarerr:\r\nошибка открытия файла %s\r\n",
																	 file_in);
			goto lab;
		}

	rewind (fin);
	nname = getw (fin);
    for (i = 0; i < nname; i++) {
		fread (&tema, sizeof(tema), 1, fin);
        if (strcmp (name, tema.name) == 0) { found = 1; break; }
	}
    if (found == 0) {
        fprintf (fout, " !!! Функция %s не найдена в файле %s\r\n", name, file_in);
		goto lab;
	}
    fprintf (fout, "Функция   %s :\r\n", tema.name);
	fseek (fin, tema.offset, SEEK_SET);

	nmess = tema.len;
	found = 0;
    for (i = 0; i < nmess; i++) {
		fread (&tema, sizeof(tema), 1, fin);
        if (strcmp (mess, tema.name) == 0) { found = 1; break; }
	}
    if (found == 0) {
        fprintf (fout, " !!! Сообщение %s не найдено в файле %s\r\n",
															mess, file_in);
		goto lab;
	}

	fprintf (fout, "%s -\r\n", tema.name);
	fseek (fin, tema.offset, SEEK_SET);

    if ((pbuf = calloc (1, (size_t)tema.len)) == NULL) {
        fprintf (fout, "Функция dbwarerr:\r\nне хватает памяти\r\n");
		goto lab;
	}
	fread (pbuf, (size_t)tema.len, 1, fin);
	fwrite (pbuf, (size_t)tema.len, 1, fout);
    strncpy (pret, pbuf, 74);
    pret[74] = '\0';
    if ((p = strchr (pret, '\n')) != NULL)  *p = '\0';
lab:
	fclose (fout);
    free   (pbuf);
    return pret;
}

