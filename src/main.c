#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdbool.h>
#include<unistd.h>
#include <sys/types.h>
#include "structs.h"

char * ruta_archivo;
Directory* Dir_disk[4];

Entry * entry_init(){
  Entry * entrada = malloc(sizeof(Entry));
  entrada->file_name = malloc(sizeof(char)*29);
  entrada->valid = malloc(sizeof(int));
  entrada->number = malloc(sizeof(char)*23);
  return entrada;
}

void cr_mount(char* diskname){
  ruta_archivo = diskname;
}

void *directorio_append(Directory* bloque, Entry *entrada, int i){

  bloque-> entries[i] = *entry_init();
  memcpy(bloque-> entries[i].file_name, entrada->file_name, 29);
}

int cr_exists(Directory disk, char* filename){

  for(int i = 0; i<256;i++){
    if(strncmp(disk.entries[i].file_name , filename, 32) ==0 ){
      //printf("existe %s\n", disk.entries[i].file_name );
      return 1;
    }
  }
  //printf("no existe\n");
  return 0;
}


void* create_dir_blocks(){

  //Directory * bloque[4];
  Entry* entrada[256];
  Entry_aux entrada_aux;

  FILE* disk = fopen("simdiskfilled.bin", "r");

  for(int i = 0;i < 256 ; i++){
    entrada[i]= entry_init();
  }

  for(int i = 0;i < 4 ; i++){
    Dir_disk[i]= malloc(sizeof(Directory));
  }

  for(int f = 0; f < 4 ; f++){
    fseek(disk, 536870912*f, SEEK_SET);
    for(int i = 0; i< 256;i++){
      fread(&entrada_aux, 32, 1, disk);
      memcpy(entrada[i]->file_name, entrada_aux.file_name, 29);
      directorio_append(Dir_disk[f], entrada[i], i);
    }
  }

  //printf("%s\n",Dir_disk[0]-> entries[0].file_name);

  //cr_exists(*Dir_disk[0], "Baroque.mp3");

  //fseek(disk, 0, SEEK_END);

  //printf("%ld", ftell(disk));

  for(int i = 0;i < 256 ; i++){
    entrada[i]= entry_init();
    free(entrada[i]->file_name);
    free(entrada[i]);
  }

}

int main() {

  cr_mount("simdiskfilled.bin");

  create_dir_blocks();

  return 0;
}

// bloque directorio partición 1 parte en 0
// bloque directorio partición 2 parte en 536870912
// bloque directorio partición 3 parte en 536870912*2
// bloque directorio partición 4 parte en 536870912*3
