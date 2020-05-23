#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdbool.h>
#include<unistd.h>
#include <math.h>
#include <sys/types.h>
#include "structs.h"


char* ruta_archivo;
Directory* Dir_disk[4];
Bitmap* bitmaps[4];

Entry * entry_init()
{
  Entry * entrada = malloc(sizeof(Entry));
  entrada->file_name = malloc(sizeof(char)*29);
  entrada->number = malloc(sizeof(char)*24);
  return entrada;
}

Data_Block * init_datablock(){
  Data_Block * data = malloc(sizeof(Data_Block));
  data -> content = malloc(sizeof(char)*8192);
  return data;
}

Indirect * init_ind_simple(){
  Indirect * indirect = malloc(sizeof(Indirect));
  for(int i = 0; i < 2048 ; i++){
    indirect -> indirect_blocks_data[i] = malloc(sizeof(int));
  }
  return indirect;
}

Index * init_indice(){
  Index * indice = malloc(sizeof(Index));

  indice -> indirect_simple = init_ind_simple();

  for(int i = 0; i < 2044 ; i++){
    indice -> blocks_data[i]= init_datablock();
  }

  return indice;
}

crFILE* init_crfile(){
  crFILE* file = malloc(sizeof(crFILE));

  file -> file_name = malloc(sizeof(char)*29);
  file -> valid = malloc(sizeof(char)*24);
  file -> mode = malloc(sizeof(char));
  file -> indice = init_indice();

  // Donde estamos en el archivo
  file -> estado = 0;
  file -> dir = 0;

  return file;

}

void destroy_crfile(crFILE*file){
  free(file-> file_name);
  free(file-> valid);
  free(file-> mode);
  free(file);
}

void destroy_data(Data_Block * data){
  free(data -> content);
  free(data);
}

void destroy_ind_dir(Indirect * indirect){
  for(int i = 0; i < 2048 ; i++){
    free(indirect -> indirect_blocks_data[i]);
  }
  free(indirect);
}

void destroy_indice(Index * index){

  free(index -> references);
  free(index -> file_size);
  //destroy_data(indice -> blocks_data);
  //destroy_ind_dir(indice -> indirect_simple);
  free(index);
}

void cr_mount(char* diskname)
{
  ruta_archivo = diskname;
}


// https://gist.github.com/ryankshah/bf45790b968540cdabdeef702883ddbb modificada
int * get_bloque(char * string) {
    int count = 0;
    int num_bloque = 0;
    int stringLength = strlen(string);
    int mask = 0x80; /* 10000000 */
    int * bloque = malloc(sizeof(int)*24);

    for(int i = 0; i < stringLength; i++) {
        mask = 0x80;
        char c = string[i];
        int x = 0;
        while(mask > 0) {
            char n = (c & mask) > 0;
            bloque[count] = (int) n;
            //printf("%i\n", bloque[count]);
            mask >>= 1; /* move the bit down */
            count++;
        }
    }

    // transformamos ahora los bit al numero de bloque
    for(int k = 1; k < 24; k++){
      // no consideramos el 0 ya que ese corresponde al valid bit
      if(bloque[k] == 1){
        num_bloque += pow(2,23-k);
      }
    }

    free(bloque);
    //printf("%d\n", num_bloque);
    return num_bloque;
}

// No la estamos usando por ahora
void directorio_append(Directory* bloque, Entry *entrada, int i)
{
  bloque-> entries[i] = entry_init();
  memcpy(bloque-> entries[i]->file_name, entrada->file_name, 29);
}

int cr_exists(unsigned disk, char* filename)
{ char *particion;
  char *nombre;
  char *str = malloc(sizeof(char)*32);
  int valid = 0;

  for(int i = 0; i<256;i++){
    if(strncmp(Dir_disk[disk-1]->entries[i]->file_name , filename, 32) ==0 ){
      printf("existe\n");
      return 1;
    }else{

      memcpy(str, Dir_disk[disk-1]->entries[i]->file_name, 32);
      particion = strtok(str,"/");
      nombre = strtok(NULL,"/");
      if(nombre!=NULL){
        if(strncmp(nombre , filename, 32) ==0){
          // llamo denuevo a la función y busco el nombre en la partición indicada
          valid = cr_exists(atoi(particion), nombre);
          if(valid == 1){
          return 1;
        }
      }
    }
  }
}

  free(str);
  printf("no existe\n");
  return 0;
}


void cr_ls(unsigned disk)
{
	Directory* disco = Dir_disk[disk-1];
	for (int i =0; i< 256; i++)
	{
		Entry* entrada = disco->entries[i];
		int a = !!((entrada->number[0] << 1) & 0x800000);
		if (a == 1)
		{
			printf("%s\n",entrada->file_name);
		}
	}
}

void create_dir_blocks()
{
  Entry_aux entrada_aux;
  FILE* disk = fopen("simdiskfilled.bin", "r");
  for(int i = 0;i < 4 ; i++)
  {
  	Dir_disk[i]= malloc(sizeof(Directory));
  }
  for(int f = 0; f < 4 ; f++)
  {
    fseek(disk, 536870912*f, SEEK_SET);
    for(int i = 0; i< 256;i++)
    {
      fread(&entrada_aux, 32, 1, disk);
      Dir_disk[f]-> entries[i] = entry_init();
      memcpy(Dir_disk[f]-> entries[i]->file_name, entrada_aux.file_name, 29);
      memcpy(Dir_disk[f]-> entries[i]->number, entrada_aux.valid, 24);
    }
  }
  fclose(disk);
 }

//https://stackoverflow.com/questions/18327439/printing-binary-representation-of-a-char-in-c
void print_bitmap_bin(Bitmap* bitmap_block, bool hex)
{
	int libres = 0;
	int ocupados = 0;
	for (int j =1; j <= 8192; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			int a = !!((bitmap_block->map[j-1] << i) & 0x80);
			if(a == 1)
			{
				ocupados++;
			}
			else
			{
				libres++;
			}
			if(hex)
			{
				printf("0x%x ", a);
			}
			else
			{
				printf("%d ", a);
			}
      	}

      	if(j%64 == 0)
      	{
      		printf("\n");
      	}
	}

	printf("Bloques Libres: %d\n", libres);
	printf("Bloques ocupados: %d\n", ocupados);
}


void cr_bitmap(unsigned disk, bool hex)
{
	if(disk == 0)
	{
		if(hex)
		{
			for(int i =0; i < 4; i++)
			{
				print_bitmap_bin(bitmaps[i], 1);
				printf("\n");
				printf("\n");
			}
		}
		else
		{
			for(int i =0; i < 4; i++)
			{
				print_bitmap_bin(bitmaps[i], 0);
				printf("\n");
				printf("\n");
			}
		}
	}
	else if (disk == 1 || disk == 2 || disk == 3|| disk == 4)
	{
		if(hex)
		{
			print_bitmap_bin(bitmaps[disk-1], 1);
		}
		else
		{
			print_bitmap_bin(bitmaps[disk-1], 0);
		}
	}
	else
	{
		printf("Debe elegir una opcion valida de particion (1-4) o 0 si quiere el de todo el disco\n");
	}
}

Bitmap* bitmap_init()
{
	Bitmap *mapa = malloc(sizeof(Bitmap));
	mapa->map = malloc(sizeof(char)*8192);
	return mapa;
}

void create_cr_bitmaps()
{
  for(int i = 0; i < 4; i++)
	{
		bitmaps[i] = bitmap_init();
	}

	FILE* disk = fopen("simdiskfilled.bin", "r");
	for(int i = 0; i < 4; i++)
	{
		char* mapp = malloc(sizeof(char)*8192);
		int hasta = 536870912*i + 8192;
  	fseek(disk, hasta , SEEK_SET);
  	fread(mapp, 8192, 1, disk);
  	memcpy(bitmaps[i]->map, mapp, 8192);
		free(mapp);
	}
	fclose(disk);

}

void destroy_directories()
{
	for(int i = 0; i <4; i++)
	{
		for(int j = 0; j< 256; j++)
			{
				free(Dir_disk[i]->entries[j]->file_name);
				free(Dir_disk[i]->entries[j]->number);
				free(Dir_disk[i]->entries[j]);
			}
		free(Dir_disk[i]);
	}
}

void destroy_bitmaps()
{
	for(int i = 0; i < 4; i++)
	{
		free(bitmaps[i]->map);
		free(bitmaps[i]);
	}
}

crFILE* cr_open(unsigned disk, char* filename, char *mode){

  char *particion;
  char *nombre;
  char *str = malloc(sizeof(char)*32);
  int prt;

  FILE* disco = fopen(ruta_archivo, "r");

  if(strncmp(mode , "r", 1) == 0){
    int exist;
    // Cheamos que exista el archivo
    exist = cr_exists(disk, filename);
    if(exist == 1){
      // el archivo en el disco si existe creamos el crFILE
      char *ptr;
      crFILE* file;
      file = init_crfile();

      // ahora tengp que buscar el archivo
      for(int i = 0; i<256;i++){

          if(strncmp(Dir_disk[disk-1]->entries[i]->file_name , filename, 32) ==0 ){
            // si el nombre esta en la partición

            // le doy a file los atributos del archivo
            memcpy(file->file_name, Dir_disk[disk-1]->entries[i]->file_name, 29);
            memcpy(file->valid, Dir_disk[disk-1]->entries[i]->number, 3);

            // le cambio asigno el modo en el que se abrio
            memcpy(file->mode, mode, 1);
            //memcpy(file->dir, 0, 1);

            break;
          }
          else{
            // si el nombre no esta en la partición
            memcpy(str, Dir_disk[disk-1]->entries[i]->file_name, 32);
            particion = strtok(str,"/");
            nombre = strtok(NULL,"/");

            if(nombre!=NULL){
              if(strncmp(nombre , filename, 32) == 0){
                prt = atoi(particion);
                for(int j = 0; j<256;j++){
                  // buscamos en la particion
                  if(strncmp(Dir_disk[prt-1]->entries[j]->file_name , filename, 32)==0){
                    // obtengo el indice del archivo que indica la partición a la que pertence

                    memcpy(file->file_name, Dir_disk[prt-1]->entries[j]->file_name, 29);
                    memcpy(file->valid, Dir_disk[prt-1]->entries[j]->number, 3);

                    memcpy(file->mode, mode, 1);

                    // apunta a otro directorio
                    //memcpy(file->dir, 1, 1);

                    break;
                  }
                }
              }
            }
          }
        }

        // Cargamos en bloque indice del archivo a crFILE

        // Buscamos el puntero al ruta_archivo--> decodificar valid del filename
        char *st= malloc(sizeof(char)*3);
        int n_bloque_indice;
        char* indice_aux = malloc(sizeof(char)*8192);

        memcpy(st, file->valid, 3);
        n_bloque_indice = get_bloque(st);
        printf("%d\n", n_bloque_indice);

        //seteamos el archivo en el bloque indice
        fseek(disco, 8192 * n_bloque_indice , SEEK_SET);

        // esto tenemos que decodificarlo a los bytes que correspondan por segnmento
        fread(indice_aux, 8192, 1, disco);

        free(st);
        free(indice_aux);
    }
    else{
      // exist es 0
      printf("El archivo no existe\n");
      }
    }
    else if(strncmp(mode , "w", 1) ==0){
      printf("Mode escritura\n");
    }

free(str);
}

int main() {
  cr_mount("simdiskfilled.bin");
  create_dir_blocks();
  cr_exists(1, "Baroque.mp3");
  cr_ls(1);
  cr_open(1, "Baroque.mp3","r");
  destroy_directories();
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
