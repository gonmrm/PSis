#define MSG_LIMIT 100
#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>

#include "clipboard.h"


int main(void){

	char buf[MSG_LIMIT];
	int region;
	void *pointer;
	int n;

	int clipboard_id = clipboard_connect("./");
	if(clipboard_id == -1){
		printf("Clipboard connection error\n");
		exit(-1);
	}

	printf("REGION: \n");
	scanf("%d", &region); // region
        while(getchar()!='\n'); //limpa o buffer


	while(1){
		pointer = buf;

		n = clipboard_wait(clipboard_id,region,pointer,sizeof(buf));
		if(n>0)	printf("Region %d changed: %s\n", region, (char *)pointer);
		else printf("WAIT ERROR\n");
	
	}	
}
