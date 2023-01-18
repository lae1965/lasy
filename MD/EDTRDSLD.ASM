include begasm.mac
;
; int edtrdsld (int handto,   void far * pto,   int tosize,
;               int handfrom, void far * pfrom, long lenfrom, word fromsize,
;                                                           long * total);
;
;
    handto      equ     word  ptr [bp + stkoff +  0]
    pto         equ    dword  ptr [bp + stkoff +  2]
    tosize      equ     word  ptr [bp + stkoff +  6]
    handfrom    equ     word  ptr [bp + stkoff +  8]
    pfrom       equ    dword  ptr [bp + stkoff + 10]
    lenfroml    equ     word  ptr [bp + stkoff + 14]
    lenfromh    equ     word  ptr [bp + stkoff + 16]
    fromsize    equ     word  ptr [bp + stkoff + 18]
    ptotal      equ    dword  ptr [bp + stkoff + 20]

    totalh      equ     word  ptr [bp - 2]
    totall      equ     word  ptr [bp - 4]
    first       equ     word  ptr [bp - 6]

beginProg swapto
    push    ax
    push    bx
    push    cx
    push    dx
    push    ds

    mov     ah, 40H
    mov     bx, handto
    lds     dx, pto
    mov     cx, tosize
    int     21H

    add     di, tosize

    pop     ds
    pop     dx
    pop     cx
    pop     bx
    pop     ax

    ret
endprog swapto

beginProg seekfrom
    push    cx

    mov     ax, 4201H       ; utlseek (handfrom, - cx, UT_CUR);
    mov     dx, cx
    neg     dx
    mov     cx, -1
    int     21H

    pop     cx

    ret
endprog seekfrom

beginProg readfrom
    push    dx
    push    bx
    push    ax

    mov     cx, fromsize
    cmp     lenfromh, 0     ;Calculate remain for later reading
    jne     short above
    cmp     lenfroml, cx
    ja      short above
    mov     cx,  lenfroml
    cmp     cx,  0
    je      short c888
above:
    sub     lenfroml, cx
    sbb     lenfromh, 0

    mov     bx, handfrom
    call    _seekfrom
    mov     ah, 3fH
    lds     dx, pfrom       ; utread (handfrom, pfrom, cx);
    int     21H
    call    _seekfrom
;**************** AH contain 0 if last in my stack is symbol ***********
;**************** CX contain size of PFROM *****************************
    lds     si, pfrom
    add     si, cx
    dec     si

    cmp     first, YES
    jne     short c888
    mov     first,  0
    cmp     byte ptr ds:[si], 10
    je      short c888
    inc     si
    inc     cx
    mov     byte ptr ds:[si], 10
    jmp     short c888
c888:
    pop     ax
    pop     bx
    pop     dx
    cmp     cx, 0
    ret
endprog readfrom

beginProg mulch
    push    cx

    xor     cx, cx
    mov     cl, al
    mov     al, dh

cycle:
    stosb
    cmp     di, bx
    jb      short c100
c200:
    loop    short cycle
    jmp     short   c300
c100:
    call    _swapto
    jmp     short   c200
c300:
    pop    cx

    ret
endprog mulch

beginProg edtrdsld

    StartCode  6

    std

    mov     totall, -1
    mov     totalh, -1
    mov     first,  YES

    mov     ax, 4201H
    mov     bx, handfrom
    mov     cx, lenfromh    ;Replace logic file position to end of text
    mov     dx, lenfroml
    int     21H

    les     di, pto
    mov     bx, di
    add     di, tosize
    dec     di

    mov     ah, 2
    call    _readfrom
    jne     short roll
    jmp     c7
;**************** BX contain begin offset to PTO ***********************
getchfrom:
    lodsb

    cmp     al, 1       ; Mult code
    je      short specsym
movch:                    ; if (dl > 13)
    xchg    dh, dl     ; dl - current symbol
    xchg    al, dl     ; dh - prev symbol   ; al - prevprev symbol
    cmp     al, 10     ; '\n'
    je      lf
stor:
    stosb
    cmp     di,  bx
    jb      short swpto
cont:
    loop short getchfrom
    call    _readfrom
    jne     short getchfrom
    jmp     short final
specsym:
    xchg    dh, dl     ; dl - current symbol
    xchg    al, dl     ; dh - prev symbol   ; al - prevprev symbol
    call    _mulch
    mov     ah, 2
    jmp     short c1000
roll:
    cmp     ah, 0
    je      short getchfrom
    lodsb
    xchg    dh, dl     ; dl - current symbol
    xchg    al, dl     ; dh - prev symbol   ; al - prevprev symbol
    dec     ah
c1000:
    loop    short roll
    call    _readfrom
    je      short final
    jmp     short roll
swpto:
    call    _swapto
    jmp     short   cont
lf:
    add     totall, 1
    adc     totalh, 0
    jmp     short stor
final:
    cmp     ah,  2
    jne     c5
    sub     di, bx
    inc     di
    cmp     di, tosize
    je      c6
    mov     ax, di
    call    _swapto
    jmp     short   c7
c5:
    cmp     ah, 1
    je      short   c8
    mov     byte ptr es:[di], dh
    dec     di
    cmp     di,  bx
    jae     c8
    call    _swapto
c8:
    mov     byte ptr es:[di], dl
    mov     ax, di
    sub     ax, bx
    call    _swapto
    jmp     short c7
c6:
    xor     ax, ax
c7:
    lds     si, ptotal
    mov     bx, totall
    mov     word ptr ds:[si], bx
    mov     bx, totalh
    mov     word ptr ds:[si+2], bx
    FinishCode
endprog edtrdsld

end
