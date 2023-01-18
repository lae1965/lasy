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
static  char   *err_hard     =  "Ошибка устройства";
static  char   *err_disk     =  " Ошибка диска  X:";
static  char   *err_text[16] = {
    "    защита записи    ",
    " неверное устройство ",
    "устройство не готово ",
    "   неверная команда  ",
    "     ошибка данных   ",
    "     плохой запрос   ",
    "сбой позиционирования",
    "   неизвестный тип   ",
    "   сектор не найден  ",
    "     конец бумаги    ",
    "    ошибка записи    ",
    "    ошибка чтения    ",
    "    общая ошибка     ",
    "       резерв        ",
    "       резерв        ",
    "    неверный диск    " 
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

