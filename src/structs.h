#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct block_indirect_simple {
  unsigned int *indirect_blocks_data[2048]; //array de punteros a data blocks
} Indirect;


typedef struct data {
  char *content[1024]; // array del contenido del bloque
} Data_Block;


typedef struct block_index {
  unsigned int references; //numero de hardlinks del archivo
  unsigned long file_size; //tamaño
  Data_Block *blocks_data[2044]; //array de punteros a data blocks
  Indirect *indirect_simple; //puntero a bloque de indireccionamiento simple
} Index;

/// estructura entrada  directorio auxiliar-->tami
typedef struct directory_entry_aux{
  char valid[3];
  char file_name[29];
} Entry_aux;

<<<<<<< HEAD
=======
typedef struct directory_entry{
  bool valid; //1 bit --> validez
  Index *file_pointer; //23 bits --> puntero al archivo
  char *file_name[29]; //29 bytes --> nombre de archivo incluyendo extension (ASCII)
} Entry;
>>>>>>> updating structs

/// estructura entrada directorio final-->tami
typedef struct directory_entry{
  int valid;
  char *number;
  char *file_name;
} Entry;

<<<<<<< HEAD

typedef struct block_directory {
Entry entries[256]; // cad aentrada corresponde a un archivo en particular
=======
typedef struct block_directory {
Entry *entries; // cad aentrada corresponde a un archivo en particular
>>>>>>> updating structs
} Directory;


typedef struct block_bitmap {
bool *map[64000]; //array que representa a cada uno de los bloques de una
} Bitmap;        //particion --> 0: libre, 1: ocupado.


typedef struct partition {
  Directory *directory_partition;
  Bitmap *bitmap_partition;
} Partition;
