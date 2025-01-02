#include <manejo_archivos.h>


void bitmap_escribir_archivo(int bloque_inical,int bloques_tamanio){
    pthread_mutex_lock(&mutex_manejo_bitmap);
	for(int i=bloque_inical;i< bloque_inical+bloques_tamanio;i++){
			bitarray_set_bit (bitmap,i);
		}
    pthread_mutex_unlock(&mutex_manejo_bitmap);
}


void bitmap_modificar_archivo_bitmap(){
    
    int fd_bitmap = open(PATH_BITMAP, O_WRONLY);

	if (fd_bitmap == -1) {
		perror("Error al abrir el archivo de bitmap para escritura");
		return;
	}
    pthread_mutex_lock(&mutex_manejo_bitmap);
	int resultado = write(fd_bitmap, bitmap->bitarray, bitmap->size);
    pthread_mutex_unlock(&mutex_manejo_bitmap);
	if (resultado == -1) {
		perror("Error al escribir el bitmap en el archivo");
		close(fd_bitmap);
		return;
	}
    fsync(fd_bitmap);

	close(fd_bitmap);
    
}


void blockFile_escribir_archivo(void*datos,size_t posicion, int tamanio_datos, char* nombre_archivo, char* tipo){
	int dataSize = tamanio_datos;
	int tamanio_archivo_bloques = BLOCK_SIZE * BLOCK_COUNT;
	fd_archivoBloques= open(PATH_BLOQUES, O_RDWR | O_CREAT , 0666);
	
    log_info(fs_logger_debug,"abri bloques");

    if (fd_archivoBloques == -1) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd_archivoBloques, tamanio_archivo_bloques) == -1) {
    perror("Error al ajustar el tamaño del archivo");
    close(fd_archivoBloques);
    exit(EXIT_FAILURE);
    }


    // Mapear el archivo en memoria
    void* mappedFile = mmap(NULL, tamanio_archivo_bloques, PROT_WRITE, MAP_SHARED, fd_archivoBloques, 0);
    log_info(fs_logger_debug,"mapie");
    if (mappedFile == MAP_FAILED) {
        perror("Error al mapear el archivo");
        close(fd_archivoBloques);
        exit(EXIT_FAILURE);
    }

    // Escribir los datos en bloques 
    size_t numBlocks = (dataSize + BLOCK_SIZE - 1) / BLOCK_SIZE; // Redondeo hacia arriba
    log_info(fs_logger_debug,"bloques: %d",(int)numBlocks);

    for (size_t i = 0; i < numBlocks; i++) {

        // Determinar cuántos bytes copiar en este bloque
        size_t bytesToCopy = (i == numBlocks - 1) ? dataSize % BLOCK_SIZE : BLOCK_SIZE;
        if (bytesToCopy == 0) bytesToCopy = BLOCK_SIZE; // Para el último bloque si es múltiplo de 64


        if (posicion + i * BLOCK_SIZE + bytesToCopy > tamanio_archivo_bloques) {
            fprintf(stderr, "Error: Escritura fuera de los límites del archivo mapeado\n");
            munmap(mappedFile, tamanio_archivo_bloques);
            close(fd_archivoBloques);
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&mutex_manejo_blockfile);

        memcpy((unsigned char*)mappedFile + posicion + i * BLOCK_SIZE,
               (unsigned char*)datos + i * BLOCK_SIZE,
               bytesToCopy);

        pthread_mutex_unlock(&mutex_manejo_blockfile);

        // Sincronizar los cambios con el archivo físico
        if (msync(mappedFile, tamanio_archivo_bloques, MS_SYNC) == -1) {
            perror("Error al sincronizar el archivo");
            munmap(mappedFile, tamanio_archivo_bloques);
            close(fd_archivoBloques);
            exit(EXIT_FAILURE);
        }
        log_info(fs_logger_obligatorio,"## Acceso Bloque - Archivo: <%s> - Tipo Bloque: <%s> - Bloque File System <%d>",nombre_archivo,tipo,(posicion/BLOCK_SIZE)+ i);

        // Retardo
        usleep(RETARDO_ACCESO_BLOQUE*1000);
    }

    // Liberar el mapeo y cerrar el archivo
    if (munmap(mappedFile, tamanio_archivo_bloques) == -1) {
        perror("Error al desmapear el archivo");
    }
    close(fd_archivoBloques);
	}

void blockFile_escribir_archivo_indices(void*datos,size_t posicion, int tamanio_datos, char* nombre_archivo, char* tipo){
	int dataSize = tamanio_datos;
	int tamanio_archivo_bloques = BLOCK_SIZE * BLOCK_COUNT;
	fd_archivoBloques= open(PATH_BLOQUES, O_RDWR | O_CREAT , 0666);
	
    log_info(fs_logger_debug,"abri bloques");

    if (fd_archivoBloques == -1) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd_archivoBloques, tamanio_archivo_bloques) == -1) {
    perror("Error al ajustar el tamaño del archivo");
    close(fd_archivoBloques);
    exit(EXIT_FAILURE);
    }


    // Mapear el archivo en memoria
    void* mappedFile = mmap(NULL, tamanio_archivo_bloques, PROT_WRITE, MAP_SHARED, fd_archivoBloques, 0);
    log_info(fs_logger_debug,"mapie");
    if (mappedFile == MAP_FAILED) {
        perror("Error al mapear el archivo");
        close(fd_archivoBloques);
        exit(EXIT_FAILURE);
    }

    // Escribir los datos en bloques 
    size_t numBlocks = (dataSize + BLOCK_SIZE - 1) / BLOCK_SIZE; // Redondeo hacia arriba
    log_info(fs_logger_debug,"bloques: %d",(int)numBlocks);

    for (size_t i = 0; i < numBlocks; i++) {

        // Determinar cuántos bytes copiar en este bloque
        size_t bytesToCopy = (i == numBlocks - 1) ? dataSize % BLOCK_SIZE : BLOCK_SIZE;
        if (bytesToCopy == 0) bytesToCopy = BLOCK_SIZE; // Para el último bloque si es múltiplo de 64


        if (posicion + i * BLOCK_SIZE + bytesToCopy > tamanio_archivo_bloques) {
            fprintf(stderr, "Error: Escritura fuera de los límites del archivo mapeado\n");
            munmap(mappedFile, tamanio_archivo_bloques);
            close(fd_archivoBloques);
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&mutex_manejo_blockfile);

        memcpy((void*)mappedFile + posicion + i * BLOCK_SIZE,
               (void*)datos + i * BLOCK_SIZE,
               bytesToCopy);

        pthread_mutex_unlock(&mutex_manejo_blockfile);

        // Sincronizar los cambios con el archivo físico
        if (msync(mappedFile, tamanio_archivo_bloques, MS_SYNC) == -1) {
            perror("Error al sincronizar el archivo");
            munmap(mappedFile, tamanio_archivo_bloques);
            close(fd_archivoBloques);
            exit(EXIT_FAILURE);
        }
        log_info(fs_logger_obligatorio,"## Acceso Bloque - Archivo: <%s> - Tipo Bloque: <%s> - Bloque File System <%d>",nombre_archivo,tipo,(posicion/BLOCK_SIZE) + i);

        // Retardo
        usleep(RETARDO_ACCESO_BLOQUE*1000);
    }

    // Liberar el mapeo y cerrar el archivo
    if (munmap(mappedFile, tamanio_archivo_bloques) == -1) {
        perror("Error al desmapear el archivo");
    }
    close(fd_archivoBloques);
	}


bool hay_espacio_en_bitmap(int cant_bloques){
	for (int i=0; i<BLOCK_COUNT;i++){
		if (!bitarray_test_bit(bitmap,i)){
			cant_bloques--;
		}
		if (cant_bloques == 0){
			return true;
		}
	}
    log_info(fs_logger_debug,"no HAY ESPACIO");
	return false;
}


int bitmap_obtener_bloques_libres(int cantidad,char* nombre_archivo) {
    if (bitmap == NULL) {
        fprintf(stderr, "Error: Bitmap no inicializado\n");
        return -1;
    }

    pthread_mutex_lock(&mutex_manejo_bitmap);

    int bloque_libre = -1;
    int bloques_consecutivos = 0;

    for (int i = 0; i < BLOCK_COUNT; ++i) {
        if (i >= bitmap->size * 8) {
            fprintf(stderr, "Error: Índice de bit fuera de rango\n");
            return -1;
        }
        if (!bitarray_test_bit(bitmap, i)) {
            if (bloques_consecutivos == 0) {
                bloque_libre = i;
            }
            bloques_consecutivos++;
            if (bloques_consecutivos == cantidad) {
                pthread_mutex_unlock(&mutex_manejo_bitmap);
                for(int j=0;j<cantidad;j++){
                    log_info(fs_logger_obligatorio,"## Bloque asignado: <%d> - Archivo: <%s> - Bloques Libres: <%d>",bloque_libre+j,nombre_archivo,BLOCK_COUNT-bloque_libre-j);
                }
                return bloque_libre;
            }
        } else {
            bloques_consecutivos = 0;
            bloque_libre = -1;
        }
    }

    

    pthread_mutex_unlock(&mutex_manejo_bitmap);
    return -1;
}

char* metadata_crear_archivo(int base_bloque, int size_archivo, char *nombre_archivo){
	char PATH_META[50] ;
	sprintf(PATH_META, "%s/files/%s", MOUNT_DIR, nombre_archivo);

	int fd_archivo = open(PATH_META, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd_archivo == -1) {
    	perror("Error al crear el archivo");
    	//return;
	}

    log_info(fs_logger_obligatorio,"## Archivo Creado: <%s> - Tamaño: <%d>",nombre_archivo,size_archivo);
    
    log_info(fs_logger_debug,"size: %d",size_archivo);
	char* linea_tamanio_archivo = string_new();
	sprintf(linea_tamanio_archivo, "SIZE=%d\n", size_archivo);
    if (write(fd_archivo, linea_tamanio_archivo, strlen(linea_tamanio_archivo)) == -1) {
        perror("Error al escribir TAMANIO_ARCHIVO en el archivo");
        close(fd_archivo);
        //return;
    }
    log_info(fs_logger_debug,"index: %d",base_bloque);
	char* linea_bloque_inicial = string_new();
    sprintf(linea_bloque_inicial, "INDEX_BLOCK=%d\n", base_bloque);
    if (write(fd_archivo, linea_bloque_inicial, strlen(linea_bloque_inicial)) == -1) {
        perror("Error al escribir BLOQUE_INICIAL en el archivo");
        close(fd_archivo);
        //return;
    }
    free(linea_tamanio_archivo);
    free(linea_bloque_inicial);
	return PATH_META;
}

t_bitarray* cargar_bitmap_desde_archivo(char* ruta_archivo, size_t tamanio_bitmap) {
    if (ruta_archivo == NULL) {
        fprintf(stderr, "La ruta al archivo es NULL\n");
        return NULL;
    }
    log_info(fs_logger_debug,"escribi bitmap en %s",ruta_archivo);

    int fd_bitmap = open(ruta_archivo, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd_bitmap == -1) {
        perror("Error al abrir el archivo del bitmap");
        return NULL;
    }

    unsigned char* buffer = malloc(tamanio_bitmap);
    if (buffer == NULL) {
        perror("Error al reservar memoria para el buffer");
        close(fd_bitmap);
        return NULL;
    }

    ssize_t bytes_leidos = read(fd_bitmap, buffer, tamanio_bitmap);
    if (bytes_leidos < 0) {
        perror("Error al leer el archivo del bitmap");
        free(buffer);
        close(fd_bitmap);
        return NULL;
    }

    if (bytes_leidos < tamanio_bitmap) {
        memset(buffer + bytes_leidos, 0, tamanio_bitmap - bytes_leidos);
    }
    log_info(fs_logger_debug,"buffer: %d",buffer);
    t_bitarray* bitarray = bitarray_create_with_mode(buffer, tamanio_bitmap, LSB_FIRST);
    if (bitarray == NULL) {
        perror("Error al crear el bitarray");
        free(buffer);
        close(fd_bitmap);
        return NULL;
    }

    close(fd_bitmap);
    return bitarray;
}

void* generar_bloque_index(int base,int cantidad_bloques){
    void* indices_acumulados = malloc(1024);

    
 //   indices_acumulados[0] = '\0'; // Inicializar como cadena vacía

    for (int i = 0; i < cantidad_bloques; i++) {

        uint32_t valor = (base + (i + 1)) * BLOCK_SIZE;
        log_info(fs_logger_debug,"puntero: %u",valor);
        log_info(fs_logger_debug,"Valor con i <%d>: %d",i,(base + (i + 1) )* BLOCK_SIZE);
        // Calculamos el índice como una cadena
    //    char indice_actual[32]; // Buffer temporal para el índice
      //  snprintf(indice_actual, sizeof(indice_actual), "%d", base + ((i + 1) * BLOCK_SIZE));
        memcpy((uint32_t*)indices_acumulados + i * sizeof(uint32_t), &valor, sizeof(uint32_t));
        // Verificar si hay espacio suficiente para concatenar
        
     /**  if (strlen(indices_acumulados) + strlen(indice_actual) + 1 >= 1024) {
            fprintf(stderr, "Error: Buffer insuficiente para concatenar\n");
            free(indices_acumulados);
            return NULL;
        
        }*/

        // Concatenar el índice actual al buffer acumulado
      //  strcat(indices_acumulados, indice_actual);

        // Añadir un separador si no es el último
     /*   if (i < cantidad_bloques - 1) {
            strcat(indices_acumulados, ",");
        }*/
    }

    return indices_acumulados;
}
