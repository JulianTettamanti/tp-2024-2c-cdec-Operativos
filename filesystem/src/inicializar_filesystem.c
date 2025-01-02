#include <inicializar_filesystem.h>
#include <manejo_archivos.h>


void inicializar_estructuras_fs(){
    
    // ELIMINAR EL DIRECTORIO EN CASO DE QUE YA EXISTA
    struct stat info;
    char* PATH_FILES = malloc(200);
    PATH_BLOQUES = malloc(200);
    PATH_BITMAP = malloc(200);
    sprintf(PATH_FILES, "%s/files", MOUNT_DIR);
    sprintf(PATH_BLOQUES, "%s/bloques.dat", MOUNT_DIR);
    sprintf(PATH_BITMAP, "%s/bitmap.dat", MOUNT_DIR);

    // CREACION DEL DIRECTORIO
    
    if (stat(MOUNT_DIR, &info) == -1) {

        log_info(fs_logger_debug, "El directorio no existe.");
    
        if (mkdir(MOUNT_DIR, 0777) == -1) {
            perror("Error al crear el directorio");
        }
        if (mkdir(PATH_FILES, 0777) == -1) {
            perror("Error al crear el directorio");
        }

        // CREACION DE ARCHIVOS
        
        //INICIALIZACION ARCHIVO DE BLOQUES
        fd_archivoBloques = open(PATH_BLOQUES, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        int tamanio_archivo_bloques = BLOCK_SIZE * BLOCK_COUNT;
        if (ftruncate(fd_archivoBloques, tamanio_archivo_bloques) == -1) {
            perror("Error al ajustar el tamaño del archivo de bloques");
            close(fd_archivoBloques);
        }

        //INICIALIZACION ARCHIVO BITMAP
        fd_bitmap = open(PATH_BITMAP, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (ftruncate(fd_bitmap, BLOCK_COUNT/8) == -1) {
            perror("Error al ajustar el tamaño del archivo de bloques");
            close(fd_bitmap);
        }
    }else{
        log_info(fs_logger_debug, "El directorio ya existe.");
    }
    //CREACION DE LA ESTRUCTURA BITARRAY
    
	bitmap= cargar_bitmap_desde_archivo( PATH_BITMAP,(size_t) (BLOCK_COUNT/8));
    

    if (bitmap == NULL || bitmap->bitarray == NULL) {
    fprintf(stderr, "Error: El bitarray o su buffer no están inicializados\n");
    return -1;
    }

    


   free(PATH_FILES);
}


void eliminar_filesystem(){

    bitarray_destroy(bitmap);
    
}

void remove_files_in_directory(const char* path) {
    DIR *dir;
    struct dirent *entry;
    struct stat entry_info;

    if ((dir = opendir(path)) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

            if (lstat(filepath, &entry_info) == 0) {
                if (S_ISDIR(entry_info.st_mode)) {
                    // Es un directorio: llamada recursiva
                    remove_files_in_directory(filepath);
                    if (rmdir(filepath) != 0) {
                        perror("Error eliminando subdirectorio");
                    }
                } else {
                    // Es un archivo: eliminarlo
                    if (remove(filepath) != 0) {
                        perror("Error eliminando archivo");
                    }
                }
            } else {
                perror("Error obteniendo información de archivo");
            }
        }
        closedir(dir);
    } else {
        perror("Error abriendo directorio");
    }
}