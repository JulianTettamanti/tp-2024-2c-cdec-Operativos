#include <pcb.h>

int* tids = NULL;
int max_pid = 0;



char* estado_to_string(int un_valor){
	switch (un_valor) {
		case NEW_PROCCES:
			return "NEW_PROCCES";
			break;
		case READY_PROCCES:
			return "READY_PROCCES";
			break;
		case EXEC_PROCCES:
			return "EXEC_PROCCES";
			break;
		case BLOCKED_PROCCES:
			return "BLOCKED";
			break;
		case EXIT_PROCCES:
			return "EXIT_PROCCES";
			break;
		default:
			log_error(kernel_logger, "No se reconocio el nombre del estado");
            return "ERROR";
			break;
    }
}

char* motivo_to_string(t_motivo_exit motivo_exit){

	switch(motivo_exit){
	case SUCCESS:
		return "SUCCESS";
		break;
	case INVALID_RESOURCE:
		return "INVALID_RESOURCE";
		break;
	case INVALID_INTERFACE:
		return "INVALID_INTERFACE";
		break;
    case OUT_OF_MEMORY:
		return "OUT_OF_MEMORY";
		break;
    case INTERRUPTED_BY_USER:
		return "INTERRUPTED_BY_USER";
		break;
	default:
		return "ERROR";
	}
}

char* algoritmo_to_string(t_algoritmo algoritmo){

	switch(algoritmo){
	case FIFO:
		return "FIFO";
		break;
	case PRIORIDADES:
		return "PRIORIDADES";
		break;
	case CMN:
		return "CMN";
		break;
	default:
		return "ERROR";
	}
}



int asign_pid(){
    int valor_pid;

    pthread_mutex_lock(&mutex_pid);
    valor_pid = identificador_PID;
    identificador_PID++;
    pthread_mutex_unlock(&mutex_pid);

    return valor_pid;
}


t_pcb* create_pcb(char* path, int tam_proceso, int prioridad_hilo_main){
    t_pcb* new_PCB = malloc(sizeof(t_pcb));
	new_PCB->pid = asign_pid();
    new_PCB->size = tam_proceso;
	new_PCB->prioridad_hilo_main = prioridad_hilo_main;
	char* ruta = malloc(50 * sizeof(char));
	strcpy(ruta, "../the-last-of-c-pruebas/");
	strcat(ruta, path);
	new_PCB->path = ruta;
	new_PCB->estado = NEW_PROCCES;
	new_PCB->quantum = QUANTUM;
	new_PCB->prioridad_hilo_main = prioridad_hilo_main;
	new_PCB->motivo_exit = NOTHING;
	new_PCB->motivo_block = NOTHINGG;
    new_PCB->lista_tid = list_create(); //esta lista almacena es tcb
    new_PCB->lista_mutex = list_create();
	new_PCB->identificador_TID = 0;
	//pthread_mutex_init(&new_PCB->mutex, NULL);
	return new_PCB;

}

char* lista_pids(t_list* lista_estado, pthread_mutex_t mutex_lista){
	char* pids_en_string = string_new();
	string_append(&pids_en_string, "[");

//	pthread_mutex_lock(&mutex_lista);
	for(int i = 0; i < list_size(lista_estado); i++){
		t_pcb* pcb = list_get(lista_estado, i);
		if(i == 0){
			string_append(&pids_en_string, string_itoa(pcb->pid));
		}else{
			string_append(&pids_en_string, ", ");
			string_append(&pids_en_string, string_itoa(pcb->pid));
		}

	}
//	pthread_mutex_unlock(&mutex_lista);

	string_append(&pids_en_string, "]");

	return pids_en_string;
}

void agregar_pcb_lista(t_pcb* pcb, t_list* lista_estado, pthread_mutex_t mutex_lista){
	pthread_mutex_lock(&mutex_lista);
	list_add(lista_estado, pcb);
	pthread_mutex_unlock(&mutex_lista);
}
//-------------------------------Ojo con la función esta ------------------------------------------------------------------------
void destruir_pcb(t_pcb* un_pcb){
	//free(un_pcb->path); //OJO CON ESTO puede dar problemas
	list_destroy(un_pcb->lista_tid);
	list_destroy(un_pcb->lista_mutex); //USar el destroy y luego hacer free(pcb) libera todas las posiciones de memoria que usan las listas?
	//pthread_mutex_destroy(&un_pcb->lista_mutex); es correcto esto?
	free(un_pcb);
}//----------------------------------------------------------------------------------------------------------------------------------

void cambiar_estado(t_pcb* un_pcb, est_pcb nex_state){
	un_pcb->estado = nex_state;
}

int un_pid_a_buscar;

bool __buscar_pcb(t_pcb* void_pcb){
		if(void_pcb->pid == un_pid_a_buscar){
			return true;
		}else{
			return false;
		}
	}

t_pcb* buscar_pcb_por_pid_en(int un_pid, t_list* lista_estado, pthread_mutex_t mutex_lista){
	t_pcb* un_pcb;

	un_pid_a_buscar = un_pid;
	
	pthread_mutex_lock(&mutex_lista);
	if(list_any_satisfy(lista_estado, __buscar_pcb)){
		un_pcb = list_find(lista_estado, __buscar_pcb);
	}
	else{
		un_pcb = NULL;
	}
	pthread_mutex_unlock(&mutex_lista);
	return un_pcb;
}

t_pcb* buscar_pcb_por_pid(int un_pid){
	t_pcb* un_pcb;
	int elemento_encontrado = 0;

	un_pid_a_buscar = un_pid;

	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_new);
		if(list_any_satisfy(lista_new, __buscar_pcb)){
			elemento_encontrado = 1;
			un_pcb = list_find(lista_new, __buscar_pcb);
		}
		pthread_mutex_unlock(&mutex_lista_new);
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_ready);
	//	int cantidad = list_size(lista_ready_thread);
	//	log_info(kernel_logger, "CANTIDAD DE ENTRADAS EN LA LISTA READY: %d", cantidad);
		if(list_any_satisfy(lista_ready, __buscar_pcb)){
			elemento_encontrado = 1;
			un_pcb = list_find(lista_ready, __buscar_pcb);
		}
		pthread_mutex_unlock(&mutex_lista_ready);
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_exec);
	//	int cantidad = list_size(lista_execute_thread);
	//	t_tcb* pepe = list_get(lista_execute_thread, 0);
	//	log_info(kernel_logger, "CANTIDAD DE ENTRADAS EN LA LISTA EXECUTE: %d", cantidad);
	//	log_info(kernel_logger, "TID DE LA LISTA: %d", pepe->tid);
		if(list_any_satisfy(lista_execute, __buscar_pcb)){
			elemento_encontrado = 1;
			un_pcb = list_find(lista_execute, __buscar_pcb);
		}
		pthread_mutex_unlock(&mutex_lista_exec); 
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_exit);
		if(list_any_satisfy(lista_exit, __buscar_pcb)){
			elemento_encontrado = 1;
			un_pcb = list_find(lista_exit, __buscar_pcb); 
		}
		pthread_mutex_unlock(&mutex_lista_exit);
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_blocked);
		if(list_any_satisfy(lista_blocked, __buscar_pcb)){
			elemento_encontrado = 1;
			un_pcb = list_find(lista_blocked, __buscar_pcb);
		}
		pthread_mutex_unlock(&mutex_lista_blocked);
	}
	if(elemento_encontrado == 0){
		//Si es que no se encontro en ninguna lista
		log_error(kernel_logger, "[PID:%d] no encontrada en ninguna lista",un_pid);
		un_pcb = NULL;
		
	}

	return un_pcb;
}


t_pcb* buscar_y_remover_pcb_por_pid(int un_pid){
	t_pcb* un_pcb;
	int elemento_encontrado = 0;


	un_pid_a_buscar = un_pid;

	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_new);
		if(list_any_satisfy(lista_new, __buscar_pcb)){
			elemento_encontrado = 1;
			un_pcb = list_find(lista_new, __buscar_pcb);
			list_remove_element(lista_new, un_pcb);
		}
		pthread_mutex_unlock(&mutex_lista_new);
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_ready);
		if(list_any_satisfy(lista_ready, __buscar_pcb)){
			elemento_encontrado = 1;
			un_pcb = list_find(lista_ready, __buscar_pcb);
			list_remove_element(lista_ready, un_pcb);
		}
		pthread_mutex_unlock(&mutex_lista_ready);
	}
	//-- OJO el proceso nunca va a estar en una lista de EXEC
	//if(elemento_encontrado == 0){
	//	pthread_mutex_lock(&mutex_lista_exec);
	//	if(list_any_satisfy(lista_execute, __buscar_pcb)){
	//		elemento_encontrado = 1;
	//		un_pcb = list_find(lista_execute, __buscar_pcb);
	//		list_remove_element(lista_execute, un_tcb);
	//	}
	//	pthread_mutex_unlock(&mutex_lista_exec);
	//}

	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_exit);
		if(list_any_satisfy(lista_exit, __buscar_pcb)){
			elemento_encontrado = 1;
			un_pcb = list_find(lista_exit, __buscar_pcb);
			list_remove_element(lista_exit, un_pcb);
		}
		pthread_mutex_unlock(&mutex_lista_exit);
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_blocked);
		if(list_any_satisfy(lista_blocked, __buscar_pcb)){
			elemento_encontrado = 1;
			un_pcb = list_find(lista_blocked, __buscar_pcb);
			list_remove_element(lista_blocked, un_pcb);
		}
		pthread_mutex_unlock(&mutex_lista_blocked);
	}
	if(elemento_encontrado == 0){
		//Si es que no se encontro en ninguna lista
		un_pcb = NULL;
		log_error(kernel_logger, "PID no encontrada en ninguna lista");
	}

	return un_pcb;

}

//ESTRUCTURAS PARA LOS HILOS

t_tcb* create_tcb(int PID_asociado, int prioridad, char* path){
    t_tcb* new_TCB = malloc(sizeof(t_tcb));
	t_pcb* un_pcb =  buscar_pcb_por_pid_en(PID_asociado, lista_ready, mutex_lista_ready);
	int valor_tid;
    pthread_mutex_lock(&mutex_tid);
    valor_tid = un_pcb->identificador_TID;
    un_pcb->identificador_TID++;
	new_TCB->ticket = 0;
    pthread_mutex_unlock(&mutex_tid);
	new_TCB->tid = valor_tid;
	new_TCB->prioridad = prioridad;
	new_TCB->pid= PID_asociado;
	new_TCB->path= path;
	new_TCB->estado = NEW_THREAD;
	new_TCB->lista_mutex_thread = list_create();
	new_TCB->lista_blocked_thread = list_create();
	return new_TCB;
}


int asign_tid(int PID){

if (PID >= max_pid) {
        // Si el PID es mayor que el tamaño del array actual, expandimos el array
        tids = (int*) realloc(tids, (PID + 1) * sizeof(int));
        
        // Inicializamos los nuevos elementos del array (los nuevos PIDs)
        for (int i = max_pid; i <= PID; i++) {
            tids[i] = -1;  // Inicializamos a -1 para que el primer TID sea 0
        }

        // Actualizamos el máximo PID registrado
        max_pid = PID + 1;
    }
    
    // Incrementamos el TID para el PID dado
    tids[PID]++;
    
    // Retornamos el nuevo TID
    return tids[PID];

}

void destruir_tcb(t_tcb* un_tcb){
	free(un_tcb);
}


void cambiar_estado_tcb(t_tcb* un_tcb, est_tcb nex_state){
	un_tcb->estado = nex_state;
}




void _enviar_tcb_a_CPU_por_dispatch(t_tcb* un_tcb){
	t_buffer* a_enviar = new_buffer();
	log_info(kernel_logger,"Envio a CPU [TID: %d] del proceso [PID: %d]", un_tcb->tid, un_tcb->pid);
	add_int_to_buffer(a_enviar, un_tcb->pid);
	add_int_to_buffer(a_enviar, un_tcb->tid);
	t_paquete* un_paquete = create_super_pck(EJECUTAR_HILO_KC, a_enviar);
	send_pckg(un_paquete, socket_cpu_dispatch);
	delete_pckg(un_paquete);
}

void _enviar_tcb_a_CPU_por_interrupt(t_tcb* un_tcb){
	t_buffer* a_enviar = new_buffer();
	add_int_to_buffer(a_enviar, un_tcb->pid);
	add_int_to_buffer(a_enviar, un_tcb->tid);
	add_string_to_buffer(a_enviar, "DESALOJO_RR");
	t_paquete* un_paquete = create_super_pck(DESALOJO_POR_QUAMTUN_KC, a_enviar);
	send_pckg(un_paquete, socket_cpu_interrupt);
	delete_pckg(un_paquete);
}


void aviso_finalizacion_syscall(int resp){
	t_buffer* a_enviar = new_buffer();
	log_info(kernel_logger,"Envio a CPU aviso finalizacion de Syscall");
	add_int_to_buffer(a_enviar, resp);
	t_paquete* un_paquete = create_super_pck(FINALIZACION_SYSCALL_KC, a_enviar);
	send_pckg(un_paquete, socket_cpu_interrupt);
	log_info(kernel_logger,"Envio a CPU aviso finalizacion de Syscall CON EXITO");
	delete_pckg(un_paquete);
}

int un_pid_buscar_tcb;
int un_tid_a_buscar;

bool __buscar_tcb(t_tcb* void_tcb){
	//log_info(kernel_logger, "EL TID A BUSCAR ES: %d Y EL TID DE LA ITERACION ES %d", un_tid_a_buscar, void_tcb->tid);
	if(void_tcb->tid == un_tid_a_buscar && void_tcb->pid == un_pid_buscar_tcb){
		return true;
	} else {
		return false;
	}
}

t_tcb* buscar_tcb_por_tid(int un_tid, int un_pid){
	t_tcb* un_tcb;
	int elemento_encontrado = 0;

	un_tid_a_buscar = un_tid;
	un_pid_buscar_tcb = un_pid;

	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_new_thread);
		if(list_any_satisfy(lista_new_thread, __buscar_tcb)){
			elemento_encontrado = 1;
			un_tcb = list_find(lista_new_thread, __buscar_tcb);
		}
		pthread_mutex_unlock(&mutex_lista_new_thread);
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_ready_thread);
	//	int cantidad = list_size(lista_ready_thread);
	//	log_info(kernel_logger, "CANTIDAD DE ENTRADAS EN LA LISTA READY: %d", cantidad);
		if(list_any_satisfy(lista_ready_thread, __buscar_tcb)){
			elemento_encontrado = 1;
			un_tcb = list_find(lista_ready_thread, __buscar_tcb);
		}
		pthread_mutex_unlock(&mutex_lista_ready_thread);
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_exec_thread);
	//	int cantidad = list_size(lista_execute_thread);
	//	t_tcb* pepe = list_get(lista_execute_thread, 0);
	//	log_info(kernel_logger, "CANTIDAD DE ENTRADAS EN LA LISTA EXECUTE: %d", cantidad);
	//	log_info(kernel_logger, "TID DE LA LISTA: %d", pepe->tid);
		if(list_any_satisfy(lista_execute_thread, __buscar_tcb)){
			elemento_encontrado = 1;
			un_tcb = list_find(lista_execute_thread, __buscar_tcb);
		}
		pthread_mutex_unlock(&mutex_lista_exec_thread); 
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_exit_thread);
		if(list_any_satisfy(lista_exit_thread, __buscar_tcb)){
			elemento_encontrado = 1;
			un_tcb = list_find(lista_exit_thread, __buscar_tcb); 
		}
		pthread_mutex_unlock(&mutex_lista_exit_thread);
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_blocked_thread);
		if(list_any_satisfy(lista_blocked_thread, __buscar_tcb)){
			elemento_encontrado = 1;
			un_tcb = list_find(lista_blocked_thread, __buscar_tcb);
		}
		pthread_mutex_unlock(&mutex_lista_blocked_thread);
	}
	if(elemento_encontrado == 0){
		//Si es que no se encontro en ninguna lista
		log_error(kernel_logger, "[TID:%d] no encontrada en ninguna lista(PCB: %d)",un_tcb->tid, un_tcb->pid);
		un_tcb = NULL;
		
	}

	return un_tcb;
}

t_tcb* buscar_y_remover_tcb_por_tid(int un_tid, int un_pid){ 
	t_tcb* un_tcb;
	int elemento_encontrado = 0;

	un_tid_a_buscar = un_tid;
	un_pid_buscar_tcb = un_pid;

	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_new_thread);
		if(list_any_satisfy(lista_new_thread, __buscar_tcb)){ 
			elemento_encontrado = 1;
			un_tcb = list_find(lista_new_thread, __buscar_tcb); 
			log_info(kernel_logger,"Encontrado en lista new: %d", un_tcb->tid);
			list_remove_element(lista_new_thread, un_tcb);
		}
		pthread_mutex_unlock(&mutex_lista_new_thread);
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_ready_thread);
		if(list_any_satisfy(lista_ready_thread, __buscar_tcb)){
			elemento_encontrado = 1;
			un_tcb = list_find(lista_ready_thread, __buscar_tcb);
			log_info(kernel_logger,"Encontrado en lista ready: %d", un_tcb->tid);
			list_remove_element(lista_ready_thread, un_tcb);
		}
		pthread_mutex_unlock(&mutex_lista_ready_thread);
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_exec_thread);
		if(list_any_satisfy(lista_execute_thread, __buscar_tcb)){
			elemento_encontrado = 1;
			un_tcb = list_find(lista_execute_thread, __buscar_tcb);
			log_info(kernel_logger,"Encontrado en lista execute: %d", un_tcb->tid);
			list_remove_element(lista_execute_thread, un_tcb);
		}
		pthread_mutex_unlock(&mutex_lista_exec_thread);
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_exit_thread);
		if(list_any_satisfy(lista_exit_thread, __buscar_tcb)){
			elemento_encontrado = 1;
			un_tcb = list_find(lista_exit_thread, __buscar_tcb);
			log_info(kernel_logger,"Encontrado en lista exit: %d", un_tcb->pid);
			list_remove_element(lista_exit_thread, un_tcb);
		}
		pthread_mutex_unlock(&mutex_lista_exit_thread);
	}
	if(elemento_encontrado == 0){
		pthread_mutex_lock(&mutex_lista_blocked_thread);
		
		if(list_any_satisfy(lista_blocked_thread, __buscar_tcb)){
			elemento_encontrado = 1;
			un_tcb = list_find(lista_blocked_thread, __buscar_tcb);
			log_info(kernel_logger,"Encontrado en lista block (byr): %d", un_tcb->pid);
			list_remove_element(lista_blocked_thread, un_tcb);
		}
		pthread_mutex_unlock(&mutex_lista_blocked_thread);
	}
	if(elemento_encontrado == 0){
		//Si es que no se encontro en ninguna lista
		log_error(kernel_logger, "[TID:%d] no encontrada en ninguna lista(PCB: %d)",un_tid, un_pid);
		un_tcb = NULL;
	}

	return un_tcb;
	
}

char* estado_tcb_to_string(est_tcb estado){

	switch(estado){
	case NEW_THREAD:
		return "NEW_THREAD";
		break;
	case READY_THREAD:
		return "READY_THREAD";
		break;
	case EXEC_THREAD:
		return "EXEC_THREAD";
		break;
	case BLOCKED_THREAD:
		return "BLOCKED_THREAD";
		break;
	case EXIT_THREAD:
		return "EXIT_THREAD";
		break;
	default:
		return "ERROR";
	}
}

void agregar_tcb_lista(t_tcb* tcb, t_list* lista_estado, pthread_mutex_t mutex_lista){
	pthread_mutex_lock(&mutex_lista);
	list_add(lista_estado, tcb);
	pthread_mutex_unlock(&mutex_lista);
}

void liberar_lista_bloqueados(t_tcb* tcb){
	t_tcb_aux* tcb_aux;
	t_tcb* aux;
	if(tcb == NULL || list_is_empty(tcb->lista_blocked_thread)){
		return;
	}
		while((tcb_aux = list_remove(tcb->lista_blocked_thread, 0)) != NULL){ 
			aux = buscar_y_remover_tcb_por_tid(tcb_aux->tid, tcb_aux->pid);
			agregar_tcb_lista(aux, lista_ready_thread, mutex_lista_ready_thread);
			free(tcb_aux);
		}
		pthread_mutex_lock(&mutex_lista_ready_thread);
		if (strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN") == 0){
					ordenar_lista(lista_ready_thread, comparador_menorAmayor);
				}
		pthread_mutex_unlock(&mutex_lista_ready_thread);
}

void bloquear_hilo_syscall(int tid, char* motivo, int pid){
	log_info(kernel_logger, "Saco al hilo de alguna lista para bloquearlo por IO");
	t_tcb* tcb = buscar_y_remover_tcb_por_tid(tid, pid);  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	log_info(kernel_logger, "Hilo removidod de alguna lista para bloquearlo: %d",tcb->pid);
	cambiar_estado_tcb(tcb, BLOCKED_THREAD);
	tcb->motivo_block = SYSCALL;
	log_info(kernel_log_obligatorio, "Motivo de Bloqueo: ## (%d:%d) - Bloqueado por: %s", tcb->pid, tcb->tid, motivo);
	pthread_mutex_lock(&mutex_lista_blocked_thread);
	list_add(lista_blocked_thread, tcb);
	pthread_mutex_unlock(&mutex_lista_blocked_thread);
}

void bloquear_hilo_syscall_tcb(t_tcb* tcb, char* motivo){
	cambiar_estado_tcb(tcb, BLOCKED_THREAD);
	//tcb->motivo_block = SYSCALL;
	log_info(kernel_log_obligatorio, "Motivo de Bloqueo: ## (%d:%d) - Bloqueado por: %s", tcb->pid, tcb->tid, motivo);
	pthread_mutex_lock(&mutex_lista_blocked_thread);
	list_add(lista_blocked_thread, tcb);
	pthread_mutex_unlock(&mutex_lista_blocked_thread);
}


void desbloquear_hilo_finalizacion_syscall(int tid, int pid){
	t_tcb* tcb = buscar_y_remover_tcb_por_tid(tid, pid);
	cambiar_estado_tcb(tcb, READY_THREAD);
	pthread_mutex_lock(&mutex_lista_ready_thread);
	list_add(lista_ready_thread, tcb);
	pthread_mutex_unlock(&mutex_lista_ready_thread);
	log_info(kernel_logger,"hilo desbloqueado");
	pthread_mutex_lock(&mutex_lista_exec_thread);
	if(list_is_empty(lista_execute_thread)){
		sem_post(&sem_cpu_disponible);
	}
	pthread_mutex_unlock(&mutex_lista_exec_thread);
}

//Funcion para transformar una lista
t_list *transformar_lista(t_list *lista_original, void *(*transformar_elemento)(void *)) {
    t_list *nueva_lista = list_create();
    nueva_lista->head = NULL;
    nueva_lista->elements_count = 0;

    t_link_element *actual = lista_original->head;
    while (actual != NULL) {
        // Aplicar la función de transformación al dato actual
        t_tcb* aux = transformar_elemento(actual->data);
		liberar_lista_bloqueados(aux);
        // Agregar el nuevo dato a la nueva lista
        list_add(nueva_lista, aux);

        // Avanzar al siguiente nodo
        actual = actual->next;
    }

    return nueva_lista;
}

void send_thread_to_cpu(t_tcb* tcb_aux){
		pthread_mutex_lock(&mutex_lista_exec_thread);
		list_add(lista_execute_thread, tcb_aux);
		pthread_mutex_unlock(&mutex_lista_exec_thread);
		cambiar_estado_tcb(tcb_aux, EXEC_THREAD);
        _enviar_tcb_a_CPU_por_dispatch(tcb_aux);
}
void liberar_hilos_bloqueados(t_list *lista_original){

	log_info(kernel_logger,"ENTRO A LA FUNCION LIBERAR_HILOS_BLOQUEADOS");
    /*t_link_element *actual = lista_original->head;
    while (actual != NULL) {
        // Aplicar la función de transformación al dato actual
		pthread_mutex_lock(&mutex_lista_blocked_thread);
		//bool encontrado = list_remove_element(lista_blocked_thread, actual->data);
		bool encontrado = list_remove_element(lista_original, actual->data);
		log_info(kernel_logger,"ENCONTRE ALGO: %d", encontrado);
		pthread_mutex_unlock(&mutex_lista_blocked_thread);
        if(encontrado){
		cambiar_estado_tcb(actual->data, READY_THREAD);

        // Agregar el nuevo dato a la nueva lista
		pthread_mutex_lock(&mutex_lista_ready_thread);
        list_add(lista_ready_thread, actual->data);
		pthread_mutex_unlock(&mutex_lista_ready_thread);
		}
        // Avanzar al siguiente nodo
        actual = actual->next;
    } 
	list_clean(lista_original);*/
	t_list_iterator* iterator = list_iterator_create(lista_original);
            while(list_iterator_has_next(iterator)){
				log_info(kernel_logger,"SENTRO AL WHILE");
                pthread_mutex_lock(&mutex_lista_blocked_thread);
				t_tcb* element = list_iterator_next(iterator);
				log_info(kernel_logger,"SE EXTRAJO AL TID %d", element->tid);
				pthread_mutex_unlock(&mutex_lista_blocked_thread);

				if(element != NULL){
					cambiar_estado_tcb(element, READY_THREAD);

        			// Agregar el nuevo dato a la nueva lista
					pthread_mutex_lock(&mutex_lista_ready_thread);
        			list_add(lista_ready_thread, element);
					pthread_mutex_unlock(&mutex_lista_ready_thread);
				}
            }
            list_iterator_destroy(iterator);
			log_info(kernel_logger,"ANTES DE DESTRUIR LA LISTA");
			list_destroy(lista_original);
			log_info(kernel_logger,"DESPUES DE DESTRUIR LA LISTA");
}

