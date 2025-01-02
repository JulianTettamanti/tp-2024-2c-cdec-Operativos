#include <Iniciar_memoria.h>



void iniciar_memoria(){

    iniciar_listas();
    iniciar_mutex();
    iniciar_semaforos();

    espacio_usuario = malloc(TAM_MEMORIA);
    if(espacio_usuario == NULL){
        log_error(memo_logger, "no se pudo crear el espacio de usuario");
        //log_error(memo_logger_debug, "no se pudo crear el espacio de usuario");
        exit(EXIT_FAILURE);
    }
    memset(espacio_usuario, 0, TAM_MEMORIA);

    if (strcmp(ESQUEMA, "FIJAS") == 0){
        int nueva_base = 0;
        int size = string_array_size(PARTICIONES);
        
        for(int i=0;i < size ;i++){
            char* extraer_string = PARTICIONES[i];
            int tamanio = atoi(extraer_string);
            log_debug(memo_logger_debug, "Tamanio de la proxima particion es: %d", tamanio);
            t_marco* nuevo_marco  = marco_create(nueva_base, true, -1, tamanio);
            pthread_mutex_lock(&mutex_lista_particiones);
            list_add(lista_particiones,nuevo_marco);
            pthread_mutex_unlock(&mutex_lista_particiones);

            nueva_base += tamanio;
        }
    }else if(strcmp(ESQUEMA, "DINAMICAS") == 0){
        t_marco* nuevo_marco = marco_create(0, true, -1, TAM_MEMORIA);
        pthread_mutex_lock(&mutex_lista_particiones);
        list_add(lista_particiones,nuevo_marco);
        pthread_mutex_unlock(&mutex_lista_particiones);
    }

}
t_marco* marco_create(int base, bool libre, int index, int tam){
    t_marco *nuevo_marco = malloc(sizeof(t_marco));
    nuevo_marco->pid = index;
    nuevo_marco->base = base;
    nuevo_marco->libre = libre;
    nuevo_marco->tamanio = tam;

    return nuevo_marco;
}

void iniciar_listas(){
    lista_particiones = list_create();
    lista_procesos = list_create();
}
void iniciar_mutex(){
    pthread_mutex_init(&mutex_lista_particiones, NULL);
    pthread_mutex_init(&mutex_memoria_usuario, NULL);
    pthread_mutex_init(&mutex_lista_procesos, NULL);
}

void iniciar_semaforos(){

}
