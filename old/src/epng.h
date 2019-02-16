#ifndef ENTIS_EPNG_H_
#define ENTIS_EPNG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <png.h>

void entis_write_png_from_buffer(const char* file);
void entis_read_png_to_buffer(const char* file);

#ifdef __cplusplus
}
#endif

#endif  // ENTIS_EPNG_H_
