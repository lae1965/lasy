include begasm.mac
extrn _swaptxt:far
extrn _readsld:far
;
; int makepn (int handto,   void far * pto,   int tosize,
;               int handfrom, void far * pfrom, long lenfrom, word fromsize);
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


    offl        equ     word  ptr [bp - 2]
    numstr      equ     word  ptr [bp - 4]

beginProg makepn

    StartCode     4

    cld

    les     di, pto
    mov     bx, di
    add     bx, tosize
    xor     dx, dx
    xor     ax, ax
    mov     numstr, ax
    mov     offl, ax

    call    _readsld
    je      c7
    stosw

;**************** BX contain end offset to PTO ***********************
getchfrom:
    lodsb
    inc     dx
    cmp     al, 1
    je      short specsym
    cmp     al, 10      ; '\n'
    je      short lf
cont:
    loop short getchfrom
    call    _readsld
    jne     short getchfrom
    jmp     short final
specsym:
    mov     ah, 2
    jmp     short cont1
getch:
    lodsb
    inc     dx
    dec     ah
    jz      short cont
cont1:
    loop short getch
    call    _readsld
    jne     short getch
    jmp     short final
lf:
    inc     numstr
    add     offl, dx
    xor     dx,   dx
    mov     ax,   offl
    stosw
    cmp     bx,     di
    ja      short cont
    call    _swaptxt
    jmp     short cont
final:
    call    _swaptxt
    inc     numstr
c7:
    mov     ax, numstr

    FinishCode
endprog makepn

end
