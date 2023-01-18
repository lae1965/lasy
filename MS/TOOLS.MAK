acomp=tasm /Mx /i$(apath)
ccomp=bcc -c -m$(model) -w -O

$(path)tools_$(model).lib :       MSBOUND.ASM
        $(acomp) MSBOUND;   >>con
        echo -+MSBOUND  &   >>~libresp

$(path)tools_$(model).lib :       MSDATA.ASM
        $(acomp) MSDATA;    >>con
        echo -+MSDATA   &   >>~libresp

$(path)tools_$(model).lib :       MSEMUKBD.C
        $(ccomp) MSEMUKBD.C   >>con
        echo -+MSEMUKBD &   >>~libresp

$(path)tools_$(model).lib :       MSEXCLUS.ASM
        $(acomp) MSEXCLUS;  >>con
        echo -+MSEXCLUS &   >>~libresp

$(path)tools_$(model).lib :       MSFUNKEY.C
        $(ccomp) MSFUNKEY.C   >>con
        echo -+MSFUNKEY &   >>~libresp

$(path)tools_$(model).lib :       MSGETPOS.ASM
        $(acomp) MSGETPOS;  >>con
        echo -+MSGETPOS &   >>~libresp

$(path)tools_$(model).lib :       MSHANDLR.ASM
        $(acomp) MSHANDLR;  >>con
        echo -+MSHANDLR &   >>~libresp

$(path)tools_$(model).lib :       MSHIDE.ASM
        $(acomp) MSHIDE;    >>con
        echo -+MSHIDE   &   >>~libresp

$(path)tools_$(model).lib :       MSINST.ASM
        $(acomp) MSINST;    >>con
        echo -+MSINST   &   >>~libresp

$(path)tools_$(model).lib :       MSPTGMSK.ASM
        $(acomp) MSPTGMSK;  >>con
        echo -+MSPTGMSK &   >>~libresp

$(path)tools_$(model).lib :       MSPTHMSK.ASM
        $(acomp) MSPTHMSK;  >>con
        echo -+MSPTHMSK &   >>~libresp

$(path)tools_$(model).lib :       MSPTSMSK.ASM
        $(acomp) MSPTSMSK;  >>con
        echo -+MSPTSMSK &   >>~libresp

$(path)tools_$(model).lib :       MSSETPOS.ASM
        $(acomp) MSSETPOS;  >>con
        echo -+MSSETPOS &   >>~libresp

$(path)tools_$(model).lib :       MSSPEED.ASM
        $(acomp) MSSPEED;   >>con
        echo -+MSSPEED  &   >>~libresp

$(path)tools_$(model).lib :       MSUNHIDE.ASM
        $(acomp) MSUNHIDE;  >>con
        echo -+MSUNHIDE &   >>~libresp
