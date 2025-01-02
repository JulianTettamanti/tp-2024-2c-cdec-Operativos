#include  <../../utils/src/utils.h>


extern t_log* fs_logger;
extern t_log* fs_logger_debug;
extern t_log* fs_logger_obligatorio;
extern t_config* fs_config;

extern char* PUERTO_ESCUCHA;
extern char* MOUNT_DIR;
extern int BLOCK_SIZE;
extern int BLOCK_COUNT;
extern int RETARDO_ACCESO_BLOQUE;
extern t_log_level LOG_LEVEL;

extern pthread_mutex_t mutex_manejo_bitmap;
extern pthread_mutex_t mutex_manejo_blockfile;

//extern int socket_memoria;
extern int socket_fs;

extern char* PATH_BLOQUES;
extern char* PATH_BITMAP;

extern char* bitmap_memoria;
extern t_bitarray* bitmap;

extern int fd_archivoBloques;
extern int fd_bitmap;
