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
    Resp paquete_resp;
    paquete_sus = malloc(sizeof(struct Sus));
    paquete_resp = malloc(sizeof(struct Resp));    
    int len, enviados,recibidos;

    paquete_sus = Mensaje_crear_sus(0, "text/plain;medicion temperatura");
    enviados = Mensaje_enviar_sus(sdf, paquete_sus);
    printf("Enviados %d Bytes - Esperando RESP! \n", enviados);
    
    header = Mensaje_recibir_header(sdf);
    printf("Recibi un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);
    
    paquete_resp = Mensaje_recibir_resp(sdf,header->dlen);
    printf("Recibi un RESP de tipo %d y codigo %d\n", paquete_resp->tipo, paquete_resp->codigo);
    
    if(paquete_resp->tipo==0 && paquete_resp->codigo==11){
        
    
        printf("MI ID ES %s\n",paquete_resp->data);
        //TODO: ENVIAR POSTS!!

    }
    close(sdf);

    return 0;

}
