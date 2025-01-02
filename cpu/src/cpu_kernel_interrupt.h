#ifndef CPU_KERNEL_INTERRUPT_H_
#define CPU_KERNEL_INTERRUPT_H_

#include "cpu-gestor.h"

void attend_cpu_kernel_interrupt();
void interrupt_manager(t_buffer* unBuffer);
void atender_respuesta_syscall(t_buffer *unBuffer);

#endif