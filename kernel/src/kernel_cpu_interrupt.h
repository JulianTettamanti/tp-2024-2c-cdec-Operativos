#ifndef KERNEL_CPU_INTERRUPT_H_
#define KERNEL_CPU_INTERRUPT_H_


#include <kernel-gestor.h>
#include <pcb.h>



void attend_kernel_cpu_interrupt();
void _gestionar_interrupt();

#endif /*KERNEL_CPU_INTERRUPT_H_*/