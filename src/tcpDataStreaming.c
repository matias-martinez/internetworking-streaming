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

    //return n==-1?-1:0; // return -1 en caso de falla (si pasar len como puntero y no como int)
    return total;
} 


int pack(int op, char *buf, msj_t *package){
	package->dlen = (uint16_t) strlen(buf);
	if (package->dlen < 128) {
		strcpy( package->data,buf);
	}else return 0;

	switch (op){
		case GET:
			package->opcode = GET;
			break;
		case POST:
			package->opcode = POST;
			break;
		case SUS:
			package->opcode = SUS;
			break;
		case RESP:
			package->opcode = RESP;
			break;
	}
	return 1;
}

Get Paquete_crear_get(int dlen, int idFuente, int op, int idDestino, char data[]) {
    Get paquete_get;
    paquete_get = (Get) malloc(sizeof(Get));
    paquete_get->idFuente = (uint16_t) idFuente;
    paquete_get->op = (uint8_t) op;
    paquete_get->idDestino = (uint16_t) idDestino;
    strcpy(paquete_get->data, data);

    return paquete_get;
}

Sus Paquete_crear_sus(int op, char data[]) {
    Sus paquete_sus;
    paquete_sus = (Sus) malloc(sizeof(Sus));
    paquete_sus->opcode = SUS;
    paquete_sus->dlen = strlen(data);
    paquete_sus->op = (uint8_t) op;
    strcpy(paquete_sus->data, data);


    return paquete_sus;
}

int Paquete_sus_len(Sus paquete) {
    return strlen(paquete->data) + 3;
}


Header Paquete_recibir_header(int sdf) {
    char *paquete;
    int recibidos;

    paquete = malloc(sizeof(Header));
    recibidos = receiveall(sdf, paquete, 3);

    return (Header) paquete; 
}

Sus Paquete_recibir_sus(int sdf, int dlen) {
    char *paquete;
    int recibidos;
    
    paquete = malloc(sizeof(Sus));
    recibidos = receiveall(sdf, paquete, dlen);

    return (Sus) paquete;
}


