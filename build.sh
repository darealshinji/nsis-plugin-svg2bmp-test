#!/bin/sh
set -e

CC="i686-w64-mingw32-gcc"
#CC="i686-w64-mingw32.static-gcc"
CC64="x86_64-w64-mingw32-gcc"
#CC64="x86_64-w64-mingw32.static-gcc"
CFLAGS="-Wall -Wfloat-conversion -O3 -ffast-math -ffunction-sections -fdata-sections"
LDFLAGS="-Wl,--gc-sections -static -s"

set -x

mkdir -p plugins/x86-ansi plugins/x86-unicode plugins/amd64-unicode

$CC $CFLAGS -UUNICODE -U_UNICODE -c plugin.c -o pluginA.o
$CC $CFLAGS -UUNICODE -U_UNICODE -c pluginapi.c -o pluginapiA.o
$CC -shared -o plugins/x86-ansi/svg2bmp.dll pluginA.o pluginapiA.o $LDFLAGS

$CC $CFLAGS -DUNICODE -D_UNICODE -c plugin.c -o pluginW.o
$CC $CFLAGS -DUNICODE -D_UNICODE -c pluginapi.c -o pluginapiW.o
$CC -shared -o plugins/x86-unicode/svg2bmp.dll pluginW.o pluginapiW.o $LDFLAGS

$CC64 $CFLAGS -DUNICODE -D_UNICODE -c plugin.c -o plugin64.o
$CC64 $CFLAGS -DUNICODE -D_UNICODE -c pluginapi.c -o pluginapi64.o
$CC64 -shared -o plugins/amd64-unicode/svg2bmp.dll plugin64.o pluginapi64.o $LDFLAGS

