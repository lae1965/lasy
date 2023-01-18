acomp=tasm /Mx /i$(apath)
ccomp=bcc -c -m$(model) -w -O

$(path)tools_$(model).lib :       TROUTTR.C
        $(ccomp) TROUTTR.C      >> con
        echo -+TROUTTR    &   >>~libresp

$(path)tools_$(model).lib :       TROUTPRN.C
        $(ccomp) TROUTPRN.C      >> con
        echo -+TROUTPRN    &   >>~libresp

$(path)tools_$(model).lib :       TRFUNTR.C
        $(ccomp) TRFUNTR.C      >> con
        echo -+TRFUNTR    &   >>~libresp

$(path)tools_$(model).lib :       TRINITR.C
        $(ccomp) TRINITR.C      >> con
        echo -+TRINITR    &   >>~libresp

