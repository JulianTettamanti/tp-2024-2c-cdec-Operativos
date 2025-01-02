#include <main.h>

// Logs
t_log* cpu_logger;
t_log* cpu_logger_debug;
t_config* cpu_config;
t_log *cpu_log_obligatorio;

// Configs
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* PUERTO_ESCUCHA_DISPATCH;
char* PUERTO_ESCUCHA_INTERRUPT;
t_log_level LOG_LEVEL;

// Sockets
int socket_cpu_dispatch;
int socket_cpu_interrupt;
int socket_memoria;
int socket_kernel_dispatch;
int socket_kernel_interrupt;

// Interrupciones y Desalojo
int desalojar;
int desalojo_mssg;
int interruptFlag; 
int enmascarar_interrupcionFlag = 0;
int syscall_bloquea;

// Contexto
t_contexto* contexto;
t_interrupt* contexto_interrupt;

char* instruccion_actual;

int marco;
int page_size;
int valor_lectura;

// Semaforos y Mutex
pthread_mutex_t mutex_manejo_contexto;
pthread_mutex_t mutex_interruptFlag;

sem_t sem_control_peticion_contexto_memoria;
sem_t sem_control_fetch_decode;
sem_t sem_control_decode_execute;
sem_t sem_control_peticion_marco_a_memoria;
sem_t sem_control_peticion_escritura_a_memoria;
sem_t sem_control_peticion_lectura_a_memoria;
sem_t sem_retorno_syscalls;

//----------------------------------------------- MAIN -----------------------------------------

int main(int argc, char* argv[]) {
    
    cpu_config = config_create("src/cpu.config");
	    if (cpu_config == NULL){
		perror("error en la creación del config");
		exit(EXIT_FAILURE);
	}

    IP_MEMORIA = config_get_string_value(cpu_config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(cpu_config, "PUERTO_MEMORIA");
    PUERTO_ESCUCHA_DISPATCH = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_DISPATCH");
    PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_INTERRUPT");
    char* log_levelchar = config_get_string_value(cpu_config, "LOG_LEVEL");

    LOG_LEVEL = log_level_from_string(log_levelchar);

    cpu_log_obligatorio = log_create("cpu_log_obligatorio.log","[CPU - Log obligatorio]",0,LOG_LEVEL);
    
        if(cpu_log_obligatorio==NULL){
        perror("no se pudo crear el log");
        exit(EXIT_FAILURE);
    }

    cpu_logger = log_create("cpu.log","LOGGER_cpu",0,LOG_LEVEL);
        if(cpu_logger==NULL){
        perror("no se pudo crear el log");
        exit(EXIT_FAILURE);
    }

    cpu_logger_debug = log_create("cpu.log","LOGGER_DEBUG_cpu",0,LOG_LEVEL);
        if(cpu_logger==NULL){
        perror("no se pudo crear el log");
        exit(EXIT_FAILURE);
    }

   

    init_sem();

    //iniciar servidor CPU - DISPATCH
    socket_cpu_dispatch = server_start(PUERTO_ESCUCHA_DISPATCH,cpu_log_obligatorio,"CPU DISPATCH LEVANTADO");

    //iniciar servidor CPU - INTERRUPT
    socket_cpu_interrupt = server_start(PUERTO_ESCUCHA_INTERRUPT,cpu_log_obligatorio,"CPU INTERRUPT LEVANTADO");

    //conectar con MEMORIA como cliente
    socket_memoria = new_connection(IP_MEMORIA,PUERTO_MEMORIA);
    log_info(cpu_logger, "CONEXION CON MEMORIA EXITOSA");

    //esperar conexion del KERNEL - DISPATCH
    log_info(cpu_logger,"ESPERANDO AL KERNEL - DISPATCH");
    socket_kernel_dispatch = wait_client(socket_cpu_dispatch,cpu_log_obligatorio,"SE CONECTO EL KERNEL - DISPATCH");

    //esperar conexion del KERNEL - INTERRUPT
    log_info(cpu_logger,"ESPERANDO AL KERNEL - INTERRUPT");
    socket_kernel_interrupt = wait_client(socket_cpu_interrupt,cpu_log_obligatorio,"SE CONECTO EL KERNEL - INTERRUPT");


    pthread_t hilo_cpu_kernel_dispatch;
    pthread_create(&hilo_cpu_kernel_dispatch,NULL,(void*)attend_cpu_kernel_dispatch,NULL);
    pthread_detach(hilo_cpu_kernel_dispatch);

    //atender los mensajes de kernel - interrupt
    pthread_t hilo_cpu_kernel_interrupt;
    pthread_create(&hilo_cpu_kernel_interrupt,NULL,(void*)attend_cpu_kernel_interrupt,NULL);
    pthread_detach(hilo_cpu_kernel_interrupt);

    //atender los mensajes de memoria
    pthread_t hilo_cpu_memoria;
    pthread_create(&hilo_cpu_memoria,NULL,(void*)attend_cpu_memoria,NULL);
    pthread_join(hilo_cpu_memoria,NULL);

    delete_sem();

    
    return EXIT_SUCCESS;
}

void init_sem(){

    pthread_mutex_init(&mutex_manejo_contexto, NULL);
    pthread_mutex_init(&mutex_interruptFlag, NULL);

    sem_init(&sem_control_peticion_contexto_memoria,0, 0);
    sem_init(&sem_control_fetch_decode,0, 0);
    sem_init(&sem_control_decode_execute,0, 0);
    sem_init(&sem_control_peticion_marco_a_memoria,0, 0);
    sem_init(&sem_control_peticion_escritura_a_memoria,0, 0);
    sem_init(&sem_control_peticion_lectura_a_memoria,0, 0);
    sem_init(&sem_retorno_syscalls,0, 0);
}

void delete_sem(){
    sem_destroy(&sem_control_peticion_contexto_memoria);
    sem_destroy(&sem_control_fetch_decode);
    sem_destroy(&sem_control_decode_execute);
    sem_destroy(&sem_control_peticion_marco_a_memoria);
    sem_destroy(&sem_control_peticion_escritura_a_memoria);
    sem_destroy(&sem_control_peticion_lectura_a_memoria);
    sem_destroy(&sem_retorno_syscalls);
	
	pthread_mutex_destroy(&mutex_manejo_contexto);
    pthread_mutex_destroy(&mutex_interruptFlag);
}