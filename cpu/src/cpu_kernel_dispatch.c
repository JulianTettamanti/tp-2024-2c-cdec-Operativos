#include <cpu_kernel_dispatch.h>

void attend_cpu_kernel_dispatch(){
    bool control_key = 1;
	t_buffer* unBuffer;
    while (control_key) {
		int cod_op = recv_op(socket_kernel_dispatch);
//		log_debug(cpu_logger_debug,"El cod_op recibido en CPU-KERNEL DISPATCH [COD_OP: %d", cod_op);
		switch (cod_op) {
		    case EJECUTAR_HILO_KC:
			    unBuffer = recv_buffer(socket_kernel_dispatch);
				pthread_t hilo_cpu_kernel_process;
				log_info(cpu_logger,"Se atiende un nuevo HILO!");
				pthread_create(&hilo_cpu_kernel_process,NULL,(void*)atender_proceso_del_kernel,unBuffer);
				pthread_join(hilo_cpu_kernel_process, NULL);
			    break;
		    case PAQUETE:
			    break;
		    case -1:
			    log_error(cpu_logger, "DESCONEXION DE KERNEL - DISPATCH");
			    control_key = 0;
                break;
		    default:
			    log_warning(cpu_logger,"OPERACION DESCONOCIDA - CPU - DISPATCH");
			    break;
		}
	}
}

//falta implementar los semaforos y los flags
void atender_proceso_del_kernel(t_buffer* unBuffer){

    log_debug(cpu_logger_debug,"Se esta atendiendo al kernel");
	pthread_mutex_lock(&mutex_manejo_contexto);
	init_contexto(unBuffer); 
	pthread_mutex_unlock(&mutex_manejo_contexto);

	while(1){


		//Inicicar ciclo de instruccion
		//--------------------------------------------------------------------------------------------------------
		ciclo_de_instruccion();
		//--------------------------------------------------------------------------------------------------------

		log_debug(cpu_logger_debug,"Sigo en el ciclo de instrucción [Desalojar: %d] (Atender_proceso_del_kernel", desalojar);
		if(desalojar){break;}
	}

	log_debug(cpu_logger_debug,"Salí del ciclo de instrccion (Atender_proceso_del_kernel");
	//pthread_mutex_lock(&mutex_manejo_contexto);

//-----------------------------DESALOJAR------------------------------------------------------------
	manejo_desalojo();
//---------------------------------------------------MANEJO DESALOJO FIN----------------------------------------------------------------------------------
	log_warning(cpu_logger, "Proceso_desalojado <PID:%d>", contexto->proceso_pid);

	pthread_mutex_lock(&mutex_manejo_contexto);
	delete_contexto();
	pthread_mutex_unlock(&mutex_manejo_contexto);

	log_warning(cpu_logger, "Todo el contexto se elimino correctamente .....");

	//pthread_mutex_unlock(&mutex_manejo_contexto);

}

void init_contexto(t_buffer* unBuffer){

	contexto = malloc(sizeof(t_contexto));
	contexto_interrupt = malloc(sizeof(t_interrupt));
	contexto->proceso_pid = extract_int_from_buffer(unBuffer);// MODIFICAR EL ORDEN
	contexto->proceso_tid = extract_int_from_buffer(unBuffer);
	contexto_interrupt->interrupt_pid = -1;
	contexto_interrupt->interrupt_tid = -1;
	contexto_interrupt->interrupt_name = NULL;
	log_info(cpu_logger_debug, "Se esta atendiendo [TID %d], [PID %d]", contexto->proceso_tid, contexto->proceso_pid);
	t_buffer* buffer_memoria = new_buffer();

	add_int_to_buffer(buffer_memoria,contexto->proceso_pid);
	add_int_to_buffer(buffer_memoria,contexto->proceso_tid);

	t_paquete* pckgPedidoContexto = create_super_pck(PEDIDO_CONTEXTO_CM, buffer_memoria);
	send_pckg(pckgPedidoContexto , socket_memoria);
	delete_pckg(pckgPedidoContexto );
	log_info(cpu_log_obligatorio, "## TID: <%d> - ## PID <%d> - Solicito Contexto Ejecución", contexto->proceso_tid, contexto->proceso_pid);
	sem_wait(&sem_control_peticion_contexto_memoria);

	log_info(cpu_logger,"Se recibio un contexto con exito");
	free(unBuffer);

	//avisar_kernel_contexto();


}

void manejo_desalojo(){

			
		log_info(cpu_logger_debug," [syscall_bloquea: %d]- [desalojar: %d]- [desalojo mensaje: %d]", syscall_bloquea,desalojar,desalojo_mssg);
		

		if(desalojar && interruptFlag && !syscall_bloquea){
			log_debug(cpu_logger_debug, "Se ingreso a desalojar interrupcion");
			log_debug(cpu_logger_debug,"El tipo de interrupción es: %s",contexto_interrupt->interrupt_name);
			//desalojar por intr
			
			if(!strcmp(contexto_interrupt->interrupt_name,"DESALOJO_RR")){
				desalojo_mssg = DESALOJO_INTR;
				log_debug(cpu_logger_debug,"El tipo de interrupción es: %s",contexto_interrupt->interrupt_name);
			}

			t_buffer* buffer_memoria = new_buffer();
			t_buffer* buffer_kernel = new_buffer();

			add_int_to_buffer(buffer_kernel,contexto->proceso_pid);
			add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
			t_paquete* pckgDesalojoKernel = create_super_pck(DESALOJO_QUAMTUN_CK, buffer_kernel);
			t_paquete* pckgDesalojoMemoria = create_super_pck_desalojo(desalojo_mssg, buffer_memoria);

			send_pckg(pckgDesalojoMemoria , socket_memoria);
			delete_pckg(pckgDesalojoMemoria );
			send_pckg(pckgDesalojoKernel , socket_kernel_interrupt);
			delete_pckg(pckgDesalojoKernel );

		}else if(desalojar && (desalojo_mssg == DESALOJO_SYSCALL_CK )){
				//desalojar por syscall
				log_debug(cpu_logger_debug, "Se ingreso a desalojar por SYSCALL");
				t_buffer* buffer_kernel = new_buffer();
				t_paquete* pckgDesalojoKernel = create_super_pck(DESALOJO_SYSCALL_CK,buffer_kernel);

				send_pckg(pckgDesalojoKernel , socket_kernel_dispatch);
				delete_pckg(pckgDesalojoKernel );

				
				
		}else if(desalojar && (desalojo_mssg == THREAD_EXIT_CK /*|| desalojo_mssg == PROCESS_EXIT_CK*/ )){
				
				//desalojar por exit 
				log_debug(cpu_logger_debug, "Se ingreso a desalojar por exit, con mensaje desalojo <%d> ", desalojo_mssg);
				t_buffer* buffer_memoria = new_buffer();
				t_buffer* buffer_kernel = new_buffer();
				
				add_int_to_buffer(buffer_kernel,contexto->proceso_pid);
				add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
				//envios_desalojo(DESALOJO_INSTRUCCION_CM,desalojo_mssg,buffer_kernel,buffer_memoria);

				t_paquete* pckgDesalojoKernel = create_super_pck(desalojo_mssg, buffer_kernel);
				t_paquete* pckgDesalojoMemoria = create_super_pck_desalojo(DESALOJO_INSTRUCCION_CM, buffer_memoria);

				send_pckg(pckgDesalojoMemoria , socket_memoria);
				delete_pckg(pckgDesalojoMemoria );
				send_pckg(pckgDesalojoKernel , socket_kernel_dispatch);
				delete_pckg(pckgDesalojoKernel );
				log_debug(cpu_logger_debug, "Se envio desalojo por exit a Kernel");

		}else if (desalojar && (desalojo_mssg == PROCESS_EXIT_CK)){
			log_debug(cpu_logger_debug, "Se ingreso a desalojar por exit");
			t_buffer* buffer_kernel = new_buffer();
			t_paquete* pckgDesalojoKernel = create_super_pck(DESALOJO_SYSCALL_CK,buffer_kernel);

			send_pckg(pckgDesalojoKernel , socket_kernel_dispatch);
			delete_pckg(pckgDesalojoKernel );
		
		}else if(desalojar && (desalojo_mssg == ERROR_EXC || desalojo_mssg ==ERROR_SEGFAULT_CK)){
				
				//desalojar por error
				log_debug(cpu_logger_debug, "Se ingreso a desalojar por error");
				t_buffer* buffer_memoria = new_buffer();
				t_buffer* buffer_kernel = new_buffer();

				add_int_to_buffer(buffer_kernel,contexto->proceso_pid);
				add_int_to_buffer(buffer_kernel,contexto->proceso_tid);
				envios_desalojo(DESALOJO_ERROR_CM,desalojo_mssg,buffer_kernel,buffer_memoria);
				log_debug(cpu_logger_debug, "Se envio desalojo por ERROR a Kernel");

		}
}

void delete_contexto(){
	free(contexto);
	if(contexto_interrupt->interrupt_name != NULL){free(contexto_interrupt->interrupt_name);}
	free(contexto_interrupt);

	pthread_mutex_lock(&mutex_interruptFlag);
	interruptFlag = false;
	pthread_mutex_unlock(&mutex_interruptFlag);
	desalojar = false;

	syscall_bloquea = 0;

}

void avisar_kernel_contexto(){
	t_buffer* buffer_kernel = new_buffer();
	t_paquete* avisoKernel = create_super_pck(CONTEXTO_LISTO_CK, buffer_kernel);
	send_pckg(avisoKernel , socket_kernel_dispatch);
	delete_pckg(avisoKernel );
}

