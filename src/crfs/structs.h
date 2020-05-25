#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct block_indirect_simple {
  unsigned int *indirect_blocks_data; //array de punteros a data blocks
} Indirect;


typedef struct data {
  unsigned int *content; // numero de bloque donde esta el contenido
} Data_Block;


typedef struct block_index {
  unsigned int references; //numero de hardlinks del archivo
  unsigned long file_size; //tamaño
  unsigned int *blocks_data; //array de punteros a data blocks
  unsigned int indirect_simple; //puntero a bloque de indireccionamiento simple
} Index;

/// estructura entrada  directorio auxiliar-->tami
typedef struct directory_entry_aux{
  char valid[3];
  char file_name[29];
} Entry_aux;


/// estructura entrada directorio final-->tami
typedef struct directory_entry{
  char *number;
  char *file_name;
} Entry;


typedef struct block_directory {
  Entry *entries[256]; // cad aentrada corresponde a un archivo en particular
} Directory;


typedef struct block_bitmap {
bool *map; //array que representa a cada uno de los bloques de una
} Bitmap;        //particion --> 0: libre, 1: ocupado.


typedef struct partition {
  Directory *directory_partition;
  Bitmap *bitmap_partition;
} Partition;

typedef struct cr_file {
  char *file_name;
  char *valid;
  char *mode;
  int n_b_indice;
  int  estado;
  int  dir;
  Index* indice;
} crFILE;
