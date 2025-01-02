
#ifndef MEMORIA_GESTOR_H_
#define MEMORIA_GESTOR_H_

#include "../../utils/src/utils.h"
//#include "../../memoria/src/main.h"

extern t_config* memo_config;
extern t_log* memo_logger;
extern t_log* memo_logger_debug;
extern t_log* memo_logger_obligatorio;

extern int socket_cpu;
extern int socket_memoria;
extern int socket_kernel;
extern int socket_fs;

extern char* PUERTO_ESCUCHA;
extern char* IP_FILESYSTEM;
extern char* PUERTO_FILESYSTEM;
extern int TAM_MEMORIA;
extern char* PATH_INSTRUCCIONES; 
extern int RETARDO_RESPUESTA;
extern char* ESQUEMA;
extern char* ALGORITMO_BUSQUEDA;
extern char** PARTICIONES; 
extern char* LOG_LEVEL;

extern void* espacio_usuario;

typedef struct{
    char* instruccion;
    char* fst_param;
    char* snd_param;
    char* trd_param;
}t_codigo_instruccion;
typedef struct{
    uint32_t TID;
	uint32_t PC;
	uint32_t AX;
	uint32_t BX;
	uint32_t CX;
	uint32_t DX;
	uint32_t EX;
	uint32_t FX;
	uint32_t GX;
	uint32_t HX;
    //char* path_instrucciones; 
    t_list* instrucciones;
}t_hilo;

typedef struct{

    uint32_t pid;
    uint32_t base;
    uint32_t limite;
    t_list* hilos;
}t_proceso;

typedef struct{
    int pid;
    int base;
    bool libre;
    int tamanio;
}t_marco;

// Listas

extern t_list* lista_procesos;
extern t_list* lista_particiones;

// Mutex 

extern pthread_mutex_t mutex_lista_particiones;
extern pthread_mutex_t mutex_memoria_usuario;
extern pthread_mutex_t mutex_lista_procesos;

#endif
