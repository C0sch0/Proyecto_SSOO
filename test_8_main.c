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

  printf("LOAD\n");
  printf("-------------------------------------------------\n");

  printf("Funcionalidad 1: Subir un archivo a cierta particion\n");

  printf("1 - En la particion 3 tenemos:\n");
  printf("------------------ CR LS Particion 3 ------------------ \n");
  cr_ls(3);
  printf("Particion 3 vacia \n");
  printf("2. Probaremos primero copiando un archivo txt y un .gif a esta particion.\n");
  printf("3. Archivo .txt copiado\n");
  printf("4. Archivo .gif copiado\n");

  char file1[40] =  "prueba.txt";
  char file2[40] = "unload/copia_QPC.gif";
  cr_load(3, file1);
  //cr_load(3, file2);

  //printf("------------------ CR LS Particion 3 ------------------ \n");
  //cr_ls(3);
  //printf("------------------ Ambos archivos han sido creados dentro de 3 ------------------ \n");
  //printf("------------------------------------------------------------------- \n");
  //printf("------------------ Ahora copiamos una carpeta a particion 3 ------------------ \n");
  //printf("------------------ CR LS Particion 3 ------------------ \n");
  //cr_ls(3);
  //char carpeta[40] = "Intrucciones/";
  //printf("------------------ Copiando carpeta ------------------ \n");
  //cr_load(3, carpeta);
  //printf("------------------ Archivos carpeta %s subidos a particion 3 ------------------ \n", carpeta);
  //printf("------------------ CR LS Particion 3 ------------------ \n");
  //cr_ls(3);
  destroy_directories();
  destroy_bitmaps();

  return 0;
}
