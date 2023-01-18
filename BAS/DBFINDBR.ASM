;
; dbfindbr - Копирует строку текста в image окна с указанным атрибутом.
;
; int dbfindbr (void far *from, int * pbeg, int * plen, int numbr);
;
; void far    * from    Указатель на строку текста
; int         * pbeg    Адрес начала поля (должна быть проинициализирована 0)
; int         * plen    Адрес длины поля  (должна быть проинициализирована 0)
; int           numbr   Номер поля отн. from
;
;  Возвращает : Количество пройденных символов
;

include begasm.mac
beginProg dbfindbr

    from    equ    dword  ptr [bp + stkoff +  0]
    pbeg    equ    dword  ptr [bp + stkoff +  4]
    plen    equ    dword  ptr [bp + stkoff +  8]
    numbr   equ     word  ptr [bp + stkoff +  12]

    StartCode

        cld                            ; forward

        lds     si, plen
        mov     bx, word ptr ds:[si]  ; beg += len of prev field
        lds     si, from
        mov     cx, si
        xor     ah, ah
        jmp     short cyc2
cyc1:
        inc     bx
cyc2:
        lodsb
        cmp     al, 1
        jne     short cyc1
        lodsb
        mov     dl, al
        lodsb
        cmp     dl, 178
        je      short vfybr
cyc3:
        add     bx, ax
        jmp     short cyc2
vfybr:
        dec     numbr
        jz      short final
        jmp     short cyc3
final:
        les     di, plen
        mov     word ptr es:[di], ax

        les     di, pbeg
        add     word ptr es:[di], bx

        mov     ax, si
        sub     ax, cx

    FinishCode
endProg dbfindbr
end

