CC		:= i686-w64-mingw32.static-gcc
CC64	:= x86_64-w64-mingw32.static-gcc
CFLAGS	:= -Wall -Wextra -Wfloat-conversion -O3 -std=c99 -ffast-math -ffunction-sections -fdata-sections
LDFLAGS	:= -shared -Wl,--gc-sections -static -s

DLL_X86ANSI		= plugins/x86-ansi/svg2bmp.dll
DLL_X86UNICODE	= plugins/x86-unicode/svg2bmp.dll
DLL_AMD64		= plugins/amd64-unicode/svg2bmp.dll

SRCS			= pluginapi.c plugin.c
OBJS_X86ANSI	= pluginapi.ans.o plugin.ans.o
OBJS_X86UNICODE = pluginapi.uni.o plugin.uni.o
OBJS_AMD64		= pluginapi.x64.o plugin.x64.o


all: $(DLL_X86ANSI) $(DLL_X86UNICODE) $(DLL_AMD64)

clean:
	rm -f *.o
	rm -rf plugins

$(DLL_X86ANSI): $(OBJS_X86ANSI)
	mkdir -p plugins/x86-ansi
	$(CC) -o $@ $^ $(LDFLAGS)

$(DLL_X86UNICODE): $(OBJS_X86UNICODE)
	mkdir -p plugins/x86-unicode
	$(CC) -o $@ $^ $(LDFLAGS)

$(DLL_AMD64): $(OBJS_AMD64)
	mkdir -p plugins/amd64-unicode
	$(CC64) -o $@ $^ $(LDFLAGS)

%.ans.o: %.c
	$(CC) $(CFLAGS) -UUNICODE -U_UNICODE -c $< -o $@

%.uni.o: %.c
	$(CC) $(CFLAGS) -DUNICODE -D_UNICODE -c $< -o $@

%.x64.o: %.c
	$(CC64) $(CFLAGS) -DUNICODE -D_UNICODE -c $< -o $@

