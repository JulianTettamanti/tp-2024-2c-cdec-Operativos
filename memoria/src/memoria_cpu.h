#ifndef MEMORIA_CPU_H_
#define MEMORIA_CPU_H_
#include <memoria-gestor.h>
#include <memoria_kernel.h>

void attend_memoria_cpu();

void pedido_contexto(t_buffer* un_buffer);
void actualizar_contexto(t_buffer* un_buffer);
void pedido_instruccion (t_buffer* un_buffer);
void pedido_lectura_cm (t_buffer* un_buffer);
void pedido_escritura_cm (t_buffer* un_buffer);
void desalojo_instruccion_cm (t_buffer* un_buffer);

//bool search_pid(t_proceso* proceso);
//t_proceso* search_process_for(int pid);

bool search_tid(t_hilo* un_hilo);
t_hilo* search_thread_for(t_proceso* un_proceso, int tid);

char* get_instruction_from_index(t_hilo* un_hilo, int indice_instruccion);
void reply_to_cpu_instruction_request(char* instruccion);

void read_from_dir(t_buffer* un_buffer);
void write_in_dir(t_buffer* un_buffer);

#endif