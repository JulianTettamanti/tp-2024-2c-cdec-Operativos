#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include <kernel-gestor.h>
#include <pcb.h>
#include <planificador_largo_plazo.h>
#include <planificador_corto_plazo.h>

void process_create_sys(char* nombre_archivo, int tam_proceso, int PRIORIDAD_TID_0);
void process_exit_sys(int PID, int TID);
void thread_create(char* nombre_archivo, int prioridad, int pid);
void thread_join(int TID_BLOQUEADOR, int TID_INVOCADOR, int pid);
void thread_cancel(int TID_A_FINALIZAR, int TID_INVOCADOR, int pid);
void thread_exit(int TID, int pid);
bool __buscar_mutex(t_mutex_thread* void_mutex);
void mutex_create(char* id_mutex, int PID);
void mutex_lock(char* mutex_solicitado, int TID, int PID);
void mutex_unlock(char* mutex_solicitado, int TID, int PID);
//void asignar_mutex_bloqueado(t_mutex_thread* mutex, t_tcb* TCB);
void dump_memory(int TID, int PID);
void syscall_io(t_syscall_io* parametros);
t_mutex_thread* find_mutex(char* ID_mutex, int PID);

void replanificar_cmn(t_tcb* un_tcb);

#endif /* SYSCALLS_H_ */