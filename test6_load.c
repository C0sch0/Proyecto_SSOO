#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cr_API.h"

//TEST 4

int main(int argc, char *argv[]) {
  if (argc != 2)
  {
    printf("Input debe coindicir con: ./crfs <simdisk.bin>\n");
    return 0;
  }
  char* diskname = argv[1];

  cr_mount(diskname);

  if (!ruta_archivo) {
    printf("Disk not mounted");
    return 0;
  }

  create_dir_blocks();
  create_cr_bitmaps();

  char file1[20] =  "guides.txt";
  char file2[20] = "copia_guides.txt";
  cr_ls(1);
  printf("-------------------------------------------------:\n");
  cr_load(1, file1, file2);
  printf("-------------------------------------------------:\n");
  cr_ls(1);
  destroy_directories();
  destroy_bitmaps();

  return 0;
}
