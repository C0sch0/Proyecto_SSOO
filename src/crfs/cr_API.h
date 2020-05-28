#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
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
  file -> dir = 0;
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
{ char *particion;
  char *nombre;
  char *str = malloc(sizeof(char)*32);
  int valid = 0;

  for(int i = 0; i<BLOCK_ENTRIES;i++){
    if(strncmp(Dir_disk[disk-1]->entries[i]->file_name , filename, 32) ==0 ){
      //printf("existe\n");
      free(str);
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
          free(str);
          return 1;
        }
      }
    }
  }
}
  free(str);
  //printf("no existe\n");
  return 0;
}


void cr_ls(unsigned disk)
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
      return i
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
    for (int j = 0; j < 8; j++)
    {
      int k = !!((bitmap->map[i] << j) & 0x80);
      if(k == 0)
      {
        num_bloque_relat = 8*i + j;
        a = i;
        b = j;
        encontre = 1;
        break;
      }
    }
  }
  if (encontre == 1){
    bitmap->map[a] |= 1 << b;
    return num_bloque_relat;}
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
		printf("Debe elegir una opcion valida de particion (1-4) o 0 si quiere el de todo el disco\n");
	}
}



void create_cr_bitmaps()
{
  for(int i = 0; i < 4; i++)
	{
		bitmaps[i] = bitmap_init();
	}

	FILE* disk = fopen(ruta_archivo, "r");
	for(int i = 0; i < 4; i++)
	{
		char* mapp = malloc(sizeof(char)*BLOCK_BYTES);
		int hasta = 536870912*i + BLOCK_BYTES;
  	fseek(disk, hasta , SEEK_SET);
  	fread(mapp, 8192, 1, disk);
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

void destroy_crfile(crFILE*file){
  free(file-> file_name);
  free(file-> valid);
  free(file-> mode);
  destroy_indice(file->indice);
  free(file);
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

  int a[24],i;
  for(i=0;num_bloque>0;i++)
  {
  a[i] = num_bloque%2;
  num_bloque = num_bloque/2;

  }

  int b[24];
  for(int y = 0; y< 24; y++)
  {
    b[y] = 0;

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


// No la estamos usando por ahora


crFILE* cr_open(unsigned disk, char* filename, char *mode){

  char *particion;
  char *nombre;
  char *str = malloc(sizeof(char)*32);
  int prt;

  FILE* disco = fopen(ruta_archivo, "r");

  if(strncmp(mode , "r", 1) == 0){
    int exist;
    // Chequeamos que exista el archivo
    exist = cr_exists(disk, filename);
    if(exist == 1){
      // el archivo en el disco si existe creamos el crFILE
      char* ptr;
      crFILE* file;
      file = init_crfile();

      // ahora tengp que buscar el archivo
      for(int i = 0; i<BLOCK_ENTRIES;i++)
      {
      	if(strncmp(Dir_disk[disk-1]->entries[i]->file_name , filename, 32) ==0 )
      	{
            // si el nombre esta en la partición
            // le doy a file los atributos del archivo
            memcpy(file->file_name, Dir_disk[disk-1]->entries[i]->file_name, 29);
            memcpy(file->valid, Dir_disk[disk-1]->entries[i]->number, 3);
            // le cambio asigno el modo en el que se abrio
            memcpy(file->mode, mode, 1);
            //memcpy(file->dir, 0, 1);
            break;
          }
        else
        {
          // si el nombre no esta en la partición
          memcpy(str, Dir_disk[disk-1]->entries[i]->file_name, 32);
          particion = strtok(str,"/");
          nombre = strtok(NULL,"/");

          if(nombre!=NULL)
          {
            if(strncmp(nombre , filename, 32) == 0)
            {
              prt = atoi(particion);
              for(int j = 0; j<BLOCK_ENTRIES;j++)
              {
                // buscamos en la particion
                if(strncmp(Dir_disk[prt-1]->entries[j]->file_name , filename, 32)==0)
                {
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
      fread(indice_aux, 8192, 1, disco);
      Index* ind= file->indice;
      char* aux_ref = malloc(sizeof(char)*4);
      memcpy(aux_ref, indice_aux, 4);
      ind->references = buscar_ref(aux_ref);
      free(aux_ref);
      char* aux_size = malloc(sizeof(char)*8);
      memcpy(aux_size, &(indice_aux[4]), 8);
      ind->file_size = buscar_size(aux_size);
      free(aux_size);
      for (int i = 0; i < 2044; i++)
      {
      	char* aux_ptr = malloc(sizeof(char)*4);
      	int q = 12+4*i;
      	memcpy(aux_ptr, &(indice_aux[q]), 4);
      	ind->blocks_data[i] = buscar_ref(aux_ptr);
      	free(aux_ptr);
      }
      char* aux_ind = malloc(sizeof(char)*4);
      memcpy(aux_ind, &(indice_aux[8188]), 4);
      ind->indirect_simple = buscar_ref(aux_ind);
      free(aux_ind);
      free(st);
      free(str);
      free(indice_aux);
      fclose(disco);
      return file;
    }
    else{
      // exist es 0
      printf("El archivo no existe\n");
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
      	// el archivo en el disco si existe creamos el crFILE
      		char* ptr;
      		crFILE* file;
      		file = init_crfile();

      		// ahora tengp que buscar el archivo
      		for(int i = 0; i<BLOCK_ENTRIES;i++)
      		{
      			if(strncmp(Dir_disk[disk-1]->entries[i]->file_name , filename, 32) ==0 )
      			{
            		// si el nombre esta en la partición
            		// le doy a file los atributos del archivo
            		memcpy(file->file_name, Dir_disk[disk-1]->entries[i]->file_name, 29);
		            memcpy(file->valid, Dir_disk[disk-1]->entries[i]->number, 3);
		            // le cambio asigno el modo en el que se abrio
		            memcpy(file->mode, mode, 1);
		            //memcpy(file->dir, 0, 1);
		            break;
          		}
         	 	else
          		{
		            // si el nombre no esta en la partición
		            memcpy(str, Dir_disk[disk-1]->entries[i]->file_name, 32);
		            particion = strtok(str,"/");
		            nombre = strtok(NULL,"/");

		            if(nombre!=NULL)
            		{
		            	if(strncmp(nombre , filename, 32) == 0)
		              	{
		                	prt = atoi(particion);
		                	for(int j = 0; j<BLOCK_ENTRIES;j++)
		                	{
		                  		// buscamos en la particion
		                  		if(strncmp(Dir_disk[prt-1]->entries[j]->file_name , filename, 32)==0)
		                  		{
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
        	for (int i = 0; i < 2044; i++)
        	{
	        	char* aux_ptr = malloc(sizeof(char)*4);
	        	int q = 12+4*i;
	        	memcpy(aux_ptr, &(indice_aux[q]), 4);
	        	ind->blocks_data[i] = buscar_ref(aux_ptr);
	        	free(aux_ptr);
        	}
       		 char* aux_ind = malloc(sizeof(char)*4);
	        memcpy(aux_ind, &(indice_aux[8188]), 4);
	        ind->indirect_simple = buscar_ref(aux_ind);
	        free(aux_ind);
	        free(st);
	        free(str);
	        free(indice_aux);
	        fclose(disco);
        	return file;
    	}
    	else
    	{
      		// exist es 0
      		//tengo que buscarle espacio al bloque indice
          Bitmap* mapeo = bitmaps[disk-1];
          int n_bloque = buscar_bloque_disponible(mapeo); //numero del primer bloque que esta disponible (numero relativo) y cambio en el bitmap el 0 por un 1 en ese bloque si hay bloques disponibles
          if (n_bloque == 0)//no queda espacio
          {
            printf("No existe espacio en la particion\n");
            free(str);
            fclose(disco);
            return NULL;
          }
          Directory* directorio_actual = Dir_disk[disk-1];
          int new_entry = buscar_entry_disponible(directorio_actual);
          if(new_entry == 0){
            printf("No quedan entradas disponibles en el directorio\n");
            free(str);
            fclose(disco);
            return NULL;
          }
          char* nombre_a_copiar = calloc(29, sizeof(char));
          memcpy(nombre_a_copiar, filename, 29);
          crFILE* file = init_crfile();
          int n_bloque_abs = 65536*disk + n_bloque;
          file->n_b_indice = n_bloque_abs;
          char* n_bloque_traducido = traducir_num_bloque(n_bloque_abs);
          memcpy(file->file_name, nombre_a_copiar, 29);
          memcpy(Dir_disk[disk -1]->entries[new_entry]->file_name, nombre_a_copiar, 29);
          memcpy(file->valid, n_bloque_traducido, 3);
          memcpy(Dir_disk[disk -1]->entries[new_entry]->number, n_bloque_traducido, 3);
          free(nombre_a_copiar);
          free(n_bloque_traducido);
          memcpy(file->mode, mode, 1);
          free(str);
          fclose(disco);
          return file;
        }
    }
}


int cr_read(crFILE* file_desc, void* buffer, int nbytes)
{
  // chequear modo read
  if (!file_desc)
  {
    printf("Archivo no abierto correctamente\n" );
    return -1;
  }

  if(strncmp(file_desc -> mode, "r", 32) != 0)
  {
    printf("El archivo no fue abierto en modo de lectura\n" );
    return -1;
  }


  FILE* disco = fopen(ruta_archivo, "r");
  char* read_aux = malloc(sizeof(char)*BLOCK_BYTES);
  char* buffer_aux = malloc(sizeof(char)*nbytes);
  char* byte = malloc(sizeof(char));

  // vemos en que bloque y byte quedamos leyendo
  fseek(disco, file_desc -> indice-> blocks_data [file_desc -> bloque]*BLOCK_BYTES
    + file_desc -> byte, SEEK_SET);

  // obtenemos lo que queda por leer del bloque
  fread(read_aux, BLOCK_BYTES - (file_desc -> byte), 1, disco);

  int byte_actual = file_desc -> byte;
  int bloque_actual = file_desc -> bloque;

  // nuestro for esta condicionado a la cantidad de bytes que queramos leer
  // debo el primer bloque
  for(int i = 0; i < nbytes; i++){

    // el numero de posición va de 0 a 8192, si supero este numero debo irme a otro bloque a leer
    if(byte_actual < BLOCK_BYTES){
      //printf("%c**\n", read_aux[byte_actual]);
      memcpy(&buffer_aux [i], &read_aux[byte_actual], 1);
      byte_actual++;
    }
    else{
      // cuando esto pase debo "reiniciar" el contador en 0
      printf("cambiamos de bloque al bloque:");
      bloque_actual++;
      byte_actual = 0;
      printf("%i\n", file_desc -> indice-> blocks_data [bloque_actual]);
      fseek(disco, file_desc -> indice-> blocks_data [bloque_actual]*BLOCK_BYTES , SEEK_SET);
      fread(read_aux, BLOCK_BYTES, 1, disco);
    }
  }
  // liberar read y byte me tira error
  free(buffer_aux);
}
