;           utcode - кодирует строку текста pfrom, помещая закодированное
;                    значение в pto. Pto должно быть в 2 раза больше pfrom.
;
;
;           void utcode (void far * pto, void far * pfrom, int len);
;
;

include begasm.mac
extrn      _rand:far

    pto      equ     dword  ptr [bp + stkoff +  0]
    pfrom    equ     dword  ptr [bp + stkoff +  4]
    len      equ      word  ptr [bp + stkoff +  8]

beginProg utcode

    StartCode

    cld

    mov     cx,     len
    cmp     cx,     0
    jle     final
    lds     si,     pfrom
    les     di,     pto
    mov     dx,     0FF0h

cycle:
    push    ds
    push    cx
    push    dx
    call    far ptr _rand
    pop     dx
    pop     cx
    pop     ds
    mov     bx,     ax      ; bx содержит случайное целое число
    and     ax,     dx      ; определяем, какие биты с 4 по 11 содержат 1-цы
    xor     bx,     ax      ; заменяем в этих битах единицы на нули
; теперь bx содержит целое число со случайными значениями в битах с 0 по 3 и
; с 12 по 15 и с нулевыми значениями в битах с 4 по 11
    lodsb
    xor     ah,     ah
    shl     ax,     1
    shl     ax,     1
    shl     ax,     1
    shl     ax,     1       ; сдвигаем код символа на 4 бита влево
    or      ax,     bx      ; заполняем биты с 0 по 3 и с 12 по 15 из bx
    not     ax              ; инвертируем значение ax
    stosw
    loop    cycle

final:
    FinishCode
endprog utcode

end
