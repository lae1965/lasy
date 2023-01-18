;         utremove () - Удаляет файл из указанного или умалчиваемого
;                       каталога, если файл существует и и его
;                       атрибуте нет бита only_read
;
;      bool  utremove (void far * fname);
;
; Параметры:
;   void far * fname -   Указатель  на строку с именем файла
;
; Возврат:
;                YES - Указанный файл удален
;                NO  - Ошибка
;

include     begasm.mac

beginProg   utremove
    fname   equ  dword ptr [bp + stkoff + 0]

    StartCode

    mov     ah, 41H
    lds     dx, fname
    int     21H
    jc      short unsuc
    mov     ax, YES
    jmp     short suc
unsuc:
    xor     ax, ax
suc:

    FinishCode
endProg     utremove

end
