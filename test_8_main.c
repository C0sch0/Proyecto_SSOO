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

  printf("-----------------------------------------------------------------------\n");
  printf("------------------------ TEST funciones LOAD ------------------------ \n");
  printf("-----------------------------------------------------------------------\n");
  printf("Funcionalidad 1: Subir un archivo a cierta particion\n");
  printf("A que particion desea copiar a el archivo? (1-4):");
  unsigned disk;
  scanf("%u", &disk);
  printf("En la particion %u tenemos:\n", disk);
  printf("------------------------ CR LS PARTICION %u ------------------------\n", disk);
  cr_ls(disk);
  printf("------------------------ FIN CR LS ------------------------\n");
  printf("2. Que archivo deseas subir ?\n");
  char archivo[32];
  scanf("%s", archivo);
  cr_load(disk, archivo);
  printf("3. Archivo: %s copiado\n", archivo);
  printf("------------------------ CR LS PARTICION %u ------------------------\n", disk);
  cr_ls(disk);
  printf("------------------------ FIN CR LS ------------------------\n");

  printf("-----------------------------------------------------------------------\n");
  printf("Funcionalidad 2: Subir una carpeta a cierta particion\n");
  printf("A que particion desea copiar la carpeta? (1-4):");
  unsigned disk2;
  scanf("%u", &disk2);
  printf("------------------------ CR LS PARTICION %u ------------------------\n", disk2);
  cr_ls(disk2);
  printf("------------------------ FIN CR LS ------------------------\n");
  char carpeta[32];
  scanf("Carpeta: %s", carpeta);
  printf("3. Carpeta: %s copiada\n", carpeta);
  //cr_load(3, carpeta);
  printf("------------------------ CR LS PARTICION %u ------------------------\n", disk2);
  cr_ls(disk2);
  printf("------------------------ FIN CR LS ------------------------\n");
  printf("-----------------------------------------------------------------------\n");

  destroy_directories();
  destroy_bitmaps();

  return 0;
}
