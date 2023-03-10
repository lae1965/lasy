#include "language.h"
#include <slmenu.h>


#ifdef ENGLISH
static  char   *err_hard     =  "  Device error   ";
static  char   *err_disk     =  "Error on drive X:";
static  char   *err_text[16] = {
    "  write protect  ",
    "  unknown unit   ",
    "device not ready ",
    " unknown comand  ",
    "data error (CRC) ",
    "   bad request   ",
    "   seek error    ",
    "  unknown media  ",
    "sector not found ",
    "  out of paper   ",
    "   write fault   ",
    "   read fault    ",
    " general failure ",
    "    reserved     ",
    "    reserved     ",
    "inval disk change"
};
#else
static  char   *err_hard     =  "?訡?? ???ன?⢠";
static  char   *err_disk     =  " ?訡?? ??᪠  X:";
static  char   *err_text[16] = {
    "    ?????? ??????    ",
    " ????୮? ???ன?⢮ ",
    "???ன?⢮ ?? ??⮢? ",
    "   ????ୠ? ???????  ",
    "     ?訡?? ??????   ",
    "     ???宩 ??????   ",
    "ᡮ? ????樮??஢????",
    "   ??????????? ⨯   ",
    "   ᥪ??? ?? ??????  ",
    "     ????? ?㬠??    ",
    "    ?訡?? ??????    ",
    "    ?訡?? ?⥭??    ",
    "    ????? ?訡??     ",
    "       १???        ",
    "       १???        ",
    "    ???????? ????    " 
};
#endif

int mnharder (ITEM * pitem, int nitem) {
    bool    disk = NO;

    if (!Err_Hard) return -1;
    Err_Hard = NO;
    if (Err_Disk >= 0)  {
        err_disk [15] = (char) ((Err_Disk & 0x00FF) + 'A');
        disk = YES;
    }
    return (mnwarnin (pitem, nitem, 3,
              (disk) ? err_disk:err_hard, err_text[Err_Val & 0x00FF],""));
}

