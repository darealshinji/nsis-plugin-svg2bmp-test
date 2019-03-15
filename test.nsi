!addplugindir ".\plugins\x86-ansi"
!include MUI2.nsh

Name "test"
OutFile "test.exe"
RequestExecutionLevel user
InstallDir "$ProgramFiles\zzz_test"

!insertmacro MUI_PAGE_WELCOME

Function .onInit
svg2bmp::svg2bmp "C:\Users\XXX\Documents\nsis-plugin-svg2bmp-test\tiger.svg" "C:\Users\XXX\Documents\nsis-plugin-svg2bmp-test\tiger.bmp" 200 100
FunctionEnd

Section

SectionEnd
