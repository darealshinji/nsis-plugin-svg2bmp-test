!addplugindir ".\plugins\ansi"
!include MUI2.nsh

Name "test"
OutFile "test.exe"
RequestExecutionLevel user
InstallDir "$ProgramFiles\zzz_test"

!insertmacro MUI_PAGE_WELCOME

Function .onInit
svg2bmp::svg2bmp "C:\Users\XXX\Documents\svg2bmp\tiger.svg" "C:\Users\XXX\Documents\svg2bmp\tiger.bmp" 200 100
FunctionEnd

Section

SectionEnd
