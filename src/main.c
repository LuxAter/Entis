#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "entis.h"
/* #include "window.h" */
/* #include "color.h" */
/* #include "primitives.h" */

int main(int argc, char *argv[])
{
  entis_init("Entis", 500, 500, 0, NULL);
  entis_set_background(ENTIS_BLACK);
  entis_set_color(ENTIS_WHITE);
  entis_flush();
  entis_point(100, 100);
  entis_wait_event();
  entis_term();
  return 0;
}
