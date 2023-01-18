;         utclose () - Закрывает файл, сбрасывает буфера, обновляет
;                      каталог.
;
;      int  utclose (int handle);
;
; Параметры:
;           int     handle - Handle ранее открытого файла
;
; Возврат:      -1
;

include     begasm.mac

beginProg   utclose
    handle  equ  word  ptr  [bp + stkoff + 0]

    StartCode

    mov     ah, 3eH
    mov     bx, handle
    cmp     bx, -1
    je      short suc
    int     21H
suc:
    mov     ax, -1

    FinishCode
endProg     utclose

end
