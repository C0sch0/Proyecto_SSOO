#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cr_API.h"

//TEST 1

int main(int argc, char *argv[]) {
  if (argc != 2)
  {
    printf("Input debe coindicir con: ./crfs <simdisk.bin>\n");
    return 1;
  }

  cr_mount(argv[1]);
  if (!ruta_archivo) {
    printf("Disk not mounted");
    return 1;
  }

  create_dir_blocks();
  create_cr_bitmaps();
  cr_ls(4);
  int e1 = cr_exists(4,"1/Baroque.mp3");
  printf("Existe 1/Baroque.mp3 en la particion 4? (1 es si, 0 es no) %d\n", e1);
  int e2 = cr_exists(4,"Baroque.mp3");
  printf("Existe Baroque.mp3 en la particion 4? (1 es si, 0 es no) %d\n", e2);
  destroy_directories();
  destroy_bitmaps();
  return 0;

}