#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<math.h>
#include<fcntl.h>
#include<sys/mman.h>
#include <sys/stat.h>

#include<commons/log.h>
#include<commons/config.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<commons/temporal.h>
#include<semaphore.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<pthread.h>
#include<semaphore.h>

//ESTRUCTURAS
typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef enum
{
	MENSAJE,
	PAQUETE,
	HANDSHAKE,
	RTA_HANDSHAKE,
	PACKG,
	//Kernel - Memoria
	CREACION_PROCESO_KM,
	INICIALIZAR_ESTRUCTURAS_KM,
	CREACION_HILO_KM,
	FINALIZACION_PROCESO_KM,
	DUMP_MEMORY_KM,
	//MEMORIA - KERNEL

	RTA_INICIALIZAR_ESTRUCTURAS_MK,
	RTA_CREAR_HILO_SYS_KM,
	//RTA_FINALIZACION_PROCESO,
	//LIBERAR_ESTRUCTURA_KM,
	FINALIZACION_HILO_KM,
	FINALIZAR_ESTRUCTURAS_KM,
	RTA_LIBERAR_ESTRUCTURA_KM,
	RTA_DUMP_MEMORY_MK,
	
	//Kernel - CPU
	FINALIZACION_SYSCALL_KC,
	EJECUTAR_PROCESO, //no va en realidad
	DESALOJO_POR_QUAMTUN_KC,
	RESPUESTA_INSTRUCCION_KC,
	EJECUTAR_HILO_KC,
	RTA_INSTRUCCION,
	//Kernel - IO
	ASIGNAR_NOMBRE,
	IO_GEN_SLEEP,
	IO_STDIN_READ,
	IO_STDOUT_WRITE,
	IO_FS_CREATE,
	IO_FS_DELETE,
	IO_FS_TRUNCATE,
	IO_FS_WRITE,
	IO_FS_READ,
	RTA_IO_GEN_SLEEP,
	RTA_IO_STDIN_READ,
	RTA_IO_STDOUT_WRITE,
	RTA_IO_FS_CREATE,
	RTA_IO_FS_DELETE,
	RTA_IO_FS_TRUNCATE,
	RTA_IO_FS_WRITE,
	RTA_IO_FS_READ,
	//CPU-Kernel
	UNIDAD_CPU_EJECUTADA_CK,
	DESALOJO_INTR,
	DESALOJO_CONSOLA,
	DESALOJO_QUAMTUN_CK,
	DESALOJO_SYSCALL_CK,
	DUMP_MEMORY_CK,
	ERROR_EXC,
	ERROR_SEGFAULT_CK,
	THREAD_CANCEL_CK,
	THREAD_JOIN_CK,
	THREAD_CREATE_CK,
	PROCESS_CREATE_CK,
	MUTEX_CREATE_CK,
	MUTEX_UNLOCK_CK,
	MUTEX_LOCK_CK,
	PROCESS_EXIT_CK,
	IO_CK,
	THREAD_EXIT_CK,
	CONTEXTO_LISTO_CK,
	//CPU - Memoria
	PEDIDO_INSTRUCCION,
	PEDIDO_CONTEXTO_CM,
	ACTUALIZAR_CONTEXTO_CM,
	CONSULTAR_PAGINA_CM,
	PEDIDO_PAGINA_CM,
	PEDIDO_LECTURA_CM,
	PEDIDO_ESCRITURA_CM,
	PEDIDO_RESIZE_CM,
	ACTUALIZAR_TAM_PAG,
	DESALOJO_INSTRUCCION_CM,
	DESALOJO_ERROR_CM,
	//Memoria - CPU
	RESPUESTA_INSTRUCCION,
	RESPUESTA_CONTEXTO_CM,
	RTA_ACTUALIZAR_CONTEXTO_CM,
	RTA_CONSULTAR_PAGINA_CM,
	RTA_PEDIDO_ESCRITURA_CM,
	RTA_PEDIDO_LECTURA_CM,
	//IO - Memoria
	PEDIDO_LECTURA_MIO,
	PEDIDO_ESCRITURA_MIO,
	PEDIDO_LECTURA_FS_MIO,
	PEDIDO_ESCRITURA_FS_MIO,
	//Memoria - IO
	DUMP_MEMORY_MF,
	RTA_PEDIDO_ESCRITURA_MIO,
	RTA_PEDIDO_LECTURA_FS_MIO,
	RTA_PEDIDO_ESCRITURA_FS_MIO,

	//FS - MEMORIA
	RTA_DUMP_MEMORY_FM,
	SIN_ESPACIO_FM
}op_code;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;


//DECLARACIONES
/**
* @brief Imprime un saludo por consola
* @param quien Módulo desde donde se llama a la función
* @return No devuelve nada
*/
void saludar(char* quien);
int new_connection(char *ip, char* puerto);
int server_start(char* PUERTO, t_log* logger, const char* mensaje);
int wait_client(int socket_servidor, t_log* logger, char* mensajeCliente);
int recv_op(int socket_cliente);
void send_msg(char* mensaje, int socket_cliente);

// BUFFER


t_buffer* recv_buffer(int socket_cliente);
void add_to_buffer(t_buffer* unBuffer, void* new_stream, int new_size);
void add_int_to_buffer(t_buffer* unBuffer, int int_value);
void add_uint32_to_buffer(t_buffer* unBuffer, uint32_t uint32_value);
void add_string_to_buffer(t_buffer* unBuffer, char* string_value);
void* extract_from_buffer(t_buffer* unBuffer);
int extract_int_from_buffer(t_buffer* unBuffer);
char* extract_string_from_buffer(t_buffer* unBuffer);
uint32_t extract_uint32_from_buffer(t_buffer* unBuffer);

// PACKAGE

void addto_pckg(t_paquete* paquete, void* valor, int tamanio);


void* pckg_serialize(t_paquete* paquete, int bytes);
void add_int_to_super_pck(t_paquete* paquete, int numero);
void add_string_to_super_pck(t_paquete* paquete, char* string);
void add_choclo_to_super_pck(t_paquete* paquete, void* choclo, int size);
void* rcv_choclo_from_buffer(t_buffer* coso);

//--------------Funciones más usadas----------------
t_buffer* new_buffer();
t_paquete* new_pckg(void);
t_paquete* create_super_pck(op_code code, t_buffer* unBuffer);
void send_pckg(t_paquete* paquete, int socket_cliente);
void delete_pckg(t_paquete* paquete);
//--------------Funciones más usadas----------------

void freeconnection(int socket_cliente);
void exec_in_detach(void (*f)(void*) ,void* struct_arg);
void exec_in_join(void (*f)(void*) ,void* struct_arg);


//--
void ordenar_lista(t_list *lista, bool (*comparador)(void *, void *));
bool comparador_menorAmayor(void *a, void *b);

#endif
