
#ifndef CPU_GESTOR_H_
#define CPU_GESTOR_H_

#include <../../utils/src/utils.h>

//----------------------ESTRUCTURAS----------------------------
typedef struct{
	int proceso_pid;
    int proceso_tid;
	uint32_t PC;
	uint32_t AX;
	uint32_t BX;
	uint32_t CX;
	uint32_t DX;
	uint32_t EX;
	uint32_t FX;
	uint32_t GX;
	uint32_t HX;
	uint32_t BASE;
	uint32_t LIMITE;

}t_contexto;

typedef struct {
	int interrupt_pid;
	int interrupt_tid;
	char* interrupt_name;
}t_interrupt;

// Logs
extern t_log* cpu_logger;
extern t_log* cpu_logger_debug;
extern t_config* cpu_config;
extern t_log *cpu_log_obligatorio;

// Configs
extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* PUERTO_ESCUCHA_DISPATCH;
extern char* PUERTO_ESCUCHA_INTERRUPT;
extern t_log_level LOG_LEVEL;

// Sockets
extern int socket_cpu_dispatch;
extern int socket_cpu_interrupt;
extern int socket_memoria;
extern int socket_kernel_dispatch;
extern int socket_kernel_interrupt;

// Interrupciones y Desalojo
extern int desalojar;
extern int desalojo_mssg;
extern int interruptFlag;
extern int syscall_bloquea;
extern int enmascarar_interrupcionFlag;


// Contexto
extern t_contexto* contexto;
extern t_interrupt* contexto_interrupt;

extern char* instruccion_actual;

extern int marco;
extern int page_size;
extern int valor_lectura;

// Semaforos y Mutex
extern sem_t sem_control_peticion_contexto_memoria;
extern sem_t sem_control_fetch_decode;
extern sem_t sem_control_decode_execute;
extern sem_t sem_control_peticion_marco_a_memoria;
extern sem_t sem_control_peticion_escritura_a_memoria;
extern sem_t sem_control_peticion_lectura_a_memoria;
extern sem_t sem_retorno_syscalls;

extern pthread_mutex_t mutex_manejo_contexto;
extern pthread_mutex_t mutex_interruptFlag;




#endif