#ifndef __MSG__
#define __MSG__

#include <stdio.h>
#include <time.h>
#include <sys/msg.h>
#include "sqlite3/sqlite3.h"
#include "request.h"
#include "io_helper.h"

#define TIME_SIZE 27
#define PATH_NAME  "msg.h"
#define PROJECT_ID 98123
#define DB_PATH "/home/ragnaro7/Documentos/SO2parcial/SegundoParcialOS/sqlite3/dbProject.db"

typedef struct msgtime {
    time_t start_time;
    time_t end_time;
} msgtime;

typedef struct sender_args {
    int msg_id;
    int conn_fd;
} sender_args;



void error_exit(int msg_id, sqlite3* db);

void* msg_receiver(void* arg);

void* msg_sender(void *arg);

#endif // __MSG__