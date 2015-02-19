#ifndef _STRUCTURES_H
#define _STRUCTURES_H

//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/types.h>
#include <time.h>


typedef struct Header {
	uint8_t opcode;   //Codigo del mensaje
	uint16_t dlen;    //Longitud del campo de datos
} *Header;

typedef struct _msj_ {
	uint8_t opcode;   //Codigo del mensaje
	uint16_t dlen;    //Longitud del campo de datos
    char data[128];
} msj_t;


typedef struct Get {
	uint8_t opcode;   //Codigo del mensaje
	uint16_t dlen;    //Longitud del campo de datos
	uint16_t idFuente;
	uint8_t op;
	uint16_t idDestino;
    char data[128];
} *Get;

typedef struct Post {
    Header header;
    uint16_t idFuente;
    struct tm timestamp;
    char data[128];
} *Post;

typedef struct Sus {
    uint8_t opcode;   //Codigo del mensaje
	uint16_t dlen;    //Longitud del campo de datos
	uint8_t op;
    char data[128];
} *Sus;

typedef struct Resp {
     uint8_t opcode;   //Codigo del mensaje
	uint16_t dlen;    //Longitud del campo de datos

    uint8_t tipo;
    uint16_t codigo;
    char data[128];
    
} *Resp;

#endif
