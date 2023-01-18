include begasm.mac
;
; void sld2txt (int handto,   void far * pto,   int tosize,
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



beginProg swaptxt
    push    ax
    push    bx
    push    cx
    push    dx
    push    ds

    mov     ah, 40H
    mov     bx, handto
    lds     dx, pto
    mov     cx, di
    sub     cx, dx
    int     21H

    mov     di, dx

    pop     ds
    pop     dx
    pop     cx
    pop     bx
    pop     ax

    ret
endprog swaptxt

beginProg readsld
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
    mov     ah, 3fH
    lds     dx, pfrom       ; utread (handfrom, pfrom, cx);
    int     21H
;**************** AH contain 0 if last in my stack is symbol ***********
;**************** CX contain size of PFROM *****************************
    lds     si, pfrom

c888:

    pop     ax
    pop     bx
    pop     dx
    cmp     cx, 0
    ret
endprog readsld

beginProg sld2txt

    StartCode

    cld

    les     di, pto
    mov     bx, di
    add     bx, tosize
    mov     dl, 1

    call    _readsld
    je      c7
;**************** BX contain begin offset to PTO ***********************
getchfrom:
    lodsb

    cmp     al, dl      ; Mult code
    je      short specsym
stor:
    stosb
    cmp     di,  bx
    je      short swptxt
cont:
    loop short getchfrom
    call    _readsld
    jne     short getchfrom
    jmp     short final
specsym:
    mov     dh, 2
    jmp     short   c1000
roll:
    cmp     dh, 0
    je      short c2000
    lodsb
    xchg    al, ah     ; ah - symbol   ; al - count
    dec     dh
c1000:
    loop    short roll
    call    _readsld
    je      short final
    jmp     short roll
c2000:
    push    cx
    xor     cx, cx
    mov     cl, ah
cycle:
    stosb
    cmp     di, bx
    je      short c100
c200:
    loop    short cycle
    jmp     short   c300
c100:
    call    _swaptxt
    jmp     short   c200
c300:
    pop    cx
    jmp     short getchfrom
swptxt:
    call    _swaptxt
    jmp     short   cont
final:
    call    _swaptxt
c7:
    FinishCode
endprog sld2txt

end
