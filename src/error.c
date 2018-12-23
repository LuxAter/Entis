#include "error.h"

#include <stdarg.h>
#include <stdio.h>

void entis_error(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "\033[91m[ERROR  ]");
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\033[0m\n");
  va_end(args);
}
void entis_warning(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "\033[93m[WARNING]");
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\033[0m\n");
  va_end(args);
}
void entis_info(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stdout, "\033[95m[INFO   ]");
  vfprintf(stdout, fmt, args);
  fprintf(stdout, "\033[0m\n");
  va_end(args);
}
void entis_note(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stdout, "\033[90m[NOTE   ]");
  vfprintf(stdout, fmt, args);
  fprintf(stdout, "\033[0m\n");
  va_end(args);
}
