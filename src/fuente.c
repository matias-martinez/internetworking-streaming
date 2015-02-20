#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "tcpDataStreaming.h"
#include "structures.h"

int main() {


	int sdf = connectTCP();
    Header header;
	Sus paquete_sus;
    paquete_sus = malloc(sizeof(struct Sus));
    header = malloc(sizeof(struct Header));
	int len, recibido, enviados;	
//  memset(&data2, 0, sizeof(data2));
    
    paquete_sus = Paquete_crear_sus(0, "text/plain;medicion temperatura", header);
	enviados = Paquete_enviar_sus(sdf, header, paquete_sus);
	printf("Enviados %d Bytes \n", enviados);
    
	//esperar rta

//	recibido = Paquete_recibir_header(sdf);
//	printf("Recibi %d Bytes\n", recibido);
    
//    printf("Recibi un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);

	//en base a Dlen, hacer un receive de lo que falta
	//paqueteRecepcion = (msj_t *)&data2;
	//printf("Payload Recibida: %s\n", paqueteRecepcion->data);


	close(sdf);

	return 0;

}
