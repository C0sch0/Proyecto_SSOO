#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cr_API.h"

//TEST 3: OPEN

int main(int argc, char *argv[]) {
  if (argc != 2)
  {
    printf("Input debe coindicir con: ./crfs <simdisk.bin>\n");
    return 1;
  }
  char* diskname = argv[1];

  cr_mount(diskname);
  if (!ruta_archivo) {
    printf("Disk not mounted");
    return 1;
  }

  create_dir_blocks();
  create_cr_bitmaps();

  printf("-----------------------------------------------------------------------\n");
  printf("------------------------ TEST funciones OPEN ------------------------ \n");
  printf("-----------------------------------------------------------------------\n");

  printf("1. Abriremos el archivo 'Baroque.mp3' de la partición 1 en formato de lectura\n");
  printf("Funcionalidad a probar: Archivos abiertos en formato de lectura no se pueden escribir\n");

  crFILE* arch = cr_open(1, "Baroque.mp3","r");
  FILE* prueba = fopen("prueba.txt", "r");
  char* texto = calloc(14000, sizeof(char));
  fgets(texto, 10000, prueba);

  if(arch != NULL){
    printf("Intentaremos escribir en 'Baroque.mp3'\n");
    int num = cr_write(arch, texto, 14000);
    cr_close(arch);
  }

  free(texto);
  fclose(prueba);

  printf("2. Abriremos un archivo con el nombre 'Baroque444.mp3' en la partición 1 en formato de escritura\n");
  printf("Funcionalidad a probar: Archivos abiertos en formato de escritura no se pueden leer\n");

  crFILE* arch2 = cr_open(1, "Baroque444.mp3","w");
  char* buffer = calloc(1001, sizeof(char));

  if(arch2 != NULL){
    printf("Intentaremos leer en 'Baroque444.mp3'\n");
    int num2 = cr_read(arch2, buffer, 1000);
    cr_close(arch2);
  }
  free(buffer);

  printf("3. Abriremos un archivo con el nombre 'Baroque.mp3' en la partición 1 en formato de escritura\n");
  printf("Funcionalidad a probar: Archivos que ya existen no peuden ser abiertos en formato de escritura\n");


  crFILE* arch3 = cr_open(1, "Baroque.mp3","w");
  if(arch3 != NULL){
    cr_close(arch3);
  }

  printf("4. Abriremos el archivo con el nombre '3/Baroque.mp3' de la partición 4 en formato de lectura\n");
  printf("Funcionalidad a probar: Archivos que son un 'broken link' no pueden ser abiertos\n");

  crFILE* arch4 = cr_open(4, "3/Baroque.mp3","r");
  if(arch4 != NULL){
    cr_close(arch4);
  }

  printf("5. Abriremos un archivo con el nombre 'Baroque.mp3' en la partición 4 en formato de escritura\n");
  printf("Funcionalidad a probar: Archivos que no existen en la particion no pueden ser abiertos\n");

  crFILE* arch5 = cr_open(4, "Baroque.mp3","r");
  if(arch5 != NULL){
    cr_close(arch5);
  }

  destroy_directories();
  destroy_bitmaps();
  return 0;

}
