#ifndef OUT_H_
#define OUT_H_

#include "structures.h"

/**
 * Imprime la informacion recibida en un mensaje.
 *
 * @param Header del mensaje.
 * @param mensaje el cual puede ser del tipo GET, SUS, RESP, POST.
 *
 */
void print_mensaje(Header, void *);

#endif
