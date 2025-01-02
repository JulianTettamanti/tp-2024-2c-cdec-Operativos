#ifndef CPU_MEMORIA_H_
#define CPU_MEMORIA_H_

#include "cpu-gestor.h"

void attend_cpu_memoria();
void recv_contexto(t_buffer* unBuffer);
void recv_instruction(t_buffer* unBuffer);
void recv_pagina(t_buffer* unBuffer);
void recv_rta_escritura(t_buffer* unBuffer);
void recv_rta_lectura(t_buffer* unBuffer);
#endif