#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcpDataStreaming.h"
//#include "structures.h"

int main(){
	
	int sd=passiveTCPSocket();

	listen ( sd , 5 );
	
	//accept

	close(sd);
	

	printf("socket/n");

	return 0;
}
