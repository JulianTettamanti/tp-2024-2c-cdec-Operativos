#ifndef INICIALIZAR_KERNEL_H_
#define INICIALIZAR_KERNEL_H_


#include "inicializar_estructuras.h"
#include "atender_conexiones.h"
#include "planificador_largo_plazo.h"
#include "kernel-gestor.h"
#include <pcb.h>
#include "kernel_cpu_dispatch.h"
#include "kernel_cpu_interrupt.h"

t_log* kernel_logger;
t_log* kernel_log_obligatorio;
t_config* kernel_config;
t_config* proceso_inicial_config;

char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* IP_CPU;
char* PUERTO_CPU_DISPATCH;
char* PUERTO_CPU_INTERRUPT;
t_algoritmo ALGORITMO_PLANIFICACION;
int QUANTUM;
t_log_level LOG_LEVEL;

int socket_kernel;
int socket_cpu_dispatch;
int socket_cpu_interrupt;
int flag_exit = 0; //estaba puesto como extern bool en el gestor, lo cambié aca también
bool flag_pedido_de_memoria; //si es true respuesta positiva, false respuesta negativa
bool flag_respuesta_dump;
int identificador_PID = 0;



// ------ Listas ------
t_list* lista_new;
t_list* lista_ready;
t_list* lista_execute;
t_list* lista_blocked;
t_list* lista_exit;

t_list* lista_new_thread;
t_list*	lista_ready_thread;
t_list*	lista_execute_thread;
t_list*	lista_blocked_thread;
t_list*	lista_exit_thread;
t_list* lista_iniciar_estructura;
t_list* lista_mutex_thread;

t_list* listas_prioridades;
//-------Semaforos--------
sem_t sem_cpu_disponible;
sem_t sem_pcb_nuevo;
sem_t sem_rpta_estructura_inicializada;
sem_t sem_estructura_hilo_inicializada;
sem_t sem_estructura_liberada;
sem_t sem_estructura_hilo_liberada;
sem_t sem_enviar_interrupcion;
sem_t sem_rspta_memoria;
sem_t sem_contexto_listo;

sem_t sem_syscall_process_create_finalizada;
sem_t sem_syscall_thread_create_finalizada;
sem_t sem_syscall_thread_join_no_hace_nada;
sem_t sem_rpta_dump_memory;

//--------Mutex-----------
pthread_mutex_t mutex_pid;
pthread_mutex_t mutex_lista_iniciar_estructura;
pthread_mutex_t mutex_lista_new;
pthread_mutex_t mutex_lista_ready;
pthread_mutex_t mutex_lista_exec;
pthread_mutex_t mutex_lista_blocked;
pthread_mutex_t mutex_lista_exit;
pthread_mutex_t mutex_pcb_a_memoria;
pthread_mutex_t mutex_tid;
pthread_mutex_t mutex_ticket;
pthread_mutex_t mutex_flag_exit;
pthread_mutex_t mutex_flag_pedido_memoria;
pthread_mutex_t mutex_lista_new_thread;
pthread_mutex_t mutex_lista_ready_thread;
pthread_mutex_t mutex_lista_exec_thread;
pthread_mutex_t mutex_lista_blocked_thread;
pthread_mutex_t mutex_lista_exit_thread;
pthread_mutex_t mutex_flag_respuesta_dump;
pthread_mutex_t mutex_syscall_io;
pthread_mutex_t mutex_lista_mutex_thread;


//void attend_kernel_cpu_dispatch();
//void attend_kernel_cpu_interrupt();
void crear_proceso_inicial(int tam_proceso, char* nombre_archivo_pseudocodigo);
#endif /* INICIALIZAR_KERNEL_H_ */