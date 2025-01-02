/*#include <memoria_fs.h>
t_buffer* attend_memoria_fs(int socket_fs){
	t_buffer* un_buffer;
    int cod_op = recv_op(socket_fs);

    switch (cod_op) {

        case PEDIDO_CONTEXTO_CM:
            un_buffer=recv_buffer(socket_cpu);
            pedido_contexto(un_buffer);
        break;

    }
}*/