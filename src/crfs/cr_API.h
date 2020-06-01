#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <dirent.h>
#include <stdio.h>
#include "structs.h"

#define PARTICIONES 4
#define BLOCK_ENTRIES 256
#define BLOCK_BYTES 8192


char* ruta_archivo;
Directory* Dir_disk[4];
Bitmap* bitmaps[4];

// Inicializadores

Entry * entry_init()
{
  Entry* entrada = calloc(1, sizeof(Entry));
  entrada->file_name = malloc(sizeof(char)*29);
  entrada->number = malloc(sizeof(char)*3);
  return entrada;
}

Bitmap* bitmap_init()
{
	Bitmap *mapa = malloc(sizeof(Bitmap));
	mapa->map = malloc(sizeof(char)*BLOCK_BYTES);
	return mapa;
}


Data_Block* init_datablock()
{
  Data_Block* data = malloc(sizeof(Data_Block));
  data -> content = malloc(sizeof(unsigned int));
  return data;
}

Indirect* init_ind_simple()
{
  Indirect* indirect = malloc(sizeof(Indirect));
  indirect -> indirect_blocks_data = malloc(sizeof(unsigned int)*2048);
  return indirect;
}

Index* init_indice()
{
  Index* indice = malloc(sizeof(Index));
  indice->references = 0;
  indice->file_size = 0;
  indice->indirect_simple = 0;
  indice->blocks_data = malloc(sizeof(unsigned int)*2044);
  return indice;
}

crFILE* init_crfile(){
  crFILE* file = malloc(sizeof(crFILE));
  file -> file_name = malloc(sizeof(char)*29);
  file -> valid = calloc(3, sizeof(char));
  file -> mode = malloc(sizeof(char));
  file -> indice = init_indice();

  // Donde estamos en el archivo
  file -> estado = 0;
  file -> bloque_dir = 0;
  file-> bloques_ocupados = 0;
  file->n_particion = 0;
  file -> byte = 0;
  file -> byte_total = 0;
  file -> bloque= 0;
  return file;
}


// Funciones

void cr_mount(char* diskname)
{
  ruta_archivo = diskname;
}

void directorio_append(Directory* bloque, Entry *entrada, int i)
{
  bloque-> entries[i] = entry_init();
  memcpy(bloque-> entries[i]->file_name, entrada->file_name, 29);
}

int cr_exists(unsigned disk, char* filename)
{
	if(disk != 1 && disk != 2 && disk != 3 && disk != 4){
		printf("ERROR: la particion indicada por disk es invalida\n");
		return 2;
	}
	for(int i = 0; i<BLOCK_ENTRIES;i++){
    int a = !!((Dir_disk[disk-1] -> entries[i] -> number[0] << 1) & 0x800000); // Revisa el bit de validez
    if (a == 1 && strncmp(Dir_disk[disk-1]->entries[i]->file_name, filename,32) == 0){
      	return 1;
    	}
	}
	return 0;
}

void cr_ls(unsigned disk)
{
	if(disk != 1 && disk != 2 && disk != 3 && disk != 4){
		printf("ERROR: la particion indicada por disk es invalida\n");
	}
	else
	{
		Directory* disco = Dir_disk[disk-1];
		for (int i =0; i< BLOCK_ENTRIES; i++)
		{
			Entry* entrada = disco->entries[i];
			int a = !!((entrada->number[0] << 1) & 0x800000);
			if (a == 1)
			{
				printf("%s\n",entrada->file_name);
			}
		}

	}
}


void create_dir_blocks()
{
  Entry_aux entrada_aux;
  FILE* disk = fopen(ruta_archivo, "r");
  for(int i = 0;i < 4 ; i++)
  {
  	Dir_disk[i]= malloc(sizeof(Directory));
  }
  for(int f = 0; f < 4 ; f++)
  {
    fseek(disk, 536870912*f, SEEK_SET);
    for(int i = 0; i< BLOCK_ENTRIES;i++)
    {
      fread(&entrada_aux, 32, 1, disk);
      Dir_disk[f]-> entries[i] = entry_init();
      memcpy(Dir_disk[f]-> entries[i]->file_name, entrada_aux.file_name, 29);
      memcpy(Dir_disk[f]-> entries[i]->number, entrada_aux.valid, 3);
    }
  }
  fclose(disk);
 }

 int buscar_entry_disponible(Directory* directorio)
 {
  for (int i = 0; i < BLOCK_ENTRIES; i ++)
  {
    if(directorio->entries[i]->number[0] == 0)
    {
      return i;
    }
  }
  return 0;
 }


//https://stackoverflow.com/questions/18327439/printing-binary-representation-of-a-char-in-c
void print_bitmap_bin(Bitmap* bitmap_block, bool hex)
{
	int libres = 0;
	int ocupados = 0;
	for (int j =1; j <= BLOCK_BYTES; j++)
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

int buscar_bloque_disponible(Bitmap* bitmap){
  int a;
  int b;
  int encontre = 0;
  int num_bloque_relat = 0;
  for (int i = 0; i< BLOCK_BYTES; i++)
  {
    if(encontre == 0){
      for (int j = 0; j < 8; j++)
      {
        if(encontre == 0){
          int k = !!((bitmap->map[i] << j) & 0x80);
          if(k == 0)
          {
            num_bloque_relat = 8*i + j;
            a = i;
            b = j;
            encontre = 1;
          }
        }
        else{
          break;
        }
      }
    }
    else{
      break;
    }
  }
  if (encontre == 1){
    char* ah = calloc(1, sizeof(char));
    if (b == 0){
      char aw = (char) (bitmap->map[a] + pow(2, 7));
      memcpy(ah, &(aw), 1);
      }
    if (b == 1){
      char aw = (char) (bitmap->map[a] + pow(2, 6));
      memcpy(ah, &(aw), 1);
    }
    if (b == 2){
      char aw = (char) (bitmap->map[a] + pow(2,5));
      memcpy(ah, &(aw), 1);
    }
    if (b == 3){
     char aw = (char) (bitmap->map[a] + pow(2, 4));
     memcpy(ah, &(aw), 1);
    }
    if (b == 4){
       char aw = (char) (bitmap->map[a] + pow(2, 3));
       memcpy(ah, &(aw), 1);
    }
    if (b == 5){
      char aw =(char) (bitmap->map[a] + pow(2, 2));
      memcpy(ah, &(aw), 1);
    }
    if (b == 6){
      char aw = (char) (bitmap->map[a] + pow(2, 1));
      memcpy(ah, &(aw), 1);
    }
    if (b == 7){
      char aw = (char) (bitmap->map[a] + pow(2, 0));
      memcpy(ah, &(aw), 1);
    }
    memcpy(&(bitmap->map[a]), ah, 1);
    free(ah);
    return num_bloque_relat;
  }
  else
  {
    //no hay bloques disponibles
    return 0;
  }
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
		printf("ERROR: Debe elegir una opcion valida de particion (1-4) o 0 si quiere el de todo el disco\n");
	}
}



void create_cr_bitmaps()
{
  for(int i = 0; i < 4; i++)
	{
		bitmaps[i] = bitmap_init();
	}

	FILE* disk = fopen(ruta_archivo, "r");
	for(int i = 0; i < PARTICIONES; i++)
	{
		char* mapp = malloc(sizeof(char)*BLOCK_BYTES);
		int hasta = 536870912*i + BLOCK_BYTES;
  	fseek(disk, hasta , SEEK_SET);
  	fread(mapp, BLOCK_BYTES, 1, disk);
  	memcpy(bitmaps[i]->map, mapp, BLOCK_BYTES);
		free(mapp);
	}
	fclose(disk);

}

void destroy_directories()
{
	for(int i = 0; i < PARTICIONES; i++)
	{
		for(int j = 0; j< BLOCK_ENTRIES; j++)
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


void destroy_indice(Index * index){
	free(index->blocks_data);
  	free(index);
}

void destroy_data(Data_Block * data){
  free(data -> content);
  free(data);
}

void destroy_indirect_simple(Indirect* indirect_simple){
  free(indirect_simple -> indirect_blocks_data);
  free(indirect_simple);
}

// https://gist.github.com/ryankshah/bf45790b968540cdabdeef702883ddbb modificada
int get_bloque(char* string) {
    int count = 0;
    int num_bloque = 0;
    int stringLength = 3;
    int mask = 0x80; /* 10000000 */
    int *bloque = malloc(sizeof(int)*24);

    for(int i = 0; i < stringLength; i++) {
        mask = 0x80;
        char c = string[i];
        int x = 0;
        while(mask > 0) {
            char n = (c & mask) > 0;
            bloque[count] = (int) n;
            mask >>= 1; /* move the bit down */
            count++;
        }
    }
    // transformamos ahora los bit al numero de bloque
	for(int k = 1; k < 24; k++)
	{
  	// no consideramos el 0 ya que ese corresponde al valid bit
  		if(bloque[k] == 1)
  		{
  			int ap = 23 - k;
    	num_bloque += pow(2,ap);
  		}
	}

	free(bloque);
	return num_bloque;
}

char* traducir_num_bloque(int num_bloque){
  char* num_a_chars = calloc(3, sizeof(char));
  char pormientras[3];
  int aux;
  int w = 0;

  int a[24], b[24], i;
  for(int y = 0; y< 24; y++)
  {
    b[y] = 0;
    a[y] = 0;

  }
  for(i=0;num_bloque>0;i++)
  {
  a[i] = num_bloque%2;
  num_bloque = num_bloque/2;
  }
  for(i=i-1;i>=0;i--)
  {
  b[23 - i] = a[i];
  }
  b[0] = 1;
  for(int r = 0; r < 3; r++){
    w = 0;
    for(int t = 0; t < 8; t++){
      if(b[8*r + t] == 1){
        int q = 7 - t;
        w += pow(2, q);
      }
    }
    pormientras[r] = w;
  }
  memcpy(num_a_chars, &(pormientras), 3);
  return num_a_chars;
}

int buscar_ref(char* str)
{
	int bloque[32];
    int count = 0;
    for (int j = 0; j < 4; j++)
    {
        int mask = 0x80;
        while(mask > 0) {
            char n = (str[j] & mask) > 0;
            bloque[count] = (int) n;
            mask >>= 1; /* move the bit down */
            count++;
        }
    }
    int ref = 0;
    for(int i = 0; i< 32; i++)
    {
    	if(bloque[i]==1)
    	{
    		int ar = 31 - i;
    		int n = pow(2,ar);
    		ref += n;
    	}
    }
    return ref;
}

long buscar_size(char* str)
{
	int bloque[64];
    int count = 0;
    for (int j = 0; j < 8; j++)
    {
        int mask = 0x80;
        while(mask > 0) {
            char n = (str[j] & mask) > 0;
            bloque[count] = (int) n;
            mask >>= 1; /* move the bit down */
            count++;
        }
    }
    long siz = 0;
    for(int i = 0; i< 64; i++)
    {
    	if(bloque[i]==1)
    	{
    		int ar = 63 - i;
    		int n = pow(2,ar);
    		siz += n;
    	}
    }
    return siz;
}

int cr_close(crFILE* file_desc){
	if (file_desc != NULL){
		free(file_desc-> file_name);
		free(file_desc-> valid);
		free(file_desc-> mode);
  		if(file_desc->indice->indirect_simple)
  		{
    		if(file_desc->indice->indirect_simple>0){
     		destroy_indirect_simple(file_desc->indice->bloque_indireccion);
    		}
  		}
  		destroy_indice(file_desc->indice);
  		free(file_desc);
  		return 0;
	}
	else{
		printf("ERROR: el archivo no es valido por lo que no se puede cerrar\n");
		return 1;
	}
}


crFILE* cr_open(unsigned disk, char* filename, char *mode){

	if(disk != 1 && disk != 2 && disk != 3 && disk != 4){
		printf("ERROR: la particion indicada por disk es invalida\n");
		return NULL;
	}

  char *particion;
  char *nombre;
  char *str = malloc(sizeof(char)*32);
  int prt;


  FILE* disco = fopen(ruta_archivo, "r");

  if(strncmp(mode , "r", 1) == 0)
  {
    int exist;
    // Chequeamos que exista el archivo
    exist = cr_exists(disk, filename);
    //printf("exist = %d\n", exist);
    if(exist == 1)
    {
      // el archivo en el disco si existe creamos el crFILE
      char* ptr;
      crFILE* file;
      file = init_crfile();
      char* sl = strchr(filename, '/');
      //no es un softlink
      if(sl == NULL)
      {
        // ahora tengp que buscar el archivo
        for(int i = 0; i<BLOCK_ENTRIES;i++)
        {
          if(strncmp(Dir_disk[disk-1]->entries[i]->file_name , filename, 32) ==0)
          {
            // si el nombre esta en la partición
            // le doy a file los atributos del archivo
            memcpy(file->file_name, Dir_disk[disk-1]->entries[i]->file_name, 29);
            memcpy(file->valid, Dir_disk[disk-1]->entries[i]->number, 3);
            // le cambio asigno el modo en el que se abrio
            memcpy(file->mode, mode, 1);
            //memcpy(file->dir, 0, 1);
            file->n_particion = disk;
            break;
          }
        }
      }
      else //es un softlink
      {
        // la info no esta en la partición
        memcpy(str, filename, 32);
        particion = strtok(str,"/");
        nombre = strtok(NULL,"/");
        prt = atoi(particion);
        int existe_2 = cr_exists(prt, nombre);
        if(existe_2 == 1)
        {
          for(int j = 0; j<BLOCK_ENTRIES;j++)
          {
            // buscamos en la particion
            if(strncmp(Dir_disk[prt-1]->entries[j]->file_name , nombre, 32)==0)
            {
              // obtengo el indice del archivo que indica la partición a la que pertence
              memcpy(file->file_name, filename, 29);
              memcpy(file->valid, Dir_disk[prt-1]->entries[j]->number, 3);
              memcpy(file->mode, mode, 1);
              file->n_particion = prt;
              // apunta a otro directorio
              //memcpy(file->dir, 1, 1);
            }
          }
        }
        else
        {
          printf("ERROR: el archivo que trata de abrir es un broken link\n");
          free(str);
          fclose(disco);
          cr_close(file);
          return NULL;
        }
      }
      // Cargamos el bloque indice del archivo a crFILE
      // Buscamos el puntero al ruta_archivo--> decodificar valid del filename
      char *st= malloc(sizeof(char)*3);
      int n_bloque_indice;
      char* indice_aux = malloc(sizeof(char)*BLOCK_BYTES);

      memcpy(st, file->valid, 3);
      n_bloque_indice = get_bloque(st);
      file->n_b_indice = n_bloque_indice;

      //seteamos el archivo en el bloque indice
      fseek(disco, BLOCK_BYTES * n_bloque_indice , SEEK_SET);

      // esto tenemos que decodificarlo a los bytes que correspondan por segnmento
      fread(indice_aux, BLOCK_BYTES, 1, disco);
      Index* ind= file->indice;
      char* aux_ref = malloc(sizeof(char)*4);
      memcpy(aux_ref, indice_aux, 4);
      ind->references = buscar_ref(aux_ref);
      free(aux_ref);
      char* aux_size = malloc(sizeof(char)*8);
      memcpy(aux_size, &(indice_aux[4]), 8);
      ind->file_size = buscar_size(aux_size);
      free(aux_size);
      int resto = ind->file_size % BLOCK_BYTES;
      int blocks_ocup;
      if(resto > 0){
        blocks_ocup = ind->file_size/BLOCK_BYTES + 1;
      }
      else{
        blocks_ocup= ind->file_size/BLOCK_BYTES;
      }
      if(blocks_ocup <= 2044){
        //printf("menos de 2044\n");
        //printf("los bloques de datos deberian ser: %d\n", blocks_ocup);
        for (int i = 0; i < blocks_ocup; i++)
        {
          char* aux_ptr = malloc(sizeof(char)*4);
          int q = 12+4*i;
          memcpy(aux_ptr, &(indice_aux[q]), 4);
          ind->blocks_data[i] = buscar_ref(aux_ptr);
          //printf("i=%d puntero=%d\n", i, ind->blocks_data[i]);
          file->bloques_ocupados ++;
          free(aux_ptr);
        }
        ind->indirect_simple = 0;
      }
      else{
        //printf("mas de 2044\n");
        //printf("los bloques de datos deberian ser: %d\n", blocks_ocup);
        for (int i = 0; i < 2044; i++)
        {
          char* aux_ptr = malloc(sizeof(char)*4);
          int q = 12+4*i;
          memcpy(aux_ptr, &(indice_aux[q]), 4);
          ind->blocks_data[i] = buscar_ref(aux_ptr);
          //printf("i=%d puntero=%d\n", i, ind->blocks_data[i]);
          file->bloques_ocupados ++;
          free(aux_ptr);
        }
        char* aux_ind = malloc(sizeof(char)*4);
        memcpy(aux_ind, &(indice_aux[8188]), 4);
        ind->indirect_simple = buscar_ref(aux_ind);
        //printf("bloque de indireccion = %d\n", ind->indirect_simple);
        //seteamos el archivo en el bloque indireccionamiento simple
        fseek(disco, BLOCK_BYTES * ind->indirect_simple , SEEK_SET);
        char* indireccionamiento = malloc(sizeof(char)*BLOCK_BYTES);
        fread(indireccionamiento, BLOCK_BYTES, 1, disco);
        ind->bloque_indireccion = init_ind_simple();
        int bloques_i = blocks_ocup - 2044;
        for (int i = 0; i < bloques_i; i++)
        {
          char* aux_ptr = malloc(sizeof(char)*4);
          memcpy(aux_ptr, &(indireccionamiento[4*i]), 4);
          ind-> bloque_indireccion->indirect_blocks_data[i] = buscar_ref(aux_ptr);
          //printf("i=%d puntero=%d\n", i, ind-> bloque_indireccion->indirect_blocks_data[i]);
          file->bloques_ocupados ++;
          free(aux_ptr);
        }
        free(indireccionamiento);
        free(aux_ind);

      }
      printf("filename = %s\n", file->file_name);
      printf("file references = %d\n", ind->references);
      printf("file size = %ld\n", ind->file_size);
      printf("numero bloque indice = %d\n", file->n_b_indice);
      printf("primer bloque de datos = %d\n", ind->blocks_data[0]);
      printf("ultimo bloque de datos = %d\n", ind->blocks_data[blocks_ocup - 1]);
      free(st);
      free(str);
      free(indice_aux);
      fclose(disco);
      return file;
    }
    else{
      // exist es 0
      printf("ERROR: El archivo no existe en la particion\n");
      free(str);
      fclose(disco);
      return NULL;
      }
    }
    else if(strncmp(mode , "w", 1) ==0)
    {
      int exist;
    // Chequeamos que exista el archivo
      exist = cr_exists(disk, filename);
      if(exist == 1)
      {
        // el archivo ya existe -> no lo puedo abrir en este modo
        printf("ERROR: el archivo ya existe por lo que no se puede abrir en modo de escritura\n");
        free(str);
        fclose(disco);
        return NULL;

      }
      else
      {
          // exist es 0
          //tengo que buscarle espacio al bloque indice
          Bitmap* mapeo = bitmaps[disk-1];
          //numero del primer bloque que esta disponible (numero relativo)
          //Ademas cambio en el bitmap el 0 por un 1 en ese bloque (si hay bloques disponibles)
          int n_bloque = buscar_bloque_disponible(mapeo);
          if (n_bloque == 0)//no queda espacio
          {
            printf("ERROR: No existe espacio en la particion para agregar un archivo\n");
            free(str);
            fclose(disco);
            return NULL;
          }
          Directory* directorio_actual = Dir_disk[disk-1];
          //Busco la primera entrada vacia para llenarla con la info del archivo que estoy creando
          int new_entry = buscar_entry_disponible(directorio_actual);
          if(new_entry == 0){ //no quedan entradas
            printf("ERROR: No quedan entradas disponibles en el directorio para agregar un archivo\n");
            free(str);
            fclose(disco);
            return NULL;
          }
          //copio el nombre que me dan (filename)
          char* nombre_a_copiar = calloc(29, sizeof(char));
          memcpy(nombre_a_copiar, filename, 29);
          //inicializo el archivo
          crFILE* file = init_crfile();
          //calculo numero de bloque absoluto (en todo el disco)
          int n_bloque_abs = 65536*(disk - 1) + n_bloque;
          file->n_b_indice = n_bloque_abs;
          //paso del numero de bloque a los 3 char incluyendo bit validez = 1
          char* n_bloque_traducido = traducir_num_bloque(n_bloque_abs);
          //copio la info tanto en el file como en el bloque directorio (entrada correspondiente)
          memcpy(file->file_name, nombre_a_copiar, 29);
          memcpy(Dir_disk[disk -1]->entries[new_entry]->file_name, nombre_a_copiar, 29);
          memcpy(file->valid, n_bloque_traducido, 3);
          memcpy(Dir_disk[disk -1]->entries[new_entry]->number, n_bloque_traducido, 3);
          file->n_particion = disk;
          //libero la memoria de las variables auxiliares
          free(nombre_a_copiar);
          free(n_bloque_traducido);
          //copio el modo del file a "w"
          memcpy(file->mode, mode, 1);
          free(str);
          //cierro el disco
          fclose(disco);
          return file;
        }
    }

    else{
    	printf("ERROR: el modo ingresado no corresponde ni a 'r' ni a 'w'\n");
    	free(str);
        fclose(disco);
    	return NULL;
    }
}


int cr_read(crFILE* file_desc, void* buffer, int nbytes)
{

  if (nbytes <= 0) {
  	printf("ERROR: la cantidad de bytes ingresada es invalida\n");
    return 0;
  }
  if (!file_desc){
    printf("ERROR: Archivo no abierto correctamente\n");
    return 0;
  }

  if(strncmp(file_desc -> mode, "r", 1) != 0){
    printf("ERROR: El archivo no fue abierto en modo de lectura\n" );
    return 0;
  }

  FILE* disco = fopen(ruta_archivo, "r");
  char* read_aux = malloc(sizeof(char) * BLOCK_BYTES +1);
  char* buffer_aux = malloc(sizeof(char) * nbytes + 1);
  char* byte = malloc(sizeof(char));
  int efectivamente_leidos = 0;

  int min = 0;

  // encontramos el menor entre lo que me queda por leer y lo que quiero leer
  if(nbytes > file_desc ->indice ->file_size - file_desc -> byte_total){
    // lo que me queda por leer
    //printf("file size = %ld\n",file_desc ->indice ->file_size);
    //printf("byte_total = %d\n", file_desc->byte_total);
    min = file_desc -> indice -> file_size - file_desc -> byte_total;
  }
  else{
    min = nbytes;
  }

  //printf("min = %d\n", min);
  int bloque_actual = file_desc -> bloque; // En que bloque estoy
  int bloque_actual_dir = file_desc -> bloque_dir; // En que bloque de indireccionamiento directo estoy
  int byte_actual = file_desc -> byte; // En que byte del bloque estoy
  //printf("tengo indireccionamiento simple %i\n",file_desc -> indice -> indirect_simple );

  if(file_desc -> byte_total < (2044 * BLOCK_BYTES)){
    // Seguimos en los blocks data del indice
    fseek(disco,
      file_desc -> indice -> blocks_data[file_desc -> bloque] * BLOCK_BYTES, SEEK_SET);
      bloque_actual++;
  }
  else if(file_desc -> indice -> indirect_simple == 1 && file_desc -> byte_total >= (2044 * BLOCK_BYTES)){
    // Si tenemos indireccionamiento y nos pasamos de los bloque de datos (2044 * BLOCK_BYTES)
    fseek(disco,
        file_desc -> indice -> bloque_indireccion -> indirect_blocks_data[bloque_actual_dir] * BLOCK_BYTES, SEEK_SET);
    bloque_actual_dir++;
  }
  if(file_desc -> indice -> file_size == file_desc -> byte_total){
    // Si no tenemos indireccionamiento y nos pasamos de los bloque de datos (2044 * BLOCK_BYTES)
    printf("ERROR: No quedan bytes por leer\n");
    free(buffer_aux);
  	free(read_aux);
  	free(byte);
  	fclose(disco);
    return 0;
  }

  // Obtenemos lo que queda por leer del bloque
  fread(read_aux, BLOCK_BYTES , 1, disco);


  for(int i = 0; i < min; i++){

    // El numero de posición va de 0 a 8192, si supero este numero debo irme a leer otro bloque
    if(byte_actual < BLOCK_BYTES)
    {
      memcpy(&(buffer_aux[i]), &(read_aux[byte_actual]), 1);

      // Aumentamos en que byte estamos en el bloque
      byte_actual++;
      efectivamente_leidos++;

      // Aumentamos en que byte estamos de todo el archivo
      file_desc -> byte_total++;

    }
    else if(byte_actual == BLOCK_BYTES && file_desc -> byte_total < (2044 * BLOCK_BYTES))
    {
      // El byte actual es igual a 8192 y seguimos leyendo los bloque de datos del indice
      // cuando esto pase debo "reiniciar" el contador en 0

      fseek(disco, file_desc -> indice -> blocks_data [bloque_actual] * BLOCK_BYTES , SEEK_SET);
      fread(read_aux, BLOCK_BYTES, 1, disco);
      bloque_actual++;
      byte_actual = 0;

      // al iterar y pasa por aca se pierde una lectura
      i--;

     }
     else if(byte_actual == BLOCK_BYTES && file_desc -> byte_total >= (2044 * BLOCK_BYTES) && file_desc -> indice -> indirect_simple != 0){

       // pasamos al indireccionamiento simple
       fseek(disco, file_desc -> indice-> bloque_indireccion -> indirect_blocks_data[bloque_actual_dir] * BLOCK_BYTES , SEEK_SET);
       fread(read_aux, BLOCK_BYTES, 1, disco);

       bloque_actual++;
       byte_actual = 0;
       bloque_actual_dir++;

       // al iterar y pasa por aca se pierde una lectura
       i--;
     }
  }

  for (int i = 0; i < efectivamente_leidos; i++)
  {
    ((unsigned char *)buffer)[i] = buffer_aux[i];
  }

  // actualizamos la nueva ubicacion de donde quedamos leyendo
  file_desc -> byte = byte_actual;
  file_desc -> bloque = bloque_actual - 1; // en el for se le suma uno alfinal que sobra
  file_desc -> bloque_dir = bloque_actual_dir;
  if(file_desc -> bloque_dir > 0){
    file_desc -> bloque_dir = bloque_actual_dir - 1 ;  // en el for se le suma uno alfinal que sobra pero solo si pasa esta etapa
  }

  free(buffer_aux);
  free(read_aux);
  free(byte);
  fclose(disco);

  return efectivamente_leidos;
}


void actualizar_directorio(crFILE* file){
  Directory* dir = Dir_disk[file->n_particion - 1];
  int posicion = 0;
  for(int i = 0; i < BLOCK_ENTRIES; i++){
    if(strncmp(dir->entries[i]->file_name , file->file_name, 32) ==0 ){
      posicion = i;
    }
  }
  //la posicion que tengo que cambiar es la i
  FILE* disk = fopen(ruta_archivo, "rb+");
  int f = file->n_particion - 1;
  fseek(disk, 536870912*f + 32*posicion, SEEK_SET);
  char* aux_linea = malloc(sizeof(char)*32);
  memcpy(aux_linea, file->valid, 3);
  memcpy(&(aux_linea[3]), file->file_name, 29);
  fwrite(aux_linea, 1, 32, disk);
  free(aux_linea);
  fclose(disk);
}

void actualizar_bitmap(int n_particion){
  Bitmap* bm = bitmaps[n_particion - 1];
  FILE* disk = fopen(ruta_archivo, "rb+");
  int hasta = 536870912*(n_particion - 1)+ BLOCK_BYTES;
  fseek(disk, hasta, SEEK_SET);
  char* aux_bitmap = malloc(sizeof(char)*BLOCK_BYTES);
  memcpy(aux_bitmap, bm->map, BLOCK_BYTES);
  fwrite(aux_bitmap, 1, BLOCK_BYTES, disk);
  free(aux_bitmap);
  fclose(disk);
}

char* traducir_int(int num){
  char* result = calloc(4, sizeof(char));
  char pormientras[4];
  int aux;
  int w = 0;
  int a[32], b[32], i;
  for(int y = 0; y< 32; y++)
  {
    b[y] = 0;
    a[y] = 0;
  }
  for(i=0;num>0;i++)
  {
  a[i] = num%2;
  num = num/2;
  }
  for(i=i-1;i>=0;i--)
  {
  b[31 - i] = a[i];
  }
  for(int r = 0; r < 4; r++){
    w = 0;
    for(int t = 0; t < 8; t++){
      if(b[8*r + t] == 1){
        int q = 7 - t;
        w += pow(2, q);
      }
    }
    pormientras[r] = w;
  }
  memcpy(result, &(pormientras), 4);
  return result;
}

char* traducir_long(long num){
  char* result = calloc(8, sizeof(char));
  char pormientras[8];
  int aux;
  int w = 0;
  int a[64], b[64], i;
  for(int y = 0; y< 64; y++)
  {
    b[y] = 0;
    a[y] = 0;
  }
  for(i=0;num>0;i++)
  {
  a[i] = num%2;
  num = num/2;
  }
  for(i=i-1;i>=0;i--)
  {
    b[63 - i] = a[i];
  }
  for(int r = 0; r < 8; r++){
    w = 0;
    for(int t = 0; t < 8; t++){
      if(b[8*r + t] == 1){
        int q = 7 - t;
        w += pow(2, q);
      }
    }
    pormientras[r] = w;
  }
  memcpy(result, &(pormientras), 8);
  return result;
}

void escribir_en_bloque_indice(crFILE* file){
  FILE* disk = fopen(ruta_archivo, "rb+");
  //primero escribo las referencias(4bytes)
  int hasta = BLOCK_BYTES * file->n_b_indice;
  fseek(disk, hasta, SEEK_SET);
  char* aux_ref = traducir_int(file->indice->references);
  fwrite(aux_ref, 1, 4, disk);
  free(aux_ref);
  //escribo el filesize(8bytes)
  fseek(disk, hasta + 4, SEEK_SET);
  char* aux_siz = traducir_long(file->indice->file_size);
  fwrite(aux_siz, 1, 8, disk);
  free(aux_siz);
  //no hay indireccionamiento simple
  if(file->bloques_ocupados <= 2044){
    //escribo los punteros a bloques de datos ocupados
    for(int i = 0; i< file->bloques_ocupados; i++){
      int q = 12 + 4*i;
      fseek(disk, hasta + q, SEEK_SET);
      char* aux_ref = traducir_int(file->indice->blocks_data[i]);
      fwrite(aux_ref, 1, 4, disk);
      free(aux_ref);
    }
  }
  //si hay indireccionamiento simple
  else{
    //escribo los 2044 primero
    for(int i = 0; i< 2044; i++){
      int q = 12 + 4*i;
      fseek(disk, hasta + q, SEEK_SET);
      char* aux_ref = traducir_int(file->indice->blocks_data[i]);
      fwrite(aux_ref, 1, 4, disk);
      free(aux_ref);
    }
    //escribo el numero de bloque del indireccionamiento
    fseek(disk, 8188, SEEK_SET);
    char* aux_ref = traducir_int(file->indice->indirect_simple);
    fwrite(aux_ref, 1, 4, disk);
    free(aux_ref);
    //escribo los bloques ocupados del indireccionamiento
    int b_i = file->bloques_ocupados - 2044;
    int hasta_2 = BLOCK_BYTES * file->indice->indirect_simple;
    for(int i = 0; i< b_i; i++){
      fseek(disk, hasta_2 + 4*i, SEEK_SET);
      char* aux_ref = traducir_int(file->indice->bloque_indireccion->indirect_blocks_data[i]);
      fwrite(aux_ref, 1, 4, disk);
      free(aux_ref);
    }
  }
  fclose(disk);
}

void guardar_info_archivo(crFILE* file){
  //en el directorio:
  //necesito guardar el nombre del archivo
  //necesito guardar el valid
  actualizar_directorio(file);
  //bitmap:
  //necesito actualizar el bitmap
  actualizar_bitmap(file->n_particion);
  //bloque indice: referencias, file size y bloques de datos ocupados (+ indireccion simple si hay)
  escribir_en_bloque_indice(file);
}

int cr_write(crFILE* file, void* buffer, int n_bytes){
  //chequeo que el modo del archivo este ok
	if (!file){
    printf("ERROR: Archivo no abierto correctamente\n");
    return 0;
    }

    if (n_bytes <= 0) {
  	printf("ERROR: la cantidad de bytes ingresada es invalida\n");
    return 0;
    }

  	if(strncmp(file->mode, "w", 1) == 0)
  	{
	    FILE* disco = fopen(ruta_archivo, "rb+");
	    //necesito ver cuantos bloques de datos ocupo con los n_bytes
	    int resto = n_bytes % BLOCK_BYTES;
	    //printf("bloque indice = %d\n", file->n_b_indice);

	    char* buffer_aux = malloc(sizeof(char)*8192);

	    int bloques_necesito;
	    if(resto > 0){
	      bloques_necesito = n_bytes/BLOCK_BYTES + 1;
	    }
	    else{
	      bloques_necesito = n_bytes/BLOCK_BYTES;
	    }
	    //busco en el bitmap de a uno los bloques, escribo en el, alfinal actualizo la info con lo que alcance a escribir
	    Bitmap* bitmap_actual = bitmaps[file->n_particion - 1];
	    //no necesito indireccionamiento simple
	    if(bloques_necesito <= 2044){
	      int bloque_disp_rel;
	      int bloque_disp_abs;
	      int voy_bloque = 0;
	      int llevo_bytes = 0;
	      file->indice->indirect_simple = 0;
	      for(int i = 0; i < bloques_necesito; i++){
	        bloque_disp_rel = buscar_bloque_disponible(bitmap_actual);//obtengo numero del primer bloque vacio (relativo)
	        if(bloque_disp_rel == 0){
	          //no quedan bloques disponibles en la particion
	          printf("ERROR: no quedan bloques disponibles en la particion\n");
	          file->indice->file_size = llevo_bytes;
	          free(buffer_aux);
	          fclose(disco);
	          guardar_info_archivo(file);
	          return llevo_bytes;
	        }
	        bloque_disp_abs = 65536*(file->n_particion - 1) + bloque_disp_rel; // lo paso a numero absoluto
	        file->indice->blocks_data[i] = bloque_disp_abs; //lo guardo en el array de data_blocks
	        //printf("bloque = %d\n", bloque_disp_abs);
	        voy_bloque ++;
	        file->bloques_ocupados++;
	        if(bloques_necesito - voy_bloque == 0 && resto > 0){ //estoy en el ultimo bloque y escribo la cantidad de bytes = resto
	          //seteamos el archivo en el bloque
	          fseek(disco, BLOCK_BYTES * bloque_disp_abs , SEEK_SET);

	          //escribo los bytes_restantes(resto)
	          memcpy(buffer_aux, &(buffer[BLOCK_BYTES * i]), resto);
	          fwrite(buffer_aux, 1, resto, disco);

	          llevo_bytes += resto;
	          file->indice->file_size = llevo_bytes;
	          fclose(disco);
	          free(buffer_aux);
	          guardar_info_archivo(file);
	          return llevo_bytes;
	        }
	        else if(bloques_necesito - voy_bloque == 0 && resto == 0){//estoy en el ultimo bloque y escribo 8192 bytes.
	          //seteamos el archivo en el bloque
	          fseek(disco, BLOCK_BYTES * bloque_disp_abs , SEEK_SET);
	          //escribo los 8192 bytes restantes

	          //escribo los bytes_restantes(resto)
	          memcpy(buffer_aux, &(buffer[BLOCK_BYTES * i]), BLOCK_BYTES);
	          fwrite(buffer_aux, 1, BLOCK_BYTES, disco);

	          llevo_bytes += BLOCK_BYTES;
	          file->indice->file_size = llevo_bytes;
	          fclose(disco);
	          free(buffer_aux);
	          guardar_info_archivo(file);
	          return llevo_bytes;
	        }
	        else if(bloques_necesito > voy_bloque){
	          //seteamos el archivo en el bloque
	          fseek(disco, BLOCK_BYTES * bloque_disp_abs , SEEK_SET);

	          //escribo los 8192 bytes restantes
	          memcpy(buffer_aux, &(buffer[BLOCK_BYTES * i]), BLOCK_BYTES);
	          fwrite(buffer_aux, 1, BLOCK_BYTES, disco);

	          llevo_bytes += BLOCK_BYTES;
	        }
      }
    }

    //necesito ir a bloque de indireccionamiento simple
    else{
      //primero lleno los 2044 directos
      int bloque_disp_rel;
      int bloque_disp_abs;
      int voy_bloque = 0;
      int llevo_bytes = 0;
      for(int i = 0; i < 2044; i++){
        bloque_disp_rel = buscar_bloque_disponible(bitmap_actual); //obtengo numero del primer bloque vacio (relativo)
        if(bloque_disp_rel == 0){
          //no quedan bloques disponibles en la particion
          printf("ERROR: no quedan bloques disponibles en la particion\n");
          file->indice->file_size = llevo_bytes;
          fclose(disco);
          free(buffer_aux);
          guardar_info_archivo(file);
          return llevo_bytes;
        }
        bloque_disp_abs = 65536*(file->n_particion - 1) + bloque_disp_rel; // lo paso a numero absoluto
        file->indice->blocks_data[i] = bloque_disp_abs; //lo guardo en el array de data_blocks
        voy_bloque ++;
        file->bloques_ocupados++;
        if(bloques_necesito > voy_bloque){
          //seteamos el archivo en el bloque
          fseek(disco, BLOCK_BYTES * bloque_disp_abs , SEEK_SET);

          //escribo los 8192 bytes restantes
          memcpy(buffer_aux, &(buffer[BLOCK_BYTES * i]), BLOCK_BYTES);
          fwrite(buffer_aux, 1, BLOCK_BYTES, disco);

          llevo_bytes += BLOCK_BYTES;
        }
      }
      //ahora voy a llenar el bloque de indireccion simple
      //chequeo si hay espacio para el bloque de indireccionamiento
      bloque_disp_rel = buscar_bloque_disponible(bitmap_actual);
      if(bloque_disp_rel == 0){
          //no quedan bloques disponibles en la particion
          printf("ERROR: no quedan bloques disponibles en la particion\n");
          file->indice->file_size = llevo_bytes;
          fclose(disco);
          free(buffer_aux);
          guardar_info_archivo(file);
          return llevo_bytes;
        }
      bloque_disp_abs = 65536*(file->n_particion - 1) + bloque_disp_rel;
      file->indice->indirect_simple = bloque_disp_abs;
      file->indice->bloque_indireccion = init_ind_simple();
      int bloques_ind_sim = bloques_necesito - 2044;
      int voy_3_bloque = 0;
      for(int i = 0; i < bloques_ind_sim; i++){
        bloque_disp_rel = buscar_bloque_disponible(bitmap_actual); //obtengo numero del primer bloque vacio (relativo)
        if(bloque_disp_rel == 0){
          //no quedan bloques disponibles en la particion
          printf("ERROR: no quedan bloques disponibles en la particion\n");
          file->indice->file_size = llevo_bytes;
          fclose(disco);
          free(buffer_aux);
          guardar_info_archivo(file);
          return llevo_bytes;
        }
        bloque_disp_abs = 65536*(file->n_particion - 1) + bloque_disp_rel; // lo paso a numero absoluto
        file->indice->bloque_indireccion->indirect_blocks_data[i] = bloque_disp_abs; //lo guardo en el array de data_blocks
        voy_3_bloque ++;
        file->bloques_ocupados++;
        if(bloques_ind_sim - voy_3_bloque == 0 && resto > 0){ //estoy en el ultimo bloque y escribo la cantidad de bytes = resto
          //seteamos el archivo en el bloque
          fseek(disco, BLOCK_BYTES * bloque_disp_abs , SEEK_SET);

          //escribo los bytes_restantes(resto)
          memcpy(buffer_aux, &(buffer[BLOCK_BYTES * i]), resto);
          fwrite(buffer_aux, 1, resto, disco);

          llevo_bytes += resto;
          file->indice->file_size = llevo_bytes;
          fclose(disco);
          free(buffer_aux);
          guardar_info_archivo(file);
          return llevo_bytes;
        }
        else if(bloques_ind_sim - voy_3_bloque == 0 && resto == 0){//estoy en el ultimo bloque y escribo 8192 bytes.
          //seteamos el archivo en el bloque
          fseek(disco, BLOCK_BYTES * bloque_disp_abs , SEEK_SET);

          //escribo los 8192 bytes restantes
          memcpy(buffer_aux, &(buffer[BLOCK_BYTES * i]), BLOCK_BYTES);
          fwrite(buffer_aux, 1, BLOCK_BYTES, disco);

          llevo_bytes += BLOCK_BYTES;
          file->indice->file_size = llevo_bytes;
          fclose(disco);
          free(buffer_aux);
          guardar_info_archivo(file);
          return llevo_bytes;
        }
        else if(bloques_ind_sim > voy_3_bloque){
          //seteamos el archivo en el bloque
          fseek(disco, BLOCK_BYTES * bloque_disp_abs , SEEK_SET);

          //escribo los 8192 bytes restantes
          memcpy(buffer_aux, &(buffer[BLOCK_BYTES * i]), BLOCK_BYTES);
          fwrite(buffer_aux, 1, BLOCK_BYTES, disco);

          llevo_bytes += BLOCK_BYTES;
          if(voy_3_bloque == 2048){
            //estoy en el ultimo bloque del bloque de indireccionamiento simple
            int me_quedan = n_bytes - llevo_bytes;
            if (me_quedan > 0){ //queria seguir escribiendo pero llegue al max del archivo
              printf("ERROR: Ya se usaron todos los bloques de datos disponibles para el archivo.\n");
              file->indice->file_size = llevo_bytes;
              fclose(disco);
              free(buffer_aux);
              guardar_info_archivo(file);
              return llevo_bytes;
            }
            if(me_quedan == 0){
              file->indice->file_size = llevo_bytes;
              fclose(disco);
              free(buffer_aux);
              guardar_info_archivo(file);
              return llevo_bytes;
            }
          }
        }
      }
    }
  }
  //modo no es de escritura
  else{
    printf("ERROR: El archivo no fue abierto en modo de escritura\n");
    return 0;
  }
}

int cr_rm(unsigned disk, char* filename) {
  int bit_dir;
  char* dir = malloc(sizeof(char)*4);

  int cant_ref;

  char* indice_aux = malloc(sizeof(char)*8192);

  int n_bloque_indice;
  int direction;

  int hay_ind = 1;

  int is_soft;
  FILE* disco = fopen(ruta_archivo, "r");

  // Manejo de errores de input
  if (disk < 1 || disk > 4){
    printf("ERROR: Particion ingresada no es valida\n");
    return 1;
  }
  if (cr_exists(disk, filename)) {
    // Revisamos la cantidad de referencias del archivo en el bloque indice
    // Si es 1 debemos eliminar los datos en bitmap y bloque directorio
    // Si es < 1 debemos eliminar datos del bloque directorio y restar 1 a referencias

    // Revisamos si es un softlink
    char* sl = strchr(filename, '/');
    if(sl == NULL)
    {
      // No es softlink
      is_soft = 0;
    }
    else
    {
      // Si es softlink
      is_soft = 1;
    }

    for (int i =0; i< 256; i++)
    {
      int a = !!((Dir_disk[disk-1] -> entries[i] -> number[0] << 1) & 0x800000);

      if (a == 1 && strncmp(Dir_disk[disk-1]->entries[i]->file_name , filename, 32) == 0)
      {

        // Calculamos la direccion del bloque indice del archivo
        char *st= malloc(sizeof(char)*3);

        memcpy(st, Dir_disk[disk-1] -> entries[i] -> number, 3);
        n_bloque_indice = get_bloque(st);

        // Cambiamos el bit de validez a cero
        Dir_disk[disk-1] -> entries[i] -> number[0] &= ~(1 << 7);
        //Dir_disk[disk-1] -> entries[i] -> file_name = malloc(sizeof(char)*29);

        //Actualizamos bloque directorio en el disco
        FILE* disco_act = fopen(ruta_archivo, "rb+");
        int f = disk - 1;
        fseek(disco_act, 536870912*f + 32*i, SEEK_SET);
        char* aux_linea = malloc(sizeof(char)*32);
        memcpy(aux_linea, Dir_disk[disk-1] -> entries[i] -> number, 3);
        memcpy(&(aux_linea[3]), Dir_disk[disk-1] -> entries[i] -> file_name, 29);
        fwrite(aux_linea, 1, 32, disco_act);
        free(aux_linea);
        fclose(disco_act);

        free(st);

        break;

      }
    }

    if (is_soft == 0){
      // Seteamos el archivo en el bloque indice
      fseek(disco, 8192 * n_bloque_indice, SEEK_SET);

      // Leemos el bloque indice
      fread(indice_aux, 8192, 1, disco);

      // Leemos el cuarto byte donde se guardan la cantidad de referencias que tiene el bloque indice
      cant_ref = indice_aux[3];
      //printf("%d\n", cant_ref);

      // Le restamos 1 al indice de referencia, tambien cambiamos el byte del bloque indice
      cant_ref--;

      // Descomentar cuando se guarde lel bloque directorio para no tener inconsistencias en el archivo .bin
      indice_aux[3]--;

      FILE* disco_w = fopen(ruta_archivo, "rb+");

      fseek(disco_w, 8192 * n_bloque_indice, SEEK_SET);

      // Escribimos el bloque indice
      fwrite(indice_aux, 8192, 1, disco_w);
      fclose(disco_w);

      // Si el indice de referencias llega a cero
      // recorremos el bloque y vemos todas los punteros a bloques de datos y actualizacmos el bitmap (Actualizamos a 0)
      if (cant_ref == 0) {
        for (int k = 0; k < 2044; k++) {

          int q = 12+4*k;

          memcpy(dir, &(indice_aux[q]), 4);
          int ref = buscar_ref(dir);
          //printf("dir -> %d\n", ref);

          if (ref == 0) {
            hay_ind = 0;
            break;
          }
          // Buscamos el bit
          ref = ref - (65536*(disk - 1));

          int k = !!((bitmaps[disk - 1] -> map[ref / 8] << (ref % 8)) & 0x80);
          //printf("Antes ---> %d\n", k);

          // Cambiamos a cero el bit del bitmap
          bitmaps[disk - 1]->map[ref / 8] &= ~(1 << (ref % 8));

          int h = !!((bitmaps[disk - 1] -> map[ref / 8] << (ref % 8)) & 0x80);
          //printf("Despues ---> %d\n", h);
        }
      }

      if (hay_ind) {
        // Buscamos el bit de direccionamiento indirecto
        memcpy(dir, &(indice_aux[8188]), 4);
        int ref = buscar_ref(dir);

        // Seteamos el archivo en el bloque indice
        fseek(disco, 8192 * ref, SEEK_SET);

        char* i_indice_aux = malloc(sizeof(char)*8192);

        // Leemos el bloque indice
        fread(i_indice_aux, 8192, 1, disco);

        for (int k = 0; k < 2044; k++) {

            int q = 4*k;

            memcpy(dir, &(i_indice_aux[q]), 4);
            int ref = buscar_ref(dir);

            if (ref == 0) {
            break;
          }

            // Buscamos el bit
            ref = ref - (65536*(disk - 1));

            int k = !!((bitmaps[disk - 1] -> map[ref / 8] << (ref % 8)) & 0x80);
            //printf("Antes ---> %d\n", k);

            // Cambiamos a cero el bit del bitmap
            bitmaps[disk - 1]->map[ref / 8] &= ~(1 << (ref % 8));
        }
      }

      actualizar_bitmap(disk);

      // Liberamos la memoria
      free(indice_aux);
      free(dir);
      fclose(disco);
    }


  } else {
    printf("ERROR: Archivo no existe\n");
    return 1;
  }
  return 0;
}

int cr_hardlink (unsigned disk, char* orig, char* dest) {

  FILE* disco = fopen(ruta_archivo, "r");

  int n_bloque_indice;

  char * numero;
  char * nombre;

  int libre;

  // Manejo de errores de input
  if (disk < 1 || disk > 4){
    printf("ERROR: Particion ingresada no es valida\n");
    return 1;
  }

  // Revisamos si el archivo origen existe
  if (cr_exists(disk, orig)) {

    // Revisamos si el nombre del archivo ya existe en la particion
    for (int i =0; i< 256; i++)
    {
      if (strncmp(Dir_disk[disk-1]->entries[i]->file_name , dest, 32) == 0)
      {
        printf("ERROR: Ya existe un archivo con ese Nombre\n");
        return 1;
      }
    }

    // Debemos buscar la referencia al bloque indice del archivo
    for (int i =0; i< 256; i++)
    {
      int a = !!((Dir_disk[disk-1] -> entries[i] -> number[0] << 1) & 0x800000);

      if (a == 1 && strncmp(Dir_disk[disk-1]->entries[i]->file_name , orig, 32) == 0)
      {
        // Calculamos la direccion del bloque indice del archivo
        char *st= malloc(sizeof(char)*3);

        char* indice_aux = malloc(sizeof(char)*8192);
        memcpy(st, Dir_disk[disk-1] -> entries[i] -> number, 3);
        n_bloque_indice = get_bloque(st);

        // guardamos la informacion de la entrada
        numero = Dir_disk[disk-1] -> entries[i] -> number;
        nombre = Dir_disk[disk-1] -> entries[i] -> file_name;

        free(st);
        free(indice_aux);
        break;

      }
    }
    // Le sumamos 1 a la cantidad de referencias del archivo
    char* aux = malloc(sizeof(char)*8192);

     // Seteamos el archivo en el bloque indice
    fseek(disco, 8192 * n_bloque_indice, SEEK_SET);

    // Leemos el bloque indice
    fread(aux, 8192, 1, disco);

    fclose(disco);
    // Leemos el cuarto byte donde se guardan la cantidad de referencias que tiene el bloque indice
    // Le sumamos 1 al indice de referencia, tambien cambiamos el byte del bloque indice

    // Descomentar cuando se guarde lel bloque directorio para no tener inconsistencias en el archivo .bin
    aux[3]++;

    FILE* disco_w = fopen(ruta_archivo, "rb+");

    fseek(disco_w, 8192 * n_bloque_indice, SEEK_SET);

    // Escribimos el bloque indice
    fwrite(aux, 8192, 1, disco_w);

    free(aux);

    // Copiamos la informacion a una nueva entrada del directorio, con puntero al bloque indice del otro archivo
    // La parte "number" es igual a la del archivo original
    for (int i =0; i< 256; i++)
    {
      int a = !!((Dir_disk[disk-1] -> entries[i] -> number[0] << 1) & 0x800000);

      if (a == 0)
      {
        libre = i;

        break;
      }
    }

    Dir_disk[disk-1]-> entries[libre] = entry_init();
    memcpy(Dir_disk[disk-1]-> entries[libre]->file_name, dest, 29);
    memcpy(Dir_disk[disk-1]-> entries[libre]->number, numero, 3);

    //la posicion que tengo que cambiar es la i
    FILE* disco_act = fopen(ruta_archivo, "rb+");
    int f = disk - 1;
    fseek(disco_act, 536870912*f + 32*libre, SEEK_SET);
    char* aux_linea = malloc(sizeof(char)*32);
    memcpy(aux_linea, numero, 3);
    memcpy(&(aux_linea[3]), dest, 29);
    fwrite(aux_linea, 1, 32, disco_act);
    free(aux_linea);
    fclose(disco_act);

    //actualizar_bitmap(disk);

  } else {
    printf("ERROR: Archivo origen no existe\n");
    return 1;
  }


  return 0;
}


int cr_soflink (unsigned disk_orig, unsigned disk_dest, char* orig) {

  FILE* disco = fopen(ruta_archivo, "r");

  int n_bloque_indice;

  char * nombre_f = malloc(sizeof(char)*29);

  char * numero;
  char * nombre;

  int libre;

  // Manejo de errores de input
  if (disk_orig < 1 || disk_orig > 4){
    printf("ERROR: Particion origen ingresada no es valida\n");
    return 1;
  }

  if (disk_dest < 1 || disk_dest > 4){
    printf("ERROR: Particion destino ingresada no es valida\n");
    return 1;
  }

  // Generamos el nonmbre del soflink a partir del input
  char * copy_n = malloc(sizeof(char)*29);
  strcpy(copy_n, orig);

  sprintf(nombre_f, "%u", disk_orig);

  strcat(nombre_f, "/");
  strcat(nombre_f, copy_n);

  // Revisamos si el archivo origen existe
  if (cr_exists(disk_orig, orig)) {

    // Revisamos si el nombre del archivo ya existe en la particion
    for (int i =0; i< 256; i++)
    {
      if (strncmp(Dir_disk[disk_dest-1]->entries[i]->file_name , nombre_f, 32) == 0)
      {
        printf("ERROR: Ya existe un archivo con ese Nombre\n");
        return 1;
      }
    }

    // Debemos buscar la referencia al bloque indice del archivo
    for (int i =0; i< 256; i++)
    {
      int a = !!((Dir_disk[disk_orig-1] -> entries[i] -> number[0] << 1) & 0x800000);

      if (a == 1 && strncmp(Dir_disk[disk_orig-1]->entries[i]->file_name , orig, 32) == 0)
      {

        // guardamos la informacion de la entrada
        numero = Dir_disk[disk_orig-1] -> entries[i] -> number;
        nombre = Dir_disk[disk_orig-1] -> entries[i] -> file_name;

        break;

      }
    }

    for (int i =0; i< 256; i++)
    {
      int a = !!((Dir_disk[disk_dest-1] -> entries[i] -> number[0] << 1) & 0x800000);

      if (a == 0)
      {
        libre = i;

        break;
      }
    }

    Dir_disk[disk_dest-1]-> entries[libre] = entry_init();
    memcpy(Dir_disk[disk_dest-1]-> entries[libre]->file_name, nombre_f, 29);
    memcpy(Dir_disk[disk_dest-1]-> entries[libre]->number, numero, 3);

    //la posicion que tengo que cambiar es la i
    FILE* disco_act = fopen(ruta_archivo, "rb+");
    int f = disk_dest - 1;
    fseek(disco_act, 536870912*f + 32*libre, SEEK_SET);
    char* aux_linea = malloc(sizeof(char)*32);
    memcpy(aux_linea, numero, 3);
    memcpy(&(aux_linea[3]), nombre_f, 29);
    fwrite(aux_linea, 1, 32, disco_act);
    free(aux_linea);
    fclose(disco_act);

  } else {
    printf("ERROR: Archivo origen no existe\n");
    return 1;
  }


  return 0;
}


void cr_unload_particion_completa(unsigned disk, char* dest){
  Directory* disco = Dir_disk[disk-1];
  for (int i =0; i< BLOCK_ENTRIES; i++)
  {
    Entry* entrada = disco->entries[i];
    int a = !!((entrada->number[0] << 1) & 0x800000);
    if (a == 1){
      char* nuevo_nombre = malloc(sizeof(char)*32);
      memcpy(nuevo_nombre, dest, 32);
      strcat(nuevo_nombre,entrada->file_name);
      cr_unload(disk, entrada->file_name, nuevo_nombre);
      free(nuevo_nombre);
    }

  }
}

int cr_unload(unsigned disk, char* orig, char* dest){
  if (disk < 0 || disk > PARTICIONES){
    printf("Input disco incorrecto\n");
    return -1;
  }

  if (orig == NULL) {
    // indica que quieren copiar todo un sector. Particion o Disco completo?
    if (disk == 0){
      for (int partition = 1; partition < PARTICIONES + 1; partition++){
        cr_unload_particion_completa(partition, dest);
      }
      return 1;
    }
    else{
      // particion completa
      cr_unload_particion_completa(disk, dest);
      return 1;
    }
  }
  else{
    // Debemos copiar el archivo en "orig" en dest
    if (cr_exists(disk, orig)) {
      crFILE *unload_file = cr_open(disk, orig, "r");
      if (unload_file) {
        char* buffer = calloc(unload_file->indice->file_size, sizeof(char));
        cr_read(unload_file, buffer, unload_file->indice->file_size);
        FILE *move_to;
        if ((move_to = fopen(dest, "wb")) == NULL)
        {
          printf("PATH INCORRECTO (%s)\n", dest);
          return 0;
        }
        fwrite(buffer, sizeof(char), unload_file->indice->file_size, move_to);
        fclose(move_to);
        free(buffer);
        return 1;
      }

    }
  }

}


int cr_load(unsigned disk, char* orig){
// Funcio ́n que se encarga de copiar un archivo o los contenidos de
// una carpeta, referenciado por orig a la particio ́n designada.
// En caso de que un archivo sea demasiado pesado para el disco,
// se debe escribir todo lo posible hasta acabar el espacio disponible.
// En caso de que el sea una carpeta, se deben copiar los archivos que
// esten dentro de esta carpeta, ignorando cualquier carpeta adicional que tenga.

  if (disk < 0 || disk > PARTICIONES){
    printf("ERROR: Input disco incorrecto\n");
    return -1;
  }
  if (orig == NULL) {
    printf("ERROR: orig NULL\n");
    return -1;
  }
  int carpeta = (orig[0] == '/');
  // chequear si es carpeta al verificar su primera letra

  if (!carpeta) {
    FILE *file_to_upload = fopen(orig,"rb");
    if(!file_to_upload){
      printf("- Archivo - %s - no encontrado !\n", orig);
      return 0;
    }

    printf("Archivo - %s - encontrado !\n", orig);
    // abrimos archivo en disco
    crFILE *new_upload = cr_open(disk, orig, "w");
    if(new_upload == NULL){
      printf("Error en escritura !\n");
      return 0;
    }
    void* buffer = malloc(sizeof(char)*32);
    char block_read[32];
    while(fread(block_read, sizeof(block_read), 1, file_to_upload)){
      buffer = block_read;
      int escribir = cr_write(new_upload, buffer, 32);
    }
    fclose(file_to_upload);

    //char* texto = calloc(14000, sizeof(char));
    //fgets(texto, 10000, file_to_upload);
    //int num = cr_write(new_upload, texto, 14000);
    //cr_close(new_upload);
    //free(texto);
    //fclose(file_to_upload);
    return 1;
  }
  else{
    DIR *d;
    struct dirent *dir;
    // extraer todo el nombre excepto primer char y abrir
    memmove(orig, orig+1, strlen(orig));
    printf("------ LS directorio: %s ------\n", orig);
    d = opendir(orig);
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if (strncmp(".", dir->d_name, 1) != 0) {
          printf("%s\n", dir->d_name);
        }

      }
      closedir(d);
    }
    printf("------ FIN LS directorio: %s ------\n", orig);
    printf("------ Transfiriendo archivos %s ------\n", orig);
    d = opendir(orig);
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if (strncmp(".", dir->d_name, 1) != 0) {
          cr_load(disk, dir->d_name);
        }
      }
      closedir(d);
    }
    return 1;
    // carpeta
    // parsear nombre Carpeta
    // sacar todos los archivos  de la carpeta, y  cr_load()  ``
  }
}
