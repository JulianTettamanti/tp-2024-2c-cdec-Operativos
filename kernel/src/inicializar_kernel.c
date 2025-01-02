#include "inicializar_kernel.h"
#include <termios.h>
#include <ncurses.h>



void crear_proceso_inicial(int tam_proceso, char* nombre_archivo){
	t_pcb* un_pcb = NULL;
	log_info(kernel_logger, "El nombre del proceso es: [%s] y el tamaño del mismo es: [%d] \n", nombre_archivo, tam_proceso);
	if(list_is_empty(lista_new)){
		un_pcb = create_pcb(nombre_archivo, tam_proceso, 0);
		pthread_mutex_lock(&mutex_lista_new);
		list_add(lista_new, un_pcb);
		log_info(kernel_logger,"El PID es %d", un_pcb->pid);
		pthread_mutex_unlock(&mutex_lista_new);
		log_info(kernel_logger, "Creación de Proceso: ## (%d:0) Se crea el proceso - Estado: NEW", un_pcb->pid);
		//log_debug(kernel_log_obligatorio, "Voy a replanificar por proceso inicial");
		sem_post(&sem_cpu_disponible); //lo pongo aca porque es el inicio y la cpu esta disponible
		plp();
	}
}

int main(int argc, char** argv) {
	/*char* nombre = "PCB0"; //SACAR ESTO CUANDO EL TP ESTE LISTO
	char* tamaño = "100"; //SACAR ESTO CUANDO EL TP ESTE LISTO

	argv[2] = tamaño; //SACAR ESTO CUANDO EL TP ESTE LISTO
	(argv[1] = nombre; //SACAR ESTO CUANDO EL TP ESTE LISTO
	*/
//	char path[256]; 
//	strcpy(path, "/home/utnso/tp-2024-2c-cdec-/kernel/");
//	strcat(path, argv[3]);
	//log_info(kernel_logger,"Imprimo al path que voy a pasar para config <%s>", path);
	initialize_kernel(argv[3]);
	initialize_conexiones();
	crear_proceso_inicial(atoi(argv[2]),argv[1]); //argv[2] tam del proceso, y argv[1] es el nombre del archivo
	
	//CONSULTA. ya cuando nosotros mandamos al cpu ejecutamos esta funcion, asi que no se
	// si debe estar ejecutando por este lado tambien
	//free(path);

		exec_in_detach((void*)attend_kernel_cpu_dispatch, NULL); 

	if(!strcmp(algoritmo_to_string(ALGORITMO_PLANIFICACION), "CMN")){
		exec_in_detach((void*)attend_kernel_cpu_interrupt, NULL);
	}
	

	exec_in_detach((void*)_gestionar_interrupt, NULL);

	exec_in_join((void*)pcp, NULL);


	//aca falta como las funciones que finalicen todo
	//eliminen conexiones, semaforos, mutex, etc
	
	return EXIT_SUCCESS;
}