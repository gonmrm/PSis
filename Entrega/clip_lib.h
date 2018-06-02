#define REGIONS 10
#define MSG_LIMIT 100
#define COPY 0
#define PASTE 1
#define UPDATE 5
#define QUIT 9
#define INVALID_OPTION 2
#define OUT 'O'
#define IN 'I'
#define ON 1
#define OFF 0
#define STOP_U "/semup"
#define STOP_D "/semd"
#define NOT_UPDATED 0
#define UPDATED 1
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
#include <pthread.h>
#include <semaphore.h>



typedef struct data {
	char characters[MSG_LIMIT];
	int region;
	int option;
}DATA;



void *listen_remote(void *arg);
void *new_app(void *arg);
void *listen_local(void *arg);
void ctrl_c_callback_handler(int signum);
int check_mode(int argc, char *argv[]);
char* makes_name_sem(char *sem);
void *new_rem_clip(void *arg);
//void * sems (void *arg);
void * remote_com(void * arg);
void * up_sendt(void * arg);
void * up_recvt(void * arg);
void * d_sendt(void * arg);
void * d_recvt(void * arg);
void display_data(char data[REGIONS][MSG_LIMIT]);
