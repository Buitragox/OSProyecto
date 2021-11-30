#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "request.h"
#include "msg.h"
#include "list.h"

char default_root[] = ".";

sender_args *sender_list;
char (*buffers_list)[MAXBUF];
int active_threads = 0;

int pqSize = 0;
Node* pqueue = NULL;

pthread_mutex_t lock_activet;
pthread_mutex_t lock_queue;

int msg_id;
int bSize;

// ./wserver [-d <basedir>] [-p <portnum>] 

//Asigna la conexión y los datos respectivos a un hilo;
void assign_thread(int conn_fd, int index) {

	printf("ASSIGN starting %d %d\n", conn_fd, index);
	pthread_t thread_worker;
	sender_list[index].msg_id = msg_id;
	sender_list[index].conn_fd = conn_fd;
	sender_list[index].buff = buffers_list[index];
	sender_list[index].lockptr = &lock_activet;
	sender_list[index].activet_ptr = &active_threads;

	pthread_create(&thread_worker, NULL, msg_sender, &sender_list[index]);

	printf("assign_thread finishing\n"); 

	// pthread_mutex_lock(&lock_activet);
	// active_threads++;
	// printf("assign threads = %d\n", active_threads);
	// pthread_mutex_unlock(&lock_activet);
}


void* worker_assignment(void* arg) {
	int conn_fd;
	int index;
	while(1) {
		if (pqSize > 0 && active_threads < bSize) {
			//printf("before mutex pop\n");
			//Zona critica
			pthread_mutex_lock(&lock_queue);
			conn_fd = peekData(&pqueue);
			index = peekIndex(&pqueue);
			pop(&pqueue);
			pqSize--;

			printf("assign pqSize %d\n", pqSize);

			pthread_mutex_unlock(&lock_queue);

			//printf("after mutex pop\n");

			pthread_mutex_lock(&lock_activet);
			active_threads++;
			printf("assign threads = %d\n", active_threads);
			pthread_mutex_unlock(&lock_activet);

			assign_thread(conn_fd, index);
			//sleep(2);
		}
	}
}


int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
	char scheduler[10];

	//Struct para usar stat()
	struct stat stat_buf;

	//Buffers para leer datos la peticion http
	char method[MAXBUF], uri[MAXBUF], version[MAXBUF];
    char filename[MAXBUF], cgiargs[MAXBUF];

	int index = 0;
	
    while ((c = getopt(argc, argv, "d:p:b:s:")) != -1) {
		switch (c) {
		case 'd':
			root_dir = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'b':
			bSize = atoi(optarg);
			printf("buffer %d\n", bSize);
			//guarda, >= 1
			break;
		case 's':
			strcpy(scheduler, optarg);
			scheduler[9] = '\0';
			printf("sched %s\n", scheduler);
			//guarda, FIFO o SFF
			break;
		default:
			fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-b buffers] [-s schedalg]\n");
			exit(1);
		}
	}

	sender_args args_list[bSize + EXTRA_SPACE];
	char buffers[bSize + EXTRA_SPACE][MAXBUF];

	//Asignar datos globales
	sender_list = args_list;
	buffers_list = buffers;

	//Inicializar mutex de datos globales
	if (pthread_mutex_init(&lock_activet, NULL) != 0) {
        printf("\n mutex active_threads init has failed\n");
        exit(1);
    }

	if (pthread_mutex_init(&lock_queue, NULL) != 0) {
        printf("\n mutex queue init has failed\n");
        exit(1);
    }


    // run out of this directory
    chdir_or_die(root_dir);

	//Open the message queue
	key_t key = ftok(PATH_NAME, PROJECT_ID);
	msg_id = msgget(key, 0666 | IPC_CREAT);
	if(msg_id == -1){
		printf("msgget error\n");
		exit(-1);
	}

	// Crear el hilo encargado de la base de datos
	pthread_t thread_receiver;
	receiver_args ra;
	ra.msg_id = msg_id;
	pthread_create(&thread_receiver, NULL, msg_receiver, &ra);

	// Crear el hilo encargado de asignar los requests a los hilos
	pthread_t thread_assignment;
	pthread_create(&thread_assignment, NULL, worker_assignment, NULL);

	int conn_fd;
	int listen_fd = open_listen_fd_or_die(port);

	//Hilo maestro se encarga de aceptar conexiones y ponerlas en la cola
	while (1){

		if (pqSize == bSize)
		{
			printf("full queue %d\n", pqSize);
			continue;
		}

		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);


		// Preprocesamiento del archivo
		readline_or_die(conn_fd, buffers[index], MAXBUF);
		sscanf(buffers[index], "%s %s %s", method, uri, version);
		//printf("method:%s uri:%s version:%s\n", method, uri, version);
		
		//Check method == "GET"
		if (strcasecmp(method, "GET")) {
			request_error(conn_fd, method, "501", "Not Implemented",
						 "server does not implement this method");
			close_or_die(conn_fd);
			continue;
		}
		
		request_read_headers(conn_fd);
		
		// Obtener el nombre del archivo pedido
		request_parse_uri(uri, filename, cgiargs);

		// Revisar si existe y cuál es su tamaño con stat()
		int res = stat(filename, &stat_buf);
		if (res < 0) {
			request_error(conn_fd, filename, "404", "Not found", 
						"server could not find this file");
			close_or_die(conn_fd);
			continue;
		}

		int prio;
		if (strcasecmp("SFF\0", scheduler) == 0)
			prio = stat_buf.st_size;
		else
			prio = 0;

		
		//Zona critica de la priority queue
		pthread_mutex_lock(&lock_queue);
		push(&pqueue, conn_fd, prio, index);
		pqSize++;
		printf("master pqSize %d\n", pqSize);
		pthread_mutex_unlock(&lock_queue);
		
		//index = (index + 1) % (buffer_size + EXTRA_SPACE)
		index++;
		if (index == bSize + EXTRA_SPACE)
			index = 0;
		
	}
    return 0;
}


    


 
