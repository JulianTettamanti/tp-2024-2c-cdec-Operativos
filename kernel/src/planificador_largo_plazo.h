#ifndef PLANIFICADOR_LARGO_PLAZO_H_
#define PLANIFICADOR_LARGO_PLAZO_H_

#include <kernel-gestor.h>
#include <kernel_memoria.h>
#include <atender_conexiones.h>
#include <pcb.h>
#include <planificador_corto_plazo.h>


void plp();
void finalizar_proceso(int pid);
void crear_hilo(int prioridad, int PID_asociado, char* path);
void finalizar_hilo(int TID, int pid);

#endif /*PLANIFICADOR_LARGO_PLAZO_H_*/