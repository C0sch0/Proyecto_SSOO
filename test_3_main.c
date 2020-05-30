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
  crFILE* arch = cr_open(1, "Baroque.mp3","r");
  FILE* prueba = fopen("prueba.txt", "r");
  char* texto = calloc(14000, sizeof(char));
  fgets(texto, 10000, prueba);
  if(arch != NULL){
    int num = cr_write(arch, texto, 14000);
    cr_close(arch);  
  }
  free(texto);
  fclose(prueba);

  crFILE* arch2 = cr_open(1, "Baroque444.mp3","w");
  char* buffer = calloc(1001, sizeof(char));
  if(arch2 != NULL){
    int num2 = cr_read(arch2, buffer, 1000);
    cr_close(arch2);
  }
  free(buffer);
  
  crFILE* arch3 = cr_open(1, "Baroque.mp3","w");
  if(arch3 != NULL){
    cr_close(arch3);
  }
  
  crFILE* arch4 = cr_open(4, "3/Baroque.mp3","r");
  if(arch4 != NULL){
    cr_close(arch4);
  }

  crFILE* arch5 = cr_open(4, "Baroque.mp3","r");
  if(arch5 != NULL){
    cr_close(arch5);
  }

  destroy_directories();
  destroy_bitmaps();
  return 0;

}