#include "cpu_kernel_interrupt.h"

void attend_cpu_kernel_interrupt(){
    bool control_key = 1;
    while (control_key) {
		int cod_op = recv_op(socket_kernel_interrupt);
		t_buffer* unBuffer;// ojo porque podria romper
		switch (cod_op) {
			case DESALOJO_POR_QUAMTUN_KC:
				log_info(cpu_log_obligatorio, "## Llega interrupción al puerto Interrupt");
			    unBuffer = recv_buffer(socket_kernel_interrupt);
				interrupt_manager(unBuffer);
				free(unBuffer);
				break;
			case FINALIZACION_SYSCALL_KC:
			    unBuffer = recv_buffer(socket_kernel_interrupt);
				log_info(cpu_logger,"Llego respuesta SYSCALL!");
				atender_respuesta_syscall(unBuffer);
				free(unBuffer);
			    break;
		    case PAQUETE:
			    
			    break;
			
		    case -1:
			    log_error(cpu_logger, "DESCONEXION DE KERNEL - INTERRUPT");
			    control_key = 0;
                break;
		    default:
			    log_warning(cpu_logger,"OPERACION DESCONOCIDA - CPU - INTERRUPT");
			    break;
		}
	}
}

void interrupt_manager(t_buffer* unBuffer){

	int recv_intr_pid = extract_int_from_buffer(unBuffer);
	int recv_intr_tid = extract_int_from_buffer(unBuffer);
	char* recv_name = extract_string_from_buffer(unBuffer);
	log_info(cpu_logger_debug, "Interrupción recibida: %s", recv_name);
	//log_info(cpu_log_obligatorio, "INTERRUPCION RECIBIDA: <PID:%d> <TID:%d> [T:%s]",recv_intr_pid, recv_intr_tid,
	//									recv_name);
	if(contexto != NULL){
		if(!strcmp(recv_name,"DESALOJO_RR") && recv_intr_pid == contexto->proceso_pid && recv_intr_tid == contexto->proceso_tid){
		contexto_interrupt->interrupt_pid = recv_intr_pid;
		contexto_interrupt->interrupt_tid = recv_intr_tid; 
		contexto_interrupt->interrupt_name = recv_name;

		pthread_mutex_lock(&mutex_interruptFlag);
		interruptFlag = true;
		pthread_mutex_unlock(&mutex_interruptFlag);

		log_warning(cpu_logger_debug, "INTERRUPCION RECIBIDA: <PID:%d> <TID:%d> [T:%s]",contexto_interrupt->interrupt_pid, contexto_interrupt->interrupt_tid,
										contexto_interrupt->interrupt_name);
		}else{
			//ignora interrupcion
			log_warning(cpu_logger, "INTERRUPCION RECHAZADA POR MENSAJE ERRONEO");
		}
	}else{
		log_warning(cpu_logger, "INTERRUPCION RECHAZADA PORQUE NO HAY PROCESOS CORRIENDO EN CPU ACTUALMENTE");
		
	}
	//free(recv_name);
}

void atender_respuesta_syscall(t_buffer *unBuffer){

	int rta = extract_int_from_buffer(unBuffer);
	syscall_bloquea = !rta;
	sem_post(&sem_retorno_syscalls);

}