#ifndef _STRUCTURES_H
#define _STRUCTURES_H
#endif

//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/types.h>
#include <time.h>

typedef struct TimeStamp {
    char *dia;
    char *hora;
}

typedef struct _msj_ {
	uint8_t opcode; //Codigo del mensaje
	uint16_t dlen;//Longitud del campo de datos
	char data [128];//Long Max = 128B
} msj_t;


typedef struct Get {
    uint8_t opcode;
    uint16_t dlen;
	uint16_t idFuente;
	uint8_t op;
	uint16_t idDestino;
    char data[128];
}

typedef struct Post {
    uint8_t opcode
    uint16_t dlen;
    uint16_t idFuente;
    struct tm timestamp;
    char data[128];
}

typedef struct Sus {
	uint8_t opcode;
    uint16_t dlen;
	uint8_t op;
    char data[128];
}

typedef struct Resp {
    uint8_t opcode;
    uint16_t dlen;
    uint8_t tipo;
    uint16_t codigo;
    char data[128];
    
}

Get Package_create_get();
Post Package_create_Post();
Sus Package_create_Sus();
Get Package_create_Resp();
