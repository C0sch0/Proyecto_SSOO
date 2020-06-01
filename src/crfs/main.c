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
  char destino1[40] = "unload/QPC.gif";
  char destino2[40] = "unload/guides.txt";
  printf("-----------------------------------------------------------------------\n");
  printf("------------------------ TEST funciones UNLOAD ------------------------ \n");
  printf("-----------------------------------------------------------------------\n");
  printf("Funcionalidad 1: Copiar un archivo desde disco X \n");
  printf("En la particion 1 tenemos:\n");
  printf("------------ CR LS PARTICION 1 ------------\n");
  cr_ls(1);
  printf("------------ FIN CR LS ------------\n");
  printf("Copiaremos el archivo de nombre QPC.gif de la particion 1\n");
  printf("Copiaremos el archivo de nombre guides.txt de la particion 1\n");

  char gif_info[40] =  "QPC.gif";
  char gif_new_info[40] =  "copy_QPC.gif";
  char file2[40] =  "guides.txt";
  char file2_new_info2[40] =  "copy_guides.txt";
  cr_unload(1, gif_info, gif_new_info);
  cr_unload(1, file2, file2_new_info2);
  printf("Archivo copiado desde %s a ruta especifica %s\n", gif_info, gif_new_info);
  printf("Archivo copiado desde %s a ruta especifica %s\n", file2, file2_new_info2);


  printf("-------------------------------------------------\n");
  printf("Funcionalidad 2: Archivo a carpeta \n");
  printf("Copiaremos el archivo de nombre QPC.gif de la particion 1\n");
  printf("Copiaremos el archivo de nombre guides.txt de la particion 1\n");
  printf("En ruta apuntaremos a la carpeta ./unload/\n");

  char gif_info2[40] =  "QPC.gif";
  char gif_new_info2[40] =  "copy_QPC.gif";
  char file22[40] =  "guides.txt";
  char file2_new_info22[40] =  "copy_guides.txt";
  char direccion3[40] =  "/unload/";
  cr_unload(1, gif_info, ".unload/");
  cr_unload(1, file2, ".unload/");
  printf("Archivo copiado desde %s a carpeta %s\n", gif_info, gif_new_info);
  printf("Archivo copiado desde %s a ruta especifica %s\n", file2, file2_new_info2);

  printf("-------------------------------------------------\n");
  printf("Funcionalidad 3: Particion\n");
  printf("Que particion ? (1-4):");

  unsigned disk;

  scanf("%u", &disk);
  printf("Copiando todos los archivos de la particion %u a la carpeta %s \n", disk, direccion3);
  cr_unload(disk, NULL, ".unload/");
  printf("=== Particion copiada ===\n");

  printf("-------------------------------------------------:\n");
  char file4[40] = "ungg";
  printf("Funcionalidad 4: Disco Completo\n");
  printf("Ruta a la que copiar el disco completo:");

  //scanf("%s", file4);
  printf("Copiando todos los archivos del disco a la carpeta %s \n", direccion3);
  cr_unload(0, NULL, ".unload/"); //file4
  printf("Disco copiado\n");

  printf("-------------------------------------------------:\n");
  destroy_directories();
  destroy_bitmaps();

  return 0;
}
