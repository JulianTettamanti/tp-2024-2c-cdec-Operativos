#include <memoria_kernel.h>

int pid_a_marco;
int size_a_buscar;
int size_a_buscar_fijas;
int size_a_buscar_dinamicas;

void attend_memoria_kernel(int socket_kernel){

	t_buffer* un_buffer;
    log_info(memo_logger,"Esperando mensaje del kernel");
    int cod_op = recv_op(socket_kernel);
    switch (cod_op) {
   
        case INICIALIZAR_ESTRUCTURAS_KM:
            //t_buffer* un_buffer;
            un_buffer = recv_buffer(socket_kernel);
            inicializar_estructuras(un_buffer,socket_kernel);
            free(un_buffer->stream);
            free(un_buffer);
        break;

        case FINALIZAR_ESTRUCTURAS_KM:
            //t_buffer* un_buffer;
            un_buffer = recv_buffer(socket_kernel);
            log_info(memo_logger,"SE ENTRA A FINALIZAR UN PROCESO");
            //int pid = extract_int_from_buffer(un_buffer);
            //eliminar_proceso(pid, socket_kernel);
            pthread_mutex_lock(&mutex_lista_particiones);
            finalizar_estructuras(un_buffer,socket_kernel);
            pthread_mutex_unlock(&mutex_lista_particiones);
            free(un_buffer->stream);
            free(un_buffer);
        break;

        case CREACION_HILO_KM:
            //t_buffer* un_buffer;
            un_buffer = recv_buffer(socket_kernel);
            creacion_hilo(un_buffer,socket_kernel);
            free(un_buffer->stream);
            free(un_buffer);
        break;

        case FINALIZACION_HILO_KM:
            //t_buffer* un_buffer;
            un_buffer = recv_buffer(socket_kernel);
            finalizacion_hilo(un_buffer,socket_kernel);
            free(un_buffer->stream);
            free(un_buffer);
        break;

        case DUMP_MEMORY_KM:
            //t_buffer* un_buffer;
            un_buffer = recv_buffer(socket_kernel);
            dumpear_memoria(un_buffer, socket_kernel);
            free(un_buffer->stream);
            free(un_buffer);
        break;
    }
    //free(un_buffer->stream);
    //free(un_buffer);

}

void inicializar_estructuras(t_buffer* un_buffer, int socket_kernel){
    int pid = extract_int_from_buffer(un_buffer);
    char* path = extract_string_from_buffer(un_buffer);
    int size = extract_int_from_buffer(un_buffer);

    t_buffer* otro_buffer = new_buffer();

    if (hay_espacio_libre(size)){
        t_proceso* proc = create_process(pid,size);
        crear_tid(proc,path,0);
        //free(path);

        log_info(memo_logger, "## Proceso Creado -  PID: %d - Tamaño: %d", pid, size);
        //log_info(memo_logger_debug, "## Proceso Creado -  PID: %d - Tamaño: %d", pid, size);
        log_info(memo_logger_obligatorio, "## Proceso Creado -  PID: %d - Tamaño: %d", pid, size);

        add_int_to_buffer(otro_buffer,1);
    }else {
        add_int_to_buffer(otro_buffer,0);
    }

    free(path);
    usleep(RETARDO_RESPUESTA*1000);
    t_paquete* rta_kernel = create_super_pck (RTA_INICIALIZAR_ESTRUCTURAS_MK,otro_buffer);
    send_pckg(rta_kernel,socket_kernel);
    delete_pckg(rta_kernel);
    //free(path);
}

bool hay_espacio_libre(int size){
    bool resultado;

    pthread_mutex_lock(&mutex_lista_particiones);
    size_a_buscar = size;
    resultado = list_any_satisfy(lista_particiones, tamanio_y_libre);
    pthread_mutex_unlock(&mutex_lista_particiones);

    return resultado;
}

bool tamanio_y_libre (void* particion){
    t_marco* marco = particion;
    log_info(memo_logger_debug, "Marco libre %d marco tamaño %d size a buscar %d", marco->libre, marco->tamanio, size_a_buscar);
    return marco->libre && marco->tamanio >= size_a_buscar;
}


t_proceso* create_process(int pid, int size){
    t_proceso* proceso_nuevo = malloc(sizeof(t_proceso));
    proceso_nuevo->pid = pid;
    proceso_nuevo->hilos = list_create();

    pthread_mutex_lock(&mutex_lista_procesos);
    list_add (lista_procesos,proceso_nuevo);
    pthread_mutex_unlock(&mutex_lista_procesos);

    pthread_mutex_lock(&mutex_lista_particiones);
    asignar_memoria_al_proceso(proceso_nuevo,size);
    pthread_mutex_unlock(&mutex_lista_particiones);

    return proceso_nuevo;
}

void asignar_memoria_al_proceso (t_proceso* proceso,int size){ // si rompe, meter mutex lista particiones
    //int index=0;

    if (strcmp(ESQUEMA, "FIJAS")==0){
        t_list_iterator* iterator = list_iterator_create(lista_particiones);

        if (strcmp(ALGORITMO_BUSQUEDA, "FIRST")==0){
            log_info(memo_logger, "ME METI A FIRST DE FIJAS");
            while(list_iterator_has_next(iterator)){
                int i=0;
                t_marco* element = list_iterator_next(iterator);
                if (element->libre && element-> tamanio >= size) {   
                    element->libre = false;
                    element->pid = proceso->pid;
                    proceso->base = element->base;
                    proceso->limite = element->tamanio + element->base - 1;
                    log_info(memo_logger_debug,"Se ingreso el [PID: %d] al marco %d",proceso->pid,list_iterator_index(iterator));
                    break;
                }
                i++; 
            }
            list_iterator_destroy(iterator);
        }
        else if(strcmp(ALGORITMO_BUSQUEDA, "BEST")==0){
            log_info(memo_logger, "ME METI A BEST DE FIJAS");
            size_a_buscar_fijas = size;
	        t_marco* un_marco = list_get_minimum(lista_particiones, _min_space);  
                   
            un_marco->libre = false;
            un_marco->pid = proceso->pid;
            proceso->base = un_marco->base;
            proceso->limite = un_marco->tamanio + un_marco->base - 1;
            log_info(memo_logger_debug, "OBTENGO EL MARCO MAS CHICO DE TAMANIO %d Y SU PID ASOCIADO ES %d", un_marco->tamanio, un_marco->pid);
            

            }
        else if (strcmp(ALGORITMO_BUSQUEDA, "WORST")==0){
            log_info(memo_logger, "ME METI A WORST DE FIJAS");
            size_a_buscar_fijas = size;
	        t_marco* un_marco = list_get_maximum(lista_particiones, _max_space);  
                   
            un_marco->libre = false;
            un_marco->pid = proceso->pid;
            proceso->base = un_marco->base;
            proceso->limite = un_marco->tamanio + un_marco->base - 1;
            log_info(memo_logger_debug, "OBTENGO EL MARCO MAS GRANDE DE TAMANIO %d Y SU PID ASOCIADO ES %d", un_marco->tamanio, un_marco->pid);

            }

    }else if (strcmp(ESQUEMA ,"DINAMICAS")==0){

        if (strcmp(ALGORITMO_BUSQUEDA, "FIRST")==0){
            log_info(memo_logger, "ME METI A FIRST DE DINAMICAS");
            t_list_iterator* iterator = list_iterator_create(lista_particiones);
            while(list_iterator_has_next(iterator)){
                t_marco* element = list_iterator_next(iterator);
                if (element->libre && element-> tamanio >= size) {
                    int index = list_iterator_index (iterator);
                    t_marco *nuevo_marco = malloc(sizeof(t_marco));
                    nuevo_marco->pid = proceso->pid;
                    nuevo_marco->base = element->base;
                    proceso->base = element->base;
                    nuevo_marco->libre = false;
                    nuevo_marco->tamanio = size;
                    proceso->limite = nuevo_marco->base + nuevo_marco->tamanio - 1;
                    t_marco *marco_libre = list_get(lista_particiones, index);
                    marco_libre->tamanio -= size;
                    marco_libre->base += size;
                    list_add_in_index (lista_particiones,index,nuevo_marco);
                    //element->base = nuevo_marco->tamanio + element->base;
                    break;
                }
            }
            list_iterator_destroy(iterator);
        }
        else if(strcmp(ALGORITMO_BUSQUEDA, "BEST")==0){
            log_info(memo_logger, "ME METI A BEST DE DINAMICAS");
            size_a_buscar_fijas = size;
            t_marco* un_marco = list_get_minimum(lista_particiones, _min_space);

            t_list_iterator* iterator = list_iterator_create(lista_particiones);
            while(list_iterator_has_next(iterator)){
                t_marco* element = list_iterator_next(iterator);
                if (element->libre && element->base == un_marco->base) {
                    log_info(memo_logger, "SE OBTUVO EL ELEMENTO CON BASE %d", element->base);
                    int index = list_iterator_index (iterator);
                    log_info(memo_logger, "EN LA POSICION %d", index);
                    t_marco *nuevo_marco = malloc(sizeof(t_marco));
                    nuevo_marco->pid = proceso->pid;
                    nuevo_marco->base = element->base;
                    proceso->base = element->base;
                    nuevo_marco->libre = false;
                    nuevo_marco->tamanio = size;
                    proceso->limite = nuevo_marco->base + nuevo_marco->tamanio - 1;
                    t_marco *marco_libre = list_get(lista_particiones, index);
                    marco_libre->tamanio -= size;
                    marco_libre->base += size;
                    list_add_in_index (lista_particiones,index,nuevo_marco);
                    //element->base = nuevo_marco->tamanio + element->base;
                }
            }
            list_iterator_destroy(iterator);
        
        }
        else if (strcmp(ALGORITMO_BUSQUEDA, "WORST")==0){
            log_info(memo_logger, "ME METI A WORST DE DINAMICAS");
            size_a_buscar_fijas = size;
	        t_marco* un_marco = list_get_maximum(lista_particiones, _max_space);

            t_list_iterator* iterator = list_iterator_create(lista_particiones);
            while(list_iterator_has_next(iterator)){
                t_marco* element = list_iterator_next(iterator);
                if (element->libre && element->base == un_marco->base) {
                    int index = list_iterator_index (iterator);
                    t_marco *nuevo_marco = malloc(sizeof(t_marco));
                    nuevo_marco->pid = proceso->pid;
                    nuevo_marco->base = element->base;
                    proceso->base = element->base;
                    nuevo_marco->libre = false;
                    nuevo_marco->tamanio = size;
                    proceso->limite = nuevo_marco->base + nuevo_marco->tamanio - 1;
                    t_marco *marco_libre = list_get(lista_particiones, index);
                    marco_libre->tamanio -= size;
                    marco_libre->base += size;
                    list_add_in_index (lista_particiones,index,nuevo_marco);
                    //element->base = nuevo_marco->tamanio + element->base;  
                    break;  
                }
            }
            list_iterator_destroy(iterator);
        }
    }
}

void* _min_space(void* a, void* b) {
	     t_marco* marco_a = (t_marco*) a;
	     t_marco* marco_b = (t_marco*) b;


         if(marco_a->tamanio < size_a_buscar_fijas || marco_a->libre == false){
            return marco_b;
         }

         if(marco_b->tamanio < size_a_buscar_fijas || marco_b->libre == false){
            return marco_a;
         }

	     return marco_a->tamanio <= marco_b->tamanio ? marco_a : marco_b;
	 }
    
void* _max_space(void* a, void* b) {
	     t_marco* marco_a = (t_marco*) a;
	     t_marco* marco_b = (t_marco*) b;

         if(marco_a->libre == false){
            return marco_b;
         }

         if(marco_b->libre == false){
            return marco_a;
         }

	     return marco_a->tamanio >= marco_b->tamanio ? marco_a : marco_b;
	 }

void crear_tid(t_proceso* proceso, char* path, int tid){
    t_hilo* un_hilo = malloc(sizeof(t_hilo));

    un_hilo->TID = tid;
    un_hilo->PC = 0;
    un_hilo->AX = 0;
    un_hilo->BX = 0;
    un_hilo->CX = 0;
    un_hilo->DX= 0;
    un_hilo->EX = 0;
    un_hilo->FX = 0;
    un_hilo->GX = 0;
    un_hilo->HX = 0;
    //un_hilo->path_instrucciones = path;
    un_hilo->instrucciones = read_file(path);
    list_add (proceso->hilos,un_hilo);
	//free(path);
}

t_list* read_file(const char* path_archivo) {
    log_debug(memo_logger_debug, "Leer archivo en [PATH: %s]", path_archivo);
    FILE* archivo = fopen(path_archivo, "r");
    t_list* instrucciones = list_create();
    char* instruccion_formateada = NULL;
    int i = 0;
    if (archivo == NULL) {
        perror("No se encontró el archivo");
        log_debug(memo_logger_debug, "Se intento leer el archivo pero esta vacio o no se encontro");
        return instrucciones;
    }
    char* linea_instruccion = malloc(256 * sizeof(int));
    while (fgets(linea_instruccion, 256, archivo)) {
        //Comprobar si el ultimo caracter del string capturado tiene un salto delinea
        //Si lo tiene hay que sacarlo
        //[0][1][2][3][4]["\n"]["\0"] -> Size:6
        int size_linea_actual = strlen(linea_instruccion);
        if(size_linea_actual > 3){
            if(linea_instruccion[size_linea_actual - 1] == '\n'){
                char* linea_limpia = string_new();
                string_n_append(&linea_limpia, linea_instruccion, size_linea_actual - 1);
                free(linea_instruccion);
                linea_instruccion = malloc(256 * sizeof(int));
                strcpy(linea_instruccion,linea_limpia);
                free(linea_limpia);
            }
        }

        char** instrucciones_split = string_split(linea_instruccion, " ");
        log_info(memo_logger, "Instruccion: [%s]", linea_instruccion);
        while (instrucciones_split[i]) {
            i++;
        }
        t_codigo_instruccion* pseudo_cod = malloc(sizeof(t_codigo_instruccion));
        pseudo_cod->instruccion = strdup(instrucciones_split[0]);
        log_info(memo_logger,"SE LEYO LA INSTRUCCION: %s",instrucciones_split[0]);
        pseudo_cod->fst_param = (i > 1) ? strdup(instrucciones_split[1]) : NULL;
        pseudo_cod->snd_param = (i > 2) ? strdup(instrucciones_split[2]) : NULL;
        pseudo_cod->trd_param = (i > 3) ? strdup(instrucciones_split[3]) : NULL;
        
        if (i == 4){
            instruccion_formateada = string_from_format("%s %s %s %s", pseudo_cod->instruccion, pseudo_cod->fst_param, pseudo_cod->snd_param, pseudo_cod->trd_param);
        } else if (i == 3) {
            instruccion_formateada = string_from_format("%s %s %s", pseudo_cod->instruccion, pseudo_cod->fst_param, pseudo_cod->snd_param);
        } else if (i == 2){
            instruccion_formateada = string_from_format("%s %s", pseudo_cod->instruccion, pseudo_cod->fst_param);
        }else {
            instruccion_formateada = strdup(pseudo_cod->instruccion);
        }
        list_add(instrucciones, instruccion_formateada);
        for (int j = 0; j < i; j++) {
            free(instrucciones_split[j]);
        }
        free(instrucciones_split);
        free(pseudo_cod->instruccion);
        if(pseudo_cod->fst_param) free(pseudo_cod->fst_param);
        if(pseudo_cod->snd_param) free(pseudo_cod->snd_param);
        if(pseudo_cod->trd_param) free(pseudo_cod->trd_param);
        free(pseudo_cod);
        i = 0; // Restablece la cuenta para la próxima iteración
    }
    fclose(archivo);
    free(linea_instruccion);
    return instrucciones;
}

void finalizar_estructuras(t_buffer* un_buffer,int socket_kernel){
    int pid = extract_int_from_buffer(un_buffer);
    log_info(memo_logger_debug,"LLEGO EL PID A ELIMINAR: %d", pid);
    if(strcmp(ESQUEMA,"DINAMICAS") == 0){
        t_list_iterator* iterator = list_iterator_create(lista_particiones);
        t_marco* element;
        bool agrupado = false;
        while(!agrupado){
            if(list_iterator_has_next(iterator)){
                element = list_iterator_next(iterator);
                if (element->pid == pid) {   
                    element->libre = true;
                    //log_info(memo_logger_debug,"SE LIBERA UN MARCO BASE %d DE TAMANIO: %d", element->base,element->tamanio);
            
                    int indice = list_iterator_index(iterator);
                    agrupar_espacios_libres(indice);
                    agrupado = true;
                }
            }else{agrupado = true;}
        }
        list_iterator_destroy(iterator);
    }
    //log_info(memo_logger_debug,"termine de agrupar estructuras");
    if(strcmp(ESQUEMA,"FIJAS") == 0){
        pid_a_marco = pid;
        t_marco* marco = list_find(lista_particiones,buscar_marco_por_pid);
        marco->libre = true;
        //log_info(memo_logger_debug,"Se libera en fijas tamanio de marco: %d",marco->tamanio);
    }
    for (int i=0;i<list_size(lista_particiones);i++){
        t_marco* element = list_get(lista_particiones,i);
        log_info(memo_logger_debug,"marco %d esta libre %d con tamanio %d",i,element->libre,element->tamanio);
    }
    eliminar_proceso(pid, socket_kernel);
}


bool buscar_marco_por_pid(void* a){
    t_marco* un_marco = (t_marco*) a;
    if(un_marco->pid == pid_a_marco){
        return true;
    }
    return false;
}

void agrupar_espacios_libres(int i){
    bool anterior_libre = false;
    bool siguiente_libre = false;
    t_marco* parti_ant;
    t_marco* parti_sig;
    t_marco* parti_actual = list_get (lista_particiones,i);

    if (i>0){
        parti_ant = list_get(lista_particiones,i-1); 
        if (parti_ant->libre){
            anterior_libre = true;
        }
    }
    if (i<(list_size(lista_particiones) - 1)){
        parti_sig = list_get(lista_particiones,i+1); 
        if (parti_sig->libre){
            siguiente_libre = true;
        }   
    }
    if (anterior_libre && siguiente_libre){
        t_marco *nuevo_marco = malloc(sizeof(t_marco));

        nuevo_marco->base = parti_ant->base;
        nuevo_marco->tamanio = parti_ant->tamanio + parti_actual->tamanio + parti_sig->tamanio;
        nuevo_marco->libre = true;
        nuevo_marco->pid = NULL;
        //log_info(memo_logger_debug,"Se junta atras y adelante");
        list_remove_element(lista_particiones,parti_ant);
        list_remove_element(lista_particiones,parti_actual);
        list_remove_element(lista_particiones,parti_sig);

        list_add_in_index(lista_particiones,i-1,nuevo_marco);
        //log_info(memo_logger_debug,"ya se junto atras y adelante");
    }else if (anterior_libre){
        t_marco *nuevo_marco = malloc(sizeof(t_marco));

        nuevo_marco->base = parti_ant->base;
        nuevo_marco->tamanio = parti_ant->tamanio + parti_actual->tamanio;
        nuevo_marco->libre = true;
        nuevo_marco->pid = NULL;
        //log_info(memo_logger_debug,"Se junta solo atras");
        list_remove_element(lista_particiones,parti_ant);
        list_remove_element(lista_particiones,parti_actual);

        list_add_in_index(lista_particiones,i-1,nuevo_marco);
        //log_info(memo_logger_debug,"ya se junto atras");
    }else if (siguiente_libre){
        t_marco *nuevo_marco = malloc(sizeof(t_marco));

        nuevo_marco->base = parti_actual->base;
        nuevo_marco->tamanio = parti_sig->tamanio + parti_actual->tamanio;
        nuevo_marco->libre = true;
        nuevo_marco->pid = NULL;
        //log_info(memo_logger_debug,"Se junta solo adelante");
        list_remove_element(lista_particiones,parti_sig);
        list_remove_element(lista_particiones,parti_actual);

        list_add_in_index(lista_particiones,i,nuevo_marco);
        //log_info(memo_logger_debug,"ya se junto adelante");
    }
}

void eliminar_proceso(int pid, int socket_kernel){
    t_proceso* un_proceso = search_process_for(pid);
    int tamanio = un_proceso->limite + 1  - un_proceso->base;
    t_buffer* otro_buffer = new_buffer();
    if(list_remove_element(lista_procesos, un_proceso)){
        
        delete_process_struct(un_proceso);
        log_info(memo_logger, "## Proceso Destruido -  PID: %d - Tamaño: %d", pid, tamanio);
        //log_info(memo_logger_debug, "## Proceso Destruido -  PID: %d - Tamaño: %d", pid, tamanio);
        log_info(memo_logger_obligatorio, "## Proceso Destruido -  PID: %d - Tamaño: %d", pid, tamanio);
        add_int_to_buffer(otro_buffer,1);
        
    }else{
        log_error(memo_logger, "Proceso %d no encontrado en la lista de procesos para ser eliminados", pid);
        //log_error(memo_logger_debug, "Proceso %d no encontrado en la lista de procesos para ser eliminados", pid);
        add_int_to_buffer(otro_buffer,0);
        exit(EXIT_FAILURE);
       
    }
    
    usleep(RETARDO_RESPUESTA*1000);
    log_info(memo_logger,"Sali del sleep para mandarle la respuesta a kernel");
    t_paquete* rta_kernel = create_super_pck (RTA_LIBERAR_ESTRUCTURA_KM,otro_buffer);
    send_pckg(rta_kernel,socket_kernel);
    log_info(memo_logger,"Mande respuesta a Kernel");
    delete_pckg(rta_kernel);
}



int search_pid_value;
bool search_pid(t_proceso* proceso){
        return proceso->pid == search_pid_value;
}

t_proceso* search_process_for(int pid){
    search_pid_value = pid;

    pthread_mutex_lock(&mutex_lista_procesos);
    t_proceso* un_proceso = list_find(lista_procesos,(void*)search_pid);
    pthread_mutex_unlock(&mutex_lista_procesos);
    if(un_proceso == NULL){
        log_error(memo_logger, "PID %d No encontrado en la lista de procesos", pid);
        //log_error(memo_logger_debug, "PID %d No encontrado en la lista de procesos", pid);
        exit(EXIT_FAILURE);
    }
    return un_proceso;
}

void delete_process_struct(t_proceso* un_proceso){
    //Eliminar lista de instrucciones
    list_destroy_and_destroy_elements(un_proceso->hilos, (void*)destroy_hilo);

    free(un_proceso);
}

void destroy_hilo(void* un_hilo){
    t_hilo* hilo_eliminar = (t_hilo*) un_hilo;
    //free(hilo_eliminar->path_instrucciones);
    log_info(memo_logger_debug, "ELIMINANDO PATH DEL HILO: %d", hilo_eliminar->TID);
    list_destroy_and_destroy_elements(hilo_eliminar->instrucciones, (void*)destroy_instruction);
    log_info(memo_logger_debug, "SE ELIMINARON LAS INSTRUCCIONES");
    free(un_hilo);
}

void destroy_instruction(char* una_instruccion){
    //log_info(memo_logger_debug, "INSTRUCCION A ELIMINAR: %s", una_instruccion);
    free(una_instruccion);
}

void creacion_hilo(t_buffer* un_buffer,int socket_kernel){
    int pid = extract_int_from_buffer(un_buffer);
    int tid = extract_int_from_buffer(un_buffer);
    char* path = extract_string_from_buffer(un_buffer);

    t_proceso* un_proceso = search_process_for(pid);
    crear_tid(un_proceso,path,tid);
    free(path);
    log_info(memo_logger, "## Hilo Creado - (PID:TID) - (%d:%d)", pid, tid);
    //log_info(memo_logger_debug, "## Hilo Creado - (PID:TID) - (%d:%d)", pid, tid);
    log_info(memo_logger_obligatorio, "## Hilo Creado - (PID:TID) - (%d:%d)", pid, tid);

    t_buffer* otro_buffer = new_buffer();
    add_int_to_buffer(otro_buffer,1);

    usleep(RETARDO_RESPUESTA*1000);
    t_paquete* rta_kernel = create_super_pck (RTA_CREAR_HILO_SYS_KM,otro_buffer); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    send_pckg(rta_kernel,socket_kernel);
    delete_pckg(rta_kernel);
	//free(path);
}

    int tid_a_buscar;

void finalizacion_hilo(t_buffer* un_buffer,int socket_kernel){
    int pid = extract_int_from_buffer(un_buffer);
    int tid = extract_int_from_buffer(un_buffer);

    tid_a_buscar = tid;

    t_proceso* un_proceso =  search_process_for(pid);
    t_hilo* un_hilo = list_remove_by_condition(un_proceso->hilos,hilo_a_remover);

    destroy_hilo(un_hilo);
    //log_info(memo_logger, " - (%d:%d)", pid, tid);
    log_info(memo_logger_debug, "## Hilo Destruido - (PID:TID) - (%d:%d)", pid, tid);
    log_info(memo_logger_obligatorio, "## Hilo Destruido - (PID:TID) - (%d:%d)", pid, tid);

    t_buffer* otro_buffer = new_buffer();
    add_int_to_buffer(otro_buffer,1);

    //int rta = otro_buffer->stream;
    //log_info(memo_logger_debug, "RESPUESTA DE MEMORIA: %d", rta);

    usleep(RETARDO_RESPUESTA*1000);
    t_paquete* rta_kernel = create_super_pck (RTA_LIBERAR_ESTRUCTURA_KM,otro_buffer); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    send_pckg(rta_kernel,socket_kernel);
    delete_pckg(rta_kernel);
}

bool hilo_a_remover(void* ptr) {
    
t_hilo* hilo = (t_hilo*) ptr;
return hilo->TID == tid_a_buscar;
}

void dumpear_memoria(t_buffer* un_buffer, int socket_kernel) {
    int pid = extract_int_from_buffer(un_buffer);
    int tid = extract_int_from_buffer(un_buffer);

    log_info(memo_logger, "## Memory Dump solicitado - (PID:TID) - (%d:%d)", pid, tid);
    //log_info(memo_logger_debug, "## Memory Dump solicitado - (PID:TID) - (%d:%d)", pid, tid);
    log_info(memo_logger_obligatorio, "## Memory Dump solicitado - (PID:TID) - (%d:%d)", pid, tid);

    t_proceso* un_proceso = search_process_for(pid);

    int tamanio = un_proceso->limite + 1 - un_proceso->base;

    void* valor = malloc(tamanio);
    pthread_mutex_lock(&mutex_memoria_usuario);
    memcpy(valor, espacio_usuario + un_proceso->base, tamanio);
    pthread_mutex_unlock(&mutex_memoria_usuario);

    char* pid_string = string_itoa(pid);
    char* tid_string = string_itoa(tid);

    char* timestamp = temporal_get_string_time("%H:%M:%S:%MS");
    //log_info(memo_logger_debug, "el timestamp es: %s", timestamp);

    char *nombre_archivo = string_new();
	string_append(&nombre_archivo, pid_string);
    string_append(&nombre_archivo, "-");
	string_append(&nombre_archivo, tid_string);
    string_append(&nombre_archivo, "-");
    string_append(&nombre_archivo, timestamp);

    t_buffer* otro_buffer = new_buffer();
	add_string_to_buffer(otro_buffer, nombre_archivo);
    add_int_to_buffer(otro_buffer, tamanio);
    add_to_buffer(otro_buffer, valor, tamanio);
    
    log_info(memo_logger_debug,"Envio a MMDUMP, nombre archvio <%s>, tamanio <%d>", nombre_archivo, tamanio);

	t_paquete* un_paquete = create_super_pck(DUMP_MEMORY_MF, otro_buffer);

    conexion_fs(un_paquete, socket_kernel);
    free(pid_string);
    free(tid_string);
    free(timestamp);
    free(nombre_archivo);
    free(valor);

}

void conexion_fs(t_paquete* un_paquete, int socket_kernel){

    int socket_fs = new_connection(IP_FILESYSTEM, PUERTO_FILESYSTEM);
    send_pckg(un_paquete, socket_fs);
    t_buffer* un_buffer = attend_memoria_fs(socket_fs);
    log_info(memo_logger_debug,"LLEGO RESPUESTA DUMP");
    freeconnection(socket_fs);

    usleep(RETARDO_RESPUESTA*1000);
	t_paquete* otro_paquete = create_super_pck(RTA_DUMP_MEMORY_MK, un_buffer);
	send_pckg(otro_paquete, socket_kernel);
	delete_pckg(otro_paquete);
    delete_pckg(un_paquete);
    log_info(memo_logger_debug, "Envie respuesta Memory Dump a kernel");



}

t_buffer* attend_memoria_fs(int socket_fs){
	t_buffer* un_buffer;
    int cod_op = recv_op(socket_fs);

    if (cod_op==RTA_DUMP_MEMORY_FM) {
        un_buffer=recv_buffer(socket_fs);
        return un_buffer;
    }else if (cod_op==SIN_ESPACIO_FM) {
        un_buffer=recv_buffer(socket_fs);// HAY QUE HACER ALGO ACA
        log_info(memo_logger_debug,"SIN ESPACIO");
        return un_buffer;
    }else{
        log_error(memo_logger,"COD_OP DESCONOCIDO PROVENIENTE DE FS");
        exit (EXIT_FAILURE);
    }
}
