#include <fs-gestor.h>

void bitmap_escribir_archivo(int bloque_inical,int bloques_tamanio);
void bitmap_modificar_archivo_bitmap();
int bitmap_obtener_bloques_libres(int cantidad,char* nombre_archivo);
bool hay_espacio_en_bitmap(int cant_bloques);
t_bitarray* cargar_bitmap_desde_archivo(char* ruta_archivo, size_t tamanio_bitmap) ;

void blockFile_escribir_archivo(void*datos,size_t posicion, int tamanio_datos, char* nombre_archivo, char* tipo);
void blockFile_escribir_archivo_indices(void*datos,size_t posicion, int tamanio_datos, char* nombre_archivo, char* tipo);
void* generar_bloque_index(int base,int cantidad_bloques);

char* metadata_crear_archivo(int base_bloque, int size_archivo, char *nombre_archivo);

