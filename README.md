# Proyecto_SSOO

Integrantes

Antonia Christensen - 15636682

Diego Iruretagoyena - 14619164

Tamara Lues - 15638936

Claudio Prieto - 1664039J



Funciones:

**void cr_mount(char* diskname):** Toma el nombre del archivo binario escrito por consola (argv[1]) y lo setea como variable global (ruta_archivo) para ocupar durante la ejecucion del programa. Se controlan los posibles errores en el main para mantener el formato de retorno dado de esta funcion en el enunciado.
**ERRORES:**
1. Se chequea que se corra el programa de forma correcta, controlando que se entreguen los 2 argumentos necesitados : el programa mismo (*./crfs*) y el disco (*<simdisk.bin>*).
2. Si el archivo no existe o no esta bien ubicado se lanza una advertencia: *Disk not mounted*.
En ambos casos se finaliza el programa con un return de 1.

**void cr_bitmap(unsigned disk, bool hex):** Previamente en el main se cargan los bitmaps desde el disco a structs Bitmap, y se agregan en un array que simboliza el disco y sus particiones.Recibe un int (*disk*) que representa el numero de particion de la cual quiero imprimir el bitmap correspondiente(1-4), o si se buscar imprimir los 4 bitmaps del disco completo (0). Además, recibe un bool (*hex*) que determina si lo imprimo en binario (0) o hexagesimal (*cualquier cosa distinta de 0*).
**ERRORES:**
1. Si el numero *disk* ingresado no esta dentro de {0,1,2,3,4}, se lanza un mensaje de error explicando que el input no es valido.

**int cr_exists(unsigned disk, char* filename):** recibe el nombre del archivo que buscamos y el numero de particion donde vamos a buscar. Previamente en el main se cargan los directorios desde el disco a structs Directory, y se agregan en un array que simboliza el disco y sus particiones. Se accede al struct pedido por *disk* y se itera sobre las entradas de este struct Directory. Cada entrada (struct Entry) tiene un atributo file_name el cual se compara con el nombre ingresado *filename* usando la función strcmp. Si esta funcion retorna 0 (son iguales), el archivo existe en la particion y se retorna 1.
**SUPUESTOS:**
1. Se consideró que para que un archivo exista en la partición el nombre debe estar escrito tal cual en la entrada de directorio, en cualquier otro caso, se retorna 0 ya que se considera que el archivo no existe.
2. Para el caso de los **softlinks** se considera que el nombre es de la forma: {*particion*}/{*nombre_archivo_original*}.

**void cr_ls(unsigned disk):** recibe el número de partición que queremos imprimir sus nombres de archivos.Previamente en el main se cargan los directorios desde el disco a structs Directory, y se agregan en un array que simboliza el disco y sus particiones. Se accede al struct pedido por *disk* y se itera sobre las entradas de este struct Directory. Cada entrada (struct Entry) tiene un atributo valid el cual incluye tanto el bit de validez como el numero de bloque indice. Se ocupa un mask para obtener el valid bit. Si este es 1, se imprime el file_name correspondiente a la entrada. En caso contrario, se omite. Se repite esto hasta cubrir las 256 entradas de directorio posible.

**Manejo de archivos**

**crFILE* cr_open(unsigned disk, char* filename, char mode):** Se recibe el nombre del archivo que se quiere abrir (*filename*), el número de partición de donde debería/debiese estar el archivo (*disk*) y el modo en que se quiere abrir (*mode*) pudiendo ser este último *'r'* para leer y *'w'* para escribir.
**Caso mode = 'r'**: En primer lugar, se revisa que el archivo exista en la particion dada, utilizando cr_exists.
Si el archivo existe, se revisa si es un softlink o no (buscando que el caracter '/' sea parte del nombre).
Si no es un softlink, se crea el struct crFILE y se carga con la información del archivo que representa obtenida de la lectura del disco. Esta información consta de: el nombre del archivo, el atributo valid (valid bit + 23 bits de numero de bloque indice), cargamos un struct Index que incluye la información guardada en el bloque indice del archivo en el disco (referencias, tamaño, punteros a bloques de datos, puntero a bloque de indireccion simple, y se agregó un struct Indirect con los punteros a los bloques de datos que este apunta, si corresponde), y se retorna el struct crFILE creado.
Si es un softlink, se llama nuevamente a la función cr_exists pero ahora considerando como *disk* el numero que va antes del '/' en el nombre del archivo y como *filename* el nombre que va despues del '/'. Si el archivo existe, se procede a repetir el procedimiento de crear el struct crFILE y cargarlo de información, retornandolo como se describió anteriormente.
**ERRORES: modo read**
1. Si el *filename* no existe en la primera llamada a cr_exists, se lanza un error de que el archivo no existe en esa partición y se retorna NULL.
2. Si el *filename* es un softlink pero en la segunda llamada a cr_exist se retorna un 0 (el archivo no existe en la partición dada), se lanza un mensaje de error dado que el archivo que se trata de abrir es un broken link y se retorna NULL.
**caso mode = 'w'**: En primer lugar, se revisa que el archivo exista en la particion dada, utilizando cr_exists. En el caso en que no existe, se llama a una función que busca en el bitmap el primer bloque de la partición disponible. Si encuentra un bloque disponible, cambia el valor 0 por un 1 en el bitmap (lo marca ocupado) y retorna el numero de bloque asignado. Luego, con otra función se busca la primera entrada de directorio dispoible en el struct Directory de la particion buscada. Si se encuentra una entrada disponible y una vez que tenemos un bloque asignado se procede a cargar la información en el struct crFILE creado y se carga el bloque indice inicializado en cero sus valores. Además, se crea una nueva entrada de directorio con la información de este nuevo archivo. Finalmente se retorna el struct cargado.
**ERRORES: modo write**
1. Si el nombre ingresado *filename* ya existe en la particion, se levanta un mensaje de error ya que no se puede abrir un archivo ya existente en modo escritura, y se retorna NULL.
2. Si no hay bloques disponibles en el bitmap para asignarlo como bloque indice al archivo, se borra el struct creado, se levanta un mensaje de error diciendo que no hay bloques disponibles en el disco y se retorna NULL.
3. Si no hay entradas disponibles en el bloque de directorio, se borra el struct creado y se levanta un mensaje de error diciendo que no hay entradas disponibles en el directorio de esa particion y se retorna NULL.
**ERRORES: generales**
1. Si el modo ingresado no es ni 'r' ni 'w', se lanza un error diciendo que el modo es invalido y se retorna NULL.
2. Si la partición indicada por disk no está dentro de {1,2,3,4}, se lanza un error diciendo que la partición es inválida y se retorna NULL.
**SUPESTO:** Se asumio que solamente puedo abrir archivos en modo 'w' si estos no existen en la partición. No puedo sobreescribir directamente, para hacerlo tendría que eliminarlo y crearlo de nuevo.

**int cr_read(crFILE* file_desc, void* buffer, int nbytes):** En primer lugar, se revisa la cantidad que se quieran leer (nbytes) sea mayor a 0 y que no excedan los restantes por leer del archivo, tomando el mínimo entre los dos valores y guardándolo en la variable min. Si la cantidad que ya se ha leído del archivo es menor a los disponibles en los bloques de datos del bloque índice, se comienza la lectura desde el bloque indicado en la variable bloque del crFile buscando el puntero en el arreglo de bloque de datos en el struct Index. Si la cantidad leída es mayor, se chequea que exista inidireccionamiento simple, y de existir, se ingresa en el struct Indirect que esta el el struct Index del archivo y se setea el archivo en el en el puntero del bloque que indica el indireccionamiento indirecto. A través de un fread se obtiene el primer bloque de información que se copia en la variable read_aux. La información contenida en esta variable es traspasada de aun byte a un buffer auxiliar de tamaño nbytes. En este for que itera de 0 a mín, se manejan los cambios de bloque ya sea a otro bloque de datos, si el byte dentro del bloque es mayor a 8192 o si se debe acceder al indireccionamiento directo. Una vez finalizada la lectura se copia la información del buffer_aux al buffer ingresado y se actualiza el estado de lectura del archivo para que para el próximo llamado a la función se inicie desde ahí, retornando la cantidad de bytes efectivamente leidos.
**ERRORES:**
1. Si la cantidad de bytes por leer es menor o igual a 0, entonces se retorna 0 y se levanta un error diciendo que el input es invalido.
2. Si el archivo es NULL, se levanta el error de que el archivo no fue abierto correctamente y se retorna 0.
3. Si el modo en que fue abierto es diferente de “r” el programa indicará que hay un error en el modo en que fue abierto y retornará 0.
4. Si la cantidad de bytes que quedan por leer del archivo es 0, entonces el programa también finalizara ya que no se puede seguir leyendo y se levanta un error diciendo que se acabo el archivo.

**int cr_write(crFILE* file_desc, void* buffer, int nbytes):** se recibe el struct crFILE *file* en el que se quiere escribir y un *buffer* que contiene el contenido a escribir. Además se señala la cantidad de bytes a escribir *nbytes*. En primer lugar se chequea que el archivo se haya abierto en modo de escritura. De ser así, se abre el archivo del disco y se determina la cantidad de bloques de datos que se necesitan para escribir los *nbytes* señalados.
**caso 1 escribimos menos (o hasta) 2044 bloques de datos:** se itera hasta la cantidad de bloques necesitados. Dentro de la iteración se llama a la función que busca el primer bloque disponible en el bitmap de la particion donde se ubica el archivo. Si se asigna un bloque, primero guardamos la referencia en nuestro array de bloques de datos del file y luego, se posiciona el puntero en bloque seleccionado y escribimos 8192 bytes, subiendo uno a uno la cantidad de bloques que ocupa el archivo y acumulando el numero de bytes escritos en el ciclo. Se sigue esto hasta que llegamos a uno de dos escenarios: O estamos en el ultimo bloque necesitado pero el resto entre los *nbytes* y la cantidad de bytes por bloque (8192) es mayor a cero, lo que significa que tenemos que escribir resto bytes en el bloque actual o estamos en el ultimo bloque necesitado pero podemos escribir los 8192 bytes. En ambos casos, luego de escribir retornamos la cantidad de bytes acumulados que hemos escrito.
**caso 2 escribimos más de 2044 bloques de datos:** se hace lo del caso anterior para los primeros 2044 bloques, sin embargo siempre se escriben 8192 bytes en cada bloque asignado. Luego, nuevamente llamamos a la funcion para buscar un bloque disponible para asignar el numero de bloque del bloque de indirección simple. Si se asigna el bloque, se realiza nuevamente una iteración hasta la cantidad de bloques necesitados menos los 2044 anteriores y se repite lo que ocurre dentro de la iteración mencionada en el caso anterior con la salvedad que en vez de guardar los punteros a los bloques de datos en el array de bloques de datos del bloque indice se carga el struct Indirect, simbolizando el bloque de indirección, donde se guardan en un array los punteros a los bloques de datos accedidos por indireccionamiento simple.
*Para todos los casos donde se retorna el numero de bytes acumulados, antes se llama a una funcion que se dedica a escribir en el disco la información actualizada del archivo, incluyendo las entradas de directorio y el bitmap de la particion*
**ERRORES:**
1. Si el archivo *file* es de tipo NULL (no se abrió correctamente), se lanza un error que dice que el archivo no es válido y se retorna 0.
2. Si el archivo *file* no fue abierto en modo de escritura, se lanza un error de que no se puede escribir en ese archivo pues el modo no corresponde y se retorna 0.
3. Si la cantidad de bytes a escribir es menor o igual a 0, se lanza un error de que no se puede escribir esa cantidad de bytes ya que el input no es correcto y se retorna 0.
4. Si para cualquiera de los bloques necesitados, no se encuentra algun bloque disponible, se lanza un error diciendo que no hay bloques disponibles en la partición y se retorna la cantidad de bytes que se habia alcanzado a escribir en iteraciones anteriores.
5. Si nos encontramos en el último bloque posible del archivo (2044 + 2048) y queremos seguir escribiendo, se levanta un error ya que estamos en el final del archivo y este no puede seguir creciendo dado que llego a su tamaño máximo permitido y se retorna la cantidad de bytes que se alcanzo a escribir.
**SUPUESTO:** Se asumió que solamente estamos escribiendo en archivos recién creados a través del cr_open (no tienen información anterior que sobreescribir).


**int cr_close(crFILE* file_desc):**

Funcion para cerrar archivos. Cierra el archivo indicado por file desc. Debe garantizar que cuando esta funcion retorna, el archivo se encuentra actualizado en disco.
Lo primero que realizamos es revisar si el archivo que estamos recibiendo existe y no fue ya previamente eliminado. Luego procedemos a liberar la memoria de cada una de sus variables. Si el archivo contiene informacion guardada en su bloque de direccionamiento indirecto simple, este se elimina, procurando liberar la memoria de sus bloques de datos referenciados por los punteros.


**int cr_rm(unsigned disk, char* filename):**
Funcion para borrar archivos. Elimina el archivo referenciado por la ruta path del directorio correspondiente. Los bloques que estaban siendo usados por el archivo quedan libres si no existe ninguna otra referencia al archivo.

**int cr_hardlink(unsigned disk, char* orig, char* dest):**

**int cr_softlink(unsigned disk_orig, unsigned disk_dest, char* orig, char* dest)**

**int cr_unload(unsinged disk, char* orig, char* dest):**
Encargada de copiar un archivo, particion completa o disco a otra direccion.
Uno de los chequeos importantes es que el Input de Disco sea valido. Debe ser un numero entre 0 y 4. Esto nos servira para saber qué y cuántos archivos se desean mover por parte del usuario.

En caso de querer solo un archivo, se debe indicar su nombre, particion especifica en donde podemos encontrarlo y direccion de destino. Para lograr copiar el archivo a otra direccion, crearemos una CrFILE a partir de la direccion de origen, leyendo su contenido. A traves de un buffer se extraera y decantara la informacion a un nuevo archivo en la direccion indicada por dest.

En caso de querer una particion especifica, se debe cumplir:
- direccion de origen sea NULL como direccion de origen,
- disk ∈ {1,...,4}
Esta accion gatillara una inspeccion por todos los bloques activos de la particion, extrayendo toda la informacion de sus archivos al directorio indicado.

En caso de querer todo el disco,
- direccion de origen sea NULL como direccion de origen,
- disk == 0
Esta accion gatillara una inspeccion por todos los bloques activos de la particion, por todo el disco, extrayendo toda la informacion de sus archivos al directorio indicado.

**int cr_load(unsinged disk, char* orig):**

Sirve para introducir archivos nuevos al disco.
Esta función se encarga de copiar un archivo referenciado por orig a la particion designada.

En caso de que un archivo sea demasiado pesado para el disco, se escribe lo mas posible.

Si es una carpeta, se copian todos sus archivos. Si contiene subdirectorios, estos no son tomados en cuenta.

Se recibe una direccion de origen, con la cual verificamos al buscar la existencia del archivo. Leemos la informacion desde la direccion dada y la procesamos para poder convertirla en un struct crFILE *file*. Para esto, leemos el archivo y guardamos su informacion en un *buffer* que contiene el contenido a guardar. Se asegura que el archivo se haya abierto en modo de escritura. En caso de ser carpeta, este proceso se repite para cada uno de sus archivos.

Los archivos tienen un tamano maximo dada la estructura de los bloques. Si esta escribiendo un archivo y este supera ese tamano maximo, no se elimina el archivo, y se deja almacenado el maximo de datos posible y retornar el valor apropiado desde cr_write.

Si se escribe un archivo y ya no queda espacio disponible en el disco virtual, termina la escritura y da aviso de que esta no fue realizada en su totalidad mediante un mensaje de error en stderr y no se elimina el archivo que estaba siendo escrito.
