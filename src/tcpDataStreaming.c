#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "tcpDataStreaming.h"
#include "structures.h"

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
    int r;
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

Header *Mensaje_recibir_header(int sdf) {
    char *paquete;
    Header *msj = NULL;
    int recibidos;

    paquete = malloc(sizeof(Header));
    recibidos = receiveall(sdf, paquete, 4);
    msj = (Header *) paquete;

    msj->opcode = ntohs(msj->opcode);
    msj->dlen = ntohs(msj->dlen);

    return msj;
}

Sus *Mensaje_crear_sus(int op, char data[]) {
    Sus *msj = NULL;

    msj = (Sus *) malloc(sizeof(Sus));

    msj->opcode = (uint16_t) SUS;
    msj->dlen = (uint16_t) strlen(data);
    msj->op = (uint16_t) op;
    strcpy(msj->data, data);

    return msj;
}

int Mensaje_enviar_sus(int sdf, Sus *msj) {
    int byte_send = msj->dlen + 6;

    msj->opcode = htons(msj->opcode);
    msj->dlen = htons(msj->dlen);

    msj->op = htons(msj->op);


    return send(sdf, msj, byte_send , 0);

}

Sus *Mensaje_recibir_sus(int sdf, int dlen) {
    typedef struct {
        uint16_t op;
        char data[dlen];
    } Payload;

    Payload *payload;
    char *paquete;
    Sus *msj;
    int recibidos;

    paquete = malloc(sizeof(Sus));
    msj = malloc(sizeof(Sus));
    payload = (Payload *) malloc(130);

    recibidos = receiveall(sdf, paquete, 2 + dlen);
    payload = (Payload *) paquete;

    msj->op = ntohs(payload->op);
    strcpy(msj->data, payload->data);

    return msj;
}

Resp *Mensaje_crear_resp(int tipo, int codigo, char data[]){
    Resp *msj;

    msj = (Resp *) malloc(sizeof(Resp));

    msj->opcode = (uint16_t) RESP;
    msj->dlen = (uint16_t) strlen(data);
    msj->tipo = (uint16_t) tipo;
    msj->codigo = (uint16_t) codigo;
    strcpy(msj->data, data);

    return msj;
}

int Mensaje_enviar_resp(int sdf, Resp *msj){
    int byte_send = msj->dlen + 8;

    msj->opcode = htons(msj->opcode);
    msj->dlen = htons(msj->dlen);
    msj->tipo = htons(msj->tipo);
    msj->codigo = htons(msj->codigo);

    return send(sdf, msj, byte_send, 0);
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
    int recibidos;

    paquete = malloc(sizeof(Resp));
    msj = malloc(sizeof(Resp));
    payload = (Payload *) malloc(132);

    recibidos = receiveall(sdf, paquete, 4 + dlen);
    payload = (Payload *) paquete;

    msj->tipo = ntohs(payload->tipo);
    msj->codigo = ntohs(payload->codigo);
    strcpy(msj->data, payload->data);

    return msj;
}

Post *Mensaje_crear_post(int idFuente, uint32_t timestamp, char data []){
    Post *msj;
    msj = (Post *) malloc(sizeof(Post));
    
    msj->opcode = (uint16_t) POST;
    msj->dlen = (uint16_t) strlen(data);
    msj->idFuente = (uint16_t) idFuente;
    msj->timestamp = timestamp;
    strcpy(msj->data, data);

    return msj;
}

int Mensaje_enviar_post(int sdf, Post *msj){
    int byte_send = msj->dlen + 12; //+size of 3*uint16_t+size of time_t

    msj->opcode = htons(msj->opcode);
    msj->dlen = htons(msj->dlen);
    msj->idFuente = htons(msj->idFuente);
    msj->timestamp = htonl(msj->timestamp);
    

    return send(sdf, msj, byte_send , 0);
}

Post *Mensaje_recibir_post(int sdf, int dlen){
    typedef struct {
        uint16_t idFuente;
        uint32_t timestamp;
        char data[dlen];
    } Payload;

    Payload *payload;
    char *paquete;
    Post *msj;
    int recibidos;

    paquete = malloc(sizeof(Post));
    msj = (Post *) malloc(sizeof(Post));
    payload = (Payload *) malloc(8 + dlen);

    recibidos = receiveall(sdf, paquete, 8 + dlen);
    payload = (Payload *) paquete;

    msj->idFuente = ntohs(payload->idFuente);
    msj->timestamp = ntohl(payload->timestamp);
    strcpy(msj->data, payload->data);

    return msj;

}
//TODO: Mensaje_crear_get ; Mensaje_enviar_get; Mensaje_recibir_get

size_t getNroTokens(const char *str, const char *delim) {
    unsigned int nroTokens, i;

    for (i = 0; i < strlen(str); i++) {
        if (str[i] == *delim) {
            nroTokens += 1;
        }
    }
    return nroTokens;
}

char **wrapstrsep(const char* str, const char* delim) {
    char *s = strdup(str);
    size_t tokens_alloc = getNroTokens(str, delim);
    size_t tokens_used = 0;

    if (tokens_alloc == 0) {
        return NULL;
    }

    char **tokens = calloc(tokens_alloc, sizeof(char *));
    char *token, *rest = s;

    while ((token = strsep(&rest, delim)) != NULL) {
        tokens[tokens_used++] = strdup(token);
    }

    return tokens;
}

