;         utchsize () - Изменяет размер указанного файла.
;                       Если размер увеличивается, то новая область
;                       '\0' не заполняется
;
;      bool  utchsize (int handle, long size);
;
; Параметры:
;           int         handle - Handle ранее открытого файла
;           long        size   - Новый размер файла
;
; Возврат:
;            YES - размер файла изменен
;            NO  - ошибка.
;

include     begasm.mac

beginProg   utchsize
    handle   equ  word  ptr  [bp + stkoff + 0]
    lowsize  equ  word  ptr  [bp + stkoff + 2]
    highsize equ  word  ptr  [bp + stkoff + 4]

    StartCode

    mov     ax, 4200H
    mov     bx, handle
    mov     cx, highsize
    mov     dx, lowsize
    int     21H
    jc      short unsuc

    mov     ah, 40H
    xor     cx, cx
    int     21H
    jc      short unsuc
    mov     ax, YES
    jmp     short suc
unsuc:
    xor     ax, ax
suc:

    FinishCode
endProg     utchsize

end
