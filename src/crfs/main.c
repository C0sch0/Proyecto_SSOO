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

  printf("1 - En la particion 3 tenemos:\n");
  printf("------------------ CR LS Particion 3 ------------------ \n");
  cr_ls(3);

  printf("2. Probaremos primero copiando un archivo txt y un .gif a esta particion.\n");
  printf("3. Archivo .txt copiado\n");
  printf("4. Archivo .gif copiado\n");

  char file1[40] =  "prueba.txt";
  char file2[40] = "unload/copia_QPC.gif";
  cr_load(3, file1);
  cr_load(3, file2);
  printf("------------------ CR LS Particion 3 ------------------ \n");
  cr_ls(3);

  printf("------------------ Ahora copiamos una carpeta ------------------ \n");
  cr_ls(3);
  destroy_directories();
  destroy_bitmaps();

  return 0;
}
