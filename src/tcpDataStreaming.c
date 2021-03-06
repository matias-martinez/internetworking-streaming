#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "structures.h"
#include "tcpDataStreaming.h"
#include "flags.h"

int passiveTCPSocket(char *host, int port, int qlen){

    int sd;
    struct sockaddr_in servidor;

    memset(&servidor, 0, sizeof(servidor));
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(port);
    servidor.sin_addr.s_addr = inet_addr(host);

    sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (bind(sd, (struct sockaddr *) &servidor, sizeof(servidor)) < 0) {
        perror("Error en bind");
        exit(-1);
    }
    if (listen(sd, qlen) < 0){
        perror("Fallo listen en el puerto \n");
        exit(-1);
    }

    return sd;
}

int connectTCP(char *host, int port){

    int sdc;
    struct sockaddr_in servidor;
    struct hostent *hp, *gethostbyname();

    sdc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&servidor, 0, sizeof(servidor));
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(port);

    hp = gethostbyname(host);
    memcpy(&servidor.sin_addr, hp->h_addr, hp->h_length);
    
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
            return -1;
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

Header *Mensaje_recibir_header(int sdf) {
    char *paquete;
    Header *msj = NULL;

    paquete = malloc(LONG_HEADER);
    receiveall(sdf, paquete, LONG_HEADER);
    msj = (Header *) paquete;

    msj->opcode = ntohs(msj->opcode);
    msj->dlen = ntohs(msj->dlen);

    return msj;
}

Sus *Mensaje_crear_sus(int op, char data[]) {
    Sus *msj = NULL;
    size_t dlen = !data ? 0 : strlen(data);

    msj = (Sus *) malloc(LONG_HEADER + LONG_SUS + dlen);

    msj->opcode = (uint16_t) SUS;
    msj->dlen = (uint16_t) dlen;
    msj->op = (uint16_t) op;
    strncpy(msj->data, data, dlen);

    return msj;
}

int Mensaje_enviar_sus(int sdf, Sus *msj) {
    int byte_send = msj->dlen + LONG_SUS + LONG_HEADER;

    msj->opcode = htons(msj->opcode);
    msj->dlen = htons(msj->dlen);
    msj->op = htons(msj->op);

    return sendall(sdf, (char *) msj, byte_send);
}

Sus *Mensaje_recibir_sus(int sdf, int dlen) {
    typedef struct {
        uint16_t op;
        char data[dlen];
    } Payload;

    Payload *payload;
    char *paquete;
    Sus *msj;
    
    paquete = malloc(LONG_SUS + dlen);
    msj = malloc(LONG_HEADER + LONG_SUS + dlen);
    payload = (Payload *) malloc(LONG_SUS + dlen);

    receiveall(sdf, paquete, LONG_SUS + dlen);
    payload = (Payload *) paquete;

    msj->op = ntohs(payload->op);
    strncpy(msj->data, payload->data, dlen);

    free(payload);
    return msj;
}

Resp *Mensaje_crear_resp(int tipo, int codigo, char data[]){
    Resp *msj;
    size_t dlen = !data ? 0 : strlen(data);
    msj = (Resp *) malloc(LONG_HEADER + LONG_RESP + dlen);

    msj->opcode = (uint16_t) RESP;
    msj->dlen = (uint16_t) dlen;
    msj->tipo = (uint16_t) tipo;
    msj->codigo = (uint16_t) codigo;
    strncpy(msj->data, data, dlen);

    return msj;
}

int Mensaje_enviar_resp(int sdf, Resp *msj){
    int byte_send = LONG_HEADER + LONG_RESP + msj->dlen;

    msj->opcode = htons(msj->opcode);
    msj->dlen = htons(msj->dlen);
    msj->tipo = htons(msj->tipo);
    msj->codigo = htons(msj->codigo);

    return sendall(sdf, (char *) msj, byte_send);
}

Resp *Mensaje_recibir_resp(int sdf, int dlen){
    typedef struct {
        uint16_t tipo;
        uint16_t codigo;
        char data[dlen];
    } Payload;

    Payload *payload;
    char *paquete;
    Resp *msj;

    paquete = malloc(LONG_RESP + dlen);
    msj = malloc(LONG_HEADER + LONG_RESP + dlen);
    payload = (Payload *) malloc(LONG_RESP + dlen);

    receiveall(sdf, paquete, LONG_RESP + dlen);
    payload = (Payload *) paquete;

    msj->tipo = ntohs(payload->tipo);
    msj->codigo = ntohs(payload->codigo);
    strncpy(msj->data, payload->data, dlen);

    free(payload);
    return msj;
}

Post *Mensaje_crear_post(int idFuente, uint32_t timestamp, char data []){
    Post *msj;
    size_t dlen = !data ? 0 : strlen(data);
    msj = (Post *) malloc(LONG_HEADER + LONG_POST + dlen);
    
    msj->opcode = (uint16_t) POST;
    msj->dlen = (uint16_t) dlen;
    msj->idFuente = (uint16_t) idFuente;
    msj->timestamp = timestamp;
    strncpy(msj->data, data, dlen);

    return msj;
}

int Mensaje_enviar_post(int sdf, Post *msj){
    int byte_send = LONG_HEADER + LONG_POST + msj->dlen;

    msj->opcode = htons(msj->opcode);
    msj->dlen = htons(msj->dlen);
    msj->idFuente = htons(msj->idFuente);
    msj->timestamp = htonl(msj->timestamp);
    
    return sendall(sdf, (char *) msj, byte_send);
}

Post *Mensaje_recibir_post(int sdf, size_t dlen){
    #pragma pack(1)
    typedef struct {
        uint16_t idFuente;
        uint32_t timestamp;
        char data[dlen + 1];
    } Payload;
    #pragma pack()

    Payload *payload;
    char *paquete;
    Post *msj;

    paquete = malloc(LONG_POST + dlen);
    msj = (Post *) malloc(LONG_HEADER + LONG_POST + dlen);
    payload = (Payload *) malloc(LONG_POST + dlen);

    receiveall(sdf, paquete, LONG_POST + dlen);
    payload = (Payload *) paquete;

    msj->idFuente = ntohs(payload->idFuente);
    msj->timestamp = ntohl(payload->timestamp);
    strncpy(msj->data, payload->data, dlen);
    msj->data[dlen] = '\0';
    
    free(payload);
    return msj;

}


Get *Mensaje_crear_get(int idFuente, int op, int idDestino, char data[]){
    Get *msj;
    size_t dlen = !data ? 0 : strlen(data);
    msj = (Get *) malloc(LONG_HEADER + LONG_GET + dlen);
    
    msj->opcode = (uint16_t) GET;
    msj->dlen = (uint16_t) dlen;
    msj->idFuente = (uint16_t) idFuente;
    msj->op = (uint16_t)op;
    msj->idDestino = (uint16_t) idDestino;
    strncpy(msj->data, data, dlen);

    return msj;
}

int Mensaje_enviar_get(int sdf, Get *msj ){
    int byte_send = LONG_HEADER + LONG_GET + msj->dlen;

    msj->opcode = htons(msj->opcode);
    msj->dlen = htons(msj->dlen);
    msj->idFuente = htons(msj->idFuente);
    msj->op = htons(msj->op);
    msj->idDestino = htons(msj->idDestino);
    
    return sendall(sdf, (char *) msj, byte_send);
}

Get *Mensaje_recibir_get(int sdf, size_t dlen){
    typedef struct {
        uint16_t idFuente;
        uint16_t op;
        uint16_t idDestino;
        char data[dlen];
    } Payload;

    Payload *payload;
    char *paquete;
    Get *msj;

    paquete = malloc(LONG_GET + dlen);
    msj = (Get *) malloc(LONG_HEADER + LONG_GET + dlen);
    payload = (Payload *) malloc(LONG_GET + dlen);

    receiveall(sdf, paquete, LONG_POST + dlen);
    payload = (Payload *) paquete;

    msj->idFuente = ntohs(payload->idFuente);
    msj->op = ntohs(payload->op);
    msj->idDestino = ntohs(payload->idDestino);
    strncpy(msj->data, payload->data, dlen);

    free(payload);
    return msj;
}


