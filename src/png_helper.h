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
