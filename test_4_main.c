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
  printf("------------------------ TEST funciones Write ------------------------ \n");
  printf("-----------------------------------------------------------------------\n");


  printf("Funcionalidades a probar: Correcta escritura y lectura en el disco.\n");

  printf("1. Abrimos un archivo en modo de lectura'\n");

  crFILE* arch = cr_open(4, "Orgullo y Prejuicio.txt","w");

  printf("2. Creamos un buffer con un extracto del libro 'Orgullo y Prejuicio'\n");
  FILE* prueba = fopen("prueba.txt", "r");
  char* texto = calloc(14000, sizeof(char));
  fgets(texto, 10000, prueba);

  printf("3. Escribimos el buffer en el archivo creado\n");
  int num = cr_write(arch, texto, 14000);
  free(texto);
  fclose(prueba);
  cr_close(arch);

  printf("4. Cerramos el archivo creado y lo abrimos en modo de lectura\n");
  crFILE* arch2 = cr_open(4, "Orgullo y Prejuicio.txt","r");

  printf("5. Creamos un buffer de 1000 bytes\n");
  char* buffer = calloc(1001, sizeof(char));

  printf("6. Leemos 1000 bytes del archivo\n");
  int num2 = cr_read(arch2, buffer, 1000);
  printf("num bytes leidos = %d\n", num2);
  printf("Extracto 1 = %s\n", buffer);
  printf("\n\n\n");

  printf("7. Creamos un nuevo buffer de 1000 bytes\n");
  char* buffer2 = calloc(1001, sizeof(char));

  printf("8. Leemos 1000 bytes del archivo partiendo del punto en que quedó la lectura anterior\n");
  int num3 = cr_read(arch2, buffer2, 1000);
  printf("num bytes leidos = %d\n", num3);
  printf("Extracto 2 = %s\n", buffer2);
  printf("\n\n\n");

  printf("9. Creamos un nuevo buffer de 7000 bytes\n");
  char* buffer3 = calloc(7001, sizeof(char));

  printf("10. Leemos 7000 bytes del archivo partiendo del punto en que quedó la lectura anterior\n");
  int num4 = cr_read(arch2, buffer3, 7000);
  printf("num bytes leidos = %d\n", num4);
  printf("Extracto 3 = %s\n", buffer3);
  printf("\n\n\n");

  printf("11. Creamos un último buffer 1000 bytes\n");
  char* buffer4 = calloc(1001, sizeof(char));
  printf("12. Leemos 1000 bytes del archivo partiendo del punto en que quedó la lectura anterior\n")
  int num5 = cr_read(arch2, buffer4, 1000);
  printf("num bytes leidos = %d\n", num5);
  printf("Extracto 4 = %s\n", buffer4);
  printf("\n\n\n");
  free(buffer);
  free(buffer2);
  free(buffer3);
  free(buffer4);
  cr_close(arch2);
  destroy_directories();
  destroy_bitmaps();
  return 0;

}
