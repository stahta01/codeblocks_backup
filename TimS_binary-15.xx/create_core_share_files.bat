@echo off

REM SETLOCAL assures environment variables created in a batch file are not exported to its calling environment
setlocal

set CB_DEVEL_RESDIR=%CD%\share\CodeBlocks

if not exist share md share\

cd ..\src\

if not exist %CB_DEVEL_RESDIR% md %CB_DEVEL_RESDIR%\
if not exist %CB_DEVEL_RESDIR%\compilers md %CB_DEVEL_RESDIR%\compilers\
if not exist %CB_DEVEL_RESDIR%\lexers md %CB_DEVEL_RESDIR%\lexers\
if not exist %CB_DEVEL_RESDIR%\images md %CB_DEVEL_RESDIR%\images\
if not exist %CB_DEVEL_RESDIR%\images\settings md %CB_DEVEL_RESDIR%\images\settings\
if not exist %CB_DEVEL_RESDIR%\images\16x16 md %CB_DEVEL_RESDIR%\images\16x16\
if not exist %CB_DEVEL_RESDIR%\images\codecompletion md %CB_DEVEL_RESDIR%\images\codecompletion\
if not exist %CB_DEVEL_RESDIR%\plugins md %CB_DEVEL_RESDIR%\plugins\
if not exist %CB_DEVEL_RESDIR%\templates md %CB_DEVEL_RESDIR%\templates\
if not exist %CB_DEVEL_RESDIR%\templates\wizard md %CB_DEVEL_RESDIR%\templates\wizard\
if not exist %CB_DEVEL_RESDIR%\scripts md %CB_DEVEL_RESDIR%\scripts\

    PAUSE
echo Copying files
xcopy /D /y sdk\resources\lexers\lexer_* %CB_DEVEL_RESDIR%\lexers 
xcopy /D /y src\resources\images\*.png %CB_DEVEL_RESDIR%\images 
xcopy /D /y src\resources\images\settings\*.png %CB_DEVEL_RESDIR%\images\settings 
xcopy /D /y src\resources\images\16x16\*.png %CB_DEVEL_RESDIR%\images\16x16 
xcopy /D /y plugins\codecompletion\resources\images\*.png %CB_DEVEL_RESDIR%\images\codecompletion 
xcopy /D /y plugins\compilergcc\resources\compilers\*.xml %CB_DEVEL_RESDIR%\compilers 
echo Makefile.am >  excludes30.txt
echo Makefile.in >> excludes30.txt
echo \.svn\      >> excludes30.txt
echo *.gdb       >> excludes30.txt
xcopy /D /y /s plugins\scriptedwizard\resources\* %CB_DEVEL_RESDIR%\templates\wizard /EXCLUDE:excludes30.txt
xcopy /D /y scripts\* %CB_DEVEL_RESDIR%\scripts /EXCLUDE:excludes30.txt 
del excludes30.txt
xcopy /D /y tips.txt %CB_DEVEL_RESDIR% 

    PAUSE
