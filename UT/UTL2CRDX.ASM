;       utl2crdx () - преобразует число типа long в строку
;
;              int  utl2crdx (long num, char * p, int wid, int base);
;
;   Параметы:
;         long    num  -  преобразуемое число типа long;
;         char  * p    -  указатель на буфер;
;         int     wid  -  ширина выводимого поля & признак выравнивания
;                    = 0  Выравнивание влево
;                    > 0  Выравнивание вправо (вед. пробелы)
;                    < 0  Выравнивание вправо (ведущие нули)
;                         Если число отрицательное, знак "-" помещается в
;                         начало буфера в любом случае;
;         int     base -  основание системы счисления;
;
;   Возвращает:
;               длина строки;

include  begasm.mac

beginProg utl2crdx

    lnum    equ   word  ptr  [bp + stkoff + 0]
    hnum    equ   word  ptr  [bp + stkoff + 2]
    p       equ  DPTR_       [bp + stkoff + 4]
    wid     equ   word  ptr  [bp + stkoff + dPtrSize + 4]
    base    equ   word  ptr  [bp + stkoff + dPtrSize + 6]

    negnum  equ  word   ptr  [bp - 2]

    StartCode  2

        mov     negnum, 0
        mov     cx,     base

        mov     bx,     lnum                ; SI:BX contain new value of num
        mov     si,     hnum
        LES_    di,     p
        cld
        cmp     si,     0
        jge     cont0
        neg     si
        neg     bx
        sbb     si,     0
        mov     negnum, 1
        mov     al,     45      ; '-'
        stosb
cont0:
        push    di                              ; save begin digits
cont1:
        mov     ax,     si                      ;high word of value
        xor     dx,     dx                      ;clear for divide
        div     cx                              ;divide, DX gets remainder
        mov     si,     ax                      ;save quotient (new high word)
        mov     ax,     bx                      ;low word of value
        div     cx                              ;divide, DX gets remainder
        mov     bx,     ax                      ;save quotient (new low word)
        cmp     dl,     9
        jle     cont2
        add     dl,     7
cont2:
        add     dl,     48          ; +"0"
        mov     al,     dl
        stosb                                   ;store
        cmp     bx,     0                       ;check if low word zero
        jne     cont1                           ;jump if not
        cmp     si,     0                       ;check if high word zero
        jne     cont1                           ;jump if not

        pop     si                              ;restore start of string
        mov     cx,     di
        sub     cx,     si
    ;
        mov     dx,     wid
        cmp     dx,     0
        je      cont7
        jl      cont8
        mov     al,     32  ; ' '
        jmp     short cont9
cont8:
        neg     dx
        mov     al,     48  ; "0"
cont9:
        sub     dx,     negnum
        mov     wid,    dx

        sub     dx,     cx
        cmp     dx,     0
        jle     cont7
        mov     cx,     dx
cont10:
        stosb
        loop    cont10
        mov     cx,     wid
cont7:
        xor     al,     al
        stosb                                   ;store the terminator
;----
        mov     bx,     cx

        shr     cx,     1
        jz      cont4
        sub     di,     2
        xchg    di,     si

cont5:
        mov     al, byte ptr ES_[di]            ;load front character
        xchg    al, byte ptr ES_[si]            ;swap with end character
        stosb                                   ;store new front character
        dec     si                              ;back up
        loop    cont5                           ;loop back for each digit

;--- finished

cont4:
        mov     ax,     bx
        add     ax,     negnum

    FinishCode
endProg utl2crdx

end
