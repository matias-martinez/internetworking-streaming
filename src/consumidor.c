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
    Header *header = NULL;
    Sus *paquete_sus = NULL;
    Resp *paquete_resp = NULL;
    Get *paquete_get = NULL;
    Post *paquete_post = NULL;
    int len, enviados,recibidos,idFuente,i;
    time_t rawtime;
    struct tm *timestamp;
    char* temp = malloc(128); 
    char *temp2=malloc(128);
    char *bufferKeyboard1 = malloc(8);  //alberga id de fuente para solicitar suscripcion
    

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

    paquete_get = Mensaje_crear_get(0,0,0,""); //Mensaje para solicitar Lista de Fuentes. OP = 0 (1er parametro)
    enviados = Mensaje_enviar_get(sdf, paquete_get);
    if (enviados=10){//enviados = 10 es la longitud del header mas la carga del msj op = 0
        printf("Solicitud Enviada. Esperando Respuesta...\n");
    }else{
        printf("Error. Saliendo..\n");
        exit(1);
    }
    header = Mensaje_recibir_header(sdf);
    printf("Recibí un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);

    paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
    printf("Recibí un RESP de tipo %d y código %d\n", paquete_resp->tipo, paquete_resp->codigo);

    if(paquete_resp->tipo==0 && paquete_resp->codigo == 12){
        if (strlen(paquete_resp->data) > 0)
        {
            i=1;
            char **datos = wrapstrsep(paquete_resp->data, ";");
           // int index = strlen(datos)-1;//FIX:
            int index = getNroTokens(paquete_resp->data,";");
            printf("LISTA DE FUENTES DISPONIBLES: \n");
            for (printf("%s\n",datos[0]);i <index; i++){
                printf("F: %s\n",datos[i]);
            }
        }else
        {
            printf("Recibí una lista de fuentes vacía! Nada que consumir :(\n");
            exit(1);
        }
    }
    printf("Ingrese ID de la fuente deseada:\t");
    fgets(bufferKeyboard1, 8, stdin);
    //TODO chequear que sea un id existente!
    idFuente = atoi(bufferKeyboard1);
    paquete_sus = Mensaje_crear_sus(1,bufferKeyboard1);
    free(bufferKeyboard1);
    enviados = Mensaje_enviar_sus(sdf, paquete_sus);
    printf("Enviados %d Bytes .. Esperando RESP!\n",enviados);

    header = Mensaje_recibir_header(sdf);
    printf("Recibí un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);

    paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
    printf("Recibí un RESP de tipo %d y código %d\n", paquete_resp->tipo, paquete_resp->codigo);

    if (paquete_resp->tipo == 0 && paquete_resp->codigo ==11)
    {
        printf("Suscripto a la fuente seleccionada!\n");
    }else//TODO: dar la opcion de salir o de volver a ingresar
    {
        printf("Error en la suscripcion. Saliendo..\n");
        exit(1);
    }

    paquete_get = Mensaje_crear_get(idFuente,0,0,NULL); // VIOLACION DE SEGMENTO!
    // TODO: Resolver tema del Id Destino!Que server asigne id a consumer o que sea la ip+puerto.
    //TODO: realizar un get historico
   
    paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
    printf("Recibí un RESP de tipo %d y código %d\n", paquete_resp->tipo, paquete_resp->codigo);

    if (paquete_resp->tipo == 0 && paquete_resp->codigo ==11){
       //RECIBIR!
           printf("Exito en la solicitud GET\n");
       } else//TODO: dar la opcion de salir o de volver a ingresar
        {
        printf("Error en la solicitud GET a la Fuente. Saliendo..");
        exit(1);
    }

  

    fclose(fp);
    close(sdf);
    return 0;
}