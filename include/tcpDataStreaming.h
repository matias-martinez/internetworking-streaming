#ifndef _TCPDATASTREAMING_H
#define _TCPDATASTREAMING_H


#include "structures.h"


/**
 * Crea un socket descriptor en modo pasivo.
 * 
 * @param char\* host
 * @param int el puerto donde se abre el socket
 * @param int tamaño de la lista de conexiones en espera
 *
 * @return int del socket descriptor
 *         -1 en caso de error
 */
int passiveTCPSocket(char *, int, int);

/**
 * Conecta un socket TCP a otro que esta en listen.
 *
 * @param char\* hostname del port en listen
 * @parm int port del socket en listen
 * 
 * @return int del socket descriptor
 *         -1 en caso de error
 */
int connectTCP(char *, int);

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

/**
 * Dado un socket descriptor recibe el *Header* de un mensaje enviado por este.
 * 
 * @param int socket descriptor
 *
 * @return Header del mensaje enviado
 */
Header *Mensaje_recibir_header(int);

/**
 * Crea un mensaje del tipo Sus.
 *
 * @param int *op* si op == 0 el cliente desea registrarse como fuente.
 *                    op == 1 el cliente desea registrarse como consumidor.
 *                    op == 2 el cliente desea borrar su suscripcion.
 * @param char \* data si op == 0 se envia el tipo de dato, la codificacion
 *                          del mismo y una descripcion opcional. Estos 
 *                          campos se separan por medio de ';'.
 *                        op == 1 id de la fuente a la que se desea suscribir.
 *                        op == 2 id del cliente y direccion ip. Estos campos
 *                          se separan por medio de ';'.
 *
 * @return un mensaje del tipo Sus.
 */
Sus *Mensaje_crear_sus(int, char *);

/**
 * Envia un mensaje del tipo Sus.
 *
 * @param int socket descriptor del host que recibe el mensaje.
 * @param Sus mensaje que se desea enviar.
 *
 * @param int cantidad de bytes que se enviaron.
 *
 */
int Mensaje_enviar_sus(int, Sus *);

/**
 * Dado un socket descriptor recibe el un mensaje del tipo Sus enviado por este.
 * 
 * @param int socket descriptor
 * @param int tamaño en bytes del campo de datos
 *
 * @return Header del mensaje enviado
 */
Sus *Mensaje_recibir_sus(int, int);

/**
 * Crea un mensaje del tipo Resp. 
 *
 * @param int tipo de respuesta. Si tipo == 0, el servidor comunica que recibio
 *                                    la solicitud con exito (OK).
 *                                  tipo == 1, el servidor comunica alguna
 *                                    situacion de error.
 * @param int codigo de mensaje. Cada tipo de respuesta tiene un codigo.
 *  si tipo == 0:
 *      - codigo == 11: solicitud aceptada, procesando la respuesta.
 *      - codigo == 12: lista de fuentes.
 *      - codigo == 13: Post aceptado.
 *      - codigo == 14: Get aceptado.
 *  si tipo == 1:
 *      - codigo == 21: solicitud aceptada.
 *      - codigo == 22: idFuente inexistente.
 *      - codigo == 23: timestamp invalido.
 *      - codigo == 24: espacio insuficiente.
 *      - codigo == 25: servicio no aceptado temporalmente.
 * @param char[] si tipo == 0 y codigo == 12, el servidor envia una lista de
 *  fuentes disponibles a un consumidor con los siguientes datos: idFuente,
 *  tipo de dato y tipo de codificacion. Estos campos separados por ';'
 *
 *  @return un mensaje del tipo Resp
 */
Resp *Mensaje_crear_resp(int tipo, int codigo, char data[]);

/**
 * Envia un mensaje del tipo Resp.
 *
 * @param int socket descriptor del host que recibe el mensaje.
 * @param Sus mensaje que se desea enviar.
 *
 * @param int cantidad de bytes que se enviaron.
 *
 */
int Mensaje_enviar_resp(int, Resp*);

/**
 * Dado un socket descriptor y el tamaño del campo de datos dlen
 * recibe un mensaje del tipo Resp enviado por este.
 * 
 * @param int socket descriptor.
 * @param int tamaño del campo de datos.
 *
 * @return Resp 
 */
Resp *Mensaje_recibir_resp(int, int);

/**
 * Crea un mensaje del tipo Post.
 * Utilizado por el cliente fuente para enviar datos al servidor, luego de
 * haberse suscrito.
 *
 * @param int id de la fuente. Especifica la fuente que envia el mensaje.
 *  Este id debe estar registrado en el servidor.
 * @param uint32_t timestamp que contiene el horario correspondiente a la
 *  captura del dato que envia la fuente. 
 * @param char[] datos que envia la fuente.
 *
 *  @return un mensaje del tipo Resp
 */
Post *Mensaje_crear_post(int, uint32_t, char []);

/**
 * Envia un mensaje del tipo Post.
 *
 * @param int socket descriptor del host que recibe el mensaje.
 * @param Post mensaje que se desea enviar.
 *
 * @param int cantidad de bytes que se enviaron.
 *
 */
int Mensaje_enviar_post(int, Post *);

/**
 * Dado un socket descriptor y el tamaño del campo de datos dlen
 * recibe un mensaje del tipo Post enviado por este.
 * 
 * @param int socket descriptor.
 * @param int tamaño del campo de datos.
 *
 * @return Post
 */

Post *Mensaje_recibir_post(int, const size_t);

/**
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
 * @param char \* data se usa unicamente para enviar 2 timestamp en el caso de que
 *      *op* == 1
 */
Get *Mensaje_crear_get( int, int, int, char[]);

int Mensaje_enviar_get(int, Get *);

Get *Mensaje_recibir_get(int, const size_t);



#endif
