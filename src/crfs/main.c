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
  //cr_bitmap(4, 0);
  // cr_ls(4);
  crFILE* arch = cr_open(4, "Aprobando123.mp3","w");
  FILE* prueba = fopen("prueba.txt", "r");
  char* texto = calloc(10000, sizeof(char));
  fgets(texto, 10000, prueba);
  int num = cr_write(arch, texto, 10000);
  printf("num = %d\n", num);
  //destroy_directories();
  // destroy_bitmaps();
  // create_dir_blocks();
  // create_cr_bitmaps();
  // cr_ls(4);
  free(texto);
  fclose(prueba);
  cr_close(arch);
  crFILE* arch2 = cr_open(4, "Aprobando123.mp3","r");
  cr_close(arch2);
  // FILE* prueba_2 = fopen("simdiskfilled.bin", "r");
  // char* texto2 = calloc(10000, sizeof(char));
  // fseek(prueba_2, 196611*8192, SEEK_SET);
  // fgets(texto2, 10000, prueba_2);
  // fclose(prueba_2);
  // printf("%s\n", texto2);
  // free(texto2);
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
