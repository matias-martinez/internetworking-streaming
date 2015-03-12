#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <unistd.h> // sleep
#include <time.h>
#include "tcpDataStreaming.h"
#include "structures.h"
#include "flags.h"
#include "utils.h"
#include "out.h"

int signal_exit = 0;

void signal_handler(int sig) {
    signal_exit = 1;
}

void desuscribirse(int sdf, FILE *fp, char* id_str) {

    printf("Cerrando la fuente. Enviando Mensaje de desuscripcion\n");

    Sus *paquete_sus = Mensaje_crear_sus(SUS_OP_DEL, id_str);
    Mensaje_enviar_sus(sdf, paquete_sus);

    Header *header = Mensaje_recibir_header(sdf);
    Resp *paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
    print_mensaje(header, paquete_resp);

    if (paquete_resp->tipo == RESP_TIPO_OK) {
        printf("Enviando ACK..");
        paquete_resp = Mensaje_crear_resp(RESP_TIPO_OK, RESP_CODIGO_101, "");
        Mensaje_enviar_resp(sdf, paquete_resp);
        printf("Fuente Desconectada! Saliendo...\n");
    } else {
        printf("Falló Desuscripcion. Saliendo...\n");
    }

    close(sdf);
    fclose(fp);
    exit(2);
}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("usage: %s [FILE] [SERVERNAME] [PORT]\n", argv[0]);
        printf("Example ./fuente data.bat www.exaple.com 8888\n");
        exit(1);
    }
    
    signal(SIGINT, &signal_handler);
    char *end;
    char *host = argv[2];
    int port = strtol(argv[3], &end, 10);

    int sdf = connectTCP(host, port);
    Header *header = NULL;
    Sus *paquete_sus = NULL;
    Resp *paquete_resp = NULL;
    Post *paquete_post = NULL;
    int enviados, id;
    char *fp_linea = malloc(128);   //alberga lineas leidas del archivo
    char *id_str = malloc(4);

    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf( "Fallo apertura de archivo de entrada de datos. \n");
        exit(1);
    }

    printf("\e[1;1H\e[2J");
    printf("|||| Fuente DataStreaming - v0.2 ||||\n");
    printf("---------------------------------------\n");

    paquete_sus = Mensaje_crear_sus(SUS_OP_FUENTE, "text/plain;medicion temperatura\0");
    enviados = Mensaje_enviar_sus(sdf, paquete_sus);
    printf("Enviados %d Bytes", enviados);
    
    header = Mensaje_recibir_header(sdf);
    paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
    print_mensaje(header, paquete_resp);
    if (paquete_resp->tipo == RESP_TIPO_OK && paquete_resp->codigo == RESP_CODIGO_101) {
       
        id = atoi(paquete_resp->data);
        sprintf(id_str, "%d", id);
        printf("Mi ID ES %d\n", id );
        printf("Comienzo de Envio de Datos hacia el Servidor\n");
        
        unsigned int tipo = RESP_TIPO_OK;
        unsigned int codigo = RESP_CODIGO_103;

        while (!feof(fp) && fscanf(fp, "%s\n", fp_linea) == 1 && tipo == RESP_TIPO_OK && codigo == RESP_CODIGO_103) {
            sleep(1);    // simula tiempo de proceso de sensores.
            uint32_t tm = time(NULL);
            paquete_post = Mensaje_crear_post(id, tm, fp_linea);
            printf("Campo DATA enviado: %s, timestamp: %d\n", paquete_post->data, paquete_post->timestamp);
            Mensaje_enviar_post(sdf, paquete_post);

            header = Mensaje_recibir_header(sdf);
            paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
            print_mensaje(header, paquete_resp);
            tipo = paquete_resp->tipo;
            codigo = paquete_resp->codigo;

            if (signal_exit == 1) {
                desuscribirse(sdf, fp, id_str);
            }
        }
    } else {
            printf("Recibi un Codigo de Error. Saliendo...\n");
            exit(1);
    }
    
    
    desuscribirse(sdf, fp, id_str);
    return 0;
}
