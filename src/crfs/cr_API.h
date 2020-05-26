#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "math.h"


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


extern char* ruta_archivo;
extern Directory* Dir_disk[4];
extern Bitmap* bitmaps[4];

Entry * entry_init();
void cr_mount(char* diskname);
void directorio_append(Directory* bloque, Entry *entrada, int i);
int cr_exists(unsigned disk, char* filename);
void cr_ls(unsigned disk);
void create_dir_blocks();
int buscar_entry_disponible(Directory* directorio);
void print_bitmap_bin(Bitmap* bitmap_block, bool hex);
int buscar_bloque_disponible(Bitmap* bitmap);
void cr_bitmap(unsigned disk, bool hex);
Bitmap* bitmap_init();
void create_cr_bitmaps();
void destroy_directories();
void destroy_bitmaps();
