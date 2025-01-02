#include <mmu.h>

uint32_t mmu(uint32_t logic_adress){

	uint32_t offset = logic_adress;

	log_info(cpu_logger_debug,"contexto limite es: %u y contexto base es: %u",contexto->LIMITE,contexto->BASE);


    if(offset <= (contexto->LIMITE+1 - contexto->BASE)){
		uint32_t fisica_adress = contexto->BASE + offset;

		return fisica_adress;
	}else{
		log_info(cpu_logger, "Error de Traduccion PID: <%d> - TID: <%d> ", contexto->proceso_pid,contexto->proceso_tid);
		desalojar = true;
		desalojo_mssg = ERROR_SEGFAULT_CK;

		return -1;
	}
}