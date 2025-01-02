#include <kernel_cpu_interrupt.h>


void attend_kernel_cpu_interrupt(){

    bool control_key = 1;
	int pid;
	int tid;
	t_buffer* un_buffer;
    while (control_key) {
		int cod_op = recv_op(socket_cpu_interrupt);
		switch (cod_op) {
		    case MENSAJE:
			    //
			    break;
		    case PAQUETE:
			    //
			    break;
			case DESALOJO_QUAMTUN_CK:
				un_buffer = recv_buffer(socket_cpu_interrupt);
				pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);
				
				t_tcb* un_tcb = buscar_y_remover_tcb_por_tid(tid, pid);
				//lo agrego a la lista de ready de hilos
				pthread_mutex_lock(&mutex_lista_ready_thread);
				list_add(lista_ready_thread, un_tcb);
				pthread_mutex_unlock(&mutex_lista_ready_thread);
				cambiar_estado_tcb(un_tcb, READY_THREAD);
				log_info(kernel_log_obligatorio, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", un_tcb->pid, un_tcb->tid);
				log_info(kernel_logger, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", un_tcb->pid, un_tcb->tid);
				/*if(strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0){
					pthread_mutex_lock(&mutex_lista_ready_thread);
					ordenar_lista(lista_ready_thread, comparador_menorAmayor);
					pthread_mutex_unlock(&mutex_lista_ready_thread);
				}*/
				//log_debug(kernel_log_obligatorio, "Voy a replanificar por RR");
				sem_post(&sem_cpu_disponible);
				free(un_buffer);
			    break;
		    case -1:
			    log_error(kernel_logger, "DESCONEXION DE KERNEL - CPU INTERRUPT");
			    control_key = 0;
				//free(un_buffer);
                break;
		    default:
			    log_warning(kernel_logger,"OPERACION DESCONOCIDA - KERNEL - CPU INTERRUPT");
				control_key = 0;
				//free(un_buffer);
			    break;
		}
	}
}


void _gestionar_interrupt(){
	while(1){
		sem_wait(&sem_enviar_interrupcion);
		t_tcb* tcb_execute;
		//pthread_mutex_lock(&mutex_flag_exit);
		//flag_exit = true;
		//pthread_mutex_unlock(&mutex_flag_exit);
		pthread_mutex_lock(&mutex_lista_exec_thread);
		if(!list_is_empty(lista_execute_thread)){
			tcb_execute = list_get(lista_execute_thread, 0);

			_enviar_tcb_a_CPU_por_interrupt(tcb_execute);

			log_info(kernel_logger, "Envio interrupcion por RR");
			log_info(kernel_logger, "Envio interrupción por RR a CPU de TID <%d>, PID <%d>", tcb_execute->tid, tcb_execute->pid);
		} 
		pthread_mutex_unlock(&mutex_lista_exec_thread);
	}
}