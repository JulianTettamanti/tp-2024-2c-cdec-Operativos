#include <main.h>


t_log* fs_logger;
t_log* fs_logger_debug;
t_log*  fs_logger_obligatorio;
t_config* fs_config;

char* PUERTO_ESCUCHA;
char* MOUNT_DIR;
int BLOCK_SIZE;
int BLOCK_COUNT;
int RETARDO_ACCESO_BLOQUE;
t_log_level LOG_LEVEL;

pthread_mutex_t mutex_manejo_bitmap;
pthread_mutex_t mutex_manejo_blockfile;

int socket_memoria;
int socket_fs;

char* PATH_BLOQUES;
char* PATH_BITMAP;

char* bitmap_memoria;
t_bitarray* bitmap;

int fd_archivoBloques;
int fd_bitmap;

t_list* list_fcb;

int main(int argc, char** argv) {

  fs_config = config_create(argv[1]);
	    if (fs_config == NULL){
		perror("error en la creación del config");
		exit(EXIT_FAILURE);
	}

    PUERTO_ESCUCHA = config_get_string_value(fs_config, "PUERTO_ESCUCHA");
    MOUNT_DIR = config_get_string_value(fs_config, "MOUNT_DIR");
    BLOCK_SIZE = config_get_int_value(fs_config, "BLOCK_SIZE");
    BLOCK_COUNT = config_get_int_value(fs_config, "BLOCK_COUNT");
    RETARDO_ACCESO_BLOQUE = config_get_int_value(fs_config, "RETARDO_ACCESO_BLOQUE");
    char* log_levelchar = config_get_string_value(fs_config, "LOG_LEVEL");

    LOG_LEVEL = log_level_from_string(log_levelchar);

    fs_logger = log_create("fs.log","LOGGER_fs",0,LOG_LEVEL);
        if(fs_logger==NULL){
        perror("no se pudo crear el log");
        exit(EXIT_FAILURE);
    }

    fs_logger_debug = log_create("fs_debug.log","LOGGER_DEBUG_fs",0,LOG_LEVEL);
        if(fs_logger==NULL){
        perror("no se pudo crear el log");
        exit(EXIT_FAILURE);
    }

    fs_logger_obligatorio = log_create("fs_obligatorio.log","LOGGER_OBLIGATORIO_fs",0,LOG_LEVEL);
    if(fs_logger_obligatorio==NULL){
    perror("no se pudo crear el log");
    exit(EXIT_FAILURE);
    }

    //char path[256]; 

	//strcpy(path, "/home/utnso/tp-2024-2c-cdec-/filesystem/");

	//strcat(path, argv[1]);



  

//iniciar estructuras
    inicializar_estructuras_fs();

//iniciar servidor FS
    socket_fs = server_start(PUERTO_ESCUCHA,fs_logger,"FILESYSTEM LEVANTADO");


//atender los mensajes de memoria
  
    pthread_t hilo_fs_memoria;
    pthread_create(&hilo_fs_memoria,NULL,(void*)wait_for_memory_thread,NULL);
    pthread_join(hilo_fs_memoria,NULL);

    eliminar_filesystem();

    return EXIT_SUCCESS;
}



void wait_for_memory_thread(){
    log_info(fs_logger,"ESPERANDO UN HILO");
     log_info(fs_logger_debug,"ESPERANDO UN HILO");
    while (1) {
     pthread_t thread;
     int fd_conexion_ptr;
     fd_conexion_ptr = accept(socket_fs, NULL, NULL);
     log_info(fs_logger_debug,"SE CONECTO MEMORIA");
     pthread_create(&thread, NULL,(void*) attend_fs_memoria, fd_conexion_ptr); //no funciona bien, fijarse ultimo NULL
     pthread_detach(thread);
    }
}


