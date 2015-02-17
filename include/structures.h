#ifndef _STRUCTURES_H
#define _STRUCTURES_H
#endif

//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/types.h>

typedef struct _msj_ 
{
	uint8_t opcode; //Codigo del mensaje
	uint16_t dlen;//Longitud del campo de datos
	char data [128];//Long Max = 128B
}msj_t;

//int llenarMsj(int opcode, 
/*
typedef struct _get_
{
	uint_16 idFuente;
	uint_8 op;
	uint_16 idDestino;
}get_t

typedef struct _post_

typedef struct _sus_


typedef struct _resp_
*/
