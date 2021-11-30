#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "request.h"
#include "msg.h"
#include "list.h"

char default_root[] = ".";

int active_threads = 0;

pthread_mutex_t lock;

// ./wserver [-d <basedir>] [-p <portnum>] 

int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
	char scheduler[10];
	int buffer_size;
	pthread_t thread_receiver;
	struct stat stat_buf;
	char method[MAXBUF], uri[MAXBUF], version[MAXBUF];
    char filename[MAXBUF], cgiargs[MAXBUF];
	int is_static;

	int index = 0;
	
    while ((c = getopt(argc, argv, "d:p:b:s:")) != -1)
	switch (c) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	case 'b':
		buffer_size = atoi(optarg);
		printf("buffer %d\n", buffer_size);
		break;
	case 's':
		optarg[6];
		strcpy(scheduler, optarg);
		scheduler[9] = '\0';
		printf("sched %s\n", scheduler);
		break;
	default:
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-b buffers] [-s schedalg]\n");
	    exit(1);
	}

	Node* priorityqueue = newNode(0, 0);
	sender_args args_list[buffer_size + EXTRA_SPACE];
	char buffers[buffer_size + EXTRA_SPACE][MAXBUF];

	if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        exit(1);
    }


    // run out of this directory
    chdir_or_die(root_dir);
	//Abrir el mailbox
	key_t key = ftok(PATH_NAME, PROJECT_ID);
	int msg_id = msgget(key, 0666 | IPC_CREAT);
	printf("master msg_id %d\n", msg_id);
	if(msg_id == -1){
		printf("msgget error\n");
		exit(-1);
	}

	// Crear el hilo encargado de la base de datos
	reciever_args ra;
	ra.msg_id = msg_id;
	ra.lock = &lock;
	ra.active_threads = &active_threads;
	pthread_create(&thread_receiver, NULL, msg_receiver, &ra);

	int listen_fd = open_listen_fd_or_die(port);
	while (1){
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		

		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);

		//printf("\nOPEN %d\n", conn_fd);
		// Preprocesamiento del archivo
		readline_or_die(conn_fd, buffers[index], MAXBUF);
		sscanf(buffers[index], "%s %s %s", method, uri, version);
		printf("method:%s uri:%s version:%s\n", method, uri, version);
		

		if (strcasecmp(method, "GET")) {
			request_error(conn_fd, method, "501", "Not Implemented", "server does not implement this method");
			close_or_die(conn_fd);
			continue;
			//return;
		}
		
		request_read_headers(conn_fd);

		is_static = request_parse_uri(uri, filename, cgiargs);
		//request_parse_uri(uri, filename, cgiargs);

		int res = stat(filename, &stat_buf);
		//printf("STAT termina %d\n", res);
		if (res < 0) {
			//printf("Me demore\n");
			request_error(conn_fd, filename, "404", "Not found", "server could not find this file");
			close_or_die(conn_fd);
			continue;
			//return;
		}
		//printf("master %d %d\n", conn_fd, stat_buf.st_size);

		pthread_t thread_worker;
		
		args_list[index].msg_id = msg_id;
		args_list[index].conn_fd = conn_fd;
		args_list[index].buff = buffers[index];
		
		//printf("antes de hilo\n");

		pthread_create(&thread_worker, NULL, msg_sender, &args_list[index]); 
		index++;
		if (index == buffer_size + EXTRA_SPACE)
			index = 0;

		//printf("despues de hilo\n");

		//pthread_join(thread_receiver, NULL);
	}
    return 0;
}


    


 
