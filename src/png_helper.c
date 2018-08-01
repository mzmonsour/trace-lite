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

#include "png_helper.h"
#include <png.h>
#include <stdio.h>
#include <stdint.h>

int pnghelper_write_image_file(const char *file, const void *bytes, size_t width, size_t height)
{
    int result = 0;

    FILE *fp = NULL;
    png_structp png = NULL;
    png_infop pnginfo = NULL;
    uint8_t** rows = NULL;

    fp = fopen(file, "wb");
    if (!fp) {
        result = PNGHELPER_FILE_OPEN_ERROR;
        goto cleanup;
    }

    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        result = PNGHELPER_LIBPNG_INIT_FAILED;
        goto cleanup;
    }
    pnginfo = png_create_info_struct(png);
    if (!pnginfo) {
        result = PNGHELPER_LIBPNG_INIT_FAILED;
        goto cleanup;
    }

    if ((rows = malloc(sizeof(*rows) * height)) == NULL) {
        result = PNGHELPER_OUT_OF_MEMORY;
        goto cleanup;
    }
    for (size_t i = 0; i < height; ++i) {
        rows[i] = ((uint8_t*)bytes) + width * 3 * i;
    }

    if (setjmp(png_jmpbuf(png))) {
        result = PNGHELPER_WRITE_FAILED;
        goto cleanup;
    }
    png_init_io(png, fp);
    png_set_IHDR(
            png, pnginfo,
            width, height,
            8,
            PNG_COLOR_TYPE_RGB,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT);

    png_set_sRGB(png, pnginfo, PNG_sRGB_INTENT_ABSOLUTE);

    png_write_info(png, pnginfo);
    png_write_image(png, rows);
    png_write_end(png, NULL);

    cleanup:
    if (fp) {
        fclose(fp);
    }
    if (png) {
        png_destroy_write_struct(&png, &pnginfo);
    }
    free(rows);
    return result;
}
