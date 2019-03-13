#!/bin/sh
set -e

CC="i686-w64-mingw32-gcc"
#CC="i686-w64-mingw32.static-gcc"
CC64="x86_64-w64-mingw32-gcc"
#CC64="x86_64-w64-mingw32.static-gcc"
CFLAGS="-Wall -O2"
LDFLAGS="-static -s"

set -x

mkdir -p plugins/ansi plugins/unicode plugins/x64

$CC $CFLAGS -UUNICODE -U_UNICODE -c plugin.c -o pluginA.o
$CC $CFLAGS -UUNICODE -U_UNICODE -c pluginapi.c -o pluginapiA.o
$CC -shared -o plugins/ansi/svg2bmp.dll pluginA.o pluginapiA.o $LDFLAGS

$CC $CFLAGS -DUNICODE -D_UNICODE -c plugin.c -o pluginW.o
$CC $CFLAGS -DUNICODE -D_UNICODE -c pluginapi.c -o pluginapiW.o
$CC -shared -o plugins/unicode/svg2bmp.dll pluginW.o pluginapiW.o $LDFLAGS

$CC64 $CFLAGS -DUNICODE -D_UNICODE -c plugin.c -o plugin64.o
$CC64 $CFLAGS -DUNICODE -D_UNICODE -c pluginapi.c -o pluginapi64.o
$CC64 -shared -o plugins/x64/svg2bmp.dll plugin64.o pluginapi64.o $LDFLAGS
