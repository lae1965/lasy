;         utopen () - Открывает файл в указанном режиме
;                     Файл должен существовать, иначе ошибка.
;
;      int  utopen (void far * fname, int mode);
;
; Параметры:
;   void  * fname -   Указатель  на строку с именем файла
;   int     mode  -   Режим открытия   0 (UT_R)  - для чтения
;                                      1 (UT_W)  - для записи
;                                      2 (UT_RW) - для чтения/записи
;                              !!! Нельзя комбинировать по |
;
; Возврат:
;               Handle или -1, если произошла ошибка
;

include     begasm.mac

beginProg   utopen
    fname   equ  dword ptr  [bp + stkoff + 0]
    mode    equ  byte  ptr  [bp + stkoff + 4]

    StartCode

    mov     ah, 3dH
    lds     dx, fname
    mov     al, mode
    int     21H
    jc      short unsuc
    jmp     short suc
unsuc:
    mov     ax, -1
suc:

    FinishCode
endProg     utopen

end
