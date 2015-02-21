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
    Resp paquete_resp;
    char aux[128];
    
    
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
                        sprintf(aux, "%d", id);
                        paquete_resp = Mensaje_crear_resp(0,11,aux);
                        Mensaje_enviar_resp(sdf,paquete_resp);//tipo=0;Codigo=11;data=IDFUENTE

                         }
                    else {
                        paquete_resp = Mensaje_crear_resp(1,21,"");//Tipo=1;Codigo=21;Data=NULL
                        Mensaje_enviar_resp(sdf,paquete_resp);

                    }
                }
                if (paquete_sus->op==1){
                    //TODO: Solicitud de Sucripcion de Consumidor. Proxima Version.
                }
                if (paquete_sus->op==2){
                    //TODO: Borrado de Suscripcion. 


                }

                break;



        }

       
        close(sdf);
    }
    close(sd);
    return 0;
}
