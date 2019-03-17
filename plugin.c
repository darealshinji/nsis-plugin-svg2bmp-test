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
#include "nanosvg.h"  // patched version
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"  // patched version


//HINSTANCE g_hInstance;
//HWND g_hwndParent;


// based on nsvgParseFromFile()
static NSVGimage *parseSvgFile(LPTSTR filename, const float dpi, const unsigned int maxSize)
{
  FILE *fp = NULL;
  size_t size;
  char *data = NULL;
  NSVGimage *image = NULL;

  fp = _tfopen(filename, _T("rb"));
  if (!fp) goto HANDLE_ERROR;
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  if (size > maxSize) goto HANDLE_ERROR;
  fseek(fp, 0, SEEK_SET);
  data = malloc(size + 1);
  if (data == NULL) goto HANDLE_ERROR;
  if (fread(data, 1, size, fp) != size) goto HANDLE_ERROR;
  data[size] = '\0';  // Must be null terminated.
  fclose(fp);
  image = nsvgParse(data, "px", dpi);
  free(data);

  return image;

HANDLE_ERROR:
  if (fp) fclose(fp);
  if (data) free(data);
  if (image) nsvgDelete(image);
  return NULL;
}


static void write_bmp_file(void *context, void *data, int size)
{
  if (context && data && size > 0) {
    fwrite(data, size, 1, (FILE *)context);
  }
}


__declspec(dllexport)
void svg2bmp(HWND hwndParent,
             int string_size,
             LPTSTR variables,
             stack_t **stacktop,
             extra_parameters *extra,
             ...)
{
  LPTSTR in = NULL;  // arg1: SVG input filename
  LPTSTR out = NULL; // arg2: BMP output filename
  int set_w;         // arg3: output image width (optional)
  int set_h;         // arg4: output image height (optional)

  const float dpi = 96.0f;  // dpi value to render the image
  const int max_filesize = 2*1024*1024;  // maximum input filesize (2MB)
  const int comp = 4;  // RGBA

  FILE *fp = NULL;
  NSVGimage *svg = NULL;
  NSVGrasterizer *rast = NULL;
  unsigned char *img = NULL;
  float scale_x = 1.0f, scale_y = 1.0f;
  int w, h;

  EXDLL_INIT();
  //g_hwndParent = hwndParent;

  in = malloc((string_size + 1) * sizeof(LPTSTR));
  if (!in) return;

  out = malloc((string_size + 1) * sizeof(LPTSTR));
  if (!out) goto CLOSE;

  if (popstring(in) == 1 || popstring(out) == 1) goto CLOSE;
  set_w = popint();
  set_h = popint();

  svg = parseSvgFile(in, dpi, max_filesize);
  if (!svg) goto CLOSE;

  w = (int)(svg->width + 0.5f);
  h = (int)(svg->height + 0.5f);

  if (set_w > 0) {
    scale_x = (float)set_w / (float)w;
    w = set_w;
  }

  if (set_h > 0) {
    scale_y = (float)set_h / (float)h;
    h = set_h;
  }

  rast = nsvgCreateRasterizer();
  if (!rast) goto CLOSE;

  img = malloc(w * h * comp);
  if (!img) goto CLOSE;

  fp = _tfopen(out, _T("wb"));
  if (!fp) goto CLOSE;

  nsvgRasterizeXY(rast, svg, 0.0f, 0.0f, scale_x, scale_y, img, w, h, w * comp);
  stbi_write_bmp_to_func(write_bmp_file, (void *)fp, w, h, comp, img);

CLOSE:
  if (fp) fclose(fp);
  if (rast) nsvgDeleteRasterizer(rast);
  if (svg) nsvgDelete(svg);
  if (img) free(img);
  if (in) free(in);
  if (out) free(out);
}

/*
BOOL WINAPI DllMain(HINSTANCE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
  g_hInstance = hInst;
  return TRUE;
}
*/
