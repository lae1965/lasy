#include <stdio.h>
#include <string.h>
#include <slutil.h>

typedef struct {
	char name[32];
	long offset;
	unsigned int len;
} TEMA;

void main (int argc, char ** argv){

    FILE          * f, * fcomp, * ftmp;
    char          * pbuf, * ppbuf;
    static char     buf[130];
    TEMA            tema;
    long            offset1, offset2;
    int             first, nfun = 0, nmess = 0, nm = 0;

	if (argc != 2){
        printf ("    Задача COMP_ERF.COM формирует файл быстрого доступа\n");
        printf ("для осуществления двухуровневого поиска\n\n");
        printf ("              ПРИМЕР  ИСХОДНОГО  ФАЙЛА:\n\n");
        printf ("@ТЕМА1\n");
        printf ("*ПОДТЕМА1.1\n");
        printf ("текст подтемы 1.1 в произвольной форме\n");
        printf ("*ПОДТЕМА1.2\n");
        printf ("текст подтемы 1.2 в произвольной форме\n");
        printf ("@ТЕМА2\n");
        printf ("*ПОДТЕМА2.1\n");
        printf ("текст подтемы 2.1 в произвольной форме\n");
        printf ("*ПОДТЕМА2.2\n");
        printf ("текст подтемы 2.2 в произвольной форме\n\n");
        printf ("КОМАНДНАЯ  СТРОКА:  COMP_ERF  исходный_файл\n");
		return;
	}

    if ((f = fopen (argv[1], "rb")) == NULL) {
		printf ("File %s is not opened\n", argv[1]);
		return;
	}

    utchext (argv[1], ".err");
    if ((fcomp = fopen (argv[1], "w+b")) == NULL) {
		printf ("File %s is not created\n", argv[1]);
		return;
	}

    if ((ftmp = fopen ("tmp.$$$", "w+b")) == NULL) {
		printf ("Temporary file is not created\n");
		return;
	}

				/*P A S S   1*/

	while (fgets (buf, sizeof(buf), f) != NULL) if (*buf == '@') nfun++;
	putw (nfun, fcomp);
	rewind (f);

				/*P A S S   2*/

	tema.len = 0;
	first = 1;
    while (fgets (buf, sizeof(buf), f) != NULL) {
        if (*buf == '@') {
			ppbuf = pbuf = &buf[1];
			*(ppbuf + strlen (pbuf) - 2) = '\0';
            if (first == 0) {
				tema.len = nmess;
				fwrite (&tema, sizeof(tema), 1, fcomp);
			}
			strcpy (tema.name, pbuf);
            if (first == 1) {
				tema.offset = nfun * sizeof(tema) + sizeof(int);
				first = 0;
            } else {
				tema.offset += nmess * sizeof(tema);
				nm += nmess;
				nmess = 0;
			}
		}
		if (*buf == '*') nmess++;
	}
	tema.len = nmess;
	fwrite (&tema, sizeof(tema), 1, fcomp);
	nm += nmess;
	rewind (f);

				/*P A S S   3*/

	first = 1;
	offset1 = offset2 = 0;
    while (fgets (buf, sizeof(buf), f) != NULL) {
        if (*buf == '*') {
			ppbuf = pbuf = &buf[1];
			*(ppbuf + strlen (pbuf) - 2) = '\0';
			tema.offset = sizeof (int) + (nfun + nm) * sizeof(tema) + offset2;
            if (first == 0) {
				offset2 = ftell (ftmp);
				tema.len =(int)(offset2 - offset1);
				fwrite (&tema, sizeof(tema), 1, fcomp);
				offset1 = offset2;
            } else first = 0;
			strcpy (tema.name, pbuf);
		}
		else if (*buf != '@') fwrite (buf, strlen(buf), 1, ftmp);
	}
	offset1 = ftell (ftmp);
	tema.len =(int)(offset1 - offset2);
	tema.offset = sizeof (int) + (nfun + nm) * sizeof(tema) + offset2;
	fwrite (&tema, sizeof(tema), 1, fcomp);
	rewind (ftmp);

				/*P A S S   4*/

	while (fgets (buf, sizeof(buf), ftmp) != NULL)
		 fwrite (buf, strlen(buf), 1, fcomp);

	fclose (f);
	fclose (ftmp);
	fclose (fcomp);
	remove ("tmp.$$$");
    printf ("Creating file -- %s\n", argv[1]);
    return;
}

