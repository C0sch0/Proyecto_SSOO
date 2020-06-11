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
  printf("------------------------ TEST funciones UNLOAD ------------------------ \n");
  printf("-----------------------------------------------------------------------\n");
  printf("Funcionalidad 1: Copiar un archivo desde disco X desde una direccion de origen a una destino\n");
  printf("Usaremos archivos de la particion: 1\n");
  printf("Por ejemplo, en la particion 1 tenemos:\n");
  printf("------------------------ CR LS PARTICION 1 ------------------------\n");
  cr_ls(1);
  printf("------------------------ FIN CR LS ------------------------\n");
  printf("Ingresa el nombre de algun archivo de la Particion 1\n");
  char unload_file[32];
  scanf("%s", unload_file);
  printf("Cómo quieres llamar al nuevo archivo ?\n");
  char new_name[32];
  scanf("%s", new_name);
  printf("----------------------------------------------------\n");
  printf("Copiaremos el archivo: %s de la particion 1\n", unload_file);
  printf("Y lo llamaremos: %s\n", new_name);
  printf("----------------------------------------------------\n");

  cr_unload(1, unload_file, new_name);

  printf("Archivo copiado desde %s a ruta especifica %s\n", unload_file, new_name);

  printf("-------------------------------------------------\n");
  printf("Funcionalidad 2: Particion completa de disco.\n");
  printf("Que particion desea copiar a < unload/ >  ? (1-4):");

  unsigned disk;
  scanf("%u", &disk);
  printf("Donde quieres dejar todo esto ? ( Sugerencia: unload/ )");
  char unload_folder[32];
  scanf("%s", unload_folder);

  printf("Copiando todos los archivos de la particion %u a la carpeta %s \n", disk);
  cr_unload(disk, NULL, unload_folder);
  printf("=== Particion copiada ===\n");

  printf("-------------------------------------------------:\n");
  printf("Funcionalidad 3: Disco Completo\n");
  printf("Donde quieres dejar todo esto ? ( Sugerencia: unload_disk/ )");
  char unload_disk_folder[32];
  scanf("%s", unload_disk_folder);

  //scanf("%s", file4);
  printf("Copiando todos los archivos del disco a la carpeta %s \n", unload_disk_folder);
  cr_unload(0, NULL, unload_disk_folder); //file4
  printf("Disco copiado\n");

  printf("-------------------------------------------------:\n");
  destroy_directories();
  destroy_bitmaps();

  return 0;
}
