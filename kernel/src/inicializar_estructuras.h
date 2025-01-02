#ifndef INICIALIZAR_ESTRUCTURAS_H_
#define INICIALIZAR_ESTRUCTURAS_H_

#include "kernel-gestor.h"


void initialize_logger();
void initialize_config(char* config_path);
void initialize_list();
void initialize_semaphores();
void initialize_kernel(char* config_path);



#endif /* INICIALIZAR_ESTRUCTURAS_H_ */