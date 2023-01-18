;         utisempt () - Поиск символа, отличного от  ' ' или '\0',
;                                   в строке pbuf слева направо.
;
;      bool  utisempt (byte * pbuf);
;
; Параметры:
;   byte  * pbuf -    Указатель  на строку, в которой производится поиск.
;
; Возврат:
;               YES - строка пуста;
;               NO  - есть символы;
;

include     begasm.mac

beginProg   utisempt
    pbuf    equ  DPTR_   [bp + stkoff + 0]

    StartCode

        cld
        xor     cx,     cx

        LDS_    si,     pbuf
        xor     bh,     bh
        mov     bl,     32    ; ' '
lab:
        lodsb

        cmp     al,     bl
        je      short lab

        cmp     al,     bh
        jne     short fin
        mov     cx,     1

fin:
        mov     ax,     cx

    FinishCode
endProg     utisempt

end
