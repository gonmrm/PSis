#define REGIONS 10
#define MSG_LIMIT 100
#define COPY 0
#define PASTE 1
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <libio.h>
#include "clipboard.h"

typedef struct data {
	char characters[MSG_LIMIT];
	int region;
	int option;
}DATA;

int clipboard_connect(char * clipboard_dir){
	
	struct sockaddr_un server_addr;
	struct sockaddr_un client_addr;

	int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	
	if (sock_fd == -1){
		perror("socket: ");
		return sock_fd;
	}

	client_addr.sun_family = AF_UNIX;
	sprintf(client_addr.sun_path, "%ssocket_%d", clipboard_dir, getpid());

	server_addr.sun_family = AF_UNIX;
	sprintf(server_addr.sun_path, "%sCLIPBOARD_SOCKET", clipboard_dir);

	int err_c = connect(sock_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr));
	if(err_c==-1){
		perror("Connecting: ");
		return -1;
	}
	return sock_fd;
}



int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){

	void sigpipe_handler(){};
	signal(SIGPIPE,sigpipe_handler);
	
	int n;
	int option = COPY;
	DATA new_data;

	if (region < 0 || region > 9){
		printf("Invalid region, should just write digit between 0 and 9\n");
		return 0;
	}

	char *msg = malloc(count*sizeof(char));

	memcpy(msg, buf, count);

	printf("\nCLIPBOARD COPY\n"); // é para remover
	printf("Copying %s to region %d\n", msg, region);// é para remover

	strcpy(new_data.characters, msg);
	new_data.region = region;
	new_data.option = option;

	free(msg);

	n = send(clipboard_id, &new_data, sizeof(new_data), 0);
	if (n<=0){
		printf("CLIPBOARD NOT AVAILABLE\n");
		return 0;
	}else{
		return n;
	}
}



int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){

	void sigpipe_handler(){};
	signal(SIGPIPE,sigpipe_handler);
	
	int n;
	int option = PASTE;
	DATA new_data;

	if (region < 0 || region > 9){
		printf("Invalid region, should just write digit between 0 and 9\n");
		return 0;
	}

	char *msg = malloc(count*sizeof(char));

	new_data.region = region;
	new_data.option = option;
	
	printf("clipboard PASTE\n");

	n = send(clipboard_id, &new_data, sizeof(new_data), 0);
	if (n<=0){
		printf("CLIPBOARD UNAVAILABLE\n");
		free(msg);
		return 0;
	}

	n = recv(clipboard_id, msg, count*sizeof(char), 0);
			
	while(n == -1){
		n = recv(clipboard_id, msg, count*sizeof(char), 0);
		if (n == 0) {
			printf("CLIPBOARD EXITED\n"); 
			free(msg);
			return 0;
		}
	}

	memcpy(buf, msg, count);

	free(msg);

	return n;
	
}



int clipboard_wait(int clipboard_id, int region, void *buf, size_t count){

	void sigpipe_handler(){};
	signal(SIGPIPE,sigpipe_handler);

	int n;
	int option = PASTE;
	DATA new_data;


	if (region < 0 || region > 9){
		printf("Invalid region, should just write digit between 0 and 9\n");
		return 0;
	}

	new_data.region = region;
	new_data.option = option;
	
	n = send(clipboard_id, &new_data, sizeof(new_data), 0);
	if (n<0){
		printf("INVALID CLIP_ID\n");
		return 0;
	}

	if(n==0){ printf("CLIPBOARD EXITED\n"); return 0;}

	char *current = malloc(count*sizeof(char));

	n = recv(clipboard_id, current, count*sizeof(char), 0);
			
	while(n == -1){
		n = recv(clipboard_id, current, count*sizeof(char), 0);
		if(n == 0){ 
			printf("CLIPBOARD EXITED\n"); 
			free(current); 
			return 0;
		}
	}

	char *msg = malloc(count*sizeof(char));

	memcpy(msg, current, count);

	while(strcmp(current, msg) == 0){
		n = send(clipboard_id, &new_data, sizeof(new_data), 0);
		if (n<=0){
			printf("CLIPBOARD EXITED\n");
			free(msg);
			free(current);
			return 0;
		}

		n = recv(clipboard_id, msg, count*sizeof(char), 0);
			
		while(n == -1){
			n = recv(clipboard_id, msg, count*sizeof(char), 0);
			if(n == 0){ 
				printf("CLIPBOARD EXITED\n"); 
				free(msg);
				free(current);
				return 0;
			}
		}
	}

	memcpy(buf, msg, count);

	free(current);

	free(msg);

	return n;
}

void clipboard_close(int clipboard_id){
	close(clipboard_id);
}
