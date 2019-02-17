#ifndef ENTIS_BMP_H_
#define ENTIS_BMP_H_

#ifdef cplusplus__
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool entis_bmp_write(const char* file_name, unsigned width, unsigned height,
                     uint32_t** buffer);
bool entis_bmp_read(const char* file_name, uint32_t** buffer);

#ifdef cplusplus__
}
#endif

#endif  // ENTIS_BMP_H_
