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
	int sdf, lon;
	struct sockaddr_in fuente;

	//listen ( sd , 5 );
	lon= sizeof(fuente);
	printf("Aguardando connect\n");
	sdf = accept(sd, (struct sockaddr *) &fuente, &lon);
	
	printf("Recibí connect desde: %s \n", inet_ntoa(fuente.sin_addr));
	

	close(sd);
	

	printf("socket\n");

	return 0;
}