#include <cpu-gestor.h>
#include <mmu.h>

void ciclo_de_instruccion();

void decode_instruction();
void fetch_instruction();
void execute_instruction();
void check_interrupt();

bool instruction_validate(char* instruccion);
void* detectar_registro(char* RX);
void envios_desalojo(op_code mensaje_memoria,op_code mensaje_kernel,t_buffer * buffer_kernel,t_buffer * buffer_memoria);
t_paquete* create_super_pck_desalojo(op_code motivo,t_buffer* unBuffer);