#include <slutil.h>

static  unsigned char tab_upp[] = "";
static  unsigned char tab_low[] = " กขฃคฅฆงจฉชซฌญฎฏเแโใไๅๆ็่้๊๋์ํ๎๏";

int utcupper (int sym) {
    char *p;
    byte symu = (byte) sym;

    if (symu >= 'a' && symu <= 'z')  symu -= 32;
    else if ((p = strchr ((char *) tab_low, sym)) != NULL)
        symu = (tab_upp [(int)(p - (char *) tab_low)]);
    return (symu);
}

int utclower (int sym) {
    char *p;
    byte symu = (byte) sym;

    if (symu >= 'A' && symu <= 'Z')  symu += 32;
    else if ((p = strchr ((char *) tab_upp, sym)) != NULL)
        symu = (tab_low [(int)(p - (char *) tab_upp)]);
    return (symu);
}

