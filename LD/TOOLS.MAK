acomp=tasm /Mx /i$(apath)
ccomp=bcc -c -m$(model) -w -O

$(path)tools_$(model).lib :       LINEDRAW.C
        $(ccomp) LINEDRAW.C      >> con
        echo -+LINEDRAW    &   >>~libresp

