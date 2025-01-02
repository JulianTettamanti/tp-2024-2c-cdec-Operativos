#include "syscalls.h"

//SYSCALLS PROCESOS
void process_create_sys(char* nombre_archivo, int tam_proceso, int PRIORIDAD_TID_0){
	t_pcb* un_pcb = NULL;
	log_info(kernel_logger, "El PATH del proceso es: [%s] y el tamaño del mismo es: [%d] \n", nombre_archivo, tam_proceso);
	un_pcb = create_pcb(nombre_archivo, tam_proceso, PRIORIDAD_TID_0);
	agregar_pcb_lista(un_pcb, lista_new, mutex_lista_new);
	plp();
}

void process_exit_sys(int PID, int TID){
	log_info(kernel_logger, "ENTRO A PROCESS_EXIT_SYS");
    t_pcb* un_pcb = buscar_pcb_por_pid(PID);
	log_info(kernel_logger, "SE ENCONTRO AL PCB: %d", un_pcb->pid);
	t_list_iterator* iterator = list_iterator_create(un_pcb->lista_tid);
	//int* tid_aux = malloc(sizeof(int));  
	int tid_aux;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	t_tcb* un_tcb_aux;

	for(int i =0; i< list_size(un_pcb->lista_tid); i++){
		tid_aux = list_get(un_pcb->lista_tid, i);
		log_info(kernel_logger, "RECIBIENDO TID: %d", tid_aux);
		un_tcb_aux = buscar_y_remover_tcb_por_tid(tid_aux, PID);
		if(un_tcb_aux!=NULL){
			pthread_mutex_lock(&mutex_lista_exit_thread);
			list_add(lista_exit_thread, un_tcb_aux);
			pthread_mutex_unlock(&mutex_lista_exit_thread);	
			cambiar_estado_tcb(un_tcb_aux, EXIT_THREAD);
			log_info(kernel_logger,"Metido al [TID: %d] en exit", tid_aux);
		}
	}
    /*while(list_iterator_has_next(iterator)){
		
		tid_aux = list_iterator_next(iterator);
		//log_info(kernel_logger, "RECIBIENDO TID: %d", tid_aux);
		un_tcb_aux = buscar_y_remover_tcb_por_tid(*tid_aux, PID);
		if(un_tcb_aux!=NULL){
			pthread_mutex_lock(&mutex_lista_exit_thread);
			list_add(lista_exit_thread, un_tcb_aux);
			pthread_mutex_unlock(&mutex_lista_exit_thread);	
			cambiar_estado_tcb(un_tcb_aux, EXIT_THREAD);
			//log_info(kernel_logger,"Metido al [TID: %d] en exit", tid_aux);
		}
		
		//free(tid_aux);
	}*/
	finalizar_proceso(PID);
	//free(tid_aux);
	log_info(kernel_logger, "llegue hasta donde finalizo el proceso");
	
	/*pthread_mutex_lock(&mutex_lista_exec_thread);
	if(list_is_empty(lista_execute_thread)){
		sem_post(&sem_cpu_disponible);
	}
	pthread_mutex_lock(&mutex_lista_exec_thread);*/
	
	list_iterator_destroy(iterator);
	//free(tid_aux);
	log_info(kernel_logger, "paso destroy");
	//free(tid_aux);
}

//SYSCALLS PARA THREAD
void thread_create(char* nombre_archivo, int prioridad, int pid){
	char* ruta2 = malloc(50 * sizeof(char));
	strcpy(ruta2, "../the-last-of-c-pruebas/");
	strcat(ruta2, nombre_archivo);
	crear_hilo(prioridad, pid, ruta2);
}

void thread_join(int TID_BLOQUEADOR, int TID_INVOCADOR, int pid){
	log_info(kernel_logger, "Entrando a thread join, con [TID_INV: %d] [TID_BLOQ: %d]", TID_INVOCADOR, TID_BLOQUEADOR);
	log_info(kernel_logger,"Busco al TCB BLOQUEADOR");
	t_tcb* tcb_bloqueador = buscar_tcb_por_tid(TID_BLOQUEADOR, pid);
	log_info(kernel_logger,"Encontrado TCB BLOQ [TID:%d], BUSCO AL INVOCADOR AHORA, ",tcb_bloqueador->tid);
	//t_tcb* tcb_invocador= buscar_y_remover_tcb_por_tid(TID_INVOCADOR, pid);
	if((tcb_bloqueador == NULL) || strcmp(estado_tcb_to_string(tcb_bloqueador->estado), "EXIT_THREAD") == 0){
		aviso_finalizacion_syscall(1);
		//log_info(kernel_logger, "El TID_BLOQ es NULL o esta en exit");
		//pthread_mutex_lock(&mutex_lista_exec_thread);
		//list_add(lista_execute_thread, tcb_invocador);
		//pthread_mutex_unlock(&mutex_lista_exec_thread);
		//cambiar_estado_tcb(tcb_invocador, EXEC_THREAD);
		//pthread_mutex_lock(&mutex_flag_exit);
		//if(strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0 && flag_exit == 1){		
			/*pthread_mutex_lock(&mutex_lista_ready_thread);
			list_add(lista_ready_thread, tcb_invocador);
			pthread_mutex_unlock(&mutex_lista_ready_thread);
			cambiar_estado_tcb(tcb_invocador, READY_THREAD);
			log_info(kernel_log_obligatorio, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", pid, tcb_invocador->tid);
			log_info(kernel_logger, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", pid, tcb_invocador->tid);
			sem_post(&sem_cpu_disponible);*/
			//replanificar_cmn(tcb_invocador);
			//flag_exit = 0;
		//}else{
			//_enviar_tcb_a_CPU_por_dispatch(tcb_invocador);
		//}
		//pthread_mutex_unlock(&mutex_flag_exit);
		//return;
	}else{
		//podria poner un mutex dentro de cada hilo que bloquee esta lista
		t_tcb* tcb_invocador= buscar_y_remover_tcb_por_tid(TID_INVOCADOR, pid);
		list_add(tcb_bloqueador->lista_blocked_thread, tcb_invocador);
		log_info(kernel_logger,"Voy a bloquear al [TID_INVOCADOR: %d]", TID_INVOCADOR);
		bloquear_hilo_syscall_tcb(tcb_invocador, "THREAD_JOIN"); 
		log_info(kernel_logger,"[TID_INVOCADOR: %d] Bloqueado", TID_INVOCADOR);
		tcb_invocador->motivo_block = THREAD_JOIN;
		aviso_finalizacion_syscall(0);
		//sem_post(&sem_cpu_disponible);
	}
}

void thread_cancel(int TID_A_FINALIZAR, int TID_INVOCADOR, int pid){
	t_tcb* tcb_a_finalizar = buscar_tcb_por_tid(TID_A_FINALIZAR, pid);
	//t_tcb* tcb_invocador = buscar_y_remover_tcb_por_tid(TID_INVOCADOR, pid);
	if((tcb_a_finalizar == NULL) || strcmp(estado_tcb_to_string(tcb_a_finalizar->estado), "EXIT_THREAD") == 0){
		//pthread_mutex_lock(&mutex_lista_exec_thread);
		//list_add(lista_execute_thread, tcb_invocador);
		//pthread_mutex_unlock(&mutex_lista_exec_thread);
		//cambiar_estado_tcb(tcb_invocador, EXEC_THREAD);
        //pthread_mutex_lock(&mutex_flag_exit);
		//if(strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0 && flag_exit == 1){		
			/*pthread_mutex_lock(&mutex_lista_ready_thread);
			list_add(lista_ready_thread, tcb_invocador);
			pthread_mutex_unlock(&mutex_lista_ready_thread);
			cambiar_estado_tcb(tcb_invocador, READY_THREAD);
			log_info(kernel_log_obligatorio, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", pid, tcb_invocador->tid);
			log_info(kernel_logger, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", pid, tcb_invocador->tid);
			sem_post(&sem_cpu_disponible);*/
			//replanificar_cmn(tcb_invocador);
			//flag_exit = 0;
		//} else {
			//_enviar_tcb_a_CPU_por_dispatch(tcb_invocador);
		//}
		//pthread_mutex_unlock(&mutex_flag_exit);
		//return;
	} else {
		finalizar_hilo(TID_A_FINALIZAR, pid);
		//pthread_mutex_lock(&mutex_lista_exec_thread);
		//list_add(lista_execute_thread, tcb_invocador);
		//pthread_mutex_unlock(&mutex_lista_exec_thread);
		//cambiar_estado_tcb(tcb_invocador, EXEC_THREAD);
       // pthread_mutex_lock(&mutex_flag_exit);
		//if(strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0 && flag_exit == 1){		
			/*pthread_mutex_lock(&mutex_lista_ready_thread);
			list_add(lista_ready_thread, tcb_invocador);
			pthread_mutex_unlock(&mutex_lista_ready_thread);
			cambiar_estado_tcb(tcb_invocador, READY_THREAD);
			log_info(kernel_log_obligatorio, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", pid, tcb_invocador->tid);
			log_info(kernel_logger, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", pid, tcb_invocador->tid);
			sem_post(&sem_cpu_disponible);*/
			//replanificar_cmn(tcb_invocador);
			//flag_exit = 0;
		//}else{
		//	_enviar_tcb_a_CPU_por_dispatch(tcb_invocador);
		//}
		//pthread_mutex_unlock(&mutex_flag_exit);
	}
}

void thread_exit(int TID, int pid){
	finalizar_hilo(TID, pid);
	//sem_post(&sem_cpu_disponible);
}

void mutex_create(char* ID, int PID){
	t_mutex_thread* mutex = malloc(sizeof(t_mutex_thread));
	t_pcb* pcb_aux = buscar_pcb_por_pid_en(PID, lista_ready, mutex_lista_ready);
 	mutex->ID_mutex = malloc(50 * sizeof(char));
	mutex->thread = NULL;
	mutex->pid_asoc = NULL;
	stpcpy(mutex->ID_mutex, ID);
	mutex->mutex = 1;
	mutex->lista_bloqueados = list_create();
	pthread_mutex_lock(&mutex_lista_mutex_thread);
	list_add(pcb_aux->lista_mutex, mutex);
	pthread_mutex_unlock(&mutex_lista_mutex_thread);
	log_info(kernel_logger,"Creado el mutex con [NOMBRE: %s], para el [PID: %d]", ID, PID);
}

char un_mutex_a_buscar[50];

bool __buscar_mutex(t_mutex_thread* void_mutex){
	log_info(kernel_logger, "Estoy buscando al [MUTEX: %s]", un_mutex_a_buscar);
	if(strcmp(void_mutex->ID_mutex, un_mutex_a_buscar) == 0){
		log_info(kernel_logger, "Estoy buscando al [MUTEX: %s] y el que esta dentro de [PCB: %s]", un_mutex_a_buscar, void_mutex->ID_mutex);
		return true;
	} else {
		return false;
	}
}


t_mutex_thread* find_mutex(char* ID_mutex, int PID){
	t_mutex_thread* aux;
	t_pcb* pcb_aux = buscar_pcb_por_pid_en(PID, lista_ready, mutex_lista_ready);
	log_info(kernel_logger, "Encontre al [PID: %d] en ready", PID);
	strcpy(un_mutex_a_buscar, ID_mutex);

	//pthread_mutex_lock(&pcb_aux->mutex);
	aux = list_find(pcb_aux->lista_mutex, (void*)__buscar_mutex); 
	//pthread_mutex_unlock(&pcb_aux->mutex);
	return aux;
	
}

void mutex_lock(char* mutex_solicitado, int TID, int PID){
	t_tcb* tcb_aux = NULL; //lo agrego porque hay que tener el hilo
	t_mutex_thread* mutex_aux = find_mutex(mutex_solicitado, PID);
	if(mutex_aux == NULL){
		log_info(kernel_logger,"El mutex a buscar es NULL");
		finalizar_hilo(TID, PID); //FINALIZAR HILO REMUEVE AL HILO DE UNA LISTA
		aviso_finalizacion_syscall(0); //NO SIGAS EJECUTANDO
		//sem_post(&sem_cpu_disponible);
		//return;
	}else if(mutex_aux->mutex == 1){
		mutex_aux->thread = TID;
		mutex_aux->pid_asoc = PID;
		mutex_aux->mutex = 0;
		aviso_finalizacion_syscall(1); //SEGUI EJECUTANDO
		//tcb_aux = buscar_tcb_por_tid(TID, PID);
		//pthread_mutex_lock(&mutex_flag_exit);
		//if(strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0 && flag_exit == 1){		
			/*tcb_aux = buscar_y_remover_tcb_por_tid(TID, PID);
			//lo agrego a la lista de ready de hilos
			pthread_mutex_lock(&mutex_lista_ready_thread);
			list_add(lista_ready_thread, tcb_aux);
			pthread_mutex_unlock(&mutex_lista_ready_thread);
			cambiar_estado_tcb(tcb_aux, READY_THREAD);
			log_info(kernel_log_obligatorio, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", tcb_aux->pid, tcb_aux->tid);
			log_info(kernel_logger, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", tcb_aux->pid, tcb_aux->tid);
			sem_post(&sem_cpu_disponible);*/
			//tcb_aux = buscar_y_remover_tcb_por_tid(TID, PID);
			//replanificar_cmn(tcb_aux);
			//flag_exit = 0;
		//}else{
		//	_enviar_tcb_a_CPU_por_dispatch(tcb_aux);
		//}
		//pthread_mutex_unlock(&mutex_flag_exit);

	} else if(mutex_aux->mutex == 0){
		tcb_aux = buscar_y_remover_tcb_por_tid(TID, PID);
		tcb_aux->motivo_block = MUTEX;
		bloquear_hilo_syscall_tcb(tcb_aux, "MUTEX");
		list_add(mutex_aux->lista_bloqueados, tcb_aux);
		aviso_finalizacion_syscall(0); //NO SIGAS EJECUTANDO
		//sem_post(&sem_cpu_disponible);
	}
}	


void mutex_unlock(char* mutex_solicitado, int TID, int PID){
	t_tcb* tcb_aux = buscar_tcb_por_tid(TID, PID);
	t_mutex_thread* mutex_aux;
	mutex_aux = find_mutex(mutex_solicitado, PID);
	log_info(kernel_logger,"Encontre al [MUTEX: %s]", mutex_aux->ID_mutex);
	if(mutex_aux == NULL){
		finalizar_hilo(TID, PID); //FINALIZAR HILO REMUEVE AL HILO DE UNA LISTA
		aviso_finalizacion_syscall(0);
		//sem_post(&sem_cpu_disponible);
		//return;
	}
	if(mutex_aux->thread == TID){
		log_info(kernel_logger, "lo tiene asignado este hilo");
		if(!list_is_empty(mutex_aux->lista_bloqueados)){
			t_tcb* tcb_aux2 = NULL;	
			tcb_aux2 = list_remove(mutex_aux->lista_bloqueados, 0);
			log_info(kernel_logger, "hilo removido de lista de bloqueados es el tid: %d", tcb_aux2->tid);
			mutex_aux->thread = tcb_aux2->tid;
			agregar_tcb_lista(tcb_aux2, lista_ready_thread, mutex_lista_ready_thread);
			cambiar_estado_tcb(tcb_aux2, READY_THREAD);
			aviso_finalizacion_syscall(1);
			//pthread_mutex_lock(&mutex_flag_exit);
			//if(strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0 && flag_exit == 1){	
				//log_info(kernel_logger, "tengo que replanificar");
				/*tcb_aux = buscar_y_remover_tcb_por_tid(TID, PID);
				//lo agrego a la lista de ready de hilos
				pthread_mutex_lock(&mutex_lista_ready_thread);
				list_add(lista_ready_thread, tcb_aux);
				pthread_mutex_unlock(&mutex_lista_ready_thread);
				cambiar_estado_tcb(tcb_aux, READY_THREAD);
				log_info(kernel_log_obligatorio, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", tcb_aux->pid, tcb_aux->tid);
				log_info(kernel_logger, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", tcb_aux->pid, tcb_aux->tid);
				sem_post(&sem_cpu_disponible);*/
				//tcb_aux = buscar_y_remover_tcb_por_tid(TID, PID);
				//replanificar_cmn(tcb_aux);
				//log_info(kernel_logger, "cpu disponible");
				//flag_exit = 0;
			//}else{
			//log_info(kernel_logger, "no se acabo el Q y envie el mismo");
			//_enviar_tcb_a_CPU_por_dispatch(tcb_aux);
			//}
			//pthread_mutex_unlock(&mutex_flag_exit);
		}else{
			log_info(kernel_logger, "la lista de bloqueados del mutex esta vacia");
			mutex_aux->thread = NULL;
			mutex_aux->mutex = 1;
			aviso_finalizacion_syscall(1);
		}
		//pthread_mutex_lock(&mutex_flag_exit);
			//if(strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0 && flag_exit == 1){		
			//	tcb_aux = buscar_y_remover_tcb_por_tid(TID, PID);
			//	replanificar_cmn(tcb_aux);
			//}else{
			//	_enviar_tcb_a_CPU_por_dispatch(tcb_aux);
			//}
		//pthread_mutex_unlock(&mutex_flag_exit);
	}else{
		//pthread_mutex_lock(&mutex_flag_exit);
		//if(strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0 && flag_exit == 1){		
			/*tcb_aux = buscar_y_remover_tcb_por_tid(TID, PID);
			//lo agrego a la lista de ready de hilos
			pthread_mutex_lock(&mutex_lista_ready_thread);
			list_add(lista_ready_thread, tcb_aux);
			pthread_mutex_unlock(&mutex_lista_ready_thread);
			cambiar_estado_tcb(tcb_aux, READY_THREAD);
			log_info(kernel_log_obligatorio, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", tcb_aux->pid, tcb_aux->tid);
			log_info(kernel_logger, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", tcb_aux->pid, tcb_aux->tid);
			sem_post(&sem_cpu_disponible);*/
			//tcb_aux = buscar_y_remover_tcb_por_tid(TID, PID);
			//replanificar_cmn(tcb_aux);
			//flag_exit = 0;
		//}else{
		//	_enviar_tcb_a_CPU_por_dispatch(tcb_aux);
		//	}
		//	pthread_mutex_unlock(&mutex_flag_exit);
		aviso_finalizacion_syscall(1);
	}
}  


void dump_memory(int TID, int PID){
	t_buffer* a_enviar = new_buffer();
	add_int_to_buffer(a_enviar, PID);
	add_int_to_buffer(a_enviar, TID);
    t_paquete* un_paquete = create_super_pck(DUMP_MEMORY_KM, a_enviar);
    conexion_memoria(un_paquete);
	log_info(kernel_logger, "Se aviso a Memoria para que haga dump del proceso");
}

void syscall_io(t_syscall_io* param){

	log_info(kernel_logger, "Syscall recibida: ## (%d:%d) - Solicitó syscall: IO", param->pid, param->tid);
	log_info(kernel_logger,"Me voy a dormir hilo: %d, %d milisegundos", param->pid, param->miliseg);

pthread_mutex_lock(&mutex_syscall_io);

	log_info(kernel_logger,"empece a dormir %d ", param->miliseg);
	log_info(kernel_logger,"el PID:%d - TID:%d esta haciendo IO ", param->pid, param->tid);
	
	

	usleep(param->miliseg*1000);
	desbloquear_hilo_finalizacion_syscall(param->tid, param->pid);

	log_info(kernel_log_obligatorio, "Fin de IO: ## (<%d>:<%d>) finalizó IO y pasa a READY", param->pid, param->tid);
	log_info(kernel_logger, "Fin de IO: ## (<%d>:<%d>) finalizó IO y pasa a READY", param->pid, param->tid);

	
pthread_mutex_unlock(&mutex_syscall_io);

}

void replanificar_cmn(t_tcb* un_tcb){
	//t_tcb* un_tcb = buscar_y_remover_tcb_por_tid(tid, pid);
	//lo agrego a la lista de ready de hilos
	pthread_mutex_lock(&mutex_lista_ready_thread);
	list_add(lista_ready_thread, un_tcb);
	pthread_mutex_unlock(&mutex_lista_ready_thread);
	cambiar_estado_tcb(un_tcb, READY_THREAD);
	log_info(kernel_log_obligatorio, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", un_tcb->pid, un_tcb->tid);
	log_info(kernel_logger, "Fin de Quantum: ## (<%d>:<%d>) - Desalojado por fin de Quantum", un_tcb->pid, un_tcb->tid);
	sem_post(&sem_cpu_disponible);
}
