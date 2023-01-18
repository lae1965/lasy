acomp=tasm /Mx /i$(apath)
ccomp=bcc -c -m$(model) -w -O -N -V -vi- -r-

$(path)tools_$(model).lib :       KBANYPRE.C
        $(ccomp) KBANYPRE.C   >> con
        echo -+KBANYPRE &   >>~libresp

$(path)tools_$(model).lib :       KBFLUSHB.C
        $(ccomp) KBFLUSHB.C   >> con
        echo -+KBFLUSHB &   >>~libresp

$(path)tools_$(model).lib :       KBGETKEY.C
        $(ccomp) KBGETKEY.C   >> con
        echo -+KBGETKEY &   >>~libresp

$(path)tools_$(model).lib :       KBSETSYM.C
        $(ccomp) KBSETSYM.C   >> con
        echo -+KBSETSYM &   >>~libresp

$(path)tools_$(model).lib :       KBPLACE.ASM
        $(acomp) KBPLACE;   >> con
        echo -+KBPLACE  &   >>~libresp
