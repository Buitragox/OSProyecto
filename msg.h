#ifndef __MSG__
#define __MSG__

#include <stdio.h>
#include <time.h>
#include <sys/msg.h>
#include "sqlite3/sqlite3.h"
#include "io_helper.h"
#include <pthread.h>

#define MAXBUF (8192)
#define EXTRA_SPACE 200
#define TIME_SIZE 27
#define PATH_NAME  "msg.h"
#define PROJECT_ID 67836
#define DB_PATH "/home/ragnaro7/Escritorio/ProyectoEntrega3/sqlite3/dbProject.db"

typedef struct msgtime {
    time_t start_time;
    time_t end_time;
} msgtime;

typedef struct sender_args {
    int msg_id;
    int conn_fd;
    char *buff;
} sender_args;

typedef struct reciever_args {
    int msg_id;
    int *active_threads;
    pthread_mutex_t *lock;
} reciever_args;



void error_exit(int msg_id, sqlite3* db);

void* msg_receiver(void *arg);

void* msg_sender(void *arg);

#endif // __MSG__