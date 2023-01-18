#include <sldbas.h>

#define     MAX_FILE_LEN    13

static void hz (char * pnam);

void main (void) {
    int     i;

    if (!dbinidb (_TEXTC80, _MINROWS, NULL)) return;

    for (i = 0; i < 3; i++) dbsl2nam (Db_ext [i] + 1, 1, NULL);
    dbsl2nam (Db_ext [DB_BAS] + 1, 1, NULL);

    hz ("sld.ini");
    hz ("slq.ini");
    hz ("sll.ini");
    hz ("slb.ini");

    utremove ("sld.ini");
    utremove ("slq.ini");
    utremove ("sll.ini");
    utremove ("slb.ini");
    dbfindb ();
}

static void hz (char * pnam) {
    DCB     dcb;
    char    pfilnam [30];
    int     i, hand, handini;

    handini = utopen (pnam, UT_R);
    utread (handini, &i, sizeof (int));
    for (; i > 0; i--) {
        utread (handini, pfilnam, MAX_NAM_LEN + 1);
        utread (handini, pfilnam, MAX_FILE_LEN);
        hand = utopen (pfilnam, UT_RW);
        utread (hand, &dcb, sizeof (DCB));
        dcb.opt._global = NO;
        utlseek (hand, 0L, UT_BEG);
        utwrite (hand, &dcb, sizeof (DCB));
        utclose (hand);
    }
    utclose (handini);
}

