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

    if (argc != 4) {
        printf("usage: %s [FILE] [SERVERNAME] [PORT]\n", argv[0]);
        printf("Example ./fuente data.bat www.exaple.com 8888\n");
        exit(1);
    }
    
    char *end;
    char *host = argv[2];
    int port = strtol(argv[3], &end, 10);

    int sdf = connectTCP(host, port);
    Header *header = NULL;
    Sus *paquete_sus = NULL;
    Resp *paquete_resp = NULL;
    Post *paquete_post = NULL;
    int enviados,recibidos,id;
    time_t rawtime;
    struct tm *timestamp;
    char *fp_linea = malloc(128);   //alberga lineas leidas del archivo
    char *id_str = malloc(4);


    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf( "Fallo apertura de archivo de entrada de datos. \n");
        exit(1);
    }


    paquete_sus = Mensaje_crear_sus(0, "text/plain;medicion temperatura\0");
    enviados = Mensaje_enviar_sus(sdf, paquete_sus);
    printf("Enviados %d Bytes - Esperando RESP! \n", enviados);
    
    header = Mensaje_recibir_header(sdf);
    printf("Recibi un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);
    
    paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
    printf("Recibi un RESP de tipo %d y codigo %d\n", paquete_resp->tipo, paquete_resp->codigo);

    if(paquete_resp->tipo == 0 && paquete_resp->codigo == 11){
       
        id = atoi(paquete_resp->data);
        printf("Mi ID ES %d\n", id );
        printf("Comienzo de Envio de Datos hacia el Servidor\n");
        
        unsigned int tipo = 0;
        unsigned int codigo = 13;

        while (!feof(fp) && fscanf(fp, "%s\n", fp_linea) == 1 && tipo == 0 && codigo == 13) {
            
            uint32_t tm = time(NULL);
            paquete_post = Mensaje_crear_post(id, tm, fp_linea);
            printf("Campo DATA enviado: %s, timestamp: %d\n", paquete_post->data, paquete_post->timestamp);
            Mensaje_enviar_post(sdf, paquete_post);

            header = Mensaje_recibir_header(sdf);
            printf("Recibi un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);
            paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
            printf("Recibi un RESP de tipo %d y codigo %d\n", paquete_resp->tipo, paquete_resp->codigo);
            tipo = paquete_resp->tipo;
            codigo = paquete_resp->codigo;
        }
    } else {
            printf("Recibi un Codigo de Error. Saliendo...\n");
            exit(1);
    }
    
    sprintf(id_str, "%d", id);
    paquete_sus = Mensaje_crear_sus(2, id_str);
    Mensaje_enviar_sus(sdf, paquete_sus);

    header = Mensaje_recibir_header(sdf);
    paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);

    if (paquete_resp->tipo == 0) {
        printf("Enviando ACK..");
        paquete_resp = Mensaje_crear_resp(0, 11, "");
        Mensaje_enviar_resp(sdf, paquete_resp);
        printf("Fuente Desconectada! Saliendo...\n");
    } else {
        printf("Falló Desuscripcion. Saliendo...\n");
    }

    fclose(fp);
    close(sdf);

    return 0;
}
