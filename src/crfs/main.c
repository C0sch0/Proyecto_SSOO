#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cr_API.h"

int main(int argc, char *argv[]) {
  if (argc != 2)
  {
    printf("Input debe coindicir con: ./crfs <simdisk.bin>\n");
    return 0;
  }

  cr_mount("simdiskfilled.bin");
  if (!ruta_archivo) {
    printf("Disk not mounted");
    return 0;
  }

  create_dir_blocks();
  create_cr_bitmaps();
  crFILE* arch2 = cr_open(4, "Aprobando12345.mp3","r");
  char* buffer = calloc(1001, sizeof(char));
  int num2 = cr_read(arch2, buffer, 1000);
  printf("%s\n", buffer);
  printf("\n\n\n");
  char* buffer2 = calloc(1001, sizeof(char));
  int num3 = cr_read(arch2, buffer2, 1000);
  printf("%s\n", buffer2);
  printf("\n\n\n");
  char* buffer3 = calloc(1001, sizeof(char));
  int num4 = cr_read(arch2, buffer3, 1000);
  printf("%s\n", buffer3);
  char* buffer4 = calloc(1001, sizeof(char));
  int num5 = cr_read(arch2, buffer4, 1000);
  printf("%s\n", buffer4);
  printf("\n\n\n");
  char* buffer5 = calloc(1001, sizeof(char));
  int num6 = cr_read(arch2, buffer5, 1000);
  printf("%s\n", buffer5);
  printf("\n\n\n");
  char* buffer6 = calloc(1001, sizeof(char));
  int num7 = cr_read(arch2, buffer3, 1000);
  printf("%s\n", buffer6);
  printf("\n\n\n");
  free(buffer);
  free(buffer2);
  free(buffer3);
  free(buffer4);
  free(buffer5);
  free(buffer6);
  cr_close(arch2);
  destroy_directories();
  destroy_bitmaps();
  return 0;

}

// bloque directorio partición 1 parte en 0
// bloque directorio partición 2 parte en 536870912
// bloque directorio partición 3 parte en 536870912*2
// bloque directorio partición 4 parte en 536870912*3

//bloque bitmap particion 1 parte en 8192
//bloque bitmap particion 2 parte en 536870912 + 8192
//bloque bitmap particion 3 parte en 536870912*2 + 8192
//bloque bitmap particion 4 parte en 536870912*3 + 8192
