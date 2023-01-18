@echo off
if exist language.h     del language.h
if exist *.obj          del *.obj

echo>language.h #define TC_lang
rem echo>>language.h #define ENGLISH

echo: >e:\bc\include\compiler.mac NLARGE equ 1

echo /C n:\bc\lib\SLBASE  &                   >~libresp
echo:                                            > con
ntmake /S model=l path=n:\bc\lib\ apath=n:\bc\include\ tools.mak
if errorlevel 2 goto end
echo ,NUL                                       >>~libresp

tlib @~libresp                                  >> con

if exist language.h     del language.h
if exist ~libresp       del ~libresp
if exist *.obj          del *.obj

:end
