#include <planificador_corto_plazo.h>
int var_ticket;

int generar_ticket(){
	int valor_ticket;
	pthread_mutex_lock(&mutex_ticket);
	var_ticket++;
	valor_ticket = var_ticket;
	pthread_mutex_unlock(&mutex_ticket);
	return valor_ticket;
}

static void _programar_interrupcion_por_quantum(t_tcb* un_tcb){
	int ticket_referencia = un_tcb->ticket;
	log_info(kernel_logger,"Ticket antes de dormir [TICKET: %d]",un_tcb->ticket);
	//sem_wait(&sem_contexto_listo);
	usleep(QUANTUM*1000);

	/*Esta comprobacion de ticket es en caso de que el TCB haya salido de CPU,
	 * Puesto en READY y por casulaidades de la vida haya vuelto a la CPU
	 * Y al despertar este hilo, primero verifique que la TCB objetivo, no haya
	 * salido de la CPU, y esto lo resolvemos con el ticket.
	 * Porque si salio el mismo TCB y volvio a entrar, significa que el proceso tiene
	 * nuevo ticket*/
	log_info(kernel_logger, "Me desperte");
	pthread_mutex_lock(&mutex_ticket);
	pthread_mutex_lock(&mutex_lista_exec_thread);
	if(ticket_referencia == var_ticket && !list_is_empty(lista_execute_thread)){
	log_info(kernel_logger,"Me desperte de QUANTUM y sigue el mismo HILO");
		//pthread_mutex_lock(&mutex_flag_exit);
		//if(!flag_exit){
			pthread_mutex_lock(&mutex_flag_exit);
			flag_exit = 1;
			pthread_mutex_unlock(&mutex_flag_exit);

			sem_post(&sem_enviar_interrupcion);
			//flag_exit = true;
		//}
//		flag_exit = false;
		//pthread_mutex_unlock(&mutex_flag_exit);
	}
	pthread_mutex_unlock(&mutex_ticket);
	pthread_mutex_unlock(&mutex_lista_exec_thread);
}

void _atender_FIFO(){
	//Verificar que la lista de EXECUTE esté vacía
	log_info(kernel_logger, "Comienzo a ejecutar FIFO");
	pthread_mutex_lock(&mutex_lista_exec_thread);
	if(list_is_empty(lista_execute_thread)){
		t_tcb* un_tcb = NULL;
		log_info(kernel_logger, " FIFO - LISTA EXECUTE VACIA, BUSCANDO EN READY");

		//Verificar que haya elementos en la lista de READY
		pthread_mutex_lock(&mutex_lista_ready_thread); 
		if(!list_is_empty(lista_ready_thread)){
			un_tcb = list_remove(lista_ready_thread, 0);
		}
		pthread_mutex_unlock(&mutex_lista_ready_thread);

		
		if(un_tcb != NULL){
			list_add(lista_execute_thread, un_tcb);
			
			log_info(" TID: %d - Estado Anterior: %s", estado_tcb_to_string(un_tcb->estado));
			cambiar_estado_tcb(un_tcb, EXEC_THREAD);
			log_info(kernel_logger, "PID: %d - TID: %d - Estado Anterior: READY - Estado Actual: EXEC", un_tcb->pid, un_tcb -> tid); 
			log_info(kernel_log_obligatorio, "PID: %d - TID: %d - Estado Anterior: READY - Estado Actual: EXEC", un_tcb->pid, un_tcb -> tid); 
			un_tcb->ticket = generar_ticket();
            _enviar_tcb_a_CPU_por_dispatch(un_tcb);
		}
		else{
			log_warning(kernel_logger, "Lista de READY vacía");
		}
	} else {log_info(kernel_logger, "La lista exec no esta vacía");}
	pthread_mutex_unlock(&mutex_lista_exec_thread);
}

bool comparador_ready(t_tcb* primero, t_tcb* segundo){
	return primero->prioridad <= segundo->prioridad;
}

void _atender_RR(){
	//Verificar que la lista de EXECUTE esté vacía
	log_info(kernel_logger,"Entre a replanificar con CMN");
	pthread_mutex_lock(&mutex_lista_exec_thread);
	if(list_is_empty(lista_execute_thread)){
		t_tcb* un_tcb = NULL;

		//Verificar que haya elementos en la lista de READY
		pthread_mutex_lock(&mutex_lista_ready_thread);
		if(!list_is_empty(lista_ready_thread)){
			list_sort(lista_ready_thread,(bool*)comparador_ready);
			un_tcb = list_remove(lista_ready_thread, 0);
		
		//pthread_mutex_unlock(&mutex_lista_ready_thread);
		}else{
			log_warning(kernel_logger, "Lista de READY vacía");
		}
		pthread_mutex_unlock(&mutex_lista_ready_thread);

		if(un_tcb != NULL){
			list_add(lista_execute_thread, un_tcb);
			cambiar_estado_tcb(un_tcb, EXEC_THREAD);
			log_info(kernel_logger, "PID: %d - TID: %d - Estado Anterior: READY - Estado Actual: EXEC", un_tcb-> pid, un_tcb -> tid);
			log_info(kernel_log_obligatorio, "PID: %d - TID: %d - Estado Anterior: READY - Estado Actual: EXEC",  un_tcb->pid, un_tcb -> tid); 
			un_tcb->ticket = generar_ticket();
			_enviar_tcb_a_CPU_por_dispatch(un_tcb);
				pthread_mutex_lock(&mutex_flag_exit);
				flag_exit = 0;
				pthread_mutex_unlock(&mutex_flag_exit);
				exec_in_detach((void*)_programar_interrupcion_por_quantum, un_tcb); //se cambio la función ejecutar_un_nuevo_hilo_en_detatch por esta función
		}
	}
	pthread_mutex_unlock(&mutex_lista_exec_thread);
}

void _atender_PRIORIDADES(){
	t_tcb* un_tcb = NULL;
	t_tcb* aux = NULL;
	//Tomo el elemento de mayor prioridad
	pthread_mutex_lock(&mutex_lista_ready_thread);
	if(list_size(lista_ready_thread) == 1){
		un_tcb = list_get(lista_ready_thread, 0);
		}else{
		un_tcb = list_get_maximum(lista_ready_thread, (void*)__maxima_prioridad);
	}
	log_info(kernel_logger,"Voy a ver si meto a EXEC al TID <%d>, PID <%d>", un_tcb->tid, un_tcb->pid);
	if(un_tcb != NULL){
	pthread_mutex_lock(&mutex_lista_exec_thread);
		if(list_is_empty(lista_execute_thread)){
			if(list_remove_element(lista_ready_thread, un_tcb)){

				list_add(lista_execute_thread, un_tcb);
				//un_tcb->ticket = generar_ticket();S
				cambiar_estado_tcb(un_tcb, EXEC_THREAD);
				log_info(kernel_logger,"Nuevo estado de TID <%d>, PID <%d> ESTADO <%d>", un_tcb->tid, un_tcb->pid, un_tcb->estado);
				log_info(kernel_logger, " PID:%d-TID:%d - Estado Anterior: READY - Estado Actual: EXEC", un_tcb->pid, un_tcb->tid);
            	log_info(kernel_log_obligatorio, " PID:%d-TID:%d - Estado Anterior: READY - Estado Actual: EXEC", un_tcb->pid, un_tcb->tid); 
            	_enviar_tcb_a_CPU_por_dispatch(un_tcb);
			}

		}else{
			aux = list_get(lista_execute_thread, 0);
			log_error(kernel_logger, "Lista de Exec no esta vacia, tiene al TID <%d>", aux->tid);
			exit(EXIT_FAILURE);
		}
	}
    pthread_mutex_unlock(&mutex_lista_exec_thread);
	pthread_mutex_unlock(&mutex_lista_ready_thread);
}


t_tcb* __maxima_prioridad(t_tcb* void_1, t_tcb* void_2){
		if(void_1->prioridad <= void_2->prioridad) return void_1;
		else return void_2;
}

void pcp(){
	while (1){
	sem_wait(&sem_cpu_disponible);
    int flag_lista_ready_vacia = 0; //false
	pthread_mutex_lock(&mutex_lista_ready_thread);
	if(list_is_empty(lista_ready_thread)){
		flag_lista_ready_vacia = 1; //true
	}
	pthread_mutex_unlock(&mutex_lista_ready_thread);

	if(flag_lista_ready_vacia == 0){
		
		switch (ALGORITMO_PLANIFICACION) {
			case FIFO:
				_atender_FIFO();
				break;
			case PRIORIDADES:
				_atender_PRIORIDADES();
				break;
			case CMN:
				_atender_RR();
				break;
			default:
				log_error(kernel_logger, "ALGORITMO DE CORTO PLAZO DESCONOCIDO");
				exit(EXIT_FAILURE);
				break;
		}
	}
}
}