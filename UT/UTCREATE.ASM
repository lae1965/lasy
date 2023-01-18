;         utcreate () - Файл создается в указанном или умалчиваемом
;                       каталоге в режиме записи/чтения.
;                       Если файл уже существует, он усекается до
;                       нулевой длины.
;
;
;      int  utcreate (char * fname);
;
; Параметры:
;   char  * fname -   Указатель  на строку с именем файла
;
; Возврат:
;               Handle или -1, если произошла ошибка
;

include     begasm.mac

beginProg   utcreate
    fname   equ  dword ptr [bp + stkoff + 0]

    StartCode

    mov     ah, 3cH
    lds     dx, fname
    xor     cx, cx
    int     21H
    jc      short unsuc
    jmp     short suc
unsuc:
    mov     ax, -1
suc:

    FinishCode
endProg     utcreate

end
