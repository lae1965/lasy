;     utch2prn - ????頥? ᨬ??? sym ? ????? ?ਭ???? LPT1:;
;
;      void utch2prn (byte sym);
;
;   byte      sym    - ????頥??? ᨬ???;

include     begasm.mac

beginProg   utch2prn
    sym      equ  byte  ptr  [bp + stkoff + 0]

    StartCode

    mov     dl, sym
    mov     ah, 5H

    int     21H

    FinishCode
endProg     utch2prn

end

