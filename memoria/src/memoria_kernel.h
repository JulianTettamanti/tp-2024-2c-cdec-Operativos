#ifndef MEMORIA_KERNEL_H_
#define MEMORIA_KERNEL_H_
#include <memoria-gestor.h>

void attend_memoria_kernel(int socket_kernel);
void inicializar_estructuras(t_buffer* un_buffer, int socket_kernel);
void finalizar_estructuras(t_buffer* un_buffer,int socket_kernel);

void crear_tid(t_proceso* proceso, char* path, int tid);
t_list* read_file(const char* path_archivo);
void agrupar_espacios_libres(int i);
bool buscar_marco_por_pid(void* a);
bool hay_espacio_libre (int size);
void* _min_space(void* a, void* b);
void* _max_space(void* a, void* b);

t_proceso* create_process(int pid, int size);
void eliminar_proceso(int pid, int socket_kernel);
bool search_pid(t_proceso* proceso);
t_proceso* search_process_for(int pid);
void delete_process_struct(t_proceso* un_proceso);
void destroy_hilo(void* un_hilo);
void destroy_instruction(char* una_instruccion);
void finalizacion_hilo(t_buffer* un_buffer,int socket_kernel);
void asignar_memoria_al_proceso (t_proceso* proceso,int size);

bool hilo_a_remover(void* ptr);
bool tamanio_y_libre (void* particion);
void dumpear_memoria(t_buffer* un_buffer, int socket_kernel);
void conexion_fs(t_paquete* un_paquete, int socket_kernel);

void creacion_hilo(t_buffer* un_buffer,int socket_kernel);

t_buffer* attend_memoria_fs(int socket_fs);


#endif