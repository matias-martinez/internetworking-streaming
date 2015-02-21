#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "tcpDataStreaming.h"
#include "structures.h"

int main(int argc, char *argv[]) {

    if (argc != 2)
    {
        printf("usage: %s filename\n",argv[0] );
        exit(1);
    }
    int sdf = connectTCP();
    Header header;
    Sus paquete_sus;
    Resp paquete_resp;
    paquete_sus = malloc(sizeof(struct Sus));
    paquete_resp = malloc(sizeof(struct Resp));    
    int len, enviados,recibidos;
    FILE *fp;
    fp = fopen(argv[1],"rw+");
    if (fp == NULL) {
    printf( "Fallo apertura de archivo de entrada de datos. \n");
  
    }


    paquete_sus = Mensaje_crear_sus(0, "text/plain;medicion temperatura\0");
    enviados = Mensaje_enviar_sus(sdf, paquete_sus);
    printf("Enviados %d Bytes - Esperando RESP! \n", enviados);
    
    header = Mensaje_recibir_header(sdf);
    printf("Recibi un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);
    
    paquete_resp = Mensaje_recibir_resp(sdf,header->dlen);
    printf("Recibi un RESP de tipo %d y codigo %d\n", paquete_resp->tipo, paquete_resp->codigo);
    
    if(paquete_resp->tipo==0 && paquete_resp->codigo==11){
        
    
        printf("MI ID ES %s\n",paquete_resp->data);
        printf("Comienzo de Envio de Datos hacia el Servidor\n");
        //TODO: ENVIAR POSTS!!


    }
    close(sdf);

    return 0;

}
