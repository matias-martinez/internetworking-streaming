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


//Funcion que devuelve un socket descriptor abierto en modo pasivo.
int passiveTCPSocket(){

int sd;
struct sockaddr_in servidor;
struct sockaddr_in cliente;
//int qlen=10;

servidor.sin_family = AF_INET;
servidor.sin_port = htons(4567);//parametrizar
servidor.sin_addr.s_addr = INADDR_ANY;

sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

if (bind(sd, (struct sockaddr *) &servidor, sizeof(servidor)) < 0) {
	perror("Error en bind");
	exit(-1);
	}
/*if (listen(s, qlen) < 0){
	perror("Fallo listen en el puerto");//agregar numero de puerto
	exit(-1);
	}*/
return sd;
}

//-----------------------------------------------------------

int leerMensaje ( int sd, char * buffer, int total ) {
	int bytes=1;
	int leido=0;

	
	while ( (leido < total) && (bytes > 0) ) {

		bytes = recv ( sd , buffer + leido , total - leido , 0);
		if (bytes <0){
			perror("Error en recepcion del mensaje/ leer_mensaje");
			break;
		}
		leido = leido + bytes;

	}

	return ( leido );
}
