;         utwrite () - Пишет в файл с текущей файловой позиции,
;                      после чего передвигает файловую позицию на
;                      величину прописанных данных.
;
;      bool  utwrite (int handle, void far * pbuf, word len);
;
; Параметры:
;           int         handle - Handle ранее открытого файла
;           void far  * pbuf   - Адрес буфера, откуда необходимо взять
;                                данные для записи в файл
;           word        len    - Количество данных (in bytes)
;
; Возврат:
;           YES - в случае удачного завершения
;           NO  - ошибка
;

include     begasm.mac

beginProg   utwrite
    handle  equ  word  ptr  [bp + stkoff + 0]
    pbuf    equ  dword ptr  [bp + stkoff + 2]
    len     equ  word  ptr  [bp + stkoff + 6]

    StartCode

    mov     ah, 40H
    mov     bx, handle
    lds     dx, pbuf
    mov     cx, len
    cmp     cx, 0
    je      short lab
cont:
    int     21H
    jc      short unsuc
    cmp     ax, len    
    jne     short unsuc    
lab:
    mov     ax, 1
    jmp     short suc
unsuc:
    xor     ax, ax
suc:

    FinishCode
endProg     utwrite

end
