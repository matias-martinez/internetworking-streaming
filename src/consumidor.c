#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include "tcpDataStreaming.h"
#include "structures.h"
#include "flags.h"
#include "utils.h"

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
    char *bufferKeyboard = malloc(12);  
	char * continuar = malloc(8);

    FILE *fp;
    fp = fopen(argv[1], "a+");
    if (!fp) {
        perror( "Fallo apertura de archivo de salida de datos. \n");
        exit(-1);
    }

    printf("-Bienvenido. CONSUMIDOR DataStreaming-\n");
    
	while(memcmp(continuar, "n", 1) != 0) {
        printf("-Enviando Solicitud de Lista de Fuentes al Servidor...-\n");

        paquete_get = Mensaje_crear_get(GET_OP_NORMAL, 0, 0, ""); 
        enviados = Mensaje_enviar_get(sdf, paquete_get);
        
        header = Mensaje_recibir_header(sdf);
        printf("Recibí un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);

        paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
        printf("Recibí un RESP de tipo %d y código %d\n", paquete_resp->tipo, paquete_resp->codigo);

        if(paquete_resp->tipo == RESP_TIPO_OK && paquete_resp->codigo == RESP_CODIGO_102){
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

        enviados = Mensaje_enviar_sus(sdf, paquete_sus);
        printf("Enviados %d Bytes .. Esperando RESP!\n",enviados);

        header = Mensaje_recibir_header(sdf);
        printf("Recibí un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);

        paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
        printf("Recibí un RESP de tipo %d y código %d\n", paquete_resp->tipo, paquete_resp->codigo);

        if (paquete_resp->tipo == RESP_TIPO_OK && paquete_resp->codigo == RESP_CODIGO_101) {
            printf("Suscripto a la fuente seleccionada!\n");
            printf("Mi ID es %s\n", paquete_resp->data);
            myId = atoi(paquete_resp->data);
        } else {    
            printf("Error en la suscripcion.\n");
            printf("Desea Continuar la ejecución? [N]o - [S]i :\t");
            fgets(continuar, 8, stdin);
            continuar = strip(continuar);
            continue;
        }


        printf("\nDesea obtener datos: \n0-Todos\n1-Historicos\nIngrese su opcion: \n");
        fgets(bufferKeyboard, 8, stdin);
        int op = atoi(bufferKeyboard);
        char *data = "";

        if (op == GET_OP_TM) {
            // TODO: asegurarse que ingrese por lo menos 0
            data = malloc(25);
            printf("Ingrese 2 timestamp (0 para ingorar ese timestamp)\n");
            printf("Inicio: \n");
            fgets(bufferKeyboard, 12, stdin);
            strncat(data, strip(bufferKeyboard), strlen(bufferKeyboard));
            printf("Fin: \n");
            strncat(data, ";", 1);
            fgets(bufferKeyboard, 12, stdin);
            strncat(data, strip(bufferKeyboard), strlen(bufferKeyboard));
        }

        paquete_get = Mensaje_crear_get(idFuente, op, myId, data);
        Mensaje_enviar_get(sdf, paquete_get);
        header = Mensaje_recibir_header(sdf);
        paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);

        int existen_nuevos_paquetes = SUCCESS;
        while (existen_nuevos_paquetes == SUCCESS) {
            if (paquete_resp->codigo == RESP_CODIGO_104) {
                printf("Recibí un RESP de tipo %d y código %d y datos: %s\n", paquete_resp->tipo, paquete_resp->codigo, paquete_resp->data);

                header = Mensaje_recibir_header(sdf);
                paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
            } else {
                existen_nuevos_paquetes = FAIL;
                printf("El servidor envio todos los datos. ");
                printf("Desea Continuar la ejecución? No - Si :\t");
                fgets(continuar, 8, stdin);
                continuar = str_tolower(strip(continuar));
            }
        }

  	}

    printf("Cerrando conexion\n");
    paquete_resp = Mensaje_crear_resp(RESP_TIPO_OK, RESP_CODIGO_101, "");
    Mensaje_enviar_resp(sdf, paquete_resp);
    printf("Consumidor desconectado Desconectada! Saliendo...\n");

    fclose(fp);
    close(sdf);
    return 0;
}
