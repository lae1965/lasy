;         utcreate () - ???? ᮧ?????? ? 㪠?????? ??? 㬠?稢?????
;                       ??⠫??? ? ०??? ??????/?⥭??.
;                       ?᫨ 䠩? 㦥 ??????????, ?? ?ᥪ????? ??
;                       ?㫥??? ?????.
;
;
;      int  utcreate (char * fname);
;
; ??ࠬ????:
;   char  * fname -   ?????⥫?  ?? ??ப? ? ?????? 䠩??
;
; ???????:
;               Handle ??? -1, ?᫨ ?ந??諠 ?訡??
;

include     begasm.mac

beginProg   utcreate
    fname   equ  dword ptr [bp + stkoff + 0]

    StartCode

    mov     ah, 3cH
    lds     dx, fname
    xor     cx, cx
    int     21H
    jc      short unsuc
    jmp     short suc
unsuc:
    mov     ax, -1
suc:

    FinishCode
endProg     utcreate

end
