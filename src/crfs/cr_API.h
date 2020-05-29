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
  file-> bloques_ocupados = 0;
  file->n_particion = 0;
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

  if(strncmp(mode , "r", 1) == 0)
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
          destroy_crfile(file);
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
      for (int i = 0; i < 2044; i++)
      {
        char* aux_ptr = malloc(sizeof(char)*4);
        int q = 12+4*i;
        memcpy(aux_ptr, &(indice_aux[q]), 4);
        ind->blocks_data[i] = buscar_ref(aux_ptr);
        if(ind->blocks_data[i] > 0){
          file->bloques_ocupados ++;
        }
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
      printf("El archivo no existe en la particion\n");
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
        printf("Este archivo ya existe por lo que no se puede abrir en modo de escritura\n");
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
            printf("No existe espacio en la particion\n");
            free(str);
            fclose(disco);
            return NULL;
          }
          Directory* directorio_actual = Dir_disk[disk-1];
          //Busco la primera entrada vacia para llenarla con la info del archivo que estoy creando
          int new_entry = buscar_entry_disponible(directorio_actual);
          if(new_entry == 0){ //no quedan entradas
            printf("No quedan entradas disponibles en el directorio\n");
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
}

int cr_read(crFILE* file_desc, void* buffer, int nbytes)
{
  if (nbytes == 0) {
    return 0;
  }
  if (!file_desc){
    printf("Archivo no abierto correctamente\n" );
    return 0;
  }

  if(strncmp(file_desc -> mode, "r", 32) != 0){
    printf("El archivo no fue abierto en modo de lectura\n" );
    return 0;
  }

  FILE* disco = fopen(ruta_archivo, "r");
  char* read_aux = malloc(sizeof(char)*BLOCK_BYTES);
  char* buffer_aux = malloc(sizeof(char)*nbytes);
  char* byte = malloc(sizeof(char));
  int efectivamente_leidos = 0;

  // vemos en que bloque y byte quedamos leyendo
  fseek(disco,
    file_desc -> indice-> blocks_data[file_desc -> bloque]*BLOCK_BYTES
    + file_desc -> byte, SEEK_SET);

  // obtenemos lo que queda por leer del bloque
  fread(read_aux, BLOCK_BYTES - (file_desc -> byte), 1, disco);

  int byte_actual = file_desc -> byte;
  int bloque_actual = file_desc -> bloque;

  // nuestro for esta condicionado a la cantidad de bytes que queramos leer
  // debo el primer bloque
  for(int i = 0; i < nbytes; i++){
    // el numero de posición va de 0 a 8192, si supero este numero debo irme a otro bloque a leer
    if(byte_actual < BLOCK_BYTES)
    {
      //printf("%c**\n", read_aux[byte_actual]);
      memcpy(&buffer_aux [i], &read_aux[byte_actual], 1);
      byte_actual++;
      efectivamente_leidos++;
    }
    else
    {
      // cuando esto pase debo "reiniciar" el contador en 0
      printf("cambiamos de bloque al bloque:");
      bloque_actual++;
      byte_actual = 0;
      printf("%i\n", file_desc -> indice-> blocks_data [bloque_actual]);
      fseek(disco, file_desc -> indice-> blocks_data [bloque_actual]*BLOCK_BYTES , SEEK_SET);
      fread(read_aux, BLOCK_BYTES, 1, disco);
     }
  }

  for (int i = 0; i < efectivamente_leidos; i++)
  {
    ((unsigned char *)buffer)[i] = buffer_aux[i];
  }
  printf("%s\n", buffer);
  printf("efectivamente_leidos : %d\n", efectivamente_leidos);
  free(buffer_aux);
  free(read_aux);
  return efectivamente_leidos;
}

int cr_write(crFILE* file, void* buffer, int n_bytes){
  //chequeo que el modo del archivo este ok
  if(strncmp(file->mode, "w", 1) == 0)
  {
    FILE* disco = fopen(ruta_archivo, "rb+");
    //necesito ver cuantos bloques de datos ocupo con los n_bytes
    int resto = n_bytes % BLOCK_BYTES;
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
      for(int i = 0; i < bloques_necesito; i++){
        bloque_disp_rel = buscar_bloque_disponible(bitmap_actual); //obtengo numero del primer bloque vacio (relativo)
        if(bloque_disp_rel == 0){
          //no quedan bloques disponibles en la particion
          printf("ERROR: no quedan bloques disponibles en la particion");
          file->indice->file_size = llevo_bytes;
          return llevo_bytes;
        }
        bloque_disp_abs = 65536*(file->n_particion - 1) + bloque_disp_rel; // lo paso a numero absoluto
        file->indice->blocks_data[i] = bloque_disp_abs; //lo guardo en el array de data_blocks
        voy_bloque ++;
        file->bloques_ocupados++;
        if(bloques_necesito - voy_bloque == 0 && resto > 0){ //estoy en el ultimo bloque y escribo la cantidad de bytes = resto
          //seteamos el archivo en el bloque
          fseek(disco, BLOCK_BYTES * bloque_disp_abs , SEEK_SET);
          //escribo los bytes_restantes(resto)
          //fwrite();
          llevo_bytes += resto;
          file->indice->file_size = llevo_bytes;
          return llevo_bytes;
        }
        else if(bloques_necesito - voy_bloque == 0 && resto == 0){//estoy en el ultimo bloque y escribo 8192 bytes.
          //seteamos el archivo en el bloque
          fseek(disco, BLOCK_BYTES * bloque_disp_abs , SEEK_SET);
          //escribo los 8192 bytes restantes
          //fwrite();
          llevo_bytes += BLOCK_BYTES;
          file->indice->file_size = llevo_bytes;
          return llevo_bytes;
        }
        else if(bloques_necesito > voy_bloque){
          //seteamos el archivo en el bloque
          fseek(disco, BLOCK_BYTES * bloque_disp_abs , SEEK_SET);
          //escribo los 8192 bytes restantes
          //fwrite();
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
          printf("ERROR: no quedan bloques disponibles en la particion");
          file->indice->file_size = llevo_bytes;
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
          //fwrite();
          llevo_bytes += BLOCK_BYTES;
        }
      }
      //ahora voy a llenar el bloque de indireccion simple
      //chequeo si hay espacio para el bloque de indireccionamiento
      bloque_disp_rel = buscar_bloque_disponible(bitmap_actual);
      if(bloque_disp_rel == 0){
          //no quedan bloques disponibles en la particion
          printf("ERROR: no quedan bloques disponibles en la particion");
          file->indice->file_size = llevo_bytes;
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
          printf("ERROR: no quedan bloques disponibles en la particion");
          file->indice->file_size = llevo_bytes;
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
          //fwrite();
          llevo_bytes += resto;
          file->indice->file_size = llevo_bytes;
          return llevo_bytes;
        }
        else if(bloques_ind_sim - voy_3_bloque == 0 && resto == 0){//estoy en el ultimo bloque y escribo 8192 bytes.
          //seteamos el archivo en el bloque
          fseek(disco, BLOCK_BYTES * bloque_disp_abs , SEEK_SET);
          //escribo los 8192 bytes restantes
          //fwrite();
          llevo_bytes += BLOCK_BYTES;
          file->indice->file_size = llevo_bytes;
          return llevo_bytes;
        }
        else if(bloques_ind_sim > voy_3_bloque){
          //seteamos el archivo en el bloque
          fseek(disco, BLOCK_BYTES * bloque_disp_abs , SEEK_SET);
          //escribo los 8192 bytes restantes
          //fwrite();
          llevo_bytes += BLOCK_BYTES;
          if(voy_3_bloque == 2048){
            //estoy en el ultimo bloque del bloque de indireccionamiento simple
            int me_quedan = n_bytes - llevo_bytes;
            if (me_quedan > 0){ //queria seguir escribiendo pero llegue al max del archivo
              printf("ERROR: el archivo no puede ser tan grande\n");
              file->indice->file_size = llevo_bytes;
              return llevo_bytes;
            }
            if(me_quedan == 0){
              file->indice->file_size = llevo_bytes;
              return llevo_bytes;
            }
          }
        }
      }
    }
  }
  //modo no es de escritura
  else{
    printf("Error al tratar de escribir en un archivo abierto en modo lectura\n");
    return 0;
  }
}


int cr_close(crFILE* file_desc){
  free(file_desc-> file_name);
  free(file_desc-> valid);
  free(file_desc-> mode);
  destroy_indice(file_desc->indice);
  free(file_desc);
  return 1;
}
