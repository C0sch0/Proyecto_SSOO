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
  crFILE* arch = cr_open(4, "ABaroque.mp3","w");
  if (arch!= NULL)
  {
  	if(arch->indice->file_size != NULL)
    {
  		printf("%ld\n", arch->indice->file_size);
  	}
  	printf("%d\n", arch->n_b_indice);

    crFILE* prueba = cr_open(1, "Baroque.mp3","r");
    char * buffer = malloc(sizeof(char)*18000);;
    cr_read(prueba, buffer, 18000);

  	destroy_crfile(arch);
    destroy_crfile(prueba);
  	destroy_directories();
    destroy_bitmaps();

  return 0;
  }
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
