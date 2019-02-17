#include "entis.h"

int main(int argc, char *argv[])
{
  entis_init(500, 500, ENTIS_ALL);
  entis_fb_rect(50, 50, 200, 200);
  entis_write_bmp("test.bmp");
  entis_term();
  return 0;
}
