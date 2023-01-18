;       utc2lrdx () - преобразует строку в число типа long.
;
;              bool  utc2lrdx (char ** pp, long * num, int base);
;
;   Параметы:
;         char  ** pp   -  адрес указателя на преобразуемую строку;
;         long   * num  -  указатель на преобразуемое число типа long;
;         int      base -  основание системы счисления;
;
;   Возвращает:
;         YES       -      строка преобразована в число;
;         NO        -      число в строке не найдено;
;

include  begasm.mac

beginProg utc2lrdx

    pp      equ  DPTR_       [bp + stkoff + 0]
    num     equ  DPTR_       [bp + stkoff + dPtrSize]
    base    equ   word  ptr  [bp + stkoff + dPtrSize*2]

    negnum  equ  word   ptr  [bp - 2]
    wasdig  equ  word   ptr  [bp - 4]

    StartCode  4

        mov     cx,     base
        xor     ax,     ax
        mov     wasdig, ax
        mov     negnum, ax

        LES_    di,     pp                  ; ES:DI contain addr of ptr p
        LDS_    si,     DPTR_ ES_[di]       ; DS:SI contain ptr p

        cld

        xor     bx,     bx                    ; Now BX contain low part of num
        xor     di,     di                    ; Now DI contain high part of num

continue:
        cmp     ah,     1
        jg      final
circle:
        lodsb
        cmp     al,     0                       ; '\0'
        je      final
        cmp     al,     32                      ; ' '
        je      circle
        inc     ah

        cmp     al,     43                      ; '+'
        je      continue

        cmp     al,     45                      ; '-'
        jne     convert
        mov     negnum,    YES
        jmp     short continue

convert:
        xor     ah,     ah
        cmp     al,     48                      ; "0"
        jl      final

        cmp     al,     57                      ; "9"
        jg      basevfy
cont2:
        sub     al,     48

        cmp     al,     cl
        jge     final

        mov     wasdig, YES

        push    ax
        mov     ax,     di                      ; high part of num
        imul    cx                              ; mult h.p. to base
        jo      errover1

        mov     di,     ax
        mov     ax,     bx                      ; low part of num
        xor     dx,     dx
        mul     cx                              ; mult l.p. to base
        mov     bx,     ax
        add     di,     dx                      ; add overflow from l.p.
        jo      errover1

        pop     ax
        add     bx,     ax                      ; add new value from string
        adc     di,     0
        jo      errover

        lodsb
        jmp     short convert

basevfy:
        cmp     al,     65
        jl      short final
        sub     al,     7
        jmp     short cont2
errover1:
        pop     ax
errover:
        mov     wasdig, NO

;*****************************************************************************
final:
        mov     ax,     bx
        mov     dx,     di

        dec     si                          ; return on the prev pos of ptr p

        LES_    di,     pp                  ; ES:DI contain addr of ptr p
        mov     word ptr ES_[di],  si       ; Store new ptr p

        cmp     negnum, YES
        jne     cont3
        neg     dx
        neg     ax
        sbb     dx,     0
cont3:
        LES_    di,     num                 ; ES:DI contain addr num
        mov     word ptr ES_[di],       ax  ; Store maked value
        mov     word ptr ES_[di + 2],   dx

        mov     ax,     wasdig

    FinishCode
endProg utc2lrdx

end
