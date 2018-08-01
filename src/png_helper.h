/*
 * Copyright (c) 2018 Matt Monsour
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __LIBPNG_HELPER_H
#define __LIBPNG_HELPER_H

#include <stdlib.h>

#define PNGHELPER_WRITE_FAILED          -1
#define PNGHELPER_FILE_OPEN_ERROR       -2
#define PNGHELPER_LIBPNG_INIT_FAILED    -3
#define PNGHELPER_OUT_OF_MEMORY         -4

#ifdef __cplusplus
extern "C" {
#endif
    int pnghelper_write_image_file(const char *file, const void *bytes, size_t width, size_t height);
#ifdef __cplusplus
}
#endif

#endif
