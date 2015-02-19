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

//Mensajes
#define GET 0
#define POST 1
#define SUS 2
#define RESP 3

//Longitud Header y Carga no util de cada tipo de mensaje
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
			perror("Error en recepcion del mensaje/ receiveall()");
			break;
		}
		leido = leido + bytes;
	}
	return leido;
}


int sendall (int sd, char *buf, int len){

    int total = 0;        
    int bytesleft = len; 
    int n;

    while(total < len) {
        n = send(sd, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    total; // en *len guardo los bytes enviados

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

Get Mensaje_crear_get(int dlen, int idFuente, int op, int idDestino, char data[]) {
    Get mensaje_get = malloc(size(Get));
    mensaje_get->dlen = (uint16_t) dlen;
    mensaje_get->idFuente = (uint16_t) idFuente;
    mensaje_get->op = (uint8_t) op;
    mensaje_get->idDestino = (uint16_t) idDestino;
    mensaje_get->data = data;

    return mensaje_get;
}

Get Mensaje_recibir_get(int sdf, int dlen) {
    Get get;
    char *paquete;
    int recibidos

    paquete = malloc(sizeof(Get));
    recibidos = reciveall(sdf);
    get = (Get *) paquete;

    return get;
}

void Mensaje_recibir(int sdf, int *opcode, int *dlen) {
    char *paquete;
    int recibidos;
    uint16_t dlen;
    uint8_t opcode;
    msj_t *header;

    paquete = malloc(3);
    recibidos = reciveall(sdf, paquete, 3);
    header = (msj_t *) paquete;
    dlen = header->dlen;
    opcode = header->opcode;
}


