#include <main.h>


char* PUERTO_ESCUCHA;
char* IP_FILESYSTEM;
char* PUERTO_FILESYSTEM;
int TAM_MEMORIA;
char* PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;
char* ESQUEMA;
char* ALGORITMO_BUSQUEDA;
char** PARTICIONES;
char* LOG_LEVEL;





int main(int argc, char* argv[]) {
  /*  
    memo_config = config_create("memoria.config");
	if (memo_config == NULL){
		perror("error en la creación del config");
		exit(EXIT_FAILURE);
	}
    */
   memo_config = config_create(argv[1]);
	if (memo_config == NULL){
		perror("error en la creación del config");
		exit(EXIT_FAILURE);
	}

    PUERTO_ESCUCHA = config_get_string_value(memo_config, "PUERTO_ESCUCHA");
    IP_FILESYSTEM = config_get_string_value(memo_config, "IP_FILESYSTEM");
    PUERTO_FILESYSTEM = config_get_string_value(memo_config, "PUERTO_FILESYSTEM"); 
    TAM_MEMORIA = config_get_int_value(memo_config, "TAM_MEMORIA");
    PATH_INSTRUCCIONES = config_get_string_value(memo_config, "PATH_INSTRUCCIONES");
    RETARDO_RESPUESTA = config_get_int_value(memo_config, "RETARDO_RESPUESTA");
    ESQUEMA = config_get_string_value(memo_config, "ESQUEMA");
    ALGORITMO_BUSQUEDA = config_get_string_value(memo_config, "ALGORITMO_BUSQUEDA");
    PARTICIONES = config_get_array_value(memo_config, "PARTICIONES");
    char* log_level_str = config_get_string_value(memo_config, "LOG_LEVEL");

    t_log_level LOG_LEVEL_OBLIGATORIO = log_level_from_string(log_level_str);

    memo_logger = log_create("memo.log","LOGGER_memo",0,LOG_LEVEL_OBLIGATORIO);
    if(memo_logger==NULL){
        perror("no se pudo crear el log");
        exit(EXIT_FAILURE);
    }

    memo_logger_debug = log_create("memo.log","LOGGER_memo",0,LOG_LEVEL_OBLIGATORIO);
    if(memo_logger_debug==NULL){
        perror("no se pudo crear el log");
        exit(EXIT_FAILURE);
    }

    memo_logger_obligatorio = log_create("memo_obligatorio.log","LOGGER_memo",0,LOG_LEVEL_OBLIGATORIO);
    if(memo_logger_debug==NULL){
        perror("no se pudo crear el log");
        exit(EXIT_FAILURE);
    }



    iniciar_memoria();
    //iniciar servidor MEMORIA  
    socket_memoria = server_start(PUERTO_ESCUCHA,memo_logger_debug,"MEMORIA LEVANTADA");

    //conectar con FILESYSTEM como cliente
//    socket_fs = new_connection(IP_FILESYSTEM, PUERTO_FILESYSTEM);
//    log_info(memo_logger, "CONEXION CON FILESYSTEM EXITOSA");

    //esperar conexion de CPU
    log_info(memo_logger,"ESPERANDO A LA CPU");
    //log_info(memo_logger_debug,"ESPERANDO A LA CPU");
    socket_cpu = wait_client(socket_memoria,memo_logger,"SE CONECTO LA CPU");


    //esperar conexion de Kernel
    log_info(memo_logger,"ESPERANDO AL KERNEL");
    pthread_t hilo_memoria_cpu;
    pthread_create(&hilo_memoria_cpu,NULL,(void*)attend_memoria_cpu,NULL);
    pthread_detach(hilo_memoria_cpu);
    //socket_kernel = wait_client(socket_memoria,memo_logger,"SE CONECTO KERNEL");


    pthread_t hilo_memoria_kernel;
    pthread_create(&hilo_memoria_kernel,NULL,(void*)wait_for_kernel,NULL);
    pthread_join(hilo_memoria_kernel,NULL);

   /* //esperar conexion de FS
    pthread_t hilo_memoria_fs;
    pthread_create(&hilo_memoria_fs,NULL,(void*)attend_memoria_fs,NULL);
    pthread_join(hilo_memoria_fs,NULL);
*/

}

void wait_for_kernel(){
    log_info(memo_logger,"ESPERANDO COMUNICACION KERNEL");
    //log_info(memo_logger_debug,"ESPERANDO COMUNICACION KERNEL");
    while (1) {
        pthread_t thread;
        //int *fd_conexion_ptr = malloc(sizeof(int));

        int fd_conexion = accept(socket_memoria, NULL, NULL);
        log_info(memo_logger,"## Kernel Conectado - FD del socket: <%d>", fd_conexion);
        log_info(memo_logger_obligatorio ,"## Kernel Conectado - FD del socket: <%d>", fd_conexion);
        
        pthread_create(&thread,
                    NULL,
                    (void*) attend_memoria_kernel,
                    fd_conexion);
        pthread_detach(thread);
    }

    
}