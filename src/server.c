#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
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
    int sdf, lon;
    struct sockaddr_in fuente;
    Header header;
    Sus paquete_sus;

    printf("Servidor DataStreaming - v0.1\n");

    while (1) {

        lon = sizeof(fuente);
        printf("Aguardando connect\n");
        sdf = accept(sd, (struct sockaddr *) &fuente, &lon);
        printf("Recibí connect desde: %s \n", inet_ntoa(fuente.sin_addr));

        header = Mensaje_recibir_header(sdf);

        printf("Recibi un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);

        switch(header->opcode){

            case 2:
                paquete_sus = Mensaje_recibir_sus(sdf, header->dlen);
                printf("----\n");
                printf("Operacion Mensaje SUS = %d\n", paquete_sus->op);
                printf("recibi estos datos: %s\n", paquete_sus->data);
                if (paquete_sus->op == 0){
                    // TODO: resolver Hardcodeado
                    char *ip = inet_ntoa(fuente.sin_addr);
                    if (List_search_by_ip(fuentes, ip) == -1) {
                        ListNode fuente_node = ListNode_create("plain/text", "temp", ip);
                        if (fuente_node == NULL) {
                            printf("fallo la alocacion");
                        }
                        int id = List_push(fuentes, fuente_node);
                        printf("Se asigna a la fuente de ip: %s, el id %d\n", ip, id);   

                        //TODO: Contestar RESP exitoso
                    }
                    else {
                        //TODO: Contestar Resp fallido
                    }
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
