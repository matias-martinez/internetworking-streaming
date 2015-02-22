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
    Post paquete_post;
    int len, enviados,recibidos,id;
    time_t rawtime;
    struct tm *timestamp;
    char* temp = malloc(128);//alberga lineas leidas del archivo

    FILE *fp;
    fp = fopen(argv[1],"rw");
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
    close(sdf);
    if(paquete_resp->tipo==0 && paquete_resp->codigo==11){
       
        id=atoi(paquete_resp->data);
        printf("Mi ID ES %d\n",id );
        printf("Comienzo de Envio de Datos hacia el Servidor\n");
        // ENVIo POSTS!!
        while (fscanf(fp, "%s", temp) != EOF) {   
            //TODO: Fix Campo DATA
            sdf = connectTCP();         
            time(&rawtime);
            timestamp = localtime(&rawtime);
            paquete_post = Mensaje_crear_post(id, timestamp, temp);
            printf("Campo DATA enviado:%s\n", paquete_post->data);
            Mensaje_enviar_post(sdf,paquete_post);

            header = Mensaje_recibir_header(sdf);
            printf("Recibi un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);
            paquete_resp = Mensaje_recibir_resp(sdf,header->dlen);
            printf("Recibi un RESP de tipo %d y codigo %d\n", paquete_resp->tipo, paquete_resp->codigo);

            close(sdf);
            temp = realloc(temp, 128);
            }
        }else{
            printf("Recibi un Codigo de Error. Saliendo...\n");
            exit(1);
    }
    close(sdf);

    return 0;

}
