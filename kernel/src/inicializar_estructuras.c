#include "inicializar_estructuras.h"

void initialize_logger(){
	kernel_logger = log_create("kernel.log", "[Kernel]", 0, LOG_LEVEL);
	kernel_log_obligatorio = log_create("kernel_log_obligatorio.log", "[Kernel - Log obligatorio]", 0, LOG_LEVEL);
}

void initialize_config(char* config_path){
	
	//log_info(kernel_logger,"La ruta del config es: %s", config_path);
	kernel_config = config_create(config_path);
	    if (kernel_config == NULL){
		perror("error en la creación del config");
		exit(EXIT_FAILURE);
		}
    IP_MEMORIA = config_get_string_value(kernel_config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(kernel_config, "PUERTO_MEMORIA");
    IP_CPU = config_get_string_value(kernel_config, "IP_CPU");
    PUERTO_CPU_DISPATCH = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
    PUERTO_CPU_INTERRUPT = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
    char* algoritmo_planificacion = config_get_string_value(kernel_config, "ALGORITMO_PLANIFICACION");
    QUANTUM = config_get_int_value(kernel_config, "QUANTUM");
    char* log_level_global = config_get_string_value(kernel_config, "LOG_LEVEL");

	initialize_logger();

	
    

    if(strcmp(algoritmo_planificacion, "FIFO") == 0) {
			ALGORITMO_PLANIFICACION = FIFO;
			log_info(kernel_logger,"[Algoritmo de planificación: FIFO]");
	} else if (strcmp(algoritmo_planificacion, "PRIORIDADES") == 0) {
		ALGORITMO_PLANIFICACION = PRIORIDADES;
		log_info(kernel_logger,"[Algoritmo de planificación: PRIORIDADES]");
	} else if (strcmp(algoritmo_planificacion, "CMN") == 0) {
		ALGORITMO_PLANIFICACION = CMN;
			log_info(kernel_logger,"[Algoritmo de planificación: CMN]");
	} else {
		//log_error(kernel_logger, "No se encontro el algoritmo de planificacion de corto plazo");
		perror("error con el algoritmo");
		exit(EXIT_FAILURE);
	}
	
	LOG_LEVEL = log_level_from_string(log_level_global);
}

void initialize_list(){
	//LISTAS PARA LOS PROCESOS
	lista_new = list_create();
	lista_ready = list_create();
	lista_execute = list_create();
	lista_blocked = list_create();
	lista_exit = list_create();
	//LISTAS PARA LOS HILOS
	lista_new_thread = list_create();
	lista_ready_thread = list_create();
	lista_execute_thread = list_create();
	lista_blocked_thread= list_create();
	lista_exit_thread = list_create();
	lista_mutex_thread = list_create();
	

}

void initialize_semaphores(){
	sem_init(&sem_cpu_disponible,0,0 );
	sem_init(&sem_pcb_nuevo,0,0 );
	sem_init(&sem_rpta_estructura_inicializada,0 ,0 );
	sem_init(&sem_estructura_hilo_inicializada, 0, 0);
	sem_init(&sem_estructura_liberada, 0,0);
	sem_init(&sem_estructura_hilo_liberada,0,0);
	sem_init(&sem_enviar_interrupcion,0,0);
	sem_init(&sem_contexto_listo,0,0);
	sem_init(&sem_rspta_memoria,0,0); //este semaforo es para que al recibir una respuesta de memoria,
	                                  //entonce se puede cortar la conexion
	sem_init(&sem_syscall_process_create_finalizada,0,0);
	sem_init(&sem_syscall_thread_create_finalizada,0,0);
	sem_init(&sem_syscall_thread_join_no_hace_nada,0,0);
	sem_init(&sem_rpta_dump_memory,0,0);

}

void initialize_mutex(){

	pthread_mutex_init(&mutex_pid, NULL);
	pthread_mutex_init(&mutex_lista_new, NULL);
	pthread_mutex_init(&mutex_lista_ready, NULL);
	pthread_mutex_init(&mutex_lista_exec, NULL);
	pthread_mutex_init(&mutex_lista_blocked, NULL);
	pthread_mutex_init(&mutex_lista_exit, NULL);
	pthread_mutex_init(&mutex_pcb_a_memoria, NULL);
	pthread_mutex_init(&mutex_lista_new_thread, NULL);
	pthread_mutex_init(&mutex_lista_ready_thread, NULL);
	pthread_mutex_init(&mutex_lista_exec_thread, NULL);
	pthread_mutex_init(&mutex_lista_blocked_thread, NULL);
	pthread_mutex_init(&mutex_lista_exit_thread, NULL);
	pthread_mutex_init(&mutex_lista_iniciar_estructura, NULL);
	pthread_mutex_init(&mutex_flag_respuesta_dump, NULL);
	pthread_mutex_init(&mutex_syscall_io, NULL);
	pthread_mutex_init(&mutex_lista_mutex_thread, NULL);

}

void initialize_kernel(char* config_path){
	
	//initialize_logger();
	initialize_config(config_path);
	initialize_list();
	initialize_semaphores();
	initialize_mutex();
}
