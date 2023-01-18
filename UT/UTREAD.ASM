;         utread () - Читает из файла с текущей файловой позиции,
;                     после чего передвигает файловую позицию на
;                     величину прочитанных данных.
;
;      word  utread (int handle, void far * pbuf, word len);
;
; Параметры:
;           int         handle - Handle ранее открытого файла
;           void far  * pbuf   - Адрес буфера, куда необходимо поместить
;                                прочитанные данные
;           word        len    - Количество запрашиваемых данных (in bytes)
;
; Возврат:
;           Количество bytes реально помещенных в pbuf или 65536 (-1),
;           если ошибка
;

include     begasm.mac

beginProg   utread
    handle  equ  word  ptr  [bp + stkoff + 0]
    pbuf    equ  dword ptr  [bp + stkoff + 2]
    len     equ  word  ptr  [bp + stkoff + 6]

    StartCode

    mov     ah, 3fH
    mov     bx, handle
    lds     dx, pbuf
    mov     cx, len
    cmp     cx, 0
    jne     short cont
    mov     ax, cx
    jmp     short suc
cont:
    int     21H
    jc      short unsuc
    jmp     short suc
unsuc:
    mov     ax, -1
suc:

    FinishCode
endProg     utread

end
