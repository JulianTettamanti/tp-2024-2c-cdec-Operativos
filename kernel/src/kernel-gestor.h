#ifndef KERNEL_GESTOR_H_
#define KERNEL_GESTOR_H_
#include <../../utils/src/utils.h>

typedef enum{
	FIFO,
	PRIORIDADES,
	CMN
}t_algoritmo;


typedef enum{
	NEW_PROCCES,//=0
	READY_PROCCES,//=1
	EXEC_PROCCES,//=2
	BLOCKED_PROCCES,//=3
	EXIT_PROCCES//=4
}est_pcb;

typedef enum{
	NEW_THREAD,//=0
	READY_THREAD,//=1
	EXEC_THREAD,//=2
	BLOCKED_THREAD,//=3
	EXIT_THREAD//=4
}est_tcb;
typedef enum{
	SUCCESS,  // 0
	INVALID_RESOURCE,  // 1
	INVALID_INTERFACE, // 2
    OUT_OF_MEMORY, // 3
    INTERRUPTED_BY_USER,  //4
	SEGMENTATION_FAULT, // 5
	NOTHING,// 6
	ERROR_EXECUTE
}t_motivo_exit;

typedef enum{
	THREAD_JOIN, // 0
 	MUTEX, // 1
	IO,  // 2
	SYSCALL, // 3
	NOTHINGG //4
}t_motivo_block;

typedef struct {
	int mutex;
	int thread;
	int pid_asoc;
	char* ID_mutex;
	t_list* lista_bloqueados;
}t_mutex_thread;

typedef struct{
int tid;
int pid;
int miliseg;
}t_syscall_io;
typedef struct{
	int pid;
	//uint32_t PC;
	int size;
	char* path;
	est_pcb estado;
	int quantum;
	int prioridad_hilo_main;
	t_motivo_exit motivo_exit;
	t_motivo_block motivo_block;   // Para DEADLOCKS
	t_list* lista_tid;
 	t_list* lista_mutex;
	int identificador_TID; //variable utilizada para enumerar los tid de un proceso
	//t_mutex_thread mutex;

}t_pcb;

typedef struct {
	int tid;
	int prioridad;
	int pid;
	int ticket;
	char* path;
	t_motivo_exit motivo_exit;
	t_motivo_block motivo_block; 
	est_tcb estado;
	t_list* lista_mutex_thread;
	t_list* lista_blocked_thread; //lista de hilos bloqueados por este hilo
}t_tcb;


typedef struct {
	int tid;
	int pid;
}t_tcb_aux;

typedef struct{
	t_list* lista_ready;
	int prioridad;
}t_lista_ready_prioridad;


extern t_log* kernel_logger;
extern t_log* kernel_log_obligatorio;
extern t_config* kernel_config;


extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* IP_CPU;
extern char* PUERTO_CPU_DISPATCH;
extern char* PUERTO_CPU_INTERRUPT;
extern t_algoritmo ALGORITMO_PLANIFICACION;
extern int QUANTUM;
extern t_log_level LOG_LEVEL;

extern int socket_kernel;
extern int socket_cpu_dispatch;
extern int socket_cpu_interrupt;
extern int socket_memoria;

extern int identificador_PID;
extern int var_ticket;

extern int flag_exit;
extern bool flag_pedido_de_memoria;
extern bool flag_respuesta_dump;


extern sem_t sem_cpu_disponible;
extern sem_t sem_pcb_nuevo;
extern sem_t sem_rpta_estructura_inicializada;
extern sem_t sem_estructura_hilo_inicializada;
extern sem_t sem_estructura_liberada;
extern sem_t sem_estructura_hilo_liberada;
extern sem_t sem_enviar_interrupcion;
extern sem_t sem_rspta_memoria;
extern sem_t sem_contexto_listo;

extern sem_t sem_syscall_process_create_finalizada;
extern sem_t sem_syscall_thread_create_finalizada;
extern sem_t sem_syscall_thread_join_no_hace_nada;
extern sem_t sem_rpta_dump_memory;

// ------ Listas ------
extern t_list* lista_new;
extern t_list* lista_ready;
extern t_list* lista_execute;
extern t_list* lista_blocked;
extern t_list* lista_exit;
extern t_list* lista_new_thread;
extern t_list* lista_ready_thread;
extern t_list* lista_execute_thread;
extern t_list* lista_blocked_thread;
extern t_list* lista_exit_thread;
extern t_list* lista_mutex_thread;
extern t_list* lista_iniciar_estructura;

extern t_list* listas_prioridades;
//--------Mutex-----------
extern pthread_mutex_t mutex_pid;
extern pthread_mutex_t mutex_lista_new;
extern pthread_mutex_t mutex_lista_ready;
extern pthread_mutex_t mutex_lista_readyplus;
extern pthread_mutex_t mutex_lista_exec;
extern pthread_mutex_t mutex_lista_blocked;
extern pthread_mutex_t mutex_lista_exit;
extern pthread_mutex_t mutex_pcb_a_memoria;
extern pthread_mutex_t mutex_tid;
extern pthread_mutex_t mutex_ticket;
extern pthread_mutex_t mutex_flag_exit;
extern pthread_mutex_t mutex_flag_pedido_memoria;
extern pthread_mutex_t mutex_lista_new_thread;
extern pthread_mutex_t mutex_lista_ready_thread;
extern pthread_mutex_t mutex_lista_exec_thread;
extern pthread_mutex_t mutex_lista_blocked_thread;
extern pthread_mutex_t mutex_lista_exit_thread;
extern pthread_mutex_t mutex_lista_mutex_thread;

extern pthread_mutex_t mutex_lista_iniciar_estructura;
extern pthread_mutex_t mutex_flag_respuesta_dump;
extern pthread_mutex_t mutex_syscall_io; //la idea de este mutex es que solo uno pueda estar haciendo io a la vez

#endif /* K_GESTOR_H_ */