#include    <sldbas.h>
#include    <slkeyb.h>
#include    <slsymb.def>
#include    <alloc.h>

/***********
8  {0,0,0,0,1,0,0,0,0}   128
7  {0,0,1,0,0,0,1,0,0}   64
6  {1,0,0,0,0,0,0,0,1}   32
5  {1,0,0,0,0,0,0,0,1}   16
4  {1,0,0,0,0,0,0,0,1}   8
3  {0,0,1,0,0,0,1,0,0}   4
2  {0,0,0,0,1,0,0,0,0}   2
1  {0,0,0,0,0,0,0,0,0}   1
    1 2 3 4 5 6 7 8 9
    **********************************/
static byte buf [] = {
    27, ':', 0, 0, 0,
    27, '%', 1, 0,
    27, '&', 0, 60, 60,
    128,
    56, 0, 68, 0, 130, 0, 68, 0, 56,
    0, 0
    };
static byte buf1 [] = { 27, 51, 42};
static byte buf2 [] = { 27, '!', 33};
static byte buf3 [] = { 27, '!', 1};
static byte bufcrlf [] = { 13, 10};

char * buf4[]={"  ëÅÖêÅÄçä N6652/542",
               "  àçç 526<<<2295 ",
               " N<12799 ",
               " 11-<6-<1 îàë 14:48",
               " <<216*",
               " <<23/9......355.<<" };

void  main (void) {
    int h;

    h = utopen ("PRN", UT_RW);

    utwrite (h, buf1, 3);
    utwrite (h, buf3, 3);
    utwrite (h, buf, 26);
    utwrite (h, buf4 [0], strlen (buf4 [0]));
    utwrite (h, bufcrlf, 2);
    utwrite (h, buf4 [1], strlen (buf4 [1]));
    utwrite (h, bufcrlf, 2);
    utwrite (h, buf2, 3);
    utwrite (h, buf4 [2], strlen (buf4 [2]));
    utwrite (h, bufcrlf, 2);
    utwrite (h, buf3, 3);
    utwrite (h, buf4 [3], strlen (buf4 [3]));
    utwrite (h, bufcrlf, 2);
    utwrite (h, buf4 [4], strlen (buf4 [4]));
    utwrite (h, bufcrlf, 2);
    utwrite (h, buf4 [5], strlen (buf4 [5]));
    utwrite (h, bufcrlf, 2);

    utclose (h);
    return;
}

