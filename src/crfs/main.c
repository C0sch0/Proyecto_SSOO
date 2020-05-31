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
  printf("En la particion 1 tenemos:\n");
  printf("--- CR LS ---\n");
  cr_ls(3);
  char file1[40] =  "prueba.txt";
  char file2[40] = "unload/copia_QPC.gif";
  printf("Archivo copiado\n");
  cr_load(3, file1);
  printf("--- CR LS ---\n");
  cr_ls(3);
  destroy_directories();
  destroy_bitmaps();

  return 0;
}
