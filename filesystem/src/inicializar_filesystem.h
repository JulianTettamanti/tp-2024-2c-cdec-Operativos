#include <fs-gestor.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

void inicializar_estructuras_fs();
void remove_files_in_directory(const char* path);
void eliminar_filesystem();
