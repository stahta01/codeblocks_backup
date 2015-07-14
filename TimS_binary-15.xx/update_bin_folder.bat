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

set ZIPCMD=zip

echo Compressing core UI resources
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\resources.zip src\resources\*.xrc 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\manager_resources.zip sdk\resources\*.xrc sdk\resources\images\*.png 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\start_here.zip src\resources\start_here\*.html src\resources\start_here\*.png 
    PAUSE
echo Compressing plugins UI resources
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\astyle.zip plugins\astyle\resources\manifest.xml plugins\astyle\resources\*.xrc 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\autosave.zip plugins\autosave\manifest.xml plugins\autosave\*.xrc 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\classwizard.zip plugins\classwizard\resources\manifest.xml plugins\classwizard\resources\*.xrc 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\codecompletion.zip plugins\codecompletion\resources\manifest.xml plugins\codecompletion\resources\*.xrc 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\compiler.zip plugins\compilergcc\resources\manifest.xml plugins\compilergcc\resources\*.xrc 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\debugger.zip plugins\debuggergdb\resources\manifest.xml plugins\debuggergdb\resources\*.xrc 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\defaultmimehandler.zip plugins\defaultmimehandler\resources\manifest.xml plugins\defaultmimehandler\resources\*.xrc 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\occurrenceshighlighting.zip plugins\occurrenceshighlighting\resources\*.xrc plugins\occurrenceshighlighting\resources\manifest.xml 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\openfileslist.zip plugins\openfileslist\manifest.xml 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\projectsimporter.zip plugins\projectsimporter\resources\manifest.xml plugins\projectsimporter\resources\*.xrc 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\scriptedwizard.zip plugins\scriptedwizard\resources\manifest.xml 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\todo.zip plugins\todo\resources\manifest.xml plugins\todo\resources\*.xrc 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\xpmanifest.zip plugins\xpmanifest\manifest.xml 
%ZIPCMD% -jqu9 %CB_DEVEL_RESDIR%\abbreviations.zip plugins\abbreviations\resources\manifest.xml plugins\abbreviations\resources\*.xrc 
    PAUSE
echo Packing core UI bitmaps
cd src\resources
%ZIPCMD% -0 -qu %CB_DEVEL_RESDIR%\resources.zip images\*.png images\12x12\*.png images\16x16\*.png 
cd ..\..\sdk\resources
%ZIPCMD% -0 -qu %CB_DEVEL_RESDIR%\manager_resources.zip images\*.png images\12x12\*.png images\16x16\*.png 
echo Packing plugins UI bitmaps
cd ..\..\plugins\compilergcc\resources
%ZIPCMD% -0 -qu %CB_DEVEL_RESDIR%\compiler.zip images\*.png images\16x16\*.png 
cd ..\..\..\plugins\debuggergdb\resources
%ZIPCMD% -0 -qu %CB_DEVEL_RESDIR%\debugger.zip images\*.png images\16x16\*.png 
cd ..\..\..
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
