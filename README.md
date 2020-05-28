# Proyecto_SSOO





Funciones:


void cr_mount(char* diskname).


void cr_bitmap(unsigned disk, bool hex).


int cr_exists(unsigned disk, char* filename)


void cr_ls(unsigned disk).




**Manejo de archivos**

crFILE* cr_open(unsigned disk, char* filename, char mode).

int cr_read(crFILE* file_desc, void* buffer, int nbytes)

int cr_write(crFILE* file_desc, void* buffer, int nbytes).

int cr_close(crFILE* file_desc).

int cr_rm(unsigned disk, char* filename).

int cr_hardlink(unsigned disk, char* orig, char* dest)

int cr_softlink(unsigned disk_orig, unsigned disk_dest, char* orig, char* dest).


int cr_unload(unsinged disk, char* orig, char* dest)

Copia un archivo, particion o disco.  
(disk ∈ {1,...,4}, complete_disk = 0)

String orig sea NULL para indicar que quieren copiar todo un sector.



int cr_load(unsinged disk, char* orig)

Copia un archivo o los contenidos de una carpeta, referenciado por orig a la particion designada.

En caso de que un archivo sea demasiado pesado para el disco, se escribe lo mas posible.

En caso de que el sea una carpeta, se copian todos sus archivos. Si contiene carpetas, estas no son movidas
