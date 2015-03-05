#include <stdio.h>
#include "structures.h"
#include "flags.h"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BRED     "\033[1m\033[31m"      /* Bold Red */
#define BGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BWHITE   "\033[1m\033[37m"      /* Bold White */

static void print_get(Get *mensaje);

static void print_post(Post *mensaje);

static void print_sus(Sus *mensaje);

static void print_resp(Resp *mensaje);

void print_mensaje(Header *header, void * mensaje) {
    printf("\n================================================\n");
    printf("Recibi un mensaje del tipo ");
    switch (header->opcode) {
        case GET:
            printf("%sGet%s con un dlen %d", BGREEN, RESET, header->dlen);
            printf("%s\n--------------------------------------------\n", BGREEN);
            print_get((Get *) mensaje);
            printf("%s\n", RESET);
            break;
        case POST:
            printf("%sPOST%s con un dlen %d", BBLUE, RESET, header->dlen);
            printf("%s\n--------------------------------------------\n", BBLUE);
            print_post((Post *) mensaje);
            printf("%s\n", RESET);
            break;
        case SUS:
            printf("%sSUS%S con un dlen %d", BYELLOW, RESET, header->dlen);
            printf("%s\n--------------------------------------------\n", BYELLOW);
            print_sus((Sus *) mensaje);
            printf("%s\n", RESET);
            break;
        case RESP:
            printf("%sRESP%s con un dlen %d", BMAGENTA, RESET, header->dlen);
            printf("%s\n--------------------------------------------\n", BMAGENTA);
            print_resp((Resp *) mensaje);
            printf("%s\n", RESET);
            break;
    }
}

static void print_sus(Sus *mensaje) {
    if (mensaje->op < 2) {
        printf("Un cliente quiere registrarse como ");
    }
    switch (mensaje->op) {
        case SUS_OP_FUENTE:
            printf("FUENTE\n");
            printf("con estos datos %s\n", mensaje->data);
            break;
        case SUS_OP_CONS:
            printf("CONSUMIDOR a la fuente ");
            printf("%s\n", mensaje->data);
            break;
        case SUS_OP_DEL:
            printf("El cliente %s quiere desuscribirse\n", mensaje->data);
    }
}

static void print_get(Get *mensaje) {
    if (mensaje->idFuente == 0) {
        printf("El consumidor solicita las fuentes disponibles.\n");
    } else {
        printf("El consumidor %d solicita ", mensaje->idDestino);
        if (mensaje->op == 0) {
            printf("todos los datos ");
        } else {
            printf("datos historicos ");
        }
        printf("de la fuente %d\n", mensaje->idFuente);
    }
}

static void print_post(Post *mensaje) {
    printf("La fuente con id %d ", mensaje->idFuente);
    printf("Envia los siguientes datos %s\n", mensaje->data);
    printf("Con este timestamp %d\n", mensaje->timestamp);
}

static void print_resp(Resp *mensaje) {
    switch (mensaje->tipo) {
        case (RESP_TIPO_OK):
            printf("Todo OK. ");
            switch (mensaje->codigo) {
                case (RESP_CODIGO_101):
                    printf("101: Solicitud aceptada, procesando el mensaje.\n");
                    break;
                case (RESP_CODIGO_102):
                    printf("102: Lista de fuentes disponibles.\n");
                    break;
                case (RESP_CODIGO_103):
                    printf("103: Post aceptado.\n");
                    break;
                case (RESP_CODIGO_104):
                    printf("104: Get aceptado.\n");
                    break;
            }
            break;    
        case (RESP_TIPO_FAIL):
            printf("Algo salio mal. ");
            switch (mensaje->codigo) {
                case (RESP_CODIGO_201):
                    printf("201: Solicitud erronea.\n");
                    break;
                case (RESP_CODIGO_202):
                    printf("202: ID de fuente intexistente.\n");
                    break;
                case (RESP_CODIGO_203):
                    printf("203: Timestamp invalido.\n");
                    break;
                case (RESP_CODIGO_204):
                    printf("204: Espacio insuficiente.\n");
                    break;
                case (RESP_CODIGO_205):
                    printf("205: Servicio no disponible temporalmente.\n");
                    break;
            }
        break;
    }
}


