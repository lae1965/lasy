include begasm.mac
extrn      _swapto:far
extrn      _readfrom:far
;
; int edtrdcur (int handto,   void far * pto,   int tosize,
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

beginProg edtrdcur

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

    mov     ah, 10     ; \n
    call    _readfrom
    jne     short getchfrom
    jmp     c7
;**************** BX contain begin offset to PTO ***********************
getchfrom:
    lodsb

    cmp     al, ah      ; '\n'
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
swpto:
    call    _swapto
    jmp     short   cont
lf:
    add     totall, 1
    adc     totalh, 0
    jmp     short   stor
final:
    sub     di, bx
    inc     di
    cmp     di, tosize
    je      c6
    mov     ax, di
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
endprog edtrdcur

end
