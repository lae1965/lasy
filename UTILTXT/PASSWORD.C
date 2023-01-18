#include <io.h>
#include <fcntl.h>
#include <conio.h>
#include <string.h>


static char buf [] = "dk";

void main (int argc, char ** argv) {
    register     hand, i = 0;
    int          change = 0;
    char       * p = argv [1], buf1 [] = { '\0', '\0', '\0' };
    long         offset = 15L * 512 + 368L;

    if (argc == 2) {
        if (* p == '/' || * p == '-') p++;
        if (strcmp (p, "c") == 0) change = 1;
    }

    hand = open ("password.com", O_RDWR);
    lseek (hand, offset, SEEK_SET);
    read (hand, buf, 3);

    clrscr ();

    if (change) cprintf ("Старый пароль:");
    else { gotoxy (37, 12); cprintf ("ПАРОЛЬ:"); }

    while (1) {
        if (getch () == buf [i]) {
            if (i == 1) break;
            i = 1;
        } else i = 0;
    }

    if (change) {
        cprintf ("\r\nНовый пароль:");
        for (i = 0; i < 2; i++) buf [i] = getch ();
        while (1) {
            cprintf ("\r\nВведите еще раз:");
            for (i = 0; i < 2; i++) buf1 [i] = getch ();
            if (strcmp (buf, buf1) == 0) break;
            strcpy (buf, buf1);
        }
        lseek (hand, offset, SEEK_SET);
        write (hand, buf, 2);
    }
    close (hand);
    return;
}

