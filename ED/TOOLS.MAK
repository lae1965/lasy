acomp=tasm /Mx /i$(apath)
ccomp=bcc -c -m$(model) -w -O

$(path)tools_$(model).lib :       EDTFUN_A.ASM
        $(acomp) EDTFUN_A;     >> con
        echo -+EDTFUN_A    &   >>~libresp

$(path)tools_$(model).lib :       EDTFUN_C.C
        $(ccomp) EDTFUN_C.C      >> con
        echo -+EDTFUN_C    &   >>~libresp

$(path)tools_$(model).lib :       EDTSWAP.C
        $(ccomp) EDTSWAP.C      >> con
        echo -+EDTSWAP    &   >>~libresp

$(path)tools_$(model).lib :       EDTBLOCK.C
        $(ccomp) EDTBLOCK.C      >> con
        echo -+EDTBLOCK    &   >>~libresp

$(path)tools_$(model).lib :       EDTVISI.C
        $(ccomp) EDTVISI.C      >> con
        echo -+EDTVISI    &   >>~libresp
