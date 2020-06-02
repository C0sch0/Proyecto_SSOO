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
  char* archivo = malloc(sizeof(char)*30);
  memcpy(archivo, "prueba3.txt", 29);
  cr_load(disk, archivo);
  printf("3. Archivo: %s copiado\n", archivo);
  printf("------------------------ CR LS PARTICION %u ------------------------\n", disk);
  cr_ls(disk);
  printf("------------------------ FIN CR LS ------------------------\n");
  crFILE* fl =cr_open(4,"prueba3.txt", "r");
  cr_close(fl);
  free(archivo);
  printf("-----------------------------------------------------------------------\n");
  printf("Funcionalidad 2: Subir una carpeta a cierta particion\n");
  printf("A que particion desea copiar la carpeta? (1-4):");
  unsigned disk2;
  scanf("%u", &disk2);
  printf("------------------------ CR LS PARTICION %u ------------------------\n", disk2);
  cr_ls(disk2);
  printf("------------------------ FIN CR LS ------------------------\n");
  char* carpeta = malloc(sizeof(char)*30);
  memcpy(carpeta, "/load/", 29);
  printf("Que directorio quiere leer?: (/load/)\n");
  printf("3. Carpeta: %s copiada\n", carpeta);
  cr_load(disk2, carpeta);
  printf("------------------------ CR LS PARTICION %u ------------------------\n", disk2);
  cr_ls(disk2);
  printf("------------------------ FIN CR LS ------------------------\n");
  printf("-----------------------------------------------------------------------\n");
  free(carpeta);
  destroy_directories();
  destroy_bitmaps();

  return 0;
}
