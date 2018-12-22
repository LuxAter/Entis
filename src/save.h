#ifndef ENTIS_SAVE_H_
#define ENTIS_SAVE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <xcb/xcb.h>

void entis_save_png(const char* file_name) __attribute__((deprecated));

#ifdef __cplusplus
}
#endif

#endif  // ENTIS_SAVE_H_
