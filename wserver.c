#include "msg.h"
#include <pthread.h>

char default_root[] = ".";

// ./wserver [-d <basedir>] [-p <portnum>] 

int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
	pthread_t thread_reciever;

	sender_args args_list[1000];
	int index = 0;
	
    while ((c = getopt(argc, argv, "d:p:")) != -1)
	switch (c) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	default:
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port]\n");
	    exit(1);
	}

    // run out of this directory
    chdir_or_die(root_dir);
    
	//Abrir el mailbox
	key_t key = ftok(PATH_NAME, PROJECT_ID);
	int msg_id = msgget(key, 0666 | IPC_CREAT);
	if(msg_id == -1){
		printf("msgget error\n");
		exit(-1);
	}

	// Crear el hilo encargado de la base de datos
	pthread_create(&thread_reciever, NULL, msg_receiver, &msg_id); 

	int listen_fd = open_listen_fd_or_die(port);
	while (1){
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		//printf("\n++++++MAIN fd: %d\n\n", conn_fd);
		pthread_t thread_worker;

		if (index == 1000)
			index = 0;
		
		args_list[index].msg_id = msg_id;
		args_list[index].conn_fd = conn_fd;

		// sender_args args; 
		// args.msg_id = msg_id;
		// args.conn_fd = conn_fd;

		pthread_create(&thread_worker, NULL, msg_sender, &args_list[index]); 
		index++;
		//pthread_join(thread_worker, NULL);
	}
    return 0;
}


    


 
