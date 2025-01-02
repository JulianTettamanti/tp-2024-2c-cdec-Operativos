#include <execute_cicle.h>


//---------------------------CICLO DE INSTRUCCION----------------------------

void ciclo_de_instruccion(){

	//FETCH
	fetch_instruction();

	//DECODE
	sem_wait(&sem_control_fetch_decode); // este semaforo espera la llegada del paquete desde memoria
	decode_instruction();

	//EXECUTE
	sem_wait(&sem_control_decode_execute); // este semaforo espera que se haga el decode
	execute_instruction();

	//CHECK INTERRUPT
    check_interrupt();
}


void fetch_instruction(){

    t_paquete* super_pckg;
	t_buffer* unBuffer = new_buffer();
	// pedir al modulo de memoria la instruccion siguiente

	log_info(cpu_log_obligatorio, "TID: <%d> - FETCH - Program Counter: <%d>", contexto->proceso_tid, contexto->PC);
	add_int_to_buffer(unBuffer, contexto->proceso_pid);
    add_int_to_buffer(unBuffer, contexto->proceso_tid);
	add_int_to_buffer(unBuffer, contexto->PC);
	log_debug(cpu_logger_debug,"Pedido instrucción a memoria(fetch_instruction)");
	super_pckg = create_super_pck(PEDIDO_INSTRUCCION,unBuffer);
	send_pckg(super_pckg, socket_memoria);

	delete_pckg(super_pckg);
}



void decode_instruction(){

    char** bloques_instruccion = string_split(instruccion_actual," ");
	log_debug(cpu_logger_debug,"Instruccion a validar [Instruccion: %s]", bloques_instruccion[0] );
    if(instruction_validate(bloques_instruccion[0])){
		
		sem_post(&sem_control_decode_execute);
	}else{
		log_error(cpu_logger_debug, "Instruccion no encontrada: [TC: %d][Instruc_Header: %s]", contexto->proceso_tid, bloques_instruccion[0]);
		exit(EXIT_FAILURE); 
	}
	for (int i = 0; bloques_instruccion[i] != NULL; i++) {
        free(bloques_instruccion[i]);
    }
	free(bloques_instruccion);
    // Liberar el arreglo de punteros
}



void execute_instruction(){

    char** bloques_instruccion = string_split(instruccion_actual," ");

    if(strcmp(bloques_instruccion[0], "SET") == 0){//[SET][AX][valor]
        log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s> - <%s>", contexto->proceso_tid,  bloques_instruccion[0], bloques_instruccion[1], bloques_instruccion[2]);
        contexto->PC= contexto->PC+ 1;
        uint32_t* registro_referido = detectar_registro(bloques_instruccion[1]);
		if(registro_referido != NULL){
			*registro_referido = atoi(bloques_instruccion[2]);
		}
    }else if(strcmp(bloques_instruccion[0], "READ_MEM") == 0){//[READ_MEM][AX][BX]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s> - <%s>", contexto->proceso_tid,  bloques_instruccion[0], bloques_instruccion[1], bloques_instruccion[2]);
		contexto->PC = contexto->PC + 1;
		uint32_t* registro_direccion = detectar_registro(bloques_instruccion[2]);
		uint32_t* registro_datos = detectar_registro(bloques_instruccion[1]);
		t_buffer* buffer_memoria = new_buffer();

		uint32_t direccion = mmu(*registro_direccion);

		if(direccion != -1){
			add_int_to_buffer(buffer_memoria, contexto->proceso_pid);
			add_int_to_buffer(buffer_memoria, contexto->proceso_tid);

			add_uint32_to_buffer (buffer_memoria,direccion);
			t_paquete* pckgPedidoLectura = create_super_pck(PEDIDO_LECTURA_CM, buffer_memoria);
			send_pckg(pckgPedidoLectura , socket_memoria);
			delete_pckg(pckgPedidoLectura );
		
			sem_wait(&sem_control_peticion_lectura_a_memoria);
			log_info(cpu_log_obligatorio, "## TID: <%d> - Acción: <%s> - Dirección Física: <%s>",contexto->proceso_tid,bloques_instruccion[0],bloques_instruccion[2]);
			if(valor_lectura != NULL){
				*registro_datos = valor_lectura;
				log_info(cpu_logger_debug, "## se esta escribiendo el valor %u al registro %s %u", valor_lectura, bloques_instruccion[1], *registro_datos);
			}
		}
		
	}else if(strcmp(bloques_instruccion[0], "WRITE_MEM") == 0){//[WRITE_MEM][AX][BX]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s> - <%s>", contexto->proceso_tid,  bloques_instruccion[0], bloques_instruccion[1], bloques_instruccion[2]);
		contexto->PC = contexto->PC + 1;
		uint32_t* registro_direccion = detectar_registro(bloques_instruccion[1]);
		uint32_t* registro_datos = detectar_registro(bloques_instruccion[2]);
		t_buffer* buffer_memoria = new_buffer();

		log_info(cpu_logger_debug,"El registro direccion es: %u",*registro_direccion);
		uint32_t direccion = mmu(*registro_direccion);
		if(direccion != -1){

			add_int_to_buffer(buffer_memoria, contexto->proceso_pid);
			add_int_to_buffer(buffer_memoria, contexto->proceso_tid);

			add_uint32_to_buffer(buffer_memoria,*registro_datos);
			add_uint32_to_buffer(buffer_memoria,direccion);
			log_info(cpu_logger_debug,"Direccion fisica enviada: %d",direccion);
			t_paquete* pckgPedidoEscritura = create_super_pck(PEDIDO_ESCRITURA_CM, buffer_memoria);
			send_pckg(pckgPedidoEscritura , socket_memoria);
			delete_pckg(pckgPedidoEscritura );
			sem_wait(&sem_control_peticion_escritura_a_memoria);
			log_info(cpu_log_obligatorio, "## TID: <%d> - Acción: <%s> - Dirección Física: <%s>",contexto->proceso_tid,bloques_instruccion[0],bloques_instruccion[2]);

		}

		

	}else if(strcmp(bloques_instruccion[0], "SUM") == 0){//[SUM][destino:AX][origen:BX]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s> - <%s>",  contexto->proceso_tid, bloques_instruccion[0], bloques_instruccion[1], bloques_instruccion[2]);
		contexto->PC= contexto->PC+ 1;

		uint32_t* registro_referido_destino = detectar_registro(bloques_instruccion[1]);
		uint32_t* registro_referido_origen = detectar_registro(bloques_instruccion[2]);
		if(registro_referido_destino != NULL || registro_referido_origen != NULL){
			log_warning(cpu_logger_debug, "VALOR DESTINO ANTES DE SUMA %u", *registro_referido_destino);
			log_warning(cpu_logger_debug, "VALOR ORIGEN ANTES DE SUMA %u", *registro_referido_origen);
			*registro_referido_destino += *registro_referido_origen;
			log_warning(cpu_logger_debug, "RESULTADO SUMA %u", *registro_referido_destino);
		}
		

	}else if(strcmp(bloques_instruccion[0], "SUB") == 0){//[SUB][destino:AX][origen:BX]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s> - <%s>", contexto->proceso_tid, bloques_instruccion[0], bloques_instruccion[1], bloques_instruccion[2]);
		contexto->PC= contexto->PC+ 1;
		uint32_t* registro_referido_destino = detectar_registro(bloques_instruccion[1]);
		uint32_t* registro_referido_origen = detectar_registro(bloques_instruccion[2]);
		if(registro_referido_destino != NULL || registro_referido_origen != NULL){
			*registro_referido_destino -= *registro_referido_origen;
			log_warning(cpu_logger_debug, "RESULTADO RESTA %u", *registro_referido_destino);
		}

	}else if(strcmp(bloques_instruccion[0], "JNZ") == 0){// [JNZ][Registro][Instruccion]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s> - <%s>", contexto->proceso_tid, bloques_instruccion[0], bloques_instruccion[1], bloques_instruccion[2]);
		uint32_t* registro_referido = detectar_registro(bloques_instruccion[1]);
		//log_info(cpu_log_obligatorio, "REgistro_referido:%u", *registro_referido);
		if(registro_referido != NULL){
			if(*registro_referido != 0) {
				contexto->PC= atoi(bloques_instruccion[2]);
				//log_info(cpu_log_obligatorio, "Entre IF JNZ:");
			}else{
				contexto->PC++;
			}
		}
    }else if(strcmp(bloques_instruccion[0], "LOG") == 0){// [LOG][Registro]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s>", contexto->proceso_tid, bloques_instruccion[0], bloques_instruccion[1]);
		

		contexto->PC = contexto->PC + 1;
		log_info(cpu_logger_debug,"El registro es: %s",bloques_instruccion[1]);
		uint32_t* registro_referido = detectar_registro(bloques_instruccion[1]);
		log_info(cpu_logger_debug,"valor registro referido LOG: %u",*registro_referido);
		if(*registro_referido != NULL){
			//log_info(cpu_log_obligatorio, "Valor del registro <%s> es: %u",bloques_instruccion[1],*registro_referido);
			log_info(cpu_logger_debug, "Valor del registro <%s> es: %u",bloques_instruccion[1],*registro_referido);
		}
    }else if(strcmp(bloques_instruccion[0], "DUMP_MEMORY") == 0){// [DUMP_MEMORY]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s>", contexto->proceso_tid, bloques_instruccion[0]);

		contexto->PC = contexto->PC + 1;

		t_buffer* buffer_memoria = new_buffer();
		t_buffer* buffer_kernel = new_buffer();

		add_int_to_buffer(buffer_kernel,contexto->proceso_pid);
		add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
		
		envios_desalojo(DESALOJO_INSTRUCCION_CM,DUMP_MEMORY_CK,buffer_kernel,buffer_memoria);

	}else if(strcmp(bloques_instruccion[0], "IO") == 0){// [IO] [TIEMPO]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s>", contexto->proceso_tid, bloques_instruccion[0], bloques_instruccion[1]);

		contexto->PC = contexto->PC + 1;
		int tiempo = atoi(bloques_instruccion[1]);

		t_buffer* buffer_memoria = new_buffer();
		t_buffer* buffer_kernel = new_buffer();

		add_int_to_buffer(buffer_kernel,contexto->proceso_pid);
		add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
		add_int_to_buffer(buffer_kernel,tiempo);

		envios_desalojo(DESALOJO_INSTRUCCION_CM,IO_CK,buffer_kernel,buffer_memoria);

	}else if(strcmp(bloques_instruccion[0], "PROCESS_CREATE") == 0){// [PROCESS_CREATE] [ARCHIVO] [TAMAÑO] [PRIORIDAD]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s> - <%s> - <%s>", contexto->proceso_tid, bloques_instruccion[0], bloques_instruccion[1], bloques_instruccion[2], bloques_instruccion[3]);

		contexto->PC = contexto->PC + 1;
		char* archivo = bloques_instruccion[1];
		int tamanio = atoi(bloques_instruccion[2]);
		int prioridad = atoi(bloques_instruccion[3]);

		t_buffer* buffer_memoria = new_buffer();
		t_buffer* buffer_kernel = new_buffer();

		add_int_to_buffer(buffer_kernel,contexto->proceso_pid);  
		add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
		add_string_to_buffer(buffer_kernel,archivo);
		add_int_to_buffer(buffer_kernel,tamanio);
		add_int_to_buffer(buffer_kernel,prioridad);

		envios_desalojo(DESALOJO_INSTRUCCION_CM,PROCESS_CREATE_CK,buffer_kernel,buffer_memoria);

	}else if(strcmp(bloques_instruccion[0], "THREAD_CREATE") == 0){// [THREAD_CREATE] [ARCHIVO] [PRIORIDAD]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s> - <%s>", contexto->proceso_tid, bloques_instruccion[0], bloques_instruccion[1], bloques_instruccion[2]);
		contexto->PC = contexto->PC + 1;
		char* archivo = bloques_instruccion[1];
		int prioridad = atoi(bloques_instruccion[2]);

		t_buffer* buffer_memoria = new_buffer();
		t_buffer* buffer_kernel = new_buffer();

		add_int_to_buffer(buffer_kernel,contexto->proceso_pid);     
		add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
		add_string_to_buffer(buffer_kernel,archivo);
		add_int_to_buffer(buffer_kernel,prioridad);

		envios_desalojo(DESALOJO_INSTRUCCION_CM,THREAD_CREATE_CK,buffer_kernel,buffer_memoria);

	
	}else if(strcmp(bloques_instruccion[0], "THREAD_CANCEL") == 0){// [THREAD_CANCEL] [TID]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s>", contexto->proceso_tid, bloques_instruccion[0], bloques_instruccion[1]);
		contexto->PC = contexto->PC + 1;
		int tid_cancel = atoi(bloques_instruccion[1]);

		t_buffer* buffer_memoria = new_buffer();
		t_buffer* buffer_kernel = new_buffer();

		add_int_to_buffer(buffer_kernel,contexto->proceso_pid);  
		add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
		add_int_to_buffer(buffer_kernel,tid_cancel);

		envios_desalojo(DESALOJO_INSTRUCCION_CM,THREAD_CANCEL_CK,buffer_kernel,buffer_memoria);


	}else if(strcmp(bloques_instruccion[0], "THREAD_JOIN") == 0){// [THREAD_JOIN] [TID]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s>", contexto->proceso_tid, bloques_instruccion[0], bloques_instruccion[1]);
		contexto->PC = contexto->PC + 1;
		int tid_join = atoi(bloques_instruccion[1]);

		t_buffer* buffer_memoria = new_buffer();
		t_buffer* buffer_kernel = new_buffer();

		add_int_to_buffer(buffer_kernel,contexto->proceso_pid); 
		add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
		add_int_to_buffer(buffer_kernel,tid_join);

		envios_desalojo(DESALOJO_INSTRUCCION_CM,THREAD_JOIN_CK,buffer_kernel,buffer_memoria);

	}else if(strcmp(bloques_instruccion[0], "MUTEX_CREATE") == 0){// [MUTEX_CREATE][RECURSO_1]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s> - <%s>", contexto->proceso_tid, bloques_instruccion[0], bloques_instruccion[1], bloques_instruccion[2]);

		contexto->PC = contexto->PC + 1;
		char* recurso = bloques_instruccion[1];

		t_buffer* buffer_memoria = new_buffer();
		t_buffer* buffer_kernel = new_buffer();

		add_int_to_buffer(buffer_kernel,contexto->proceso_pid);  
		add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
		add_string_to_buffer(buffer_kernel,recurso);

		envios_desalojo(DESALOJO_INSTRUCCION_CM,MUTEX_CREATE_CK,buffer_kernel,buffer_memoria);

	}else if(strcmp(bloques_instruccion[0], "MUTEX_LOCK") == 0){// [MUTEX_LOCK][RECURSO_1]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s>", contexto->proceso_tid, bloques_instruccion[0], bloques_instruccion[1]);

		contexto->PC = contexto->PC + 1;
		char* recurso = bloques_instruccion[1];

		t_buffer* buffer_memoria = new_buffer();
		t_buffer* buffer_kernel = new_buffer();

		add_int_to_buffer(buffer_kernel,contexto->proceso_pid);
		add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
		add_string_to_buffer(buffer_kernel,recurso);

		envios_desalojo(DESALOJO_INSTRUCCION_CM,MUTEX_LOCK_CK,buffer_kernel,buffer_memoria);
	
	}else if(strcmp(bloques_instruccion[0], "MUTEX_UNLOCK") == 0){// [MUTEX_UNLOCK][RECURSO_1]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s> - <%s>", contexto->proceso_tid, bloques_instruccion[0], bloques_instruccion[1]);

		contexto->PC = contexto->PC + 1;
		char* recurso = bloques_instruccion[1];

		t_buffer* buffer_memoria = new_buffer();
		t_buffer* buffer_kernel = new_buffer();

		add_int_to_buffer(buffer_kernel,contexto->proceso_pid);
		add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
		add_string_to_buffer(buffer_kernel,recurso);

		envios_desalojo(DESALOJO_INSTRUCCION_CM,MUTEX_UNLOCK_CK,buffer_kernel,buffer_memoria);


	}else if(strcmp(bloques_instruccion[0], "THREAD_EXIT") == 0){// [THREAD_EXIT]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s>", contexto->proceso_tid, bloques_instruccion[0]);

		//desalojar = true;
		//desalojo_mssg = THREAD_EXIT_CK;
		//syscall_bloquea = 1;
		contexto->PC = contexto->PC + 1; //POR DEFAULT, PODRIA NO ESTAR
		
		t_buffer* buffer_memoria = new_buffer();
		t_buffer* buffer_kernel = new_buffer();

		add_int_to_buffer(buffer_kernel,contexto->proceso_pid);
		add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
		envios_desalojo(DESALOJO_INSTRUCCION_CM,THREAD_EXIT_CK,buffer_kernel,buffer_memoria);
		log_debug(cpu_logger_debug, "Se envio desalojo por thread exit a Kernel");
	
	}else if(strcmp(bloques_instruccion[0], "PROCESS_EXIT") == 0){// [PROCESS_EXIT]
		log_info(cpu_log_obligatorio, "TID: <%d> - Ejecutando: <%s>", contexto->proceso_tid, bloques_instruccion[0]);

		//desalojar = true;
		//desalojo_mssg = PROCESS_EXIT_CK;
		 //ACA SI VA PORQUE SI EL HILO QUE EJECUTO NO ES EL 0, SE IGNORA LA INSTRUCCION
		t_buffer* buffer_memoria = new_buffer();
		t_buffer* buffer_kernel = new_buffer();

		add_int_to_buffer(buffer_kernel,contexto->proceso_pid);
		add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
		envios_desalojo(DESALOJO_INSTRUCCION_CM,PROCESS_EXIT_CK,buffer_kernel,buffer_memoria);
		if(!syscall_bloquea){
			contexto->PC = contexto->PC + 1;
		}
		log_debug(cpu_logger_debug, "Se envio desalojo por process exit a Kernel");
		
	}

	for (int i = 0; bloques_instruccion[i] != NULL; i++) {
        free(bloques_instruccion[i]);
    }
    // Liberar el arreglo de punteros
    free(bloques_instruccion);
}


void check_interrupt(){

    pthread_mutex_lock(&mutex_interruptFlag);
	bool bool_interrupt = interruptFlag;
	pthread_mutex_unlock(&mutex_interruptFlag);
	if(bool_interrupt == true){
		desalojar = bool_interrupt;
	}
}



//-----------FUNCIONES PARA INSTRUCCIONES-------------------------------

bool instruction_validate(char* instruccion){
    char* posible_instructions[] = {"SET","WRITE_MEM","READ_MEM","SUB","SUM","JNZ","LOG","DUMP_MEMORY","IO","PROCESS_CREATE","THREAD_CREATE"
									,"THREAD_JOIN","THREAD_CANCEL","MUTEX_CREATE","MUTEX_LOCK","MUTEX_UNLOCK","THREAD_EXIT","PROCESS_EXIT"};
	char* syscalls[] = {"DUMP_MEMORY","IO","PROCESS_CREATE","THREAD_CREATE"
									,"THREAD_JOIN","THREAD_CANCEL","MUTEX_CREATE","MUTEX_LOCK","MUTEX_UNLOCK","THREAD_EXIT","PROCESS_EXIT"};
	// AL FINAL NO SE USARON
	char* syscalls_block[] = {"DUMP_MEMORY","IO","THREAD_JOIN","MUTEX_LOCK"};
	char* syscalls_noBlock[] = {"PROCESS_CREATE","THREAD_CREATE","THREAD_CANCEL","MUTEX_CREATE","MUTEX_UNLOCK","THREAD_EXIT","PROCESS_EXIT"};

	int i = 0;
	int j = 0;
/*
	while(j < 11){

		if(strcmp(syscalls[j],instruccion)==0){
			log_info(cpu_logger_debug,"Entre a hacer 1 el flag de syscall, con [j: %d]", j);
			enmascarar_interrupcionFlag = 1;
			break;
		}
		else{enmascarar_interrupcionFlag = 0;}
		j++;
		
	}*/

	while(i < (sizeof(posible_instructions))){

		if(strcmp(posible_instructions[i],instruccion)==0){return true;}
		i++;
		
	}

	return false;
}


void* detectar_registro(char* RX){
	if(strcmp(RX, "AX") == 0){
		return &(contexto->AX);
	}else if(strcmp(RX, "BX") == 0){
		log_info(cpu_logger_debug,"SOY BX");
		return &(contexto->BX);
	}else if(strcmp(RX, "CX") == 0){
		return &(contexto->CX);
	}else if(strcmp(RX, "DX") == 0){
		return &(contexto->DX);
	}else if(strcmp(RX, "EX") == 0){
		return &(contexto->EX);
	}else if(strcmp(RX, "FX") == 0){
		return &(contexto->FX);
	}else if(strcmp(RX, "GX") == 0){
		return &(contexto->GX);
	}else if(strcmp(RX, "HX") == 0){
		return &(contexto->HX);
	}else if(strcmp(RX, "BASE") == 0){
		return &(contexto->BASE);
	}else if(strcmp(RX, "LIMITE") == 0){
		return &(contexto->LIMITE);
	}else if(strcmp(RX, "PC") == 0){
		return &(contexto->PC);
	}else{
		//log_info(cpu_log_obligatorio, "Registro desconocido: %s", RX);
		desalojar = true;
		desalojo_mssg = ERROR_EXC;
		return NULL;
	}
}

t_paquete* create_super_pck_desalojo(op_code motivo,t_buffer* unBuffer){

	
	log_debug(cpu_logger_debug, "estoy mandando el pid %d nieri", contexto->proceso_pid);
	add_int_to_buffer(unBuffer, contexto->proceso_pid);
	add_int_to_buffer(unBuffer, contexto->proceso_tid);
	add_uint32_to_buffer(unBuffer, contexto->PC);
	add_uint32_to_buffer(unBuffer, contexto->AX);
	add_uint32_to_buffer(unBuffer, contexto->BX);
	add_uint32_to_buffer(unBuffer, contexto->CX);
	add_uint32_to_buffer(unBuffer, contexto->DX);
	add_uint32_to_buffer(unBuffer, contexto->EX);
	add_uint32_to_buffer(unBuffer, contexto->FX);
	add_uint32_to_buffer(unBuffer, contexto->GX);
	add_uint32_to_buffer(unBuffer, contexto->HX);
	add_uint32_to_buffer(unBuffer, contexto->BASE);
	add_uint32_to_buffer(unBuffer, contexto->LIMITE);

	t_paquete* pckg_desalojo = create_super_pck(motivo,unBuffer);

	return pckg_desalojo;

}

void envios_desalojo(op_code mensaje_memoria,op_code mensaje_kernel,t_buffer * buffer_kernel,t_buffer * buffer_memoria){

	t_paquete* pckgDesalojoKernel = create_super_pck(mensaje_kernel, buffer_kernel);
	t_paquete* pckgDesalojoMemoria = create_super_pck_desalojo(mensaje_memoria, buffer_memoria);

	//desalojar = true; //Como esto tiene que desalojar al proceso SALGO!

	send_pckg(pckgDesalojoMemoria , socket_memoria);
	delete_pckg(pckgDesalojoMemoria );
	send_pckg(pckgDesalojoKernel , socket_kernel_dispatch);
	delete_pckg(pckgDesalojoKernel );

	sem_wait(&sem_retorno_syscalls);

	if(syscall_bloquea){
		log_debug(cpu_logger_debug, "recibi respuesta del kernel para desalojar syscall");
		desalojar = true;
		desalojo_mssg = DESALOJO_SYSCALL_CK;
	}
}

