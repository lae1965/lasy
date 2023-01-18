;         utrewchr () - Поиск символа, отличного от  sym,
;                                   в строке pbuf слева направо.
;
;      int  utrewchr (void * pbuf, int sym);
;
; Параметры:
;   void  * pbuf -    Указатель  на строку, в которой производится поиск.
;   int     sym  -    Символ, который необходимо промотать.
;
; Возврат:
;               Номер найденного символа, отличного от sym, от начала буфера.
;

include     begasm.mac

beginProg   utrewchr
    pbuf    equ  DPTR_    [bp + stkoff + 0]
    sym     equ  byte ptr [bp + stkoff + dPtrSize]

    StartCode

        cld

        mov     al,     sym
        les     di,     pbuf
        mov     bx,     di
lab:
        scasb
        je      short lab

        mov     ax,     di
        dec     ax
        sub     ax,     bx
    FinishCode
endProg     utrewchr

end
