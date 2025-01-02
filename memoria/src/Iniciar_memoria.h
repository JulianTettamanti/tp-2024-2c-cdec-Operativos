#ifndef INICIAR_MEMORIA_H_
#define INICIAR_MEMORIA_H_
#include <memoria-gestor.h>

void* espacio_usuario;
void iniciar_memoria();
t_marco* marco_create(int base, bool libre, int index, int tam);
void iniciar_listas();
void iniciar_mutex();
void iniciar_semaforos();

#endif