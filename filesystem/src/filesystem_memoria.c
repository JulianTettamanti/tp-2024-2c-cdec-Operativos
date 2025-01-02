#include <filesystem_memoria.h>
#include <manejo_archivos.h>

void attend_fs_memoria(int socket_memoria){
		//t_buffer* unBuffer = new_buffer();
		t_buffer* unBuffer;
		int cod_op = recv_op(socket_memoria);
		log_info(fs_logger_debug,"recibi el codop");
		switch (cod_op) {
		    case DUMP_MEMORY_MF:
				unBuffer = recv_buffer(socket_memoria);
				log_info(fs_logger_debug,"recibi el buffer");
			    attend_dump_memory(unBuffer, socket_memoria);
				free(unBuffer);
			    break;
		    case PAQUETE:
			    //
			    break;
			/*case -1:
			    log_error(fs_logger_debug, "DESCONEXION DE FS - MEMORIA");
			    control_key = 0;
                break;
		    default:
			    log_warning(fs_logger_debug,"OPERACION DESCONOCIDA - FS - MEMORIA");
				control_key = 0;
			    break;*/
		}
	}


void attend_dump_memory(t_buffer* unBuffer, int socket_memoria){
	log_info(fs_logger_debug,"recibi el buffer en dump");
	char *nombre_archivo = extract_string_from_buffer(unBuffer);
	int size_archivo = extract_int_from_buffer(unBuffer);
	char* contenido = extract_string_from_buffer(unBuffer);
	int cantidad_bloques = (size_archivo + BLOCK_SIZE - 1) / BLOCK_SIZE;
	
	// CHEQUEAR ESPACIO SUFICIENTE
	if(!hay_espacio_en_bitmap(cantidad_bloques+1)){
		// mandar mensaje a memoria
			t_buffer* otro_buffer = new_buffer();
			add_int_to_buffer(otro_buffer,0);

			t_paquete* rta_memoria = create_super_pck (SIN_ESPACIO_FM,otro_buffer); 
			send_pckg(rta_memoria,socket_memoria);
			delete_pckg(rta_memoria);
			log_info(fs_logger_debug,"NO HAY ESPACIO");
		return;
	}
	log_info(fs_logger_debug,"HAY ESPACIO");

	// BUSCAR BLOQUES LIBRES
	log_info(fs_logger_debug,"busco bloque");
	int base_bloque = 0;
	base_bloque = bitmap_obtener_bloques_libres(cantidad_bloques,nombre_archivo);


	log_info(fs_logger_debug,"primer bloque: %d",base_bloque);

	if(base_bloque==-1){
		perror("No hay espacio consecutivo");
	}

	// ESCRIBIR BITARRAY
	log_info(fs_logger_debug,"consegui bloque");
	bitmap_escribir_archivo(base_bloque,cantidad_bloques+1);

	// ACTUALIZAR BITMAP
	bitmap_modificar_archivo_bitmap();
	log_info(fs_logger_debug,"escribi bloques");
	
	// CREAR ARCHIVO METADATA
	char* path_metadata_actual = metadata_crear_archivo(base_bloque,size_archivo,nombre_archivo);
	log_info(fs_logger_debug,"escribi metadata en %s",path_metadata_actual);
	// ESCRIBIR ARCHIVO DE DATOS 

	void* bloque_index = generar_bloque_index(base_bloque,cantidad_bloques);
	
	blockFile_escribir_archivo_indices((void*)bloque_index,base_bloque*BLOCK_SIZE,sizeof(bloque_index),nombre_archivo,"ÍNDICE");// aca hay que escribir todos los indices
	blockFile_escribir_archivo(contenido,(base_bloque+1)*BLOCK_SIZE,size_archivo,nombre_archivo,"DATOS");

	free(bloque_index);
	free(contenido);
	
	log_info(fs_logger_obligatorio,"## Fin de solicitud - Archivo: <%s>",nombre_archivo);
	free(nombre_archivo);
	// RTA A MEMORIA
	t_buffer* otro_buffer = new_buffer();
    add_int_to_buffer(otro_buffer,1);

    t_paquete* rta_memoria = create_super_pck (RTA_DUMP_MEMORY_FM,otro_buffer); 
    send_pckg(rta_memoria,socket_memoria);
    delete_pckg(rta_memoria);
	log_info(fs_logger_debug,"envie respuesta");
    
}