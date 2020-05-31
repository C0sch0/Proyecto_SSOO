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

  char file1[40] =  "guides.txt";
  char file2[40] = "copia_guides.txt";
  printf("Funcionalidad 1: Copiar un archivo\n");
  printf("Copiaremos el archivo de nombre guides.txt, de la particion 1\n");
  cr_ls(1);
  printf("-------------------------------------------------:\n");
  printf("Funcionalidad 2: Particion\n");
  cr_unload(1, file1, file2);
  printf("-------------------------------------------------:\n");
  printf("Funcionalidad 3: Disco Completo\n");
  cr_unload(1, file1, file2);
  printf("-------------------------------------------------:\n");
  cr_ls(1);
  destroy_directories();
  destroy_bitmaps();

  return 0;
}
