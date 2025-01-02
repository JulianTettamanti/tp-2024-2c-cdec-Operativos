#ifndef PLANIFICADOR_CORTO_PLAZO_H_
#define PLANIFICADOR_CORTO_PLAZO_H_

#include <kernel-gestor.h>
#include <kernel_memoria.h>
#include <pcb.h>



void _atender_FIFO();
void _atender_RR();
void _atender_PRIORIDADES();
bool comparador_ready(t_tcb* primero, t_tcb* segundo);
t_tcb* __maxima_prioridad(t_tcb* void_1, t_tcb* void_2);
int generar_ticket();
void pcp();

#endif /*PLANIFICADOR_CORTO_PLAZO_H_*/