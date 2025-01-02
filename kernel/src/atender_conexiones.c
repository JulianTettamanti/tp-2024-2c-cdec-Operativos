#include "atender_conexiones.h"

void initialize_conexiones(){

	socket_cpu_dispatch = new_connection(IP_CPU, PUERTO_CPU_DISPATCH);
    log_info(kernel_logger, "CONEXION EXITOSA CON CPU - DISPATCH");
	socket_cpu_interrupt = new_connection(IP_CPU, PUERTO_CPU_INTERRUPT);
    log_info(kernel_logger, "CONEXION EXITOSA CON CPU - INTERRUPT");
}

void conexion_memoria(t_paquete* un_paquete){

    log_info(kernel_logger, "ANTES DE NEW CONNECTION");
    int socket_memoria = new_connection(IP_MEMORIA, PUERTO_MEMORIA);
    log_info(kernel_logger, "DESPUES DE NEW CONNECTION");
    send_pckg(un_paquete, socket_memoria);
    attend_kernel_memoria(socket_memoria);
    delete_pckg(un_paquete);
    freeconnection(socket_memoria);
}
//esta funcion se utiliza para cuando se crea un nuevo hilo
void conexion_memoria_solo_aviso(t_paquete* un_paquete){

    int socket_memoria = new_connection(IP_MEMORIA, PUERTO_MEMORIA);
    send_pckg(un_paquete, socket_memoria);
    delete_pckg(un_paquete);
    freeconnection(socket_memoria);
}
