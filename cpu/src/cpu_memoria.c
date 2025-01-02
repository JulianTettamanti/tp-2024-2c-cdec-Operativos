#include <cpu_memoria.h>

void attend_cpu_memoria(){
    bool control_key = 1;
    while (control_key) {
		int cod_op = recv_op(socket_memoria);
		t_buffer* unBuffer;
		switch (cod_op) {
		    case MENSAJE:
			    //
			    break;
			
			case RESPUESTA_INSTRUCCION:
				unBuffer = recv_buffer(socket_memoria);
				recv_instruction(unBuffer);
				free(unBuffer);
			    break;
			case RESPUESTA_CONTEXTO_CM:
				unBuffer = recv_buffer(socket_memoria);
				recv_contexto(unBuffer);
				free(unBuffer);
			    break;
			case RTA_CONSULTAR_PAGINA_CM:
				unBuffer = recv_buffer(socket_memoria);
				recv_pagina(unBuffer);
				free(unBuffer);
			    break;
		    case RTA_PEDIDO_ESCRITURA_CM:
			    unBuffer = recv_buffer(socket_memoria);
				recv_rta_escritura(unBuffer);
				free(unBuffer);
			    break;
			case RTA_PEDIDO_LECTURA_CM:
				unBuffer = recv_buffer(socket_memoria);
				recv_rta_lectura(unBuffer);
				free(unBuffer);
			    break;
				case RTA_ACTUALIZAR_CONTEXTO_CM:
				unBuffer = recv_buffer(socket_memoria);
				int rta = extract_int_from_buffer(unBuffer);
				if (rta == 0){
					log_error(cpu_logger, "ACTUALIZACIÓN CONTEXTO EN MEMORIA FALLÓ");
				}
				free(unBuffer);
				break;
		    case -1:
			    log_error(cpu_logger, "DESCONEXION DE MEMORIA");
			    control_key = 0;
                break;
		    default:
			    log_warning(cpu_logger,"OPERACION DESCONOCIDA - MEMORIA");
			    break;
		}
	}
}

void recv_contexto(t_buffer* unBuffer){


	contexto->PC = extract_uint32_from_buffer(unBuffer);
	contexto->AX = extract_uint32_from_buffer(unBuffer);
	contexto->BX = extract_uint32_from_buffer(unBuffer);
	contexto->CX = extract_uint32_from_buffer(unBuffer);
	contexto->DX = extract_uint32_from_buffer(unBuffer);
	contexto->EX = extract_uint32_from_buffer(unBuffer);
	contexto->FX = extract_uint32_from_buffer(unBuffer);
	contexto->GX = extract_uint32_from_buffer(unBuffer);
	contexto->HX = extract_uint32_from_buffer(unBuffer);
	contexto->BASE = extract_uint32_from_buffer(unBuffer);
	contexto->LIMITE = extract_uint32_from_buffer(unBuffer);
	log_info(cpu_log_obligatorio, "## TID: <%d> - ## PID <%d> - - Actualizo Contexto Ejecución", contexto->proceso_tid, contexto->proceso_pid);
	log_info(cpu_log_obligatorio, "## TID: <%d> - Actualizo Contexto Ejecución", contexto->proceso_tid);
	sem_post(&sem_control_peticion_contexto_memoria);

}

void recv_instruction(t_buffer* unBuffer){
	log_info(cpu_logger,"Recive la instr");
	if( instruccion_actual != NULL){free(instruccion_actual);}
	instruccion_actual = extract_string_from_buffer(unBuffer);
	sem_post(&sem_control_fetch_decode);
	log_info(cpu_logger,"levanto el sem");
}

void recv_pagina(t_buffer* unBuffer){
	marco = extract_int_from_buffer(unBuffer);
	sem_post(&sem_control_peticion_marco_a_memoria);
}

void recv_rta_escritura(t_buffer* unBuffer){
	int RTA = extract_int_from_buffer(unBuffer);
	log_info(cpu_logger_debug,"Respuesta de memoria fue: %d",RTA);
	if (RTA == 1){
		sem_post (&sem_control_peticion_escritura_a_memoria);
	}
}

void recv_rta_lectura(t_buffer* unBuffer){
	valor_lectura = extract_int_from_buffer(unBuffer);
	log_info(cpu_logger_debug,"se leyo el valor: %d",valor_lectura);
	sem_post (&sem_control_peticion_lectura_a_memoria);
}