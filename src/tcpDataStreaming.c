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
#include <time.h>
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

Header Mensaje_recibir_header(int sdf) {
    char *paquete;
    Header msj;
    int recibidos;

    paquete = malloc(sizeof(struct Header));
    recibidos = receiveall(sdf, paquete, 4);
    msj = (Header) paquete;

    msj->opcode = ntohs(msj->opcode);
    msj->dlen = ntohs(msj->dlen);

    return msj;
}

Sus Mensaje_crear_sus(int op, char data[]) {
    Sus msj;

    msj = (Sus) malloc(sizeof(struct Sus));

    msj->opcode = (uint16_t) SUS;
    msj->dlen = (uint16_t) (strlen(data) + 2); // POR QUE + 2?
    msj->op = (uint16_t) op;
    strcpy(msj->data, data);

    return msj;
}

int Mensaje_enviar_sus(int sdf, Sus msj) {
    int byte_send = msj->dlen + 6;

    msj->opcode = htons(msj->opcode);
    msj->dlen = htons(msj->dlen);

    msj->op = htons(msj->op);


    return send(sdf, msj, byte_send , 0);

}

Sus Mensaje_recibir_sus(int sdf, int dlen) {
    typedef struct {
        uint16_t op;
        char data[dlen];
    } *Playload;

    Playload playload;
    char *paquete;
    Sus msj;
    int recibidos;

    paquete = malloc(sizeof(struct Sus));
    msj = malloc(sizeof(struct Sus));
    playload = (Playload) malloc(130);

    recibidos = receiveall(sdf, paquete, 2 + dlen);
    playload = (Playload) paquete;

    msj->op = ntohs(playload->op);
    strcpy(msj->data, playload->data);

    return msj;
}

Resp Mensaje_crear_resp(int tipo, int codigo, char data[]){
    Resp msj;
    
    msj=(Resp)malloc(sizeof(struct Resp));

    msj->opcode=(uint16_t)RESP;
    msj->dlen=(uint16_t)(strlen(data)+2);
    msj->tipo=(uint16_t)tipo;
    msj->codigo=(uint16_t)codigo;
    strcpy(msj->data, data);

    return msj;
}

int Mensaje_enviar_resp(int sdf, Resp msj){
    int byte_send = msj->dlen + 8;

    msj->opcode = htons(msj->opcode);
    msj->dlen = htons(msj->dlen);
    msj->tipo = htons(msj->tipo);
    msj->codigo = htons(msj->codigo);

    return send(sdf, msj, byte_send , 0);
}

Resp Mensaje_recibir_resp(int sdf, int dlen){
     typedef struct {
        uint16_t tipo;
        uint16_t codigo;
        char data[dlen];
    } *Payload;

    Payload payload;
    char *paquete;
    Resp msj;
    int recibidos;

    paquete = malloc(sizeof(struct Resp));
    msj = malloc(sizeof(struct Resp));
    payload = (Payload) malloc(132);

    recibidos = receiveall(sdf, paquete, 2 + dlen);
    payload = (Payload) paquete;

    msj->tipo = ntohs(payload->tipo);
    msj->codigo = ntohs(payload->codigo);
    strcpy(msj->data, payload->data);

    return msj;
}

Post Mensaje_crear_post(int idFuente, struct tm timestamp, char data []){
    Post msj;
    msj=(Resp)malloc(sizeof(struct Post));

    msj->opcode=(uint16_t)POST;
    msj->dlen=(uint16_t)(strlen(data)+2);
    msj->idFuente=(uint16_t)idFuente;
    msj->timestamp=timestamp;
    strcpy(msj->data, data);

    return msj;
}

int Mensaje_enviar_post(int sdf,Post msj){
    int byte_send = msj->dlen +62 ; //+size of 3*uint16_t+size of timestamp

    msj->opcode = htons(msj->opcode);
    msj->dlen = htons(msj->dlen);
    msj->idFuente = htons(msj->idFuente);

    return send(sdf, msj, byte_send , 0);
}

Post Mensaje_recibir_post(int sdf, int dlen){
    typedef struct {
        uint16_t idFuente;
        struct tm timestamp;
        char data[dlen];
    } *Payload;

    Payload payload;
    char *paquete;
    Post msj;
    int recibidos;

    paquete = malloc(sizeof(struct Post));
    msj = malloc(sizeof(struct Post));
    payload = (Payload) malloc(180);

    recibidos = receiveall(sdf, paquete, 2 + dlen);
    payload = (Payload) paquete;

    msj->idFuente = ntohs(payload->idFuente);
    strcpy(msj->data, payload->data);

    return msj;

}
//TODO: Mensaje_crear_get ; Mensaje_enviar_get; Mensaje_recibir_get

int wrapstrsep(char *stringIn, int nroTokens, char * out[] ){
    /*devuelve un array de punteros a strings en out
    Ejemplo: wrapstrep(data,4,dataPtr); siendo char* data2  y char *dataPtr[4]
    Longitud vector = Nro Tokens!!  */
    //TODO: Control de Errores!
    unsigned int i;
    char * auxiliar;
    size_t ss;
    for (i=0; i<nroTokens;i++){
        auxiliar=realloc(auxiliar,128);
        auxiliar=strsep(&stringIn,";");
     
        ss=strlen(auxiliar);
        out[i]=malloc(ss);
        strcpy(out[i],auxiliar);
        free(out[i]);
        free(auxiliar);
        
    }
    //return tokens;
    return 0;
}
