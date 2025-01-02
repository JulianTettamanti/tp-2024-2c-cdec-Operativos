#include "kernel_cpu_dispatch.h"
void attend_kernel_cpu_dispatch(){

    bool control_key = 1;
	t_buffer* un_buffer;
	int pid;
	int tid;
	char* nombre_archivo;
	char* mutex_solicitado;
	int tam_proceso;
	int prioridad_tid;
	int prioridad_tid_0;
	//t_tcb* un_tcb = NULL;
	t_pcb* un_pcb = NULL;
	//t_list* lista_aux;
	//t_list_iterator* iterator;
	
    while (control_key) {
		int cod_op = recv_op(socket_cpu_dispatch);
		switch (cod_op) {
		    case MENSAJE:
			    //
			    break;
		    case PAQUETE:
			    //
			    break;
			case RTA_INSTRUCCION:
			    //
			    break;
			case CONTEXTO_LISTO_CK:
				//free(un_buffer);
			//	un_buffer = recv_buffer(socket_cpu_dispatch);
				//sem_post(&sem_contexto_listo);
			    //rellenar con semaforos
			    break;
			case DESALOJO_SYSCALL_CK:
				log_info(kernel_logger,"voy a replanificar por syscall");
				log_debug(kernel_logger, "Voy a replanificar por syscall");
				sem_post(&sem_cpu_disponible);
				break;
			case PROCESS_CREATE_CK:
				un_buffer = recv_buffer(socket_cpu_dispatch);
				pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);
				nombre_archivo = extract_string_from_buffer(un_buffer);
				tam_proceso = extract_int_from_buffer(un_buffer);
				prioridad_tid_0 = extract_int_from_buffer(un_buffer);

				log_info(kernel_log_obligatorio, "Syscall recibida: ## (%d:%d) - Solicitó syscall: PROCESS CREATE", pid, tid);
				log_info(kernel_logger, "Syscall recibida: ## (%d:%d) - Solicitó syscall: PROCESS CREATE", pid, tid);
			    process_create_sys(nombre_archivo, tam_proceso, prioridad_tid_0);
				aviso_finalizacion_syscall(1);
				free(nombre_archivo);
				free(un_buffer);
				//un_tcb = buscar_tcb_por_tid(tid, pid);
				//pthread_mutex_lock(&mutex_flag_exit);
					//if(strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0 && flag_exit == 1){		
						/*un_tcb = buscar_y_remover_tcb_por_tid(tid, pid);
						//lo agrego a la lista de ready de hilos
						pthread_mutex_lock(&mutex_lista_ready_thread);
						list_add(lista_ready_thread, un_tcb);
						pthread_mutex_unlock(&mutex_lista_ready_thread);
						cambiar_estado_tcb(un_tcb, READY_THREAD);
						log_info(kernel_log_obligatorio, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", un_tcb->pid, un_tcb->tid);
						log_info(kernel_logger, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", un_tcb->pid, un_tcb->tid);
						sem_post(&sem_cpu_disponible);*/
						/*un_tcb = buscar_y_remover_tcb_por_tid(tid, pid);
						replanificar_cmn(un_tcb); 
						//flag_exit = 0;
					}else{
						_enviar_tcb_a_CPU_por_dispatch(un_tcb);
					}
				pthread_mutex_unlock(&mutex_flag_exit);*/
			    break;
			case PROCESS_EXIT_CK:
				un_buffer = recv_buffer(socket_cpu_dispatch);
				pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);
				log_info(kernel_log_obligatorio, "Syscall recibida: ## (%d:%d) - Solicitó syscall: PROCESS EXIT", pid, tid);
				log_info(kernel_logger, "Syscall recibida: ## (%d:%d) - Solicitó syscall: PROCESS EXIT", pid, tid);
				if(tid == 0){
			    process_exit_sys(pid, tid);
				log_info(kernel_logger, "tengo que enviar respuesta finalizacion syscall");
				aviso_finalizacion_syscall(0);//NO SIGAS EJECUTANDO
				}else{
					log_info(kernel_logger, "No es tid 0 del proceso");
					aviso_finalizacion_syscall(1); //SEGUI EJECUTANDO
					//un_tcb = buscar_tcb_por_tid(tid, pid);
					//pthread_mutex_lock(&mutex_flag_exit);
					//if(strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0 && flag_exit == 1){		
						/*un_tcb = buscar_y_remover_tcb_por_tid(tid, pid);
						//lo agrego a la lista de ready de hilos
						pthread_mutex_lock(&mutex_lista_ready_thread);
						list_add(lista_ready_thread, un_tcb);
						pthread_mutex_unlock(&mutex_lista_ready_thread);
						cambiar_estado_tcb(un_tcb, READY_THREAD);
						log_info(kernel_log_obligatorio, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", un_tcb->pid, un_tcb->tid);
						log_info(kernel_logger, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", un_tcb->pid, un_tcb->tid);
						sem_post(&sem_cpu_disponible);*/
						//un_tcb = buscar_y_remover_tcb_por_tid(tid, pid);
						//replanificar_cmn(un_tcb); 
						//flag_exit = 0;
					//}else{
						//_enviar_tcb_a_CPU_por_dispatch(un_tcb);
					//}
					//pthread_mutex_unlock(&mutex_flag_exit);
					
				}
				free(un_buffer);
			    break;
			case THREAD_CREATE_CK:
				un_buffer = recv_buffer(socket_cpu_dispatch);
				pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);
			    nombre_archivo = extract_string_from_buffer(un_buffer);
				prioridad_tid = extract_int_from_buffer(un_buffer);
				log_info(kernel_log_obligatorio, "Syscall recibida: ## (%d:%d) - Solicitó syscall: THREAD CREATE", pid, tid);
				log_info(kernel_logger,"Vamos a crear el hilo nuevo");
			    thread_create(nombre_archivo, prioridad_tid, pid);
				log_info(kernel_logger,"Hilo nuevo creado");
				aviso_finalizacion_syscall(1);
				free(nombre_archivo);
				free(un_buffer);
				//un_tcb = buscar_tcb_por_tid(tid, pid);
				//pthread_mutex_lock(&mutex_flag_exit);
				//if(strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0 && flag_exit == 1){		
					/*un_tcb = buscar_y_remover_tcb_por_tid(tid, pid);
					//lo agrego a la lista de ready de hilos
					pthread_mutex_lock(&mutex_lista_ready_thread);
					list_add(lista_ready_thread, un_tcb);
					pthread_mutex_unlock(&mutex_lista_ready_thread);
					cambiar_estado_tcb(un_tcb, READY_THREAD);
					log_info(kernel_log_obligatorio, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", un_tcb->pid, un_tcb->tid);
					log_info(kernel_logger, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", un_tcb->pid, un_tcb->tid);
					sem_post(&sem_cpu_disponible);*/
					//un_tcb = buscar_y_remover_tcb_por_tid(tid, pid);
					//replanificar_cmn(un_tcb); 
					//flag_exit = 0;
				//}else{
				//	_enviar_tcb_a_CPU_por_dispatch(un_tcb);
				//}
				//pthread_mutex_unlock(&mutex_flag_exit);
			    break;
			case THREAD_JOIN_CK: //BLOQUEANTE
				un_buffer = recv_buffer(socket_cpu_dispatch);
			    pid = extract_int_from_buffer(un_buffer);
				int tid_invocador = extract_int_from_buffer(un_buffer);
				int tid_bloqueador = extract_int_from_buffer(un_buffer);
				log_info(kernel_logger,"HILO QUE EJECUTO EL JOIN: %d - HILO a JOINEAR: %d", tid_invocador, tid_bloqueador);
				log_info(kernel_log_obligatorio, "Syscall recibida: ## (%d:%d) - Solicitó syscall: THREAD JOIN", pid, tid_invocador);
				thread_join(tid_bloqueador, tid_invocador, pid);
				free(un_buffer);
			    break;
			case THREAD_CANCEL_CK: //LISTA
				un_buffer = recv_buffer(socket_cpu_dispatch);
			    pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);
				int tid_a_finalizar = extract_int_from_buffer(un_buffer);
				log_info(kernel_log_obligatorio, "Syscall recibida: ## (%d:%d) - Solicitó syscall: THREAD CANCEL", pid, tid);
				thread_cancel(tid_a_finalizar, tid, pid);
				aviso_finalizacion_syscall(1);
				free(un_buffer);
			    break;
			case THREAD_EXIT_CK: //LISTA
				un_buffer = recv_buffer(socket_cpu_dispatch);
			    pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);
				log_info(kernel_logger,"Entro a eliminar al [TID: %d] y [PID: %d]", tid, pid);
				log_info(kernel_log_obligatorio, "Syscall recibida: ## (%d:%d) - Solicitó syscall: THREAD EXIT", pid, tid);
				thread_exit(tid, pid);
				aviso_finalizacion_syscall(0); //NO SIGAS EJECUTANDO
				free(un_buffer);
				//sem_post(&sem_cpu_disponible);
			    break;
			case MUTEX_CREATE_CK:
				un_buffer = recv_buffer(socket_cpu_dispatch);
			    pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);
				
				char* nombre_mutex = extract_string_from_buffer(un_buffer);
				log_info(kernel_log_obligatorio, "Syscall recibida: ## (%d:%d) - Solicitó syscall: MUTEX CREATE", pid, tid);
				log_info(kernel_logger, "Syscall recibida: ## (%d:%d) - Solicitó syscall: MUTEX CREATE", pid, tid);
				mutex_create(nombre_mutex, pid);
				//un_tcb = buscar_tcb_por_tid(tid, pid);
				//pthread_mutex_lock(&mutex_flag_exit);
				//if(strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0 && flag_exit == 1){		
					/*un_tcb = buscar_y_remover_tcb_por_tid(tid, pid);
					//lo agrego a la lista de ready de hilos
					pthread_mutex_lock(&mutex_lista_ready_thread);
					list_add(lista_ready_thread, un_tcb);
					pthread_mutex_unlock(&mutex_lista_ready_thread);
					cambiar_estado_tcb(un_tcb, READY_THREAD);
					log_info(kernel_log_obligatorio, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", un_tcb->pid, un_tcb->tid);
					log_info(kernel_logger, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", un_tcb->pid, un_tcb->tid);
					sem_post(&sem_cpu_disponible);*/
					//un_tcb = buscar_y_remover_tcb_por_tid(tid, pid);
					//replanificar_cmn(un_tcb); 
					//flag_exit = 0;
				//}else{
				//	_enviar_tcb_a_CPU_por_dispatch(un_tcb);
				//}
				//pthread_mutex_unlock(&mutex_flag_exit);
				aviso_finalizacion_syscall(1);
				free(nombre_mutex);
				free(un_buffer);
			    break;
			case MUTEX_LOCK_CK://BLOQUEANTE
				un_buffer = recv_buffer(socket_cpu_dispatch);
			    pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);
				mutex_solicitado = extract_string_from_buffer(un_buffer);
	
				log_info(kernel_log_obligatorio, "Syscall recibida: ## (%d:%d) - Solicitó syscall: MUTEX LOCK", pid, tid);
				mutex_lock(mutex_solicitado, tid, pid);
				free(un_buffer);
				free(mutex_solicitado);
			    break;
			case MUTEX_UNLOCK_CK:
				un_buffer = recv_buffer(socket_cpu_dispatch);
			    pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);
				mutex_solicitado = extract_string_from_buffer(un_buffer);

				log_info(kernel_log_obligatorio, "Syscall recibida: ## (%d:%d) - Solicitó syscall: MUTEX UNLOCK", pid, tid);
				mutex_unlock(mutex_solicitado, tid, pid);
				free(mutex_solicitado);
				free(un_buffer);
			    break;
			case DUMP_MEMORY_CK: //syscall bloqueante
				un_buffer = recv_buffer(socket_cpu_dispatch);
			    pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);

				log_info(kernel_log_obligatorio, "Syscall recibida: ## (%d:%d) - Solicitó syscall: DUMP MEMORY", pid, tid);
				bloquear_hilo_syscall(tid, "DUMP_MEMORY", pid);
				aviso_finalizacion_syscall(0);//NO SIGUE EJECUTANDO
				//sem_post(&sem_cpu_disponible);
				dump_memory(tid, pid);
				sem_wait(&sem_rpta_dump_memory);
				pthread_mutex_lock(&mutex_flag_respuesta_dump);
				if(flag_respuesta_dump){
					desbloquear_hilo_finalizacion_syscall(tid, pid);
					//sem_post(&sem_cpu_disponible);
				}else{
					process_exit_sys(pid, tid);
					//sem_post(&sem_cpu_disponible);
				}
				pthread_mutex_unlock(&mutex_flag_respuesta_dump);
				free(un_buffer);
			    break;
			case IO_CK: //BLOQUEANTE
				log_info(kernel_logger, "Entro a hacer IO");
				un_buffer = recv_buffer(socket_cpu_dispatch);
			    pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);
				int milisegundos = extract_int_from_buffer(un_buffer);

				log_info(kernel_log_obligatorio, "Syscall recibida: ## (%d:%d) - Solicitó syscall: IO", pid, tid);
				bloquear_hilo_syscall(tid, "IO", pid);
				aviso_finalizacion_syscall(0); //NO SEGUIS EJECUTANDO
		
				t_syscall_io* parametros = malloc(sizeof(t_syscall_io));//FALTA EL FREE OJO
				parametros->tid = tid;
				parametros->pid = pid;
				parametros->miliseg = milisegundos;
				log_info(kernel_logger, "Mandando a dormir al [TID: %d] por %d milisegundos", parametros->tid, parametros->miliseg);

				exec_in_detach((void*)syscall_io, parametros);

				free(un_buffer);
			    break;
			case ERROR_SEGFAULT_CK: //LISTA
			    un_buffer = recv_buffer(socket_cpu_dispatch);
				pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);

				//bloquear_hilo_syscall(tid, "SEGMENTATION_FAULT", pid);
				log_info(kernel_logger, "Segmentation Fault: ## (%d:%d)", pid, tid);
			    /*un_pcb = NULL;
				un_pcb = buscar_y_remover_pcb_por_pid(pid);
				//lista_aux = transformar_lista(&un_pcb->lista_tid, buscar_y_remover_tcb_por_tid);
				iterator = list_iterator_create(un_pcb->lista_tid);
            	while(list_iterator_has_next(iterator)){
					int tid_aux = list_iterator_next(iterator);
					t_tcb* un_tcb_aux = buscar_y_remover_tcb_por_tid(tid_aux, pid);
					pthread_mutex_lock(&mutex_lista_exit_thread);
					list_add(lista_exit_thread, un_tcb_aux);
					pthread_mutex_unlock(&mutex_lista_exit_thread);	
				}*/
				un_pcb = buscar_pcb_por_pid(pid);
				un_pcb->motivo_exit = SEGMENTATION_FAULT;
				process_exit_sys(pid, tid);
				//finalizar_proceso(pid);
				sem_post(&sem_cpu_disponible);
				free(un_buffer);
			    break;
			case ERROR_EXC:
			 	un_buffer = recv_buffer(socket_cpu_dispatch);
				pid = extract_int_from_buffer(un_buffer);
				tid = extract_int_from_buffer(un_buffer);

				//bloquear_hilo_syscall(tid, "ERROR EXECUTE", pid);
				log_info(kernel_logger, "Error Execute: ## (%d:%d)", pid, tid);
			    /*un_pcb = NULL;
				lista_aux = list_create();
				un_pcb = buscar_y_remover_pcb_por_pid(pid);
				iterator = list_iterator_create(un_pcb->lista_tid);
            	while(list_iterator_has_next(iterator)){
					int tid_aux = list_iterator_next(iterator);
					t_tcb* un_tcb_aux = buscar_y_remover_tcb_por_tid(tid_aux, pid);
					pthread_mutex_lock(&mutex_lista_exit_thread);
					list_add(lista_exit_thread, un_tcb_aux);
					pthread_mutex_unlock(&mutex_lista_exit_thread);	
				}
				un_pcb->motivo_exit = ERROR_EXECUTE; 
				finalizar_proceso(pid);
				sem_post(&sem_cpu_disponible);*/
				un_pcb = buscar_pcb_por_pid(pid);
				un_pcb->motivo_exit = ERROR_EXECUTE;
				process_exit_sys(pid, tid);
				free(un_buffer);
			    break;
		    default:
			    log_warning(kernel_logger,"OPERACION DESCONOCIDA - KERNEL - CPU DISPATCH");
				control_key = 0;
			    break;
		}

	}
	//list_iterator_destroy(iterator);
}


