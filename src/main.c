#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdbool.h>
#include<unistd.h>
#include <sys/types.h>
#include "linked_list.h"

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

void cr_mount(char* diskname)
{
  ruta_archivo = diskname;
}

void directorio_append(Directory* bloque, Entry *entrada, int i)
{
  bloque-> entries[i] = entry_init();
  memcpy(bloque-> entries[i]->file_name, entrada->file_name, 29);
}


int cr_exists(Directory disk, char* filename)
{ char *ptr;

  for(int i = 0; i<256;i++){
    ptr = strstr(disk.entries[i]->file_name, filename);
    if(strncmp(disk.entries[i]->file_name , filename, 32) ==0 ){
      printf("existe %s\n", disk.entries[i]->file_name );
      return 1;
    }

    if(ptr!=NULL){
      printf("existe %s\n", disk.entries[i]->file_name );
      return 1;
    }

  }

  printf("no existe\n");
  return 0;
}



void create_dir_blocks()
{
  //Directory * bloque[4];
  //Entry* entrada[256];
  Entry_aux entrada_aux;

  FILE* disk = fopen("simdiskfilled.bin", "r");

  //for(int i = 0;i < 256 ; i++){
  //  entrada[i]= entry_init();
  //}

  for(int i = 0;i < 4 ; i++){
    Dir_disk[i]= malloc(sizeof(Directory));
  }

  for(int f = 0; f < 4 ; f++){
    fseek(disk, 536870912*f, SEEK_SET);
    for(int i = 0; i< 256;i++){
      fread(&entrada_aux, 32, 1, disk);
      Dir_disk[f]-> entries[i] = entry_init();
      //memcpy(entrada[i]->file_name, entrada_aux.file_name, 29);
      //memcpy(entrada[i]->number, entrada_aux.valid, 24);
      //directorio_append(Dir_disk[f], entrada[i], i);
      memcpy(Dir_disk[f]-> entries[i]->file_name, entrada_aux.file_name, 29);
      memcpy(Dir_disk[f]-> entries[i]->number, entrada_aux.valid, 24);
    }
  }
  //printf("%s\n",Dir_disk[0]-> entries[0].file_name);
  //fseek(disk, 0, SEEK_END);
  //printf("%ld", ftell(disk));
  //for(int i = 0;i < 256 ; i++){
  //  entrada[i]= entry_init();
  //  free(entrada[i]->file_name);
  //  free(entrada[i]);
  //}
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

void destroy_bitmaps()
{
	for(int i = 0; i < 4; i++)
	{
		free(bitmaps[i]->map);
		free(bitmaps[i]);
	}
}

int main() {

  cr_mount("simdiskfilled.bin");
	create_dir_blocks();
  cr_exists(*Dir_disk[3], "Baroque.mp3");
	//create_cr_bitmaps();
	//destroy_bitmaps();
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
