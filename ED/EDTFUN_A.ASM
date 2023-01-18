include begasm.mac
;          extern  symbols
        extrn   _swapright:far
        extrn   _swapleft:far
        extrn   _loadleft:far
        extrn   _loadright:far
        extrn   _Ed_pright:dword
        extrn   _Ed_pleft:dword
        extrn   _Ed_pnr:dword
        extrn   _Ed_pnl:dword
        extrn   _Ed_pr:dword
        extrn   _Ed_pl:dword
        extrn   _Ed_pnrlast:dword

;
; loadlt - Load text to left bufer with filtered
;
; int loadlt (char far **p, char far *pl, char far **q,
;               long leftoff, long far *offst, char far **savplf,
;               char far *pw,  unsigned num, unsigned frsize, int first);
;
;

beginProg loadlt

    pp      equ     dword ptr [bp + stkoff +  0]
    pl      equ      word ptr [bp + stkoff +  4]     ;OFFSET IN SEG TO PR
    pq      equ     dword ptr [bp + stkoff +  8]
    lltoff  equ      word ptr [bp + stkoff +  12]
    hltoff  equ      word ptr [bp + stkoff +  14]
    offst   equ     dword ptr [bp + stkoff +  16]
    savplf  equ     dword ptr [bp + stkoff +  20]
    pw      equ     dword ptr [bp + stkoff +  24]
    num     equ     word  ptr [bp + stkoff +  28]
    polsize equ     word  ptr [bp + stkoff +  30]
    first   equ     word  ptr [bp + stkoff +  32]

    p       equ     dword ptr [bp - 4]
    hoffst  equ      word ptr [bp - 6]
    loffst  equ      word ptr [bp - 8]
    tsavplf equ      word ptr [bp - 10]

    StartCode   10

        mov     cx,     num                ; cx contain size of work bufer
        lds     si,     pp
        mov     ax,     word ptr ds:[si]
        mov     word ptr [bp-4], ax         ; offset  p
        mov     ax,     word ptr ds:[si+2]
        mov     word ptr [bp-2], ax         ; segment p

        lds     si,     pq
        mov     dx,     word ptr ds:[si]    ; offset  q

        lds     si,     savplf
        mov     bx,     word ptr ds:[si]    ; ini tsavplf
        mov     tsavplf , bx

        lds     si,     offst
        mov     bx,     word ptr ds:[si]    ; ini toffst
        mov     loffst, bx
        mov     bx,     word ptr ds:[si+2]
        mov     hoffst, bx


        lds     si,     pw                 ; ds:si point to work  bufer
        les     di,     p                  ; es:di point to left bufer
        mov     bx,     pl                 ; bx contain offset to pl

        std                                ; backward
cycle:
        lodsb

        cmp     al,     10                  ; \n
        je      newline
        stosb
continue:
        loop    cycle

        mov     ax,     0
        jmp     final
newline:
        mov     ax,     lltoff
        mov     loffst, ax
        mov     ax,     hltoff
        mov     hoffst, ax
        add     loffst, cx
        adc     hoffst, 0

        mov     tsavplf,di

        cmp     first,  1
        je      fstclr

        mov     ax,     di                       ;  p
        sub     ax,     bx                       ;  pe->pl
        inc     ax                               ;  ++

        push    di
        mov     di,     dx                       ; q
        stosw                                    ; *q = p - pe->pl + 1
        add     dx,     2
        pop     di
connl:
        mov     al,     10
        stosb
        mov     ax,     dx
        sub     ax,     di
        cmp     ax,     polsize                    ; if (q - p >= polsize)
        jae     full
        jmp     continue
fstclr:
        mov     first,  0
        jmp     connl
full:
        mov     ax,     1
final:
        lds     si,     savplf
        mov     bx,     tsavplf
        mov     word ptr ds:[si], bx           ; save savplf

        lds     si,     offst
        mov     bx,     loffst
        mov     word ptr ds:[si], bx           ; ini toffst
        mov     bx,     hoffst
        mov     word ptr ds:[si+2], bx

        lds     si, pp
        mov     word ptr ds:[si], di           ; p real point to bufer now
        lds     si, pq
        mov     word ptr ds:[si], dx           ; q real point to bufer now

    FinishCode
endprog loadlt

;
; loadrt - load text to right bufer with filtered
;
;        int loadrt (char far **p, char far *pr, char far **q,
;          int far *nbank, int far *rightoff, char far **savplf,
;          char far *pw,  int num, unsigned frsize, int newbank, int newoff);
;
;

beginProg loadrt

    pp      equ     dword ptr [bp + stkoff +  0]
    pr      equ      word ptr [bp + stkoff +  4]     ;offset in seg to pr
    pq      equ     dword ptr [bp + stkoff +  8]
    bank    equ     dword ptr [bp + stkoff +  12]
    rtoff   equ     dword ptr [bp + stkoff +  16]
    savplf  equ     dword ptr [bp + stkoff +  20]
    pw      equ     dword ptr [bp + stkoff +  24]
    num     equ     word  ptr [bp + stkoff +  28]
    polsize equ     word  ptr [bp + stkoff +  30]
    newbank equ     word  ptr [bp + stkoff +  32]
    newoff  equ     word  ptr [bp + stkoff +  34]

    p       equ     dword ptr [bp - 4]
    chbank  equ      word ptr [bp - 6]
    trtoff  equ      word ptr [bp - 8]
    tsavplf equ      word ptr [bp - 10]


    StartCode   10

        mov     cx,     num                ; cx contain size of work bufer
        lds     si,     pp
        mov     ax,     word ptr ds:[si]
        mov     word ptr [bp-4], ax    ; offset  p
        mov     ax,     word ptr ds:[si+2]
        mov     word ptr [bp-2], ax    ; segment p

        lds     si,     pq
        mov     dx,     word ptr ds:[si]    ; offset  q

        lds     si,     savplf
        mov     bx,     word ptr ds:[si]    ; ini tsavplf
        mov     tsavplf , bx

        mov     bx,     newoff
        inc     bx
        add     num,    bx                    ; num contain buf_wrk

        mov     chbank, 0

        lds     si,     pw                 ; ds:si point to work  bufer
        les     di,     p                  ; es:di point to right bufer
        mov     bx,     pr                 ; bx contain offset to pr

        cld                            ; forward
cycle_:
        lodsb

        cmp     al,     10                  ; \n
        je      newline_
        stosb

continue_:
        loop    cycle_

        mov     ax,     0                   ; work buffer is done
        jmp     final_
newline_:
        mov     chbank,  1

        mov     trtoff, cx
        mov     tsavplf,di

        mov     ax,     bx                      ; pe->pr
        sub     ax,     di                      ; p
        dec     ax                              ; --

        push    di
        sub     dx,     2
        mov     di,     dx
        stosw                                     ; *q = pe->pr - p - 1

        pop     di
        mov     al,     10
        stosb

        mov     ax,     di
        sub     ax,     dx
        cmp     ax,     polsize                    ; if (p - q >= polsize)
        jae     full_
        jmp     continue_
full_:
        mov     ax,     1
final_:
        lds     si,     savplf
        mov     bx,     tsavplf
        mov     word ptr ds:[si], bx           ; save savplf

        lds     si, pp
        mov     word ptr ds:[si], di           ; p real point to bufer now
        lds     si, pq
        mov     word ptr ds:[si], dx           ; q real point to bufer now

        cmp     chbank, 0
        je      final2_

        lds     si,     rtoff
        mov     bx,     num
        sub     bx,     trtoff                 ; save rtoff
        mov     word ptr ds:[si], bx

        lds     si,     bank
        mov     bx,     newbank                 ; save bank
        mov     word ptr ds:[si], bx
final2_:

    FinishCode
endProg loadrt

;
; movleft - Move charachter from left bufer to right (Cursor moves left)
;
;                   int movleft (EDT *pe, long far *line, long count);
;

getvar  macro
        mov     ax,     word ptr ds:_Ed_pright
        mov     ltpright, ax                 ; offset  pright
        mov     ax,     word ptr ds:_Ed_pright+2
        mov     htpright, ax                 ; segment pright

        mov     ax,     word ptr ds:_Ed_pleft
        mov     ltpleft, ax                 ; offset  pleft
        mov     ax,     word ptr ds:_Ed_pleft+2
        mov     htpleft, ax                 ; segment pleft

        mov     dx,     word ptr ds:_Ed_pnr    ; dx = offset  pnr

        mov     bx,     word ptr ds:_Ed_pnl    ; offset  pnl
        mov     tpnl,   bx

        les     di,     line
        mov     bx,     word ptr es:[di]      ; ini tline
        mov     ltline, bx
        mov     bx,     word ptr es:[di+2]    ; ini tline
        mov     htline, bx

        mov     bx,     word ptr ds:_Ed_pr       ; bx contain offset to pr

        lds     si,     dword ptr [bp - 16]  ; ds:si point to left  bufer
        les     di,     dword ptr [bp - 12]  ; es:di point to right bufer
endm
savevar macro
        push    saveds
        pop     ds

        mov     word ptr ds:_Ed_pleft, si    ; pleft real point to bufer now
        mov     word ptr ds:_Ed_pright, di   ; pright real point to bufer now
        mov     word ptr ds:_Ed_pnr, dx      ; pnr real point to bufer now

        mov     bx,     tpnl
        mov     word ptr ds:_Ed_pnl, bx      ; pnl real point to bufer now

        les     di, line
        mov     bx,   ltline
        mov     word ptr es:[di],   bx
        mov     bx,     htline
        mov     word ptr es:[di+2], bx     ; restore line
endm

beginProg movleft

    pe1     equ      word ptr [bp + stkoff +  2]
    pe2     equ      word ptr [bp + stkoff +  0]
    line    equ     dword ptr [bp + stkoff +  4]
    lcount  equ     word  ptr [bp + stkoff +  8]
    hcount  equ     word  ptr [bp + stkoff + 10]

    htline  equ     word  ptr [bp - 2]
    ltline  equ     word  ptr [bp - 4]
    tpnl    equ     word  ptr [bp - 6]
    first   equ     word  ptr [bp - 8]
    htpright equ    word  ptr [bp - 10]
    ltpright equ    word  ptr [bp - 12]
    htpleft equ     word  ptr [bp - 14]
    ltpleft equ     word  ptr [bp - 16]
    saveds  equ     word  ptr [bp - 18]

    StartCode   18

        push    ds
        pop     saveds              ; saveds contain previos value ds
        mov     ax,     hcount             ; look high part of count
        cmp     ax,     0
        je      aaaa0                      ; count is sign - error
        jmp     end10
aaaa0:
        mov     cx,     lcount             ; cx contain count
        inc     cx
        add     lcount, 20

        getvar

        mov     first,  1
        mov     ax,     di
        sub     ax,     dx
        cmp     ax,     lcount        ; if (pe->pright - pe->pnr < count + 20)
        jb     needswp0

cycle0:
        mov     al, byte ptr ds:[si]
        dec     si                        ; *(--pe->pright) = *pe->pleft--
        dec     di
        mov     byte ptr es:[di], al

        cmp     al,     10                  ; \n
        jne     continue0

        mov     first,  0
        cmp     ltline, 0
        je      cccc0
        jmp     dddd0
cccc0:
        dec     htline
dddd0:
        dec     ltline

        add     tpnl,   2

        mov     ax,     bx                      ; pe->pr
        sub     ax,     di                      ; pe->prihgt
        dec     ax                              ; --

        push    di

        mov     di,     dx
        mov     word ptr es:[di], ax           ; *pnr = pe->pr - pe->pright - 1
        add     dx,     2                      ; pe->pnr++

        pop     di

        mov     ax,     di
        sub     ax,     dx
        cmp     ax,     lcount        ; if (pe->pright - pe->pnr < count + 20)
        jb     needswp0
continue0:
        loop    cycle0
        jmp     final0
needswp0:
        push    cx
        savevar
        mov     ax,     0
        push    ax
        push    pe1
        push    pe2
        call    far ptr _swapright
        add     sp,     6
        getvar
        pop     cx
        cmp     first, 1
        je      bbbb0
        jmp     continue0
bbbb0:
        jmp      cycle0
final0:
        savevar
        mov     ax,     1
        cmp     si,     word ptr ds:_Ed_pl
        jae     end10
        push    pe1
        push    pe2
        call    far ptr _loadleft
        pop     cx
        pop     cx
end10:

    FinishCode
endprog movleft

;
; movright - Move charachter from right bufer to left (Cursor moves right)
;
;           int movright (EDT *pe, long far *line, long count);
;

getvar  macro
        mov     ax,     word ptr ds:_Ed_pright
        mov     ltpright, ax                 ; offset  pright
        mov     ax,     word ptr ds:_Ed_pright+2
        mov     htpright, ax                 ; segment pright

        mov     ax,     word ptr ds:_Ed_pleft
        mov     ltpleft, ax                  ; offset  pleft
        mov     ax,     word ptr ds:_Ed_pleft+2
        mov     htpleft, ax                  ; segment pleft

        mov     dx,     word ptr ds:_Ed_pnl     ; dx = offset  pnl

        mov     bx,     word ptr ds:_Ed_pnr     ; offset  pnr
        mov     tpnr,   bx

        les     di,     line
        mov     bx,     word ptr es:[di]     ; ini tline
        mov     ltline, bx
        mov     bx,     word ptr es:[di+2]   ; ini tline
        mov     htline, bx

        mov     bx,     word ptr ds:_Ed_pl      ; bx contain offset to pl

        lds     si,     dword ptr [bp - 16]  ; ds:si point to left  bufer
        les     di,     dword ptr [bp - 12]  ; es:di point to right bufer
endm
savevar macro
        push    saveds
        pop     ds

        mov     word ptr ds:_Ed_pright, di  ; pright real point to bufer now
        mov     word ptr ds:_Ed_pleft, si   ; pleft real point to bufer now
        mov     word ptr ds:_Ed_pnl, dx     ; pnl real point to bufer now

        mov     bx,     tpnr
        mov     word ptr ds:_Ed_pnr, bx         ; pnr real point to bufer now

        les     si, line
        mov     bx,   ltline
        mov     word ptr es:[si],   bx
        mov     bx,     htline
        mov     word ptr es:[si+2], bx        ; restore line
endm

beginProg movright

    pe1     equ      word ptr [bp + stkoff +  2]
    pe2     equ      word ptr [bp + stkoff +  0]
    line    equ     dword ptr [bp + stkoff +  4]
    lcount  equ     word  ptr [bp + stkoff +  8]
    hcount  equ     word  ptr [bp + stkoff + 10]

    htline  equ     word  ptr [bp - 2]
    ltline  equ     word  ptr [bp - 4]
    tpnr    equ     word  ptr [bp - 6]
    first   equ     word  ptr [bp - 8]
    htpright equ    word  ptr [bp - 10]
    ltpright equ    word  ptr [bp - 12]
    htpleft equ     word  ptr [bp - 14]
    ltpleft equ     word  ptr [bp - 16]
    saveds  equ     word  ptr [bp - 18]

    StartCode   18

        push    ds
        pop     saveds              ; saveds contain previos value ds
        mov     ax,     hcount             ; look high part of count
        cmp     ax,     0
        je      aaaa2                      ; count is sign - error
        jmp     end12
aaaa2:
        mov     cx,     lcount             ; cx contain count
        inc     cx
        add     lcount, 20

        getvar

        mov     first,  1
        mov     ax,     dx
        sub     ax,     si
        cmp     ax,     lcount        ; if (pe->pnl - pe->pleft < count + 20)
        jb     needswp2

cycle2:
        mov     al, byte ptr es:[di]
        inc     di                        ; *(++pe->pleft) = *pe->pright++
        inc     si
        mov     byte ptr ds:[si], al

        cmp     al,     10                  ; \n
        jne     continue2

        mov     first,  0

        add     ltline, 1                 ; pe->line++;
        adc     htline, 0

        sub     tpnr,   2                 ; pe->pnr--
        sub     dx,     2                 ; pe->pnl--

        mov     ax,     si                      ; pe->pleft
        sub     ax,     bx                      ; pe->pl
        inc     ax                              ; ++

        push    si

        mov     si,     dx
        mov     word ptr ds:[si], ax      ; *pe->pnl = pe->pleft - pe->pl + 1;

        pop     si

        mov     ax,     dx
        sub     ax,     si
        cmp     ax,     lcount        ; if (pe->pnl - pe->pleft < count + 20)
        jb     needswp2
continue2:
        loop    cycle2
        jmp     final2
needswp2:
        push    cx
        savevar
        mov     ax,     0
        push    ax
        push    pe1
        push    pe2
        call    far ptr _swapleft
        add     sp,     6
        getvar
        pop     cx
        cmp     first, 1
        je      bbbb2
        jmp     continue2
bbbb2:
        jmp      cycle2
final2:
        savevar
        mov     ax,     1
        cmp     di, word ptr ds:_Ed_pr    ; if (pe->pright > pe->pr)
        jb      end12
        push    pe1
        push    pe2
        call    far ptr _loadright
        pop     cx
        pop     cx
end12:

    FinishCode
endprog movright

;
; outcur - Fill buffer with characters and attr from current string
;
; void outcur (char far *to, char attr, int visicol, int w);
;
beginProg outcur

    to      equ     dword ptr [bp + stkoff +  0]
    attr    equ     byte  ptr [bp + stkoff +  4]
    visicol equ     word  ptr [bp + stkoff +  6]
    w       equ     word  ptr [bp + stkoff +  8]

    htpright equ    word  ptr [bp - 2]
    ltpright equ    word  ptr [bp - 4]
    tpright  equ   dword  ptr [bp - 4]
    htpleft  equ    word  ptr [bp - 6]
    ltpleft  equ    word  ptr [bp - 8]
    tpleft   equ   dword  ptr [bp - 8]

    StartCode 8

        cld                            ; forward

        mov     ax,     word ptr ds:_Ed_pright
        mov     ltpright, ax                 ; offset  pright
        mov     ax,     word ptr ds:_Ed_pright+2
        mov     htpright, ax                 ; segment pright

        mov     ax,     word ptr ds:_Ed_pl
        mov     ltpleft, ax                  ; offset  pl
        mov     ax,     word ptr ds:_Ed_pl+2
        mov     htpleft, ax                  ; segment pl

        mov     bx,     word ptr ds:_Ed_pleft
        mov     di,     word ptr ds:_Ed_pnl

        lds     si, tpleft
        add     si, word ptr ds:[di]
        add     si, visicol

        les     di, to

        mov     cx, w
        mov     ah, attr
        cmp     si, bx
        ja      next23

froml3:
        lodsb
        stosw
        cmp     si, bx
        ja      next23
        loop    froml3
next23:
        lds     si, tpright
fromr3:
        lodsb
        cmp     al, 10          ; '\n'
        je      next33
        stosw
        loop    fromr3
next33:
        mov     al, 32              ; ' '
        rep     stosw

    FinishCode
endProg outcur

;
; outpim - Fill buffer with characters and attr.
;
; void outpim (char far *to, char far *from, int attr, int num, int w);
;
beginProg outpim

    to      equ     dword ptr [bp + stkoff +  0]
    from    equ     dword ptr [bp + stkoff +  4]
    attr    equ     byte  ptr [bp + stkoff +  8]
    num     equ     word  ptr [bp + stkoff +  10]
    w       equ     word  ptr [bp + stkoff +  12]

    StartCode

        lds     si, from
        les     di, to
        mov     ah, attr
        cmp     num,    0
        je      afterlf4
        mov     cx, num

        cld                            ; forward
beforlf4:
        lodsb
        stosw
        loop beforlf4
afterlf4:
        mov     al, 32              ; ' '
        mov     cx, w
        sub     cx, num
        rep     stosw

   FinishCode

endProg outpim

end
