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
  printf("Funcionalidad 1: Copiar un archivo\n");
  printf("En la particion 1 tenemos:\n");
  printf("------------ CR LS ------------\n");
  cr_ls(1);
  printf("------------ FIN CR LS ------------\n");
  printf("Copiaremos el archivo de nombre QPC.gif de la particion 1\n");
  printf("Copiaremos el archivo de nombre QPC.gif de la particion 1\n");
  printf("En ruta apuntaremos a la carpeta ./unload/\n");

  char file1[40] =  "QPC.gif";
  char file2[40] =  "guides.txt";
  char dest1[40] = "unload/copia_QPC.gif";
  char dest2[40] = "unload/copia_guides.txt";

  cr_unload(1, file1, dest1);
  cr_unload(1, file2, dest2);
  printf("Archivo copiado desde ./guides.txt a ./unload/copy_guides.txt\n");
  printf("------------ CR LS ------------\n");
  cr_ls(1);
  printf("------------ FIN CR LS ------------\n");

  printf("-------------------------------------------------\n");

  printf("Funcionalidad 2: Particion\n");
  printf("Que particion ? (1-4):");
  char direccion3[40] =  "unload/";
  unsigned disk;

  scanf("%u", &disk);
  printf("Copiando todos los archivos de la particion %u a la carpeta %s \n", disk, direccion3);
  cr_unload(disk, NULL, direccion3);
  printf("=== Particion copiada ===\n");

  printf("-------------------------------------------------:\n");
  char file4[40] = "ungg";
  printf("Funcionalidad 3: Disco Completo\n");
  printf("Ruta a la que copiar el disco completo:");

  //scanf("%s", file4);
  printf("Copiando todos los archivos del disco a la carpeta %s \n", direccion3);
  cr_unload(0, NULL, direccion3); //file4
  printf("Disco copiado\n");

  printf("-------------------------------------------------:\n");
  destroy_directories();
  destroy_bitmaps();

  return 0;
}
