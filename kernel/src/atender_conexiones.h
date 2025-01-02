#ifndef ATENDER_CONEXIONES_H_
#define ATENDER_CONEXIONES_H_

#include "kernel-gestor.h"
#include "kernel_memoria.h"

void initialize_conexiones();
void conexion_memoria(t_paquete* un_paquete);
void conexion_memoria_solo_aviso(t_paquete* un_paquete);


#endif /* ATENDER_CONEXIONES_H_ */