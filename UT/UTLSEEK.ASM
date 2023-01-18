;         utlseek () - Перемещает логический файловый указатель
;                      на указанную позицию
;                      
;
;      long  utlseek (int handle, long pos, int from);
;
; Параметры:
;           int         handle - Handle ранее открытого файла
;           long        pos    - Величина смещения
;           int         from   - смещение относительно
;                                   UT_BEG (0) - начала файла
;                                   UT_CUR (1) - текущей позиции
;                                   UT_END (2) - конца файла
;       
; Возврат:
;            -1L, если произошла ошибка.
;
;            Новая позиция логического файлового указателя относительно
;            начала файла 
;            (Вызов utlseek (handle, 0L, UT_END) возвращает длину файла)
;

include     begasm.mac

beginProg   utlseek
    handle  equ  word  ptr  [bp + stkoff + 0]
    lowpos  equ  word  ptr  [bp + stkoff + 2]
    highpos equ  word  ptr  [bp + stkoff + 4]
    from    equ  byte  ptr  [bp + stkoff + 6]

    StartCode

    mov     ah, 42H
    mov     bx, handle
    mov     cx, highpos
    mov     dx, lowpos
    mov     al, from
    int     21H
    jc      short unsuc
    jmp     short suc
unsuc:
    mov     ax, -1
    mov     dx, -1
suc:

    FinishCode
endProg     utlseek

end
