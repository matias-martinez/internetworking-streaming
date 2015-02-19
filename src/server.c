#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcpDataStreaming.h"
#include "structures.h"
#include "list.h"


int main(){

    List fuentes = List_create(); 
	int sd = passiveTCPSocket(4567); //from tcpDataStreaming ->recibir puerto como parametro en ejecucion.
	int sdf, lon, recibido, op, dlen, recibidos;
	uint8_t op2;
	uint16_t dlen2;
	struct sockaddr_in fuente;
	msj_t *mensajeEnvio;
	msj_t *mensajeRecepcion;
	msj_t *header;
	char *paquete;
	char *aux;
	char resp[3] = "001" ;//dlen =0, tipo=0, codigo=11, VARIABLE: idfuente=001->en este caso al ser solo un dato no lo separo
	//mensajeEnvio=malloc(sizeof(msj_t));
	//mensajeRecepcion=malloc(sizeof(msj_t));
	printf("Servidor DataStreaming - v0.1\n");
	
	while (1){

		lon = sizeof(fuente);
		printf("Aguardando connect\n");
		sdf = accept(sd, (struct sockaddr *) &fuente, &lon);
		printf("Recibí connect desde: %s \n", inet_ntoa(fuente.sin_addr));

		
		//Hacer receive del HEADER, para luego leer payload
		//VERSION2		
		paquete = malloc(3);
		recibidos = receiveall(sdf, paquete, 3);
		header = (msj_t *) paquete;
		//free(paquete);

		//Recepcion Payload
		printf("Recepcion Header. OP = %d. DLEN= %d. bytesHeader = %d\n", header->opcode, header->dlen, recibidos);
		//paquete = malloc(header->dlen);
		dlen2 = header->dlen;
		op2 = header->opcode;
		free(paquete);
		paquete = malloc(dlen2);
		recibido = receiveall(sdf, paquete, dlen2);
		printf("Recibi datos %d  %d\n", recibido, dlen2);
		mensajeRecepcion = malloc(sizeof(msj_t));
		mensajeRecepcion->opcode = op2;
		mensajeRecepcion->dlen = dlen2;
		strcpy(mensajeRecepcion->data, paquete);

		switch(mensajeRecepcion->opcode){

			case 2:
				printf("----\n");
				strncpy(&aux, mensajeRecepcion->data, 1);
				op2 = (uint8_t) aux;
				printf("Operacion Mensaje SUS = %d\n",op );
				if (op2 == 0){
                    // TODO: Checkear que la fuente no este en la lista
                    // TODO: resolver Hardcodeado

                    char *ip =  inet_ntoa(fuente.sin_addr);
                    if (List_search_by_ip(fuentes, ip) == -1) {
                        ListNode fuente_node = ListNode_create("plain/text", "temp", ip);
                        if (fuente_node == NULL) {
                            printf("fallo la alocacion");
                        }
                        int id = List_push(fuentes, fuente_node);
                        printf("Se asigna a la fuente de ip: %s, el id %d\n", ip, id);
                    } else {
                        printf("Fuente ya registrada.\n");
                    }
					// Guardar fuente en lista
					//Contestar RESP

				    printf("----\n");
				}
				
				break;



		}

/*
		//VERSION 1
		recibido = receiveall(sdf, paquete, 15 );
		printf("Recibi %d\n", recibido);

		mensajeRecepcion = (msj_t *) paquete;

		printf("Payload Recibida: %s\n", mensajeRecepcion->data);
		//Agregar Fuente a Lista de Fuentes
		//Respuesta de Exito
		pack(3, &resp, &mensajeEnvio);//Envio un RESP
		lon = sizeof(mensajeEnvio);
		sendall(sdf, (char *) &mensajeEnvio, lon );

*/

        close(sdf);
	}
	close(sd);
	return 0;
}
