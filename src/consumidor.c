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
        printf("Example ./consumidor data.bat www.example.com 8888\n");
        exit(1);
    }
    
    char *end;
    char *host = argv[2];
    int port = strtol(argv[3], &end, 10);

    int sdf = connectTCP(host, port);
    int myId;
    Header *header = NULL;
    Sus *paquete_sus = NULL;
    Resp *paquete_resp = NULL;
    Get *paquete_get = NULL;
    Post *paquete_post = NULL;
    int enviados, recibidos, idFuente;
    time_t rawtime;
    struct tm *timestamp;
    char *bufferKeyboard = malloc(8);  //alberga id de fuente para solicitar suscripcion
    

    FILE *fp;
    fp = fopen(argv[1], "rw+");
    if (fp == NULL) {
        printf( "Fallo apertura de archivo de salida de datos. \n");
        exit(1);
    }

    //GET id fuente =0. if resp:0,12-->
    //ller lista recibida
    //elegir fuente
    //enviar SUS op=1, data=idFuente. si resp 0 , 11-> sigo sino cierro
    //Si streaming-> GET op = 0
    //Si historico-> get op = 1, ts 1 ts 2
    //loop
    //sus op = 2. si resp o, 11 --> termino
    printf("-Bienvenido. CONSUMIDOR DataStreaming-\n");
    printf("-Enviando Solicitud de Lista de Fuentes al Servidor...-\n");

    paquete_get = Mensaje_crear_get(0, 0, 0, ""); //Mensaje para solicitar Lista de Fuentes. OP = 0 (1er parametro)
    enviados = Mensaje_enviar_get(sdf, paquete_get);
    
    header = Mensaje_recibir_header(sdf);
    printf("Recibí un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);

    paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
    printf("Recibí un RESP de tipo %d y código %d\n", paquete_resp->tipo, paquete_resp->codigo);

    if(paquete_resp->tipo == 0 && paquete_resp->codigo == 12){
        if (strlen(paquete_resp->data) > 0) {
            int i;
            char **datos = wrapstrsep(paquete_resp->data, ";");
            printf("LISTA DE FUENTES DISPONIBLES: \n");
            printf("%s\n", datos[0]); 
            for (i = 1; strcmp(datos[i], "") != 0; i++) {
                printf("FUENTE: %s\n", datos[i]);
            }
        }
    } else {
        printf("Recibí una lista de fuentes vacía! Nada que consumir :(\n");
        exit(1);
    }

    printf("Ingrese ID de la fuente deseada:\t");
    fgets(bufferKeyboard, 8, stdin);

    //TODO chequear que sea un id existente!
    idFuente = atoi(bufferKeyboard);
    sprintf(bufferKeyboard, "%d", idFuente);
    paquete_sus = Mensaje_crear_sus(1, bufferKeyboard);
    free(bufferKeyboard);

    enviados = Mensaje_enviar_sus(sdf, paquete_sus);
    printf("Enviados %d Bytes .. Esperando RESP!\n",enviados);

    header = Mensaje_recibir_header(sdf);
    printf("Recibí un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);

    paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
    printf("Recibí un RESP de tipo %d y código %d\n", paquete_resp->tipo, paquete_resp->codigo);

    if (paquete_resp->tipo == 0 && paquete_resp->codigo == 11) {
        printf("Suscripto a la fuente seleccionada!\n");
        printf("Mi ID es %s\n", paquete_resp->data);
        myId = atoi(paquete_resp->data);
    } else {              //TODO: dar la opcion de salir o de volver a ingresar
        printf("Error en la suscripcion. Saliendo..\n");
        exit(1);
    }

    //TODO: realizar un get historico
    paquete_get = Mensaje_crear_get(idFuente, 0, myId, "");
    Mensaje_enviar_get(sdf, paquete_get);
    header = Mensaje_recibir_header(sdf);
    paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);

    int existen_nuevos_paquetes = 0;
    while (existen_nuevos_paquetes == 0) {
        if ( paquete_resp->codigo == 14) {
            printf("Recibí un RESP de tipo %d y código %d y datos: %s\n", paquete_resp->tipo, paquete_resp->codigo, paquete_resp->data);
            paquete_get = Mensaje_crear_get(idFuente, 0, myId, "");
            Mensaje_enviar_get(sdf, paquete_get);

            header = Mensaje_recibir_header(sdf);
            paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
        } else {   
            //TODO: dar la opcion de salir o de volver a ingresar
            printf("Error en la solicitud GET a la Fuente. Saliendo..");
            exit(1);
        }
    }

  

    fclose(fp);
    close(sdf);
    return 0;
}
