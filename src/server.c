#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <queue.h>
#include "tcpDataStreaming.h"
#include "structures.h"


struct fuente{
	int id;
	char *ip;

	//definir LISTA!!
};



int main(){
	
	int sd=passiveTCPSocket(); //from tdpDataStreaming
	int sdf, lon, recibido;
	struct sockaddr_in fuente;
	msj_t *mensaje;
	char paquete[132];
	

	

	
	while (1){

		lon= sizeof(fuente);
		printf("Aguardando connects\n");
		sdf = accept(sd, (struct sockaddr *) &fuente, &lon);	
		printf("Recibí connect desde: %s \n", inet_ntoa(fuente.sin_addr));
		recibido = receiveall(sdf, paquete, 250 );
		printf("Recibi %d\n", recibido);

		mensaje = (msj_t *) paquete;

		printf("DATOS: %s\n", mensaje->data);

		close(sdf);
	}
	close(sd);
	return 0;
}
