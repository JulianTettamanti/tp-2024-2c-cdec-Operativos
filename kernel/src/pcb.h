#ifndef PCB_H_
#define PCB_H_

#include <kernel-gestor.h>
#include <kernel_cpu_dispatch.h>
#include <kernel_cpu_interrupt.h>

char* estado_to_string(int valor_estado);
char* motivo_to_string(t_motivo_exit motivo_exit);
char* algoritmo_to_string(t_algoritmo algoritmo);

int asign_pid();
int asign_tid(int PID);

void bloquear_hilo_syscall_tcb(t_tcb* tcb, char* motivo);
t_pcb* create_pcb(char* path, int tam_proceso, int prioridad_hilo_main);             
t_tcb* create_tcb(int PID_asociado, int prioridad, char* path);
char* lista_pids(t_list* lista_estado, pthread_mutex_t mutex_lista);
void agregar_pcb_lista(t_pcb* pcb, t_list* lista_estado, pthread_mutex_t mutex_lista);
void cambiar_estado(t_pcb* un_pcb, est_pcb nex_state);
void cambiar_estado_tcb(t_tcb* un_tcb, est_tcb nex_state);
void destruir_pcb(t_pcb* un_pcb);
void destruir_tcb(t_tcb* un_tcb);
void _enviar_tcb_a_CPU_por_dispatch(t_tcb* un_tcb);
void _enviar_tcb_a_CPU_por_interrupt(t_tcb* un_tcb);
t_pcb* buscar_pcb_por_pid_en(int un_pid, t_list* lista_estado, pthread_mutex_t mutex_lista);
t_pcb* buscar_y_remover_pcb_por_pid(int un_pid);
t_pcb* buscar_pcb_por_pid(int un_pid);

t_tcb* buscar_tcb_por_tid(int un_tid, int pid);
t_tcb* buscar_y_remover_tcb_por_tid(int un_tid, int pid);
void agregar_tcb_lista(t_tcb* tcb, t_list* lista_estado, pthread_mutex_t mutex_lista);
bool __buscar_pcb(t_pcb* void_pcb);
bool __buscar_tcb(t_tcb* void_tcb);
void bloquear_hilo_syscall(int tid, char* motivo, int pid);
void desbloquear_hilo_finalizacion_syscall(int tid, int pid);
t_list *transformar_lista(t_list *lista_original,void *(*transformar_elemento)(void *));
char* estado_tcb_to_string(est_tcb estado);
void liberar_hilos_bloqueados(t_list *lista_original); 
void send_thread_to_cpu(t_tcb* tcb_aux);

void aviso_finalizacion_syscall(int resp);
#endif