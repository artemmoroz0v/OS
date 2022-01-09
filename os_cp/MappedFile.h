#ifndef MAPPED_FILE_H
#define MAPPED_FILE_H
#define _MAPPED_SIZE 8192
#define _SHM_OPEN_MODE S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH
#define _BUFFER_NAME "mybuffer.buf"
#define _MUTEX_NAME "mymutex.mutex"
#define _MSG_SEP '$'
struct MappedFile {
    int fd;
    char *data;
};
#endif