#ifndef MAIN_H_
#define MAIN_H_

#include <memoria-gestor.h>

t_config* memo_config;
t_log* memo_logger;
t_log* memo_logger_debug;
t_log* memo_logger_debug;
t_log* memo_logger_obligatorio;

int socket_memoria;
int socket_kernel;
int socket_cpu;
int socket_fs;


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

t_list* lista_procesos;
t_list* lista_particiones;

pthread_mutex_t mutex_lista_particiones;
pthread_mutex_t mutex_memoria_usuario;
pthread_mutex_t mutex_lista_procesos;

void attend_memoria_cpu();
void attend_memoria_kernel(int fd_conexion);
//t_buffer*  attend_memoria_fs();
void wait_for_kernel();


#endif