#!/bin/sh
set -e

CC=i686-w64-mingw32-gcc
CFLAGS="-Wall -O2"
LDFLAGS="-static -s"

set -x

$CC $CFLAGS -UUNICODE -U_UNICODE -c plugin.c -o pluginA.o
$CC $CFLAGS -UUNICODE -U_UNICODE -c pluginapi.c -o pluginapiA.o
$CC -shared -o svg2bmp_ansi.dll pluginA.o pluginapiA.o $LDFLAGS

$CC $CFLAGS -DUNICODE -D_UNICODE -c plugin.c -o pluginW.o
$CC $CFLAGS -DUNICODE -D_UNICODE -c pluginapi.c -o pluginapiW.o
$CC -shared -o svg2bmp_unicode.dll pluginW.o pluginapiW.o $LDFLAGS
