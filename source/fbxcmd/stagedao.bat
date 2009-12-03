@echo on
setlocal
if "%1" == "" (
  echo Give a version ...
  goto exit
)
set SRCPATH=%~dp0
set DSTPATH=%~dp0outdir\
set RAREXEC=D:\Program Files\WinRAR\Rar.exe
set UNRAREXEC=D:\Program Files\WinRAR\unrar.exe
set ISCCEXEC=c:\Program Files\Inno Setup 5\ISCC.exe
set ZIPEXEC=7za.exe

mkdir "%DSTPATH%"
pushd "%DSTPATH%"
rm -r -f %1
REM rm -r -f "%SRCPATH%Staging\src\%1"
popd

mkdir "%DSTPATH%%1"
pushd "%SRCPATH%"
"%ZIPEXEC%" a "%DSTPATH%%1\fbxcmd_v%1.zip" *.TXT -x!BUILD*.*
pushd "%SRCPATH%Staging\Win32\Release\"
"%ZIPEXEC%" a -r "%DSTPATH%%1\fbxcmd_v%1.zip" * -x!*.pdb -x!*.lib -x!*.exp
popd

"%ZIPEXEC%" a -r "%DSTPATH%%1\fbxcmd_v%1-src.zip" * -x@%SRCPATH%excludes.lst
popd

:exit
endlocal
