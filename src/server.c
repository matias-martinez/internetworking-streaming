#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "list.h"
#include "tcpDataStreaming.h"
#include "structures.h"


struct ListNode {
	int id;
	char *ip;	
} fuente;



int main(){
	
	int sd = passiveTCPSocket(); //from tdpDataStreaming
    List fuentes;
	int sdf, lon, recibido;
	struct sockaddr_in fuente;
	msj_t mensajeEnvio;
	msj_t *mensajeRecepcion;
	char paquete[132];
	char resp[3] = "001" ;//dlen =0, tipo=0, codigo=11, VARIABLE: idfuente=001->en este caso al ser solo un dato no lo separo
	
	while (1){

		lon = sizeof(fuente);
		printf("Aguardando connects\n");
		sdf = accept(sd, (struct sockaddr *) &fuente, &lon);	
		printf("Recibí connect desde: %s \n", inet_ntoa(fuente.sin_addr));
		
		//Hacer receive del HEADER, para luego leer payload

		recibido = receiveall(sdf, paquete, 15 );
		printf("Recibi %d\n", recibido);

		mensajeRecepcion = (msj_t *) paquete;

		printf("Payload Recibida: %s\n", mensajeRecepcion->data);
		//Agregar Fuente a Lista de Fuentes
		//Respuesta de Exito
		pack(3, &resp, &mensajeEnvio);//Envio un RESP
		lon = sizeof(mensajeEnvio);
		sendall(sdf, (char *) &mensajeEnvio, &lon );


		close(sdf);
	}
	close(sd);
	return 0;
}
