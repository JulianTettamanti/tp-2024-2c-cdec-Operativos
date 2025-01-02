#include "utils.h"

void say_hi(char* quien) {
    printf("Hola desde %s!!\n", quien);
}


// CREAR NUEVA CONEXION
int new_connection(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,
                         server_info->ai_socktype,
                         server_info->ai_protocol);;

	// Ahora que tenemos el socket, vamos a conectarlo
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);


	freeaddrinfo(server_info);

	return socket_cliente;
}

//INICIAR SERVIDOR
int server_start(char* PUERTO, t_log* logger, const char* mensajeServidor)
{

	int socket_servidor;

	struct addrinfo hints, *servinfo;//, *p; no usamos esta variable p

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PUERTO, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,
                        servinfo->ai_socktype,
                        servinfo->ai_protocol);

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_info(logger, "%s", mensajeServidor);

	return socket_servidor;
}

//ESPERAR LA CONEXION DE UN CLIENTE
int wait_client(int socket_servidor, t_log* logger, char* mensajeCliente)
{
	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL);
 	log_info(logger, "%s", mensajeCliente);
	return socket_cliente;
}

int recv_op(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

//MANDAR MENSAJE DESDE UN CLIENTE AL SERVIDOR
void send_msg(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = pckg_serialize(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	delete_pckg(paquete);
}

//FUNCIONES PARA MANEJAR BUFFER
t_buffer* new_buffer()
{
	t_buffer* unBuffer = malloc(sizeof(t_buffer));
	unBuffer->size = 0;
	unBuffer->stream = NULL;
	return unBuffer;
}

//FUNCIONES PARA AGREGAR AL BUFFER
void add_to_buffer(t_buffer* unBuffer, void* new_stream, int new_size)
{
	if(unBuffer->size == 0){
		unBuffer->stream = malloc(sizeof(int) + new_size);
		memcpy(unBuffer->stream, &new_size, sizeof(int));
		memcpy(unBuffer->stream + sizeof(int), new_stream, new_size);
	}else{
		unBuffer->stream = realloc(unBuffer->stream, unBuffer->size + sizeof(int) + new_size);
		memcpy(unBuffer->stream + unBuffer->size, &new_size, sizeof(int));
		memcpy(unBuffer->stream + unBuffer->size + sizeof(int), new_stream, new_size);
	}
	
	unBuffer->size += sizeof(int);
	unBuffer->size += new_size;
}

void add_int_to_buffer(t_buffer* unBuffer, int int_value){
	add_to_buffer(unBuffer, &int_value, sizeof(int));
}

void add_uint32_to_buffer(t_buffer* unBuffer, uint32_t uint32_value){
	add_to_buffer(unBuffer, &uint32_value, sizeof(uint32_t));
}

void add_string_to_buffer(t_buffer* unBuffer, char* string_value){
	add_to_buffer(unBuffer, string_value, strlen(string_value) + 1);
}


//FUNCIONES PARA RECIBIR DEL BUFFER
t_buffer* recv_buffer(int socket_cliente)
{
	t_buffer* unBuffer = malloc(sizeof(t_buffer));

	if(recv(socket_cliente, &(unBuffer->size), sizeof(int), MSG_WAITALL) > 0){
		unBuffer->stream = malloc(unBuffer->size);

		if(recv(socket_cliente, unBuffer->stream, unBuffer->size, MSG_WAITALL) > 0){
			return unBuffer;
		}else{
			perror("Error al recibir el void* del buffer de la conexion");
			exit(EXIT_FAILURE);
		}
	}else{
		perror("Error al recibir el tamaño del buffer de la conexion");
		exit(EXIT_FAILURE);
	}
	

	return unBuffer;
}

//FUNCIONES PARA RECIBIR DEL BUFFER
void* extract_from_buffer(t_buffer* unBuffer){
	if(unBuffer->size == 0){
		perror("\n[ERROR] Al intentar extraer un contenido de un buffer vacio\n");
		exit(EXIT_FAILURE);
	}

	if(unBuffer->size < 0){
		perror("\n[ERROR] size negativo\n");
		exit(EXIT_FAILURE);
	}

	int ext_size;
	memcpy(&ext_size, unBuffer->stream, sizeof(int));
	void* ext_stream = malloc(ext_size);
	memcpy(ext_stream, unBuffer->stream + sizeof(int), ext_size);

	int new_size = unBuffer->size - sizeof(int) - ext_size;
	if(new_size == 0){
		unBuffer->size = 0;
		free(unBuffer->stream);
		unBuffer->stream = NULL;
		return ext_stream;
	}

	if(new_size < 0){
		perror("\n[ERROR] buffer con tamaño negativo");
		exit(EXIT_FAILURE);
	}

	void* new_stream = malloc(new_size);
	memcpy(new_stream, unBuffer->stream + sizeof(int) + ext_size, new_size);
	free(unBuffer->stream);
	unBuffer->size = new_size;
	unBuffer->stream = new_stream;

	return ext_stream;
}

int extract_int_from_buffer(t_buffer* unBuffer){
	int* ext_int = extract_from_buffer(unBuffer);
	int value = *ext_int;
	free(ext_int);
	return value;
}

char* extract_string_from_buffer(t_buffer* unBuffer){
	char* ext_string = extract_from_buffer(unBuffer);
	return ext_string;
}

uint32_t extract_uint32_from_buffer(t_buffer* unBuffer){
	uint32_t* ext_uint32 = extract_from_buffer(unBuffer);
	uint32_t value = *ext_uint32;
	free(ext_uint32);
	return value;
}


void* rcv_choclo_from_buffer(t_buffer* coso){
	if(coso->size == 0){
		printf("\n[ERROR] Al intentar extraer un contenido de un t_buffer vacio\n\n");
		exit(EXIT_FAILURE);
	}

	if(coso->size < 0){
		printf("\n[ERROR] Esto es raro. El t_buffer contiene un size NEGATIVO \n\n");
		exit(EXIT_FAILURE);
	}

	int size_choclo;
	void* choclo;
	memcpy(&size_choclo, coso->stream, sizeof(int));
	choclo = malloc(size_choclo);
	memcpy(choclo, coso->stream + sizeof(int), size_choclo);

	int nuevo_size = coso->size - sizeof(int) - size_choclo;
	if(nuevo_size == 0){
		free(coso->stream);
		coso->stream = NULL;
		coso->size = 0;
		return choclo;
	}
	if(nuevo_size < 0){
		printf("\n[ERROR_CHICLO]: BUFFER CON TAMAÑO NEGATIVO\n\n");
		//free(choclo);
		//return "";
		exit(EXIT_FAILURE);
	}
	void* nuevo_choclo = malloc(nuevo_size);
	memcpy(nuevo_choclo, coso->stream + sizeof(int) + size_choclo, nuevo_size);
	free(coso->stream);
	coso->stream = nuevo_choclo;
	coso->size = nuevo_size;

	return choclo;
}



//FUNCIONES PARA MANEJAR PAQUETES

t_paquete* new_pckg(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	new_buffer(paquete);
	return paquete;
}


t_paquete* create_super_pck(op_code code, t_buffer* unBuffer){

	t_paquete* unPaquete = malloc(sizeof(t_paquete));
	unPaquete->codigo_operacion = code;
	unPaquete->buffer = unBuffer;
	return unPaquete;
}

void* pckg_serialize(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}


//FUNCIONES PARA AGREGAR LA PAQUETE
void addto_pckg(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void add_int_to_super_pck(t_paquete* paquete, int numero){
	if(paquete->buffer->size == 0){
		paquete->buffer->stream = malloc(sizeof(int));
		memcpy(paquete->buffer->stream, &numero, sizeof(int));
	}else{
		paquete->buffer->stream = realloc(paquete->buffer->stream,
											paquete->buffer->size + sizeof(int));
		/**/
		memcpy(paquete->buffer->stream + paquete->buffer->size, &numero, sizeof(int));
	}

	paquete->buffer->size += sizeof(int);
}

void add_string_to_super_pck(t_paquete* paquete, char* string){
	int size_string = strlen(string)+1;

	if(paquete->buffer->size == 0){
		paquete->buffer->stream = malloc(sizeof(int) + sizeof(char)*size_string);
		memcpy(paquete->buffer->stream, &size_string, sizeof(int));
		memcpy(paquete->buffer->stream + sizeof(int), string, sizeof(char)*size_string);

	}else {
		paquete->buffer->stream = realloc(paquete->buffer->stream,
										paquete->buffer->size + sizeof(int) + sizeof(char)*size_string);
		/**/
		memcpy(paquete->buffer->stream + paquete->buffer->size, &size_string, sizeof(int));
		memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), string, sizeof(char)*size_string);

	}
	paquete->buffer->size += sizeof(int);
	paquete->buffer->size += sizeof(char)*size_string;
}


void add_choclo_to_super_pck(t_paquete* paquete, void* choclo, int size){
	if(paquete->buffer->size == 0){
		paquete->buffer->stream = malloc(sizeof(int) + size);
		memcpy(paquete->buffer->stream, &size, sizeof(int));
		memcpy(paquete->buffer->stream + sizeof(int), choclo, size);
	}else{
		paquete->buffer->stream = realloc(paquete->buffer->stream,
												paquete->buffer->size + sizeof(int) + size);

		memcpy(paquete->buffer->stream + paquete->buffer->size, &size, sizeof(int));
		memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), choclo, size);
	}

	paquete->buffer->size += sizeof(int);
	paquete->buffer->size += size;
}

//FUNCIONES PARA ENVIAR PAQUETE
void send_pckg(t_paquete* paquete, int socket_cliente)
{
	//int bytes = malloc (sizeof(int));
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = pckg_serialize(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}


//FUNCION PARA ELIMINAR PAQUETE
void delete_pckg(t_paquete* paquete)  
{
	 if (paquete) {
        if (paquete->buffer) {
            free(paquete->buffer->stream);
            paquete->buffer->stream = NULL;
            free(paquete->buffer);
            paquete->buffer = NULL;
        }
        free(paquete);
        paquete = NULL;
    }
}

//LIBERAR BUFFER



//LIBERAR CONEXION
void freeconnection(int socket_cliente)
{
	close(socket_cliente);
}

//FUNCIONES PARA LOS HILOS

void exec_in_detach(void (*f)(void*) ,void* struct_arg){
	pthread_t thread;
	pthread_create(&thread, NULL, (void*)f, struct_arg);
	pthread_detach(thread);
}

void exec_in_join(void (*f)(void*) ,void* struct_arg){
	pthread_t thread;
	pthread_create(&thread, NULL, (void*)f, struct_arg);
	pthread_join(thread, NULL);
}


// Función para ordenar la lista enlazada
void ordenar_lista(t_list *lista, bool (*comparador)(void *, void *)) {
    if (!lista || !lista->head) return; // Verifica si la lista es NULL o está vacía

    bool intercambiado;
    do {
        intercambiado = false;
        t_link_element *actual = lista->head;

        while (actual->next != NULL) {
            if (comparador(actual->data, actual->next->data)) {
                // Intercambiar los datos si el comparador lo indica
                void *temp = actual->data;
                actual->data = actual->next->data;
                actual->next->data = temp;
                intercambiado = true;
            }
            actual = actual->next;
        }
    } while (intercambiado);
}

// Función de comparación para ordenar enteros de menor a mayor
bool comparador_menorAmayor(void *a, void *b) {
    int entero_a = *(int *)a;
    int entero_b = *(int *)b;
    return entero_a > entero_b;
}
