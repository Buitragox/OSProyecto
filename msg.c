#include "msg.h"
#include "request.h"

void error_exit(int msg_id, sqlite3* db){
    sqlite3_close(db);
    int msg_rc = msgctl(msg_id, IPC_RMID, 0);
    if (msg_rc == -1) {
        printf("msgctl(IPC_RMID) failed\n");
    }
    else{
        printf("mailbox closed\n");
    }
    exit(-1);
}


void* msg_receiver(void* arg){
    receiver_args param = *((receiver_args *) arg);
    msgtime horarios;
    uuid_t binuuid;

    printf("ThreadSafe mode = %d\n", sqlite3_threadsafe());
    //printf("horarios %ld\nmsgtime %ld\n", sizeof(horarios), sizeof(msgtime));

    long int msgtype;
    //FILE* f;
    char * errmsg;
    char uuid[UUID_STR_LEN];
    struct tm * start_time;
    struct tm * end_time;
    sqlite3* db;
    int msg_rc;
    sqlite3_open(DB_PATH, &db);
    int rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS PROCESSES(UUID varchar(38), START varchar(35), END varchar(35));", NULL, NULL, &errmsg);
    if(rc != SQLITE_OK){
        printf("Error at creating table: %s\n", errmsg);
        sqlite3_free(errmsg);
        error_exit(param.msg_id, db);
    }
    while(1){
        //printf("Start of while \n");

        //msg_rc = msgrcv(param.msg_id, (void *)&horarios, sizeof(msgtime), msgtype, 0);
        msg_rc = msgrcv(param.msg_id, (void *)&horarios, sizeof(msgtime), msgtype, 0);
        //printf("msg_rc %d\n", msg_rc);
        if(msg_rc == -1){
            printf("\nerror %d \n\n", errno); printf("\nError: %s\n\n", strerror(errno));
            printf("msgrcv fail\n");
            error_exit(param.msg_id, db);
        }

        // pthread_mutex_lock(param.lockptr);
        // (*param.activet_ptr)--;
		// pthread_mutex_unlock(param.lockptr);
        

        //Make uuid

        // f = popen("uuidgen -r", "r");
        // fgets(uuid, sizeof(uuid), f);
        // pclose(f);

        uuid_generate_random(binuuid);
        uuid_unparse_lower(binuuid, uuid);
        
        //printf("%s\n", uuid);
        start_time = localtime(&horarios.start_time);
        end_time = localtime(&horarios.end_time);
        //Quitar /n
        char* str_start_time = asctime(start_time);
        str_start_time[strlen(str_start_time) - 1] = 0;

        char* str_end_time = asctime(end_time);
        str_end_time[strlen(str_end_time) - 1] = 0;
        
        
        char sqlinsert[150];
        snprintf(sqlinsert, 150, "INSERT INTO PROCESSES VALUES('%s', '%s', '%s');", uuid, str_start_time, str_end_time);
        rc = sqlite3_exec(db, sqlinsert, NULL, NULL, &errmsg);
        //printf("Recieve MSG \n");
        if(rc != SQLITE_OK){
            printf("Error: %s\n", errmsg);
            sqlite3_free(errmsg);
            error_exit(param.msg_id, db);
        }


        // Mensajes Debug
        //printf("%s\n", sqlinsert);
        printf ("Inicio: %s", asctime(start_time));
        printf ("Final: %s", asctime(end_time));


    }
}

void* msg_sender(void *arg){
    sender_args param = *((sender_args *) arg);
    
    char method[MAXBUF], uri[MAXBUF], version[MAXBUF];
    sscanf(param.buff, "%s %s %s", method, uri, version);

    msgtime horarios;
    time(&horarios.start_time);
    request_handle(param.conn_fd, method, uri, version);

    
    close_or_die(param.conn_fd);
    //printf("\nCLOSED %d \n", param.conn_fd);
    time(&horarios.end_time);

    horarios.mtype = 1;
    int msg_rc = msgsnd(param.msg_id, (void *)&horarios, sizeof(msgtime), 0);
    if(msg_rc == -1) {
        printf("\nerror %d\n", errno); printf("Error: %s\n\n", strerror(errno));
        printf("msgsnd fail\n");
        exit(-1);
    }
    //Mensajes debug
    printf("thread %ld\n", pthread_self());

    // Zona critica active_threads
    pthread_mutex_lock(param.lockptr);
    (*param.activet_ptr)--;
    printf("threads = %d\n", (*param.activet_ptr));
    pthread_mutex_unlock(param.lockptr);

    pthread_exit(0);
}