#ifndef _TCPDATASTREAMING_H
#define _TCPDATASTREAMING_H


#include "structures.h"


/**
 * Crea un socket descriptor en modo pasivo.
 * 
 * @param int *port* el puerto donde se abre el socket
 * @return int del socket descriptor
 *         -1 en caso de error
 */
int passiveTCPSocket(int port);

/**
 * Conecta un socket TCP a otro que esta en listen.
 * 
 * @return int del socket descriptor
 *         -1 en caso de error
 */
int connectTCP();

/**
 * Lee un *total* de n bytes desde el socket *sd*
 *
 * @param int *sd* un socket descriptor
 * @param char \* *buffer* donde se almacenan los bytes leidos
 * @param int *total* de bytes a leer
 *
 * @return int cantidad de bytes leidos
 *
 */
int receiveall(int sd, char * buffer, int total);

/**
 * envia un *len* de n bytes desde al socket *sd*
 *
 * @param int *sd* un socket descriptor
 * @param char \* *buffer* donde se almacenan los bytes enviados
 * @param int *len* de bytes a leer
 *
 * @return int cantidad de bytes enviados
 *
 */
int sendall (int sd, char *buf, int len);

int pack();
//int unpack();

/*
 * Crea un mensaje del tipo Get.
 *
 * @param int *dlen* tamaño del campo de datos
 * @param int *idFuente* especifica la fuente desde la que se desea obtener
 *      datos guardados en el servidor. *idFuente* debe ser valido (suscrito en
 *      el server). Si *idFuente* == 0, el servidor responde con todas la
 *      fuentes disponibles
 * @param int *op* si *op* == 0 es una consulta normal, esto es, el server
 *      empieza a mandar datos desde el ultimo dato obtenido de la fuente. Si
 *      *op* == 1, el consumidor solicita datos historicos, debera proveer 2
 *      timestamp en el destino, el servidor actua con el mejor esfuerzo e
 *      intenta aproximar estos dos tiempos.
 * @param int *id_destino* sirve para verificar que el consumidor esta
 *      registrado en el servidor
 * @param char data se usa unicamente para enviar 2 timestamp en el caso de que
 *      *op* == 1
 */
Get Mensaje_crear_get(int, int, int, int, char[]);


#endif

void wrapstrsep(char *stringIn, int nroTokens, char * out[] );
