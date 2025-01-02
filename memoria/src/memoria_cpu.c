#include <memoria_cpu.h>
void attend_memoria_cpu(){
    bool control_key = 1;
	t_buffer* un_buffer;
    while (control_key) {
        int cod_op = recv_op(socket_cpu);
        switch (cod_op) {
            case PEDIDO_CONTEXTO_CM:
                //t_buffer* un_buffer;
                log_info(memo_logger_debug, "Pedido contexto CPU");
                un_buffer=recv_buffer(socket_cpu);
                pedido_contexto(un_buffer);
                free(un_buffer->stream);
                free(un_buffer);
            break;

            case ACTUALIZAR_CONTEXTO_CM:
                //t_buffer* un_buffer;
                log_info(memo_logger_debug, "Actualizar contexto CPU");
                un_buffer=recv_buffer(socket_cpu);
                actualizar_contexto(un_buffer);
                free(un_buffer->stream);
                free(un_buffer);
            break;

            case DESALOJO_INTR:
                //t_buffer* un_buffer;
                log_info(memo_logger_debug, "Desalojo instrucción contexto CPU");
                un_buffer=recv_buffer(socket_cpu);
                actualizar_contexto(un_buffer);
                free(un_buffer->stream);
                free(un_buffer);
            break;

            case DESALOJO_INSTRUCCION_CM:
                //t_buffer* un_buffer;
                un_buffer=recv_buffer(socket_cpu);
                actualizar_contexto(un_buffer);
                free(un_buffer->stream);
                free(un_buffer);
            break;

            case DESALOJO_ERROR_CM:
                //t_buffer* un_buffer;
                un_buffer=recv_buffer(socket_cpu);
                actualizar_contexto(un_buffer);
                free(un_buffer->stream);
                free(un_buffer);
            break;

            case PEDIDO_INSTRUCCION:
                //t_buffer* un_buffer;
                log_info(memo_logger_debug, "Pedido de instrucción CPU");
                un_buffer = recv_buffer(socket_cpu);
                pedido_instruccion(un_buffer);
                free(un_buffer->stream);
                free(un_buffer);
            break;

            case PEDIDO_LECTURA_CM:
                //t_buffer* un_buffer;
                log_info(memo_logger_debug, "Pedido de lectura CPU");
                un_buffer = recv_buffer(socket_cpu);
                read_from_dir(un_buffer);
                free(un_buffer->stream);
                free(un_buffer);
            break;

            case PEDIDO_ESCRITURA_CM:
                //t_buffer* un_buffer;
                log_info(memo_logger_debug, "Pedido escritura CPU");
                un_buffer = recv_buffer(socket_cpu);
                write_in_dir(un_buffer);
                free(un_buffer->stream);
                free(un_buffer);
            break;
            default:
            log_info(memo_logger_debug, "No se entendió el pedido de CPU");
            control_key = 0;
                break;
        }
    }
    //free(un_buffer->stream);
    //free(un_buffer);
}

void pedido_instruccion (t_buffer* un_buffer){
    int pid = extract_int_from_buffer(un_buffer);
    int tid = extract_int_from_buffer(un_buffer);
    int pc = extract_int_from_buffer(un_buffer);

    t_proceso* un_proceso = search_process_for(pid);

    t_hilo* un_hilo = search_thread_for(un_proceso, tid);

	//Obtener Instruccion especifica
	char* instruccion = get_instruction_from_index(un_hilo, pc);
    

    if(instruccion == NULL){
        log_error(memo_logger, "PID<%d> - TID<%d> - Nro de Instruccion <%d> NO VALIDA",pid ,un_hilo->TID, pc);
        //log_error(memo_logger_debug, "PID<%d> - TID<%d> - Nro de Instruccion <%d> NO VALIDA",pid ,un_hilo->TID, pc);
		exit(EXIT_FAILURE);
    }

    log_info(memo_logger, "## Obtener instrucción - (PID:TID) - (%d:%d) - Instrucción: %s", pid, tid, instruccion);
    //log_info(memo_logger_debug, "## Obtener instrucción - (PID:TID) - (%d:%d) - Instrucción: %s", pid, tid, instruccion);
    log_info(memo_logger_obligatorio, "## Obtener instrucción - (PID:TID) - (%d:%d) - Instrucción: %s", pid, tid, instruccion);

	//log_info(memo_logger, "<PID:%d> <IP:%d> <%s>", pid, pc, instruccion);

	//Enviar_instruccion a CPU
	reply_to_cpu_instruction_request(instruccion);
}
/*
int search_pid_value_cpu;
bool search_pid(t_proceso* proceso){
        return proceso->pid == search_pid_value_cpu;
}*/
/*
t_proceso* search_process_for(int pid){
    search_pid_value_cpu = pid;

    t_proceso* un_proceso = list_find(lista_procesos,(void*)search_pid);
    if(un_proceso == NULL){
        log_error(memo_logger, "PID<%d> No encontrado en la lista de procesos", pid);
        exit(EXIT_FAILURE);
    }
    return un_proceso;
}*/

int search_tid_value;
bool search_tid(t_hilo* un_hilo){
        return un_hilo->TID == search_tid_value;
}

t_hilo* search_thread_for(t_proceso* un_proceso, int tid){
    search_tid_value = tid;

    t_hilo* un_hilo = list_find(un_proceso->hilos,(void*)search_tid);
    if(un_hilo == NULL){
        log_error(memo_logger, "TID<%d> No encontrado en la lista de hilos del proceso %d", tid, un_proceso->pid);
        //log_error(memo_logger_debug, "TID<%d> No encontrado en la lista de hilos del proceso %d", tid, un_proceso->pid);
        exit(EXIT_FAILURE);
    }
    return un_hilo;
}

char* get_instruction_from_index(t_hilo* un_hilo, int indice_instruccion){
	char* instruccion_actual;
	if(indice_instruccion >= 0 && indice_instruccion < list_size(un_hilo->instrucciones)){
		instruccion_actual = list_get(un_hilo->instrucciones, indice_instruccion);
		return instruccion_actual;
	}
	else{
		return NULL;
	}
}

void reply_to_cpu_instruction_request(char* instruccion){
	usleep(RETARDO_RESPUESTA*1000);
	t_buffer* un_buffer = new_buffer();
	add_string_to_buffer(un_buffer, instruccion);
	t_paquete* un_paquete = create_super_pck(RESPUESTA_INSTRUCCION, un_buffer);
	send_pckg(un_paquete, socket_cpu);
	delete_pckg(un_paquete);
}

void pedido_contexto(t_buffer* un_buffer){
    int pid = extract_int_from_buffer(un_buffer);
    int tid = extract_int_from_buffer(un_buffer);

    t_proceso* un_proceso = search_process_for(pid);

    t_hilo* un_hilo = search_thread_for(un_proceso, tid);

    usleep(RETARDO_RESPUESTA*1000);
    t_buffer* otro_buffer = new_buffer();

    add_uint32_to_buffer(otro_buffer, un_hilo->PC);
    add_uint32_to_buffer(otro_buffer, un_hilo->AX);
    add_uint32_to_buffer(otro_buffer, un_hilo->BX);
    add_uint32_to_buffer(otro_buffer, un_hilo->CX);
    add_uint32_to_buffer(otro_buffer, un_hilo->DX);
    add_uint32_to_buffer(otro_buffer, un_hilo->EX);
    add_uint32_to_buffer(otro_buffer, un_hilo->FX);
    add_uint32_to_buffer(otro_buffer, un_hilo->GX);
    add_uint32_to_buffer(otro_buffer, un_hilo->HX);
    add_uint32_to_buffer(otro_buffer, un_proceso->base);
    add_uint32_to_buffer(otro_buffer, un_proceso->limite);
    log_info(memo_logger,"Se esta pasando base <%u> limite <%u>", un_proceso->base, un_proceso->limite);

	t_paquete* un_paquete = create_super_pck(RESPUESTA_CONTEXTO_CM, otro_buffer);
	send_pckg(un_paquete, socket_cpu);
	delete_pckg(un_paquete);
}

void actualizar_contexto(t_buffer* un_buffer){

    int pid = extract_int_from_buffer(un_buffer);
    int tid = extract_int_from_buffer(un_buffer);

    log_info(memo_logger, "## Contexto Solicitado - (PID:TID) - (%d:%d)", pid, tid);
    //log_info(memo_logger_debug, "## Contexto Solicitado - (PID:TID) - (%d:%d)", pid, tid);
    log_info(memo_logger_obligatorio, "## Contexto Solicitado - (PID:TID) - (%d:%d)", pid, tid);

    t_proceso* un_proceso = search_process_for(pid);

    t_hilo* un_hilo = search_thread_for(un_proceso, tid);
    un_hilo->PC = extract_uint32_from_buffer(un_buffer);
    un_hilo->AX = extract_uint32_from_buffer(un_buffer);
    un_hilo->BX = extract_uint32_from_buffer(un_buffer);
    un_hilo->CX = extract_uint32_from_buffer(un_buffer);
    un_hilo->DX = extract_uint32_from_buffer(un_buffer);
    un_hilo->EX = extract_uint32_from_buffer(un_buffer);
    un_hilo->FX = extract_uint32_from_buffer(un_buffer);
    un_hilo->GX = extract_uint32_from_buffer(un_buffer);
    un_hilo->HX = extract_uint32_from_buffer(un_buffer);

    //un_proceso->base = extract_uint32_from_buffer(un_buffer);
    //un_proceso->limite = extract_uint32_from_buffer(un_buffer);

	// habria que responder a cpu con "OK"
    usleep(RETARDO_RESPUESTA*1000);
    t_buffer* otro_buffer = new_buffer();
    add_int_to_buffer(otro_buffer,1);
    log_info(memo_logger, "## Contexto Actualizado - (PID:TID) - (%d:%d)", pid, tid);
    //log_info(memo_logger_debug, "## Contexto Actualizado - (PID:TID) - (%d:%d)", pid, tid);
    log_info(memo_logger_obligatorio, "## Contexto Actualizado - (PID:TID) - (%d:%d)", pid, tid);
    t_paquete* un_paquete = create_super_pck(RTA_ACTUALIZAR_CONTEXTO_CM, otro_buffer);
	send_pckg(un_paquete, socket_cpu);
	delete_pckg(un_paquete);
}

void read_from_dir(t_buffer* un_buffer){
    int pid = extract_int_from_buffer(un_buffer);
    int tid = extract_int_from_buffer(un_buffer);
	uint32_t dir_fisica = extract_uint32_from_buffer(un_buffer);

	//Copiar dato de uint32_t
	uint32_t valor;
    pthread_mutex_lock(&mutex_memoria_usuario);
    memcpy(&valor, espacio_usuario + dir_fisica, sizeof(uint32_t));
    pthread_mutex_unlock(&mutex_memoria_usuario);
	// LOG OBLIGATORIO
	log_info(memo_logger, "## Lectura - (PID:TID) - (%d:%d) - Dir. Física: %u - Tamaño: %ld", pid, tid, dir_fisica, sizeof(uint32_t));
    //log_info(memo_logger_debug, "## Lectura - (PID:TID) - (%d:%d) - Dir. Física: %u - Tamaño: %ld", pid, tid, dir_fisica, sizeof(uint32_t));
    log_info(memo_logger_obligatorio, "## Lectura - (PID:TID) - (%d:%d) - Dir. Física: %u - Tamaño: %ld", pid, tid, dir_fisica, sizeof(uint32_t));

	usleep(RETARDO_RESPUESTA*1000);
    t_buffer* otro_buffer = new_buffer();
	add_uint32_to_buffer(otro_buffer, valor);
    t_paquete* un_paquete = create_super_pck(RTA_PEDIDO_LECTURA_CM, otro_buffer);
	send_pckg(un_paquete, socket_cpu);
	delete_pckg(un_paquete);
}

void write_in_dir(t_buffer* un_buffer){

    int pid = extract_int_from_buffer(un_buffer);
    int tid = extract_int_from_buffer(un_buffer);
	
	uint32_t valor = extract_uint32_from_buffer(un_buffer);
    uint32_t dir_fisica = extract_uint32_from_buffer(un_buffer);

	log_info(memo_logger, "## Escritura - (PID:TID) - (%d:%d) - Dir. Física: %u - Tamaño: %ld", pid, tid, dir_fisica, sizeof(uint32_t));
    //log_info(memo_logger_debug, "## Escritura - (PID:TID) - (%d:%d) - Dir. Física: %u - Tamaño: %ld", pid, tid, dir_fisica, sizeof(uint32_t));
    log_info(memo_logger_obligatorio, "## Escritura - (PID:TID) - (%d:%d) - Dir. Física: %u - Tamaño: %ld", pid, tid, dir_fisica, sizeof(uint32_t));


	//Escribir en espacio de usuario
    pthread_mutex_lock(&mutex_memoria_usuario);
	memcpy(espacio_usuario + dir_fisica, &valor, sizeof(uint32_t));
    pthread_mutex_unlock(&mutex_memoria_usuario);

	// LOG OBLIGATORIO
	//log_info(memo_logger, "Acceso a espacio de usuario: PID: <%d> - Accion: <ESCRIBIR> - Direccion fisica: <%d> - Tamaño <%u>", pid, dir_fisica, TAM_PAGINA);

    usleep(RETARDO_RESPUESTA*1000);
	t_buffer* otro_buffer = new_buffer();
	add_int_to_buffer(otro_buffer, 1);
	t_paquete* un_paquete = create_super_pck(RTA_PEDIDO_ESCRITURA_CM, otro_buffer);
	send_pckg(un_paquete, socket_cpu);
	delete_pckg(un_paquete);
}
    