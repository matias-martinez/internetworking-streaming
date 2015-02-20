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
    paquete_sus = malloc(sizeof(struct Sus));

    int len, enviados;

    paquete_sus = Mensaje_crear_sus(0, "text/plain;medicion temperatura");
    enviados = Mensaje_enviar_sus(sdf, paquete_sus);
    printf("Enviados %d Bytes \n", enviados);

    close(sdf);

    return 0;

}
