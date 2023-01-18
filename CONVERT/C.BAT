@echo off
bcc -ml -w -O -N -V -vi- -v convert.c c:\bc\lib\tools_l.lib c:\bc\lib\slbase.lib
