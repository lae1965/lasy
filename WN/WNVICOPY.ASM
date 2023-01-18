;
; wnvicopy - Read or write rectangle directly from or to video adapter
;
; bool wnvicopy (char far **ppfrom, char far **ppto, int num_rows,
;                int row_length, int scn_width, int attr, int option);
;
; char far **   ppfrom          Pointer to far char pointer containing
;                               address of source buffer
; char far **   ppto            Pointer to far char pointer containing
;                               address of destination buffer
; int           num_rows        Number of rows in the rectangle
; int           row_length      Width of the rectangle (in columns)
; int           scn_width       Twice the screen width (80 or 160 bytes)
; int           attr            Foreground and background attributes
; int           option          Style and direction of I/O; also
;                               whether interference is to be prevented.
;                               See discussion and table below.
;
; Description   This function reads or writes a rectangular buffer
;               directly from or to the video memory on an IBM
;               Color/Graphics Adapter, Monochrome Adapter, or
;               compatible adapter.  It can do so with attention to the
;               horizontal and vertical retrace intervals for maximum
;               throughput without interference.  The characters are
;               transferred row by row.
;
;               The buffer addresses are stored in far char pointers
;               pointed to by ppfrom and ppto.  Either or both of these
;               may point to physical locations in video memory,
;               depending on the value of option.
;
;               Several styles of I/O are available.  They are selected
;               by the value of option.  Bit 15 of option, if set,
;               indicates that the transfer is to be performed without
;               regard to interference.  The remaining bits of option
;               form an integer which should be selected from the
;               following table:
;
;               Option  Description
;               ------  -----------
;               0       Write characters to the screen without attributes.
;                       *ppfrom points to a buffer of characters.
;               1       Write characters to the screen with attributes.
;                       *ppfrom points to a buffer of (char,attr) pairs.
;               2       Write characters to the screen with constant
;                       attribute specified by attr.
;                       *ppfrom points to a buffer of characters.
;               3       Fill screen rectangle with constant char & attr.
;                       *ppfrom points to the character.
;                       Attr specifies the attribute.
;               4       Fill screen rectangle with constant attribute
;                       without altering characters displayed.
;                       ppfrom is ignored.
;               5       Read characters from screen without attributes.
;                       **ppto is filled with the characters.  (Note
;                       that no trailing NUL is added.)
;               6       Read characters from screen with attributes.
;                       **ppto is filled with (char,attr) pairs.
;               7       Copy window (with attributes) upward or directly
;                       leftward on same display page.
;               8       Copy window (without attributes) upward or
;                       directly leftward on same display page.
;               9       Copy window (with attributes) downward or
;                       directly rightward on same display page.
;               10      Copy window (without attributes) downward or
;                       directly rightward on same display page.
;               11      Copy window (with attributes) to non-overlapping
;                       window or to another display page.
;               12      Copy window (without attributes) to non-
;                       overlapping window or to another display page.
;
;               Beware:  None of the arguments is checked for
;               correctness (except that option is checked for
;               out-of-range values)!
;
;               If bit 15 of option is clear (indicating protection
;               against interference), interrupts will be left enabled
;               when this procedure exits.
;
; Returns       YES     if okay.
;               NO      if option is unknown,
;------------------------------------------------------------
; ;* - corrected by V.Yudin 23/01/91
;

video_status_port   equ     3DAh

include     begasm.mac

EXTRN _mshide:proc, _msunhide:proc

beginMod    utmovabs

;   Symbols to aid in referencing arguments on stack
;   (Some of these items are also used as local variables.):

a           equ     0
b           equ     a + sizeDPointer
c           equ     b + sizeDPointer
d           equ     c + 2
e           equ     d + 2
f           equ     e + 2
g           equ     f + 2

ppfrom      equ               [bp + stkoff + a]
ppto        equ               [bp + stkoff + b]
num_rows    equ     byte  ptr [bp + stkoff + c]
row_length  equ     byte  ptr [bp + stkoff + d]
scn_width   equ     word  ptr [bp + stkoff + e]
attr        equ     byte  ptr [bp + stkoff + f]
option      equ     word  ptr [bp + stkoff + g]

;   Macro to wait until the VERY BEGINNING of a horizontal
;   retrace interval.  On exit, interrupts are off.

await_horiz_retrace macro
        local   await_display_on,await_display_off
;        sti
        nop
;        cli
await_display_on:
        in      al,dx
        shr     al,1
        jc      await_display_on
await_display_off:
        in      al,dx
        shr     al,1
        jnc     await_display_off
endm

;   Here are macros to define procedures for each style of I/O as
;       specified by "option".
;
;   Each style of I/O requires four procedures:
;       (1) a specialized setup procedure ("setup_proc") (which may
;           do nothing)
;       (2) a procedure ("slow_proc") to do the entire read or write
;           while avoiding video interference;
;       (3) a procedure ("fast_proc") to do the entire read or write
;           without regard to interference (i.e., as fast as possible);
;       (4) a procedure ("newrow_proc") to adjust address registers
;           (SI and/or DI) and the column counter (CX) for a new row
;           of the rectangle.
;   These four procedures will be addressed through table "option_table"
;       defined below.
;   Since these procedures must therefore be highly regular in form,
;       the following macros aid in defining the beginning and end of
;       each.

begin_setup_proc    macro   o_name  ;; Define beginning of setup procedure
o_name&_setup_proc  proc    near
                    endm

end_setup_proc      macro   o_name  ;; Define end of setup procedure
                    ret
o_name&_setup_proc  endp
                    endm

                            ; On entry to each slow_proc,
                            ;
                            ;   BX = address of option_table entry;
                            ;   CX = number of cells in one row;
                            ;   DX = I/O address of video status port.
                            ;
                            ; Also, usually
                            ;   DS:SI = address of first source byte;
                            ;   ES:DI = address of first target byte.
                            ;
                            ; Additional entry conditions are satisfied
                            ;   by the setup_proc.

begin_slow_proc     macro   o_name,save_bx_flag
o_name&_slow_proc   proc    near
                    ifidn   <save_bx_flag>,<save_bx>
                    push    bx      ;; Some options must use BX as
                                    ;; additional storage space, so
                                    ;; save BX if requested.
                    endif
o_name&_single_loop:                ;; Begin loop that transfers one
                                    ;; character cell.
                    endm

end_slow_proc       macro   o_name,save_bx_flag
                    local   done
                    loop    o_name&_single_loop ;; Transfer one more cell
                                                ;; in this row.

                    ; sti
                    ifidn   <save_bx_flag>,<save_bx>
                    pop    bx       ;; Restore BX if it was used for
                                    ;; temporary data.
                    endif
                    dec     num_rows
                    jz      done
                    call    cs:[bx].do_newrow   ;; Use BX to invoke
                                                ;; proper newrow_proc.
                    jmp     short o_name&_slow_proc ;; Do next row.
done:               ret                             ;; Done.
o_name&_slow_proc   endp
                    endm

                            ; Each fast_proc has the same entry
                            ; conditions as slow_proc.

begin_fast_proc     macro   o_name
o_name&_fast_proc   proc    near
                    endm

end_fast_proc       macro   o_name
                    local   done

                    dec     num_rows
                    jz      done
                    call    cs:[bx].do_newrow   ;; Use BX to invoke
                                                ;; proper newrow_proc.
                    jmp     short o_name&_fast_proc
done:               ret
o_name&_fast_proc   endp
                    endm

                            ; Each newrow_proc assumes CH == 0.

begin_newrow_proc   macro   o_name
o_name&_newrow_proc proc    near
                    mov     cl,row_length
                    endm

end_newrow_proc     macro   o_name
                    ret
o_name&_newrow_proc endp
                    endm

; *************************************************************
; *********   DEFINE FOUR PROCEDURES FOR EACH OPTION **********
; *************************************************************
;
;   (Note:  many of these definitions just refer to others using "equ".)

    beginCseg wnvicopy

;   W_CHARS:    Write characters to the screen without arguments.

    begin_setup_proc    w_chars     ; No setup needed.
    end_setup_proc      w_chars

    begin_slow_proc     w_chars
         await_horiz_retrace
         movsb
         inc     di
    end_slow_proc       w_chars

    begin_fast_proc     w_chars
w_chars_fast_loop:
         movsb
         inc     di
         loop    w_chars_fast_loop
    end_fast_proc       w_chars

    begin_newrow_proc   w_chars
         sub     di,cx
         sub     di,cx
         add     di,scn_width
    end_newrow_proc     w_chars

;   W_CHAR_ATTRS:  Write characters & attributes to the screen.

w_char_attrs_setup_proc     equ     w_chars_setup_proc ; No setup needed.

    begin_slow_proc     w_char_attrs,save_bx
         lodsw                      ; Insufficient time for a
         mov     bx,ax              ; whole "movsw", so do the job
         await_horiz_retrace        ; by a "lodsw" plus a "stosw".
         mov     ax,bx
         stosw
    end_slow_proc       w_char_attrs,save_bx

    begin_fast_proc     w_char_attrs
         rep  movsw
    end_fast_proc       w_char_attrs

w_char_attrs_newrow_proc    equ     w_chars_newrow_proc

;   CONST_ATTR:  Write characters to screen with constant attribute
;                specified in AH.

    begin_setup_proc    const_attr
         mov     ah,attr
    end_setup_proc      const_attr


    begin_slow_proc     const_attr,save_bx
         lodsb                      ; Insufficient time for "lodsb"
         mov     bl,al              ; plus "stosw", so do the job
         await_horiz_retrace        ; in halves.
         mov     al,bl
         stosw
    end_slow_proc       const_attr,save_bx

    begin_fast_proc     const_attr
const_attr_fast_loop:
         lodsb
         stosw
         loop    const_attr_fast_loop
    end_fast_proc       const_attr

const_attr_newrow_proc      equ     w_chars_newrow_proc

;   CONST_CHAR_ATTR:  Fill rectangle with constant character
;                     and attribute (stored in SI).

    begin_setup_proc    const_char_attr
         mov    ah,attr
         mov    al,[si]
         mov    si,ax            ; Keep char & attr in SI for the loop
    end_setup_proc      const_char_attr

    begin_slow_proc     const_char_attr
         await_horiz_retrace
         mov    ax,si
         stosw
    end_slow_proc       const_char_attr

    begin_fast_proc     const_char_attr
         mov    ax,si
         rep    stosw
    end_fast_proc       const_char_attr

const_char_attr_newrow_proc   equ   w_chars_newrow_proc

;   CONST_ATTR_ONLY:  Change attribute on rectangle to value
;                     stored in AH.

    begin_setup_proc    const_attr_only
         mov    ah,attr
    end_setup_proc      const_attr_only

    begin_slow_proc     const_attr_only
         await_horiz_retrace
         inc    di
         mov    al,ah
         stosb
    end_slow_proc       const_attr_only

    begin_fast_proc     const_attr_only
         mov    al,ah
const_attr_only_fast_loop:
         inc    di
         stosb
         loop   const_attr_only_fast_loop
    end_fast_proc       const_attr_only

const_attr_only_newrow_proc   equ   w_chars_newrow_proc

;   R_CHARS:  Read only characters from screen.

r_chars_setup_proc     equ     w_chars_setup_proc ; No setup needed.

    begin_slow_proc     r_chars
         await_horiz_retrace
         movsb
         inc    si
    end_slow_proc       r_chars

    begin_fast_proc     r_chars
r_chars_fast_loop:
         movsb
         inc    si
         loop   r_chars_fast_loop
    end_fast_proc       r_chars

    begin_newrow_proc   r_chars
         sub    si,cx
         sub    si,cx
         add    si,scn_width
    end_newrow_proc     r_chars

;   R_CHAR_ATTRS:  Read characters & attributes from screen.

r_char_attrs_setup_proc     equ     w_chars_setup_proc ; No setup needed.

    begin_slow_proc     r_char_attrs
         await_horiz_retrace
         movsw
    end_slow_proc       r_char_attrs

r_char_attrs_fast_proc      equ     w_char_attrs_fast_proc
r_char_attrs_newrow_proc    equ     r_chars_newrow_proc

;   UP_CHAR_ATTRS:  Copy characters & attributes upward on the screen
;                   or directly to the left.  Start at upper left
;                   corner of rectangle.

up_char_attrs_setup_proc     equ     w_chars_setup_proc ; No setup needed.

    begin_slow_proc     up_char_attrs,save_bx
         await_horiz_retrace
         lodsw                      ; Insufficient time for a
         mov     bx,ax              ; whole "movsw", so do the job
         await_horiz_retrace        ; by a "lodsw" plus a "stosw".
         mov     ax,bx
         stosw
    end_slow_proc       up_char_attrs,save_bx

up_char_attrs_fast_proc      equ     w_char_attrs_fast_proc

    begin_newrow_proc   up_char_attrs
         sub    si,cx
         sub    si,cx
         add    si,scn_width
         sub    di,cx
         sub    di,cx
         add    di,scn_width
    end_newrow_proc     up_char_attrs

;   UP_CHAR_ATTRS:  Copy only characters upward on the screen
;                   or directly to the left.  Start at upper left
;                   corner of rectangle.

up_chars_setup_proc     equ     w_chars_setup_proc ; No setup needed.

    begin_slow_proc     up_chars
        await_horiz_retrace
        movsb
        inc     si
        inc     di
    end_slow_proc       up_chars

    begin_fast_proc     up_chars
up_chars_fast_loop:
        movsb
        inc     si
        inc     di
        loop    up_chars_fast_loop
    end_fast_proc       up_chars

up_chars_newrow_proc    equ     up_char_attrs_newrow_proc

;   DOWN_CHAR_ATTRS:  Copy characters & attributes downward on the screen
;                     or directly to the right.  Start at lower right
;                     corner of rectangle.

    begin_setup_proc    down_char_attrs
        mov     al,num_rows     ; Adjust SI and DI to point
        dec     al              ; at the last character in the
        mov     cx,scn_width    ; rectangle instead of the first.
        mul     cl
        mov     cl,row_length
        mov     ch,0
        dec     cx
        add     cx,cx
        add     ax,cx

        add     si,ax
        add     di,ax
    end_setup_proc      down_char_attrs

down_char_attrs_slow_proc   equ     up_char_attrs_slow_proc
down_char_attrs_fast_proc   equ     up_char_attrs_fast_proc

    begin_newrow_proc   down_char_attrs
        add     si,cx
        add     si,cx
        sub     si,scn_width
        add     di,cx
        add     di,cx
        sub     di,scn_width
    end_newrow_proc     down_char_attrs

;   DOWN_CHARS:  Copy only characters downward on the screen
;                or directly to the right.  Start at lower right
;                corner of rectangle.

down_chars_setup_proc     equ     down_char_attrs_setup_proc

    begin_slow_proc     down_chars
        await_horiz_retrace
        movsb
        dec     si
        dec     di
    end_slow_proc       down_chars

    begin_fast_proc     down_chars
down_chars_fast_loop:
        movsb
        dec     si
        dec     di
        loop    down_chars_fast_loop
    end_fast_proc       down_chars

down_chars_newrow_proc      equ     down_char_attrs_newrow_proc

; *******************************************************************
; **********   DEFINE THE TABLE OF OPTION INFORMATION  **************
; *******************************************************************

;   The following is the format of the table entry for one option:

option_entry    struc
do_setup        dw      ?   ; Address of setup_proc.
do_slow         dw      ?   ; Address of slow_proc.
do_fast         dw      ?   ; Address of fast_proc.
do_newrow       dw      ?   ; Address of newrow_proc.
dirflag         db      ?   ; Direction:  down or up.
option_entry    ends

;   Values for dirflag:

down    equ     1           ; Decreasing addresses (up the screen)
up      equ     0           ; Increasing addresses (down the screen)

;   Macro to create one table entry:

o_e             macro   o_name,dir
option_entry <o_name&_setup_proc,o_name&_slow_proc,o_name&_fast_proc,o_name&_newrow_proc,dir>
                endm

;   The table itself follows.  It's part of the code segment.

option_table:
                o_e     w_chars,up           ; 0
end_of_first    label   option_entry
                o_e     w_char_attrs,up      ; 1
                o_e     const_attr,up        ; 2
                o_e     const_char_attr,up   ; 3
                o_e     const_attr_only,up   ; 4
                o_e     r_chars,up           ; 5
                o_e     r_char_attrs,up      ; 6
                o_e     up_char_attrs,up     ; 7
                o_e     up_chars,up          ; 8
                o_e     down_char_attrs,down ; 9
                o_e     down_chars,down      ; 10
                o_e     up_char_attrs,up     ; 11
                o_e     up_chars,up          ; 12

                            ; (Note that options 11 and 12 just
                            ; refer to the procedures used by options
                            ; 7 and 8, respectively, because the
                            ; same algorithms suffice.)

end_of_table    label   option_entry

size_of_entry   equ     end_of_first - option_table
table_length    equ     (end_of_table - option_table)/size_of_entry


; ********************************************************************
; **********************   BEGIN THE ACTUAL CODE   *******************
; ********************************************************************

beginProc wnvicopy

    StartCode   2      ;* Setup automatic variable [bp-2] V.Yudin 01/23/91

        mov     word ptr [bp-2], 0 ;* Default: mouse pointer is hidden.

        cmp     num_rows, 0
        jg      l1
        jmp     not_unhide
l1:
        cmp     row_length, 0
        jg      l2
        jmp     not_unhide
l2:
        call    _mshide             ;* V.Yudin 01/23/91
        or      ax, ax              ;*
        jz      not_hide            ;* Allready hided.
        inc     word ptr [bp-2]     ;* Mouse pointer is hidding now.
not_hide:                           ;*

        cli
        mov     ax,option
        mov     dx,ax               ; Spare copy of raw option
        and     ax,7fffh            ; Ignore bit 15 of option.

        cmp     ax,table_length     ; Check for option value beyond table.
        jb      option_ok

bad_option:
        xor     ax,ax               ; Invalid option
        jmp     exit

option_ok:
        mov     cl,size_of_entry    ; Convert option value into address
        mul     cl                  ; of proper table entry.
        jc      bad_option
        add     ax,offset option_table
        mov     option,ax           ; Save address of table entry.

        mov     bx,ax               ; Set DF according to dirflag value
        cmp     cs:[bx].dirflag,down; in table.
        je      downward

        cld
        jmp     short have_set_direction

downward:
        std

have_set_direction:

        if      LONGDATA
        lds     bx,dword ptr ppto
        assume  ds:nothing
        else
        mov     bx,ppto
        endif
        les     di,[bx]             ; Put target address into ES:DI.
        assume  es:nothing

        if      LONGDATA
        lds     bx,dword ptr ppfrom
        assume  ds:nothing
        else
        mov     bx,ppfrom
        endif
        lds     si,[bx]             ; Put source address into DS:SI.
        assume  ds:nothing

        mov     bx,option

        call    cs:[bx].do_setup    ; Do specialized portion
                                    ; of setup.

                                    ; Complete the setup.
        mov     cl,row_length
        mov     ch,0
        test    dh,80h
        mov     dx,video_status_port
        jnz     no_waiting

        call    cs:[bx].do_slow     ; Invoke slow_proc.
        jmp     short done

no_waiting:
        call    cs:[bx].do_fast     ; Invoke fast_proc.

done:   mov     ax,1                ; Successful completion
exit:   sti

        test    word ptr [bp-2], 1  ;* Need unhide mouse ?
        jz      not_unhide          ;* Don't
		push    ax                  ;*
		mov		ds, word ptr [bp-8]	;* Restore DS for correct call.
		call    _msunhide           ;*
		pop     ax                  ;* V.Yudin 01/23/91
not_unhide:

    FinishCode

endProc wnvicopy
endCseg wnvicopy
endMod  wnvicopy
end

