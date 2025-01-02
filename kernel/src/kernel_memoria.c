#include <kernel_memoria.h>

void attend_kernel_memoria(int socket_memoria){
    bool control_key = 1;
	t_buffer* un_buffer;
	int respuesta; // 1 o 0          
		int cod_op = recv_op(socket_memoria);
		switch (cod_op) {
		    case MENSAJE:
			    //
			    break;
		    case PAQUETE:
			    //
			    break;
			case RTA_INICIALIZAR_ESTRUCTURAS_MK:
				un_buffer = recv_buffer(socket_memoria);
                respuesta = extract_int_from_buffer(un_buffer);
				
				log_info(kernel_logger, "se recibio respuesta init memo: %d", respuesta);
				if(respuesta == 1){
					pthread_mutex_lock(&mutex_flag_pedido_memoria);
					flag_pedido_de_memoria = true;
					log_info(kernel_logger, "entramos a la respuesta correcta, flag: %d", flag_pedido_de_memoria);
					pthread_mutex_unlock(&mutex_flag_pedido_memoria);
				} else {
					pthread_mutex_lock(&mutex_flag_pedido_memoria);
					flag_pedido_de_memoria = false;
					pthread_mutex_unlock(&mutex_flag_pedido_memoria);
				}
				sem_post(&sem_rpta_estructura_inicializada);
				free(un_buffer);
				//free(un_buffer); // hay que liberar el buffer cada vez?
				break;
			case RTA_CREAR_HILO_SYS_KM:
				un_buffer = recv_buffer(socket_memoria);
				int rta = extract_int_from_buffer(un_buffer);
				
				if(rta != 1){
						log_error(kernel_logger, "Creación de hilo en memoria falló");
					}
				//free(un_buffer);
				free(un_buffer);
				break;

			case RTA_LIBERAR_ESTRUCTURA_KM:
				un_buffer = recv_buffer(socket_memoria);
				respuesta = extract_int_from_buffer(un_buffer);
				
				log_info(kernel_logger, "SE RECIBE RESPUESTA: %d", respuesta);
				if(respuesta == 1){
				sem_post(&sem_estructura_liberada);
				//sem_post(&sem_cpu_disponible);
				} else {
					log_warning(kernel_logger, "No se pudo destruir el hilo");
				}
				//free(un_buffer);
				//free(respuesta);
				free(un_buffer);
				break;

			case RTA_DUMP_MEMORY_MK:
				un_buffer = recv_buffer(socket_memoria);
				respuesta = extract_int_from_buffer(un_buffer);
				
				pthread_mutex_lock(&mutex_flag_respuesta_dump);
				if(respuesta == 1){
					flag_respuesta_dump = true;
				}else{
					flag_respuesta_dump = false;
				}
				pthread_mutex_unlock(&mutex_flag_respuesta_dump);
				sem_post(&sem_rpta_dump_memory);
				free(un_buffer);
				break;

			case FINALIZACION_HILO_KM:
				//free(un_buffer);
				break;

		    case -1:

			    log_error(kernel_logger, "DESCONEXION DE KERNEL - MEMORIA");
			    control_key = 0;
				//free(un_buffer);
                break;
				
		    default:
			    log_warning(kernel_logger,"OPERACION DESCONOCIDA - KERNEL - MEMORIA");
				//free(un_buffer);
			    break;
	}
}