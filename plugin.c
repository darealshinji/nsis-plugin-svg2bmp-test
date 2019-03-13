/**
 * Copyright (c) 2019  djcj <djcj@gmx.de>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <windows.h>
#include <stdio.h>
#include "pluginapi.h"

// Using FLTK's fork: https://github.com/fltk/nanosvg
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

#if defined(UNICODE) && defined(_UNICODE)
# define STBI_WINDOWS_UTF8
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"  // patched version


//HINSTANCE g_hInstance;
//HWND g_hwndParent;


#ifdef STBI_WINDOWS_UTF8
static char *wchar_to_utf8(wchar_t *input)
{
  char *buffer = NULL;

  int bufferlen = WideCharToMultiByte(65001 /* UTF8 */, 0, input, -1, buffer, 0, NULL, NULL);
  if (bufferlen == 0) {
    return NULL;
  }

  buffer = malloc(bufferlen + 1);
  if (!buffer) {
    return NULL;
  }

  if (WideCharToMultiByte(65001 /* UTF8 */, 0, input, -1, buffer, bufferlen, NULL, NULL) == 0) {
    free(buffer);
    return NULL;
  }
  return buffer;
}
#endif


__declspec(dllexport)
void svg2bmp(HWND hwndParent,
             int string_size,
             LPTSTR variables,
             stack_t **stacktop,
             extra_parameters *extra,
             ...)
{
  EXDLL_INIT();
  //g_hwndParent = hwndParent;

  LPTSTR in = NULL;  // arg1: SVG input filename
  LPTSTR out = NULL; // arg2: BMP output filename
  int set_w;         // arg3: output image width (optional)
  int set_h;         // arg4: output image height (optional)

  FILE *fp;
  NSVGimage *svg;
  NSVGrasterizer *rast;
  unsigned char *data;
  double scale_x = 1.0, scale_y = 1.0;
  long fsize;
  int w, h;

#define FREE()  free(in); free(out);

  in = malloc((string_size + 1) * sizeof(*in));
  if (!in) return;

  out = malloc((string_size + 1) * sizeof(*out));
  if (!out) {
    free(in);
    return;
  }

  if (popstring(in) == 1 || popstring(out) == 1) {
    FREE();
    return;
  }
  set_w = popint();
  set_h = popint();

#ifdef STBI_WINDOWS_UTF8
  char *inUtf8 = wchar_to_utf8(in);
  if (!inUtf8) {
    FREE();
    return;
  }

  char *outUtf8 = wchar_to_utf8(out);
  if (!outUtf8) {
    free(inUtf8);
    FREE();
    return;
  }
# undef FREE
# define FREE()  free(in); free(out); free(inUtf8); free(outUtf8);
#else
# define inUtf8 in
# define outUtf8 out
#endif

  fp = _tfopen(in, _T("rb"));
  if (!fp) {
    FREE();
    return;
  }

  fseek(fp, 0, SEEK_END);
  fsize = ftell(fp);

  if (fclose(fp) != 0 || fsize > (2*1024*1024)) {
    FREE();
    return;
  }

  svg = nsvgParseFromFile(inUtf8, "px", 96.0);
  if (!svg) {
    FREE();
    return;
  }

  w = (int)(svg->width + 0.5);
  h = (int)(svg->height + 0.5);

  if (set_w > 0) {
    scale_x = (double)set_w / (double)w;
    w = set_w;
  }

  if (set_h > 0) {
    scale_y = (double)set_h / (double)h;
    h = set_h;
  }

  rast = nsvgCreateRasterizer();
  data = malloc(w*h*4);

  if (rast && data) {
    nsvgRasterizeXY(rast, svg, 0, 0, scale_x, scale_y, data, w, h, w*4);
    stbi_write_bmp(outUtf8, w, h, 4, data);
  }

  if (rast) nsvgDeleteRasterizer(rast);
  if (svg) nsvgDelete(svg);
  if (data) free(data);
  FREE();
}

/*
BOOL WINAPI DllMain(HINSTANCE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
  g_hInstance = hInst;
  return TRUE;
}
*/
