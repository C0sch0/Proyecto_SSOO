#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cr_API.h"

//TEST 2

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
  cr_bitmap(4,1);
  crFILE* arch = cr_open(4, "Orgullo y Prejuicio2.txt","w");
  cr_close(arch);
  cr_bitmap(4,0);
  cr_bitmap(0,0);
  destroy_directories();
  destroy_bitmaps();
  return 0;

}