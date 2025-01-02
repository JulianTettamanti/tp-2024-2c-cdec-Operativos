#include <planificador_largo_plazo.h>

void plp(){
	t_pcb* pcb = NULL;
	t_tcb* un_tcb = NULL;
	
	pthread_mutex_lock(&mutex_lista_new);
	log_info(kernel_logger,"lista vacia: %d",list_is_empty(lista_new));
	if(!list_is_empty(lista_new)){
		//log_info(kernel_logger,"Pase");
		pcb = list_get(lista_new, 0); //sale por fifo // 1/12 -Gonza: con list_get funciona
		log_info(kernel_logger,"el pid del pcb es: %d",pcb->pid);
		if(pcb != NULL){
				int hay_pcb = 1;
				while(hay_pcb){
					pcb = list_get(lista_new, 0);
					//Enviar Mensaje a memoria para que inicialice estructuras
					t_buffer* a_enviar = new_buffer();
					add_int_to_buffer(a_enviar, pcb->pid);
					add_string_to_buffer(a_enviar, pcb->path);
					add_int_to_buffer(a_enviar, pcb->size);
					t_paquete* un_paquete = create_super_pck(INICIALIZAR_ESTRUCTURAS_KM, a_enviar);
					conexion_memoria(un_paquete);
					log_info(kernel_logger, "Se aviso a Memoria del nuevo proceso");
					// 	esperamos hasta que se cree la estructura
					sem_wait(&sem_rpta_estructura_inicializada);
					pthread_mutex_lock(&mutex_flag_pedido_memoria);
					//log_info(kernel_logger, "esperando el flag");
					if(flag_pedido_de_memoria){
						list_remove_element(lista_new, pcb);
						//	log_info(kernel_logger, "pase el flag");
						//creo tcb con prioridad maxima por ser el TID0
						//agrego pcb a la lista de ready provisional
						log_info(kernel_log_obligatorio, "Creación de Proceso: ## (%d:0) Se crea el proceso - Estado: NEW", pcb->pid);
						agregar_pcb_lista(pcb, lista_ready, mutex_lista_ready);
						cambiar_estado(pcb, READY_PROCCES);
						un_tcb = create_tcb(pcb->pid, pcb->prioridad_hilo_main, pcb->path);
						//esto me parece que es necesario, ya que el path se debe asociar es al hilo main
						//se repite un pooc de logica
						list_add(pcb->lista_tid, un_tcb->tid); //almacena es tid
						//lo agrego a la lista de ready de hilos
						pthread_mutex_lock(&mutex_lista_ready_thread);
						list_add(lista_ready_thread, un_tcb);
						pthread_mutex_unlock(&mutex_lista_ready_thread);
						cambiar_estado_tcb(un_tcb, READY_THREAD);
						log_info(kernel_log_obligatorio, "Creación de Hilo: ## (<PID:%d>:<TID:%d>) Se crea el Hilo - Estado: READY", un_tcb->pid, un_tcb->tid);
					
						if(list_is_empty(lista_new)){
							hay_pcb = 0;
						}

					} else {
						hay_pcb = 0;
					}
					pthread_mutex_unlock(&mutex_flag_pedido_memoria);
					
				}
				
    	} 
	}
	pthread_mutex_unlock(&mutex_lista_new);
//	pthread_mutex_lock(&mutex_lista_exec_thread);
//	if(list_is_empty(lista_execute_thread)){
//	pthread_mutex_unlock(&mutex_lista_exec_thread);
//	exec_in_detach((void*)pcp, NULL); //mandamos a ejecutar pcp en un hilo
	//}
	//sem_post(&sem_cpu_disponible);
}


void finalizar_proceso(int pid){ //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	t_pcb* un_pcb = buscar_y_remover_pcb_por_pid(pid);
	if(un_pcb != NULL){
		log_info(kernel_logger, "entre a finalizar proceso");
		t_buffer* a_enviar = new_buffer();
        add_int_to_buffer(a_enviar, un_pcb->pid);
		log_info(kernel_logger, "AGREGO AL BUFFER EL PID A ELIMINAR: %d", un_pcb->pid);
        t_paquete* un_paquete = create_super_pck(FINALIZAR_ESTRUCTURAS_KM, a_enviar);
		log_info(kernel_logger, "INTENTANDO CONECTAR CON MEMORIA");
        conexion_memoria(un_paquete);
		log_info(kernel_logger, "Se aviso a Memoria para que libere las estructuras del proceso");
		// 	esperamos hasta que se libere la estructura
		sem_wait(&sem_estructura_liberada);
		agregar_pcb_lista(un_pcb, lista_exit, mutex_lista_exit);
		log_info(kernel_log_obligatorio, "Fin de Proceso: ## Finaliza el proceso %d", un_pcb->pid);
		plp();
	}
}

void crear_hilo(int prioridad, int PID_asociado, char* path){
		t_tcb* un_tcb = create_tcb(PID_asociado, prioridad, path);
		t_pcb* un_pcb = buscar_pcb_por_pid_en(PID_asociado, lista_ready, mutex_lista_ready);
		t_buffer* a_enviar = new_buffer();
		log_info(kernel_logger, "Se esta creando el hilo %d, enviando a memoria para que inicialice...", un_tcb->tid);
		log_info(kernel_logger, "Se envia a memoria el [PATH: %s]", path);
        add_int_to_buffer(a_enviar, un_tcb->pid);
        add_int_to_buffer(a_enviar, un_tcb->tid);
		add_string_to_buffer(a_enviar, path);
        t_paquete* un_paquete = create_super_pck(CREACION_HILO_KM, a_enviar);
        conexion_memoria_solo_aviso(un_paquete);
		log_info(kernel_logger, "Se aviso a Memoria del nuevo hilo");
		list_add(un_pcb->lista_tid, un_tcb->tid);
		pthread_mutex_lock(&mutex_lista_ready_thread);
		list_add(lista_ready_thread, un_tcb);
		t_tcb* otro_tcb = list_get(lista_ready_thread,un_tcb->tid - 1);
		log_info(kernel_logger, "SE OBTIENE EL TID: %d", otro_tcb->tid);
		pthread_mutex_unlock(&mutex_lista_ready_thread);
		cambiar_estado_tcb(un_tcb, READY_THREAD);
		log_info(kernel_log_obligatorio, "Creación de Hilo: ## (<PID:%d>:<TID:%d>) Se crea el Hilo - Estado: READY", un_tcb->pid, un_tcb->tid);
}

void finalizar_hilo(int TID, int pid){
	t_tcb* un_tcb = buscar_y_remover_tcb_por_tid(TID, pid);
	log_info(kernel_logger,"Encontre el TID a terminar");
	if(un_tcb != NULL){
		t_buffer* a_enviar = new_buffer();
		add_int_to_buffer(a_enviar, un_tcb->pid);
        add_int_to_buffer(a_enviar, un_tcb->tid);
		//hay que ver si es necesario enviarle el pid, creeria que si
		
        t_paquete* un_paquete = create_super_pck(FINALIZACION_HILO_KM, a_enviar);
		log_info(kernel_logger, "Envio a memoria el hilo a finalizar");
        conexion_memoria(un_paquete);
		log_info(kernel_logger, "Se aviso a Memoria para que libere las estructuras del hilo");
		// 	esperamos hasta que se libere la estructura OJO el enunciado no menciona que hay que esperar, pero seria lo mas logico
		sem_wait(&sem_estructura_liberada); //podemos dejar o agregar uno de hilos solamente
		//esto hay que mejorarlo
		agregar_tcb_lista(un_tcb, lista_exit_thread, mutex_lista_exit_thread);
		un_tcb->estado = EXIT_THREAD;
		liberar_hilos_bloqueados(un_tcb->lista_blocked_thread);
		log_info(kernel_log_obligatorio, "Fin de Hilo: ## (%d:%d) Finaliza el hilo", un_tcb->pid, un_tcb->tid);
	}
}