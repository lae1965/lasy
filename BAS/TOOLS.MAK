acomp=tasm /Mx /i$(apath)
ccomp=bcc -c -m$(model) -w -O -V -vi- -r- -v

$(path)slbase.lib :       DBCLCWRD.ASM
        $(acomp) DBCLCWRD;      >> con
        echo -+DBCLCWRD    &   >>~libresp

$(path)slbase.lib :       DBCLRBLN.ASM
        $(acomp) DBCLRBLN;      >> con
        echo -+DBCLRBLN    &   >>~libresp

$(path)slbase.lib :       DBCMPSMP.ASM
        $(acomp) DBCMPSMP;      >> con
        echo -+DBCMPSMP    &   >>~libresp

$(path)slbase.lib :       DBFINDBR.ASM
        $(acomp) DBFINDBR;      >> con
        echo -+DBFINDBR    &   >>~libresp

$(path)slbase.lib :       DBFMTSCR.ASM
        $(acomp) DBFMTSCR;      >> con
        echo -+DBFMTSCR    &   >>~libresp

$(path)slbase.lib :       DBFNDFRG.ASM
        $(acomp) DBFNDFRG;      >> con
        echo -+DBFNDFRG    &   >>~libresp

$(path)slbase.lib :       DBOUTSTR.ASM
        $(acomp) DBOUTSTR;      >> con
        echo -+DBOUTSTR    &   >>~libresp

$(path)slbase.lib :       DBPRNSTR.ASM
        $(acomp) DBPRNSTR;      >> con
        echo -+DBPRNSTR    &   >>~libresp

$(path)slbase.lib :       DBKHLPAT.ASM
        $(acomp) DBKHLPAT;      >> con
        echo -+DBKHLPAT    &   >>~libresp

$(path)slbase.lib :       DBINIDB.C
        $(ccomp) DBINIDB.C      >> con
        echo -+DBINIDB    &   >>~libresp

$(path)slbase.lib :       DBDOCEDT.C
        $(ccomp) DBDOCEDT.C      >> con
        echo -+DBDOCEDT    &   >>~libresp

$(path)slbase.lib :       DBEDTCRT.C
        $(ccomp) DBEDTCRT.C      >> con
        echo -+DBEDTCRT    &   >>~libresp

$(path)slbase.lib :       DBEDTNAV.C
        $(ccomp) DBEDTNAV.C      >> con
        echo -+DBEDTNAV    &   >>~libresp

$(path)slbase.lib :       DBEDTEDT.C
        $(ccomp) DBEDTEDT.C      >> con
        echo -+DBEDTEDT    &   >>~libresp

$(path)slbase.lib :       DBEDTPRN.C
        $(ccomp) DBEDTPRN.C      >> con
        echo -+DBEDTPRN    &   >>~libresp

$(path)slbase.lib :       DBEDSTAT.C
        $(ccomp) DBEDSTAT.C      >> con
        echo -+DBEDSTAT    &   >>~libresp

$(path)slbase.lib :       DBSL2NAM.C
        $(ccomp) DBSL2NAM.C      >> con
        echo -+DBSL2NAM    &   >>~libresp

$(path)slbase.lib :       DBEDMOUS.C
        $(ccomp) DBEDMOUS.C      >> con
        echo -+DBEDMOUS    &   >>~libresp

$(path)slbase.lib :       DBEDKHLP.C
        $(ccomp) DBEDKHLP.C      >> con
        echo -+DBEDKHLP    &   >>~libresp

$(path)slbase.lib :       DBSTATDB.C
        $(ccomp) DBSTATDB.C      >> con
        echo -+DBSTATDB    &   >>~libresp

$(path)slbase.lib :       DBPRVIEW.C
        $(ccomp) DBPRVIEW.C      >> con
        echo -+DBPRVIEW    &   >>~libresp

$(path)slbase.lib :       DBPRPRN.C
        $(ccomp) DBPRPRN.C      >> con
        echo -+DBPRPRN    &   >>~libresp

$(path)slbase.lib :       DBCRTPRP.C
        $(ccomp) DBCRTPRP.C      >> con
        echo -+DBCRTPRP    &   >>~libresp

$(path)slbase.lib :       DBCRTWRK.C
        $(ccomp) DBCRTWRK.C      >> con
        echo -+DBCRTWRK    &   >>~libresp

$(path)slbase.lib :       DBBASWRK.C
        $(ccomp) DBBASWRK.C      >> con
        echo -+DBBASWRK    &   >>~libresp

$(path)slbase.lib :       DBGRPWRK.C
        $(ccomp) DBGRPWRK.C      >> con
        echo -+DBGRPWRK    &   >>~libresp

$(path)slbase.lib :       DBDSTROY.C
        $(ccomp) DBDSTROY.C      >> con
        echo -+DBDSTROY    &   >>~libresp

$(path)slbase.lib :       DBFMTCOL.C
        $(ccomp) DBFMTCOL.C      >> con
        echo -+DBFMTCOL    &   >>~libresp

$(path)slbase.lib :       DBINIDOC.C
        $(ccomp) DBINIDOC.C      >> con
        echo -+DBINIDOC    &   >>~libresp

$(path)slbase.lib :       DBCRTOCR.C
        $(ccomp) DBCRTOCR.C      >> con
        echo -+DBCRTOCR    &   >>~libresp

$(path)slbase.lib :       DBOUTDOC.C
        $(ccomp) DBOUTDOC.C      >> con
        echo -+DBOUTDOC    &   >>~libresp

$(path)slbase.lib :       DBOUTPRN.C
        $(ccomp) DBOUTPRN.C      >> con
        echo -+DBOUTPRN    &   >>~libresp

$(path)slbase.lib :       DBQUERY.C
        $(ccomp) DBQUERY.C      >> con
        echo -+DBQUERY    &   >>~libresp

$(path)slbase.lib :       DBTSTREC.C
        $(ccomp) DBTSTREC.C      >> con
        echo -+DBTSTREC    &   >>~libresp

$(path)slbase.lib :       DBRECWRK.C
        $(ccomp) DBRECWRK.C      >> con
        echo -+DBRECWRK    &   >>~libresp

$(path)slbase.lib :       DBSRTCRT.C
        $(ccomp) DBSRTCRT.C      >> con
        echo -+DBSRTCRT    &   >>~libresp

$(path)slbase.lib :       DBOPEN.C
        $(ccomp) DBOPEN.C      >> con
        echo -+DBOPEN    &   >>~libresp

$(path)slbase.lib :       DBFILNAM.C
        $(ccomp) DBFILNAM.C      >> con
        echo -+DBFILNAM    &   >>~libresp

$(path)slbase.lib :       DBROLNAM.C
        $(ccomp) DBROLNAM.C      >> con
        echo -+DBROLNAM    &   >>~libresp

$(path)slbase.lib :       DBCALSUM.C
        $(ccomp) DBCALSUM.C      >> con
        echo -+DBCALSUM    &   >>~libresp

$(path)slbase.lib :       DBCUTIL.C
        $(ccomp) DBCUTIL.C      >> con
        echo -+DBCUTIL    &   >>~libresp

$(path)slbase.lib :       DBMAKLST.C
        $(ccomp) DBMAKLST.C      >> con
        echo -+DBMAKLST    &   >>~libresp

$(path)slbase.lib :       DBMENULV.C
        $(ccomp) DBMENULV.C      >> con
        echo -+DBMENULV    &   >>~libresp

$(path)slbase.lib :       DBMKMNLV.C
        $(ccomp) DBMKMNLV.C      >> con
        echo -+DBMKMNLV    &   >>~libresp

$(path)slbase.lib :       DBLODLST.C
        $(ccomp) DBLODLST.C      >> con
        echo -+DBLODLST    &   >>~libresp

$(path)slbase.lib :       DBSQZBAS.C
        $(ccomp) DBSQZBAS.C      >> con
        echo -+DBSQZBAS    &   >>~libresp

$(path)slbase.lib :       DBUNIREC.C
        $(ccomp) DBUNIREC.C      >> con
        echo -+DBUNIREC    &   >>~libresp

$(path)slbase.lib :       DBTXTINI.C
        $(ccomp) DBTXTINI.C      >> con
        echo -+DBTXTINI    &   >>~libresp

$(path)slbase.lib :       LSTSCRNV.C
        $(ccomp) LSTSCRNV.C      >> con
        echo -+LSTSCRNV    &   >>~libresp

$(path)slbase.lib :       LSTSTDFN.C
        $(ccomp) LSTSTDFN.C      >> con
        echo -+LSTSTDFN    &   >>~libresp

$(path)slbase.lib :       LSPRNLST.C
        $(ccomp) LSPRNLST.C      >> con
        echo -+LSPRNLST    &   >>~libresp

