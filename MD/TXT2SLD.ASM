include begasm.mac
extrn _swaptxt:far
extrn _readsld:far
;
; void txt2sld (int handto,   void far * pto,   int tosize,
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



beginProg storsym

    stosb
    cmp     di,  bx
    jne     short fin1
    call    _swaptxt
fin1:
    ret
endprog storsym

beginProg txt2sld

    StartCode

    cld

    les     di, pto
    mov     bx, di
    add     bx, tosize
    xor     dh, dh

    call    _readsld
    je      c7
;**************** BX contain begin offset to PTO ***********************
    lodsb
    xchg    al, ah
    dec     cx
    cmp     cx, 0
    jne     short getchfrom
    jmp     short final
getchfrom:
    lodsb

    cmp     al, ah
    jne     short c888
    cmp     al, 10           ; '\n'
    je      short stor
    inc     dh
    jmp     short cont
c888:
    xchg    al, ah
    cmp     al, 178          ; '²'
    je      short storbrick
    cmp     dh, 0
    jne     short  vfycount
stor:
    xor     dh,  dh
    call    _storsym
cont:
    loop short getchfrom
    call    _readsld
    jne     short getchfrom
    xchg    al, ah
    jmp     short final

vfycount:
    cmp     dh,  2
    jb      c555
storbrick:
    inc     dh
    mov     dl, al
    mov     al, 1
    call    _storsym
    mov     al, dl
    call    _storsym
    mov     al, dh
    call    _storsym
    xor     dh, dh
    jmp     short cont
c555:
    call    _storsym
    call    _storsym
    xor     dh, dh
    jmp     short cont
final:
    stosb
    call    _swaptxt
c7:
    FinishCode
endprog txt2sld

end
