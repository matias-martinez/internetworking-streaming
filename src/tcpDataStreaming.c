#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <unistd.h>
#include <errno.h>
#include "tcpDataStreaming.h"
#include "structures.h"

//Paquetes
#define GET 0
#define POST 1
#define SUS 2
#define RESP 3

//Longitud Header y Carga no util de cada tipo de paquete
#define LONG_HEADER 3
#define LONG_GET 5
#define LONG_POST 6
#define LONG_SUS 1
#define LONG_RESP 3

//Encodings Soportados
// TODO: arreglar esto, hacer un enum con mime
#define BINARY 0
#define TEXT 1


int passiveTCPSocket(int port){

	int sd;
	struct sockaddr_in servidor;
	int qlen=10;//parametrizar
	
	memset(&servidor, 0, sizeof(servidor));
	servidor.sin_family = AF_INET;
	servidor.sin_port = htons(port);
	servidor.sin_addr.s_addr = INADDR_ANY;

	sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (bind(sd, (struct sockaddr *) &servidor, sizeof(servidor)) < 0) {
		perror("Error en bind");
		exit(-1);
	}
	if (listen(sd, qlen) < 0){
		perror("Fallo listen en el puerto");//agregar numero de puerto
		exit(-1);
	}
	return sd;
}

int connectTCP(){
	
	int sdc;
	struct sockaddr_in servidor;
	
	sdc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&servidor, 0, sizeof(servidor));
	servidor.sin_family = AF_INET;
	servidor.sin_port = htons(4567);//parametrizar
	servidor.sin_addr.s_addr = inet_addr("127.0.0.1");//usar gethostbyname o getaddrinfo
	if ( connect ( sdc, (struct sockaddr *) &servidor, sizeof(servidor)) < 0){
		perror("Error en connect al socket servidor");
		exit(-1);
	}

	return sdc;
}
	

int receiveall (int sd, char * buffer, int total) {
	int bytes = 1;
	int leido = 0;
	
	while ( (leido < total) && (bytes > 0) ) {
		bytes = recv (sd, buffer + leido, total - leido, MSG_WAITALL);
		if (bytes < 0){
			perror("Error en recepcion del paquete/ receiveall()");
			break;
		}
		leido = leido + bytes;
	}
	return leido;
}


int sendall(int sd, char *buf, int len){

    int total = 0;        
    int bytesleft = len; 
    int n;

    while(total < len) {
        n = send(sd, buf + total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    return total;
} 

Sus Paquete_crear_sus(int op, char data[], Header header) {
    Sus paquete_sus;

    paquete_sus = (Sus) malloc(sizeof(struct Sus));

    header->opcode = (uint16_t) SUS;
    header->dlen = (uint16_t) (strlen(data) + 2); // POR QUE + 2?
    paquete_sus->op = (uint16_t) op;
    strcpy(paquete_sus->data, data);
    
    return paquete_sus;
}

int Paquete_enviar_sus(int sdf, Header header, Sus paquete) {
    int byte_send = 0;

    header->opcode = htons(header->opcode);
    header->dlen = htons(header->dlen);

    paquete->op = htons(paquete->op);
    
    byte_send += send(sdf, header, sizeof(struct Header), 0);
    byte_send += send(sdf, paquete, 2 + strlen(paquete->data) * sizeof(char), 0);
    return byte_send;
}


Header Paquete_recibir_header(int sdf) {
    char *paquete;
    Header mensaje;
    int recibidos;

    paquete = malloc(sizeof(Header));
    recibidos = receiveall(sdf, paquete, sizeof(struct Header));
    mensaje = (Header) paquete;
    
    mensaje->opcode = ntohs(mensaje->opcode);
    mensaje->dlen = ntohs(mensaje->dlen);

    return mensaje; 
}

Sus Paquete_recibir_sus(int sdf, int dlen) {
    
    void *paquete;
    Sus mensaje;
    int recibidos;
    
    paquete = malloc(sizeof(struct Sus));
    recibidos = receiveall(sdf, paquete, dlen);
    mensaje = (Sus) paquete;

    mensaje->op = ntohs(mensaje->op);
    return mensaje;
}


