#ifndef ENTIS_ERROR_H_
#define ENTIS_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>

void entis_error(const char* fmt, ...);
void entis_warning(const char* fmt, ...);
void entis_info(const char* fmt, ...);
void entis_debug(const char* fmt, ...);
void entis_note(const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif  // ENTIS_ERROR_H_
