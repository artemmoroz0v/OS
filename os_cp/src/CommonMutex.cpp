#include "CommonMutex.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>

CommonMutex shared_mutex_init(const char *name) {
    CommonMutex mutex = {NULL, 0, NULL, 0};
    errno = 0;
    mutex.shm_fd = shm_open(name, O_RDWR, 0660);
    if (errno == ENOENT) {
        mutex.shm_fd = shm_open(name, O_RDWR | O_CREAT, 0660);
        mutex.created = 1;
    }
    if (mutex.shm_fd == -1) {
        std:: cout << "An error while shm_open has been detected!" << std:: endl;
        return mutex;
    }
    if (ftruncate(mutex.shm_fd, sizeof(pthread_mutex_t)) != 0) {
        std:: cout << "An error while ftruncate has been detected!" << std:: endl;
        return mutex;
    }
    void *address = mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, mutex.shm_fd, 0);
    if (address == MAP_FAILED) {
        std:: cout << "An error with mmaping has been detected!" << std:: endl;
        return mutex;
    }
    pthread_mutex_t *mutex_ptr = (pthread_mutex_t *)address;
    // If shared memory was just created -- initialize the mutex as well.
    if (mutex.created) {
        pthread_mutexattr_t attr; // Deadlock to common shared data!
        if (pthread_mutexattr_init(&attr)) {
            std:: cout << "An error while pthread_mutexattr_init has been detected!" << std:: endl;
            return mutex;
        }
        if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)) { // PTHREAD_PROCESS_SHARED - may be operated on by any thread in any process that has access to it
            std:: cout << "An error while pthread_mutexattr_setpshared has been detected!" << std:: endl;
            return mutex;
        } //pthread_mutexattr_setpsharedshall set the process-shared attribute in an initialized attributes object referenced by attr.
        if (pthread_mutex_init(mutex_ptr, &attr)) {
            std:: cout << "An error while pthread_mutex_init has been detected!" << std:: endl;
            return mutex;
        }
    }
    mutex.ptr = mutex_ptr;
    mutex.name = (char *)malloc(NAME_MAX + 1);
    strcpy(mutex.name, name);
    return mutex;
}
int shared_mutex_destroy(CommonMutex mutex) {
    if ((errno = pthread_mutex_destroy(mutex.ptr))) {
        std:: cout << "An error while destroying mutex has been detected!" << std:: endl;
        return -1;
    }
    if (munmap((void *)mutex.ptr, sizeof(pthread_mutex_t))) {
        std:: cout << "An error while munmap has been detected!" << std:: endl;
        return -1;
    }
    mutex.ptr = NULL;
    if (close(mutex.shm_fd)) {
        std:: cout << "An error while closing has been detected!" << std:: endl;
        return -1;
    }
    mutex.shm_fd = 0;
    if (shm_unlink(mutex.name)) {
        std:: cout << "An error while shm_unlink has been detected!" << std:: endl;
        return -1;
    }
    free(mutex.name);
    return 0;
}