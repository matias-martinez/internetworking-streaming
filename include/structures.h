#ifndef _STRUCTURES_H
#define _STRUCTURES_H

#include <stdint.h>

#pragma pack(push, 1)
/**
 * Header comun a todos los mensajes.
 */
typedef struct {
    uint16_t opcode;   ///< codigo de operacion.
    uint16_t dlen;     ///< tamaño del campo de datos. 
} Header;

/**
 * Utilizado por el cliente consumidor para solicitar datos al servidor.
 */
typedef struct {
    uint16_t opcode;    ///< codigo de operacion.
    uint16_t dlen;      ///< tamaño del campo de datos. 
    uint16_t idFuente;  ///< Especifica la fuente desde la que se desea obtener datos
    uint16_t op;        ///< Opciones
    uint16_t idDestino; ///< Sirve para verificar que el consumidor
    char data[128];     ///< Se usa para enviar 2 timestamp en el caso de que OP = 1
} Get;

/**
 * Utilizado por el cliente fuente para enviar datos al servidor, luego de
 * haberse suscrito.
 */
typedef struct {
    uint16_t opcode;    ///< Codigo de operacion.
    uint16_t dlen;      ///< Tamaño del campo de datos. 
    uint16_t idFuente;  ///< Especifica la fuente que env ́ıa el mensaje.
    uint32_t timestamp; ///< timestamp correspondiente a la captura de los datos.
    char data[128];     ///< Datos que se enviaran.
} Post;

/**
 * Mensaje utilizado por clientes, fuente y consumidor, para suscribirse al
 * servicio.
 *
 */
typedef struct {
    uint16_t opcode;    ///< Codigo de operacion.
    uint16_t dlen;      ///< Tamaño del campo de datos. 
    uint16_t op;        ///< Opciones.
    char data[128];     ///< datos extra.
} Sus;

/**
 * Mensajes de respuesta del servidor hacia los clientes.
 *
 */
typedef struct {
    uint16_t opcode;    ///< Codigo de operacion.
    uint16_t dlen;      ///< Tamaño del campo de datos. 
    uint16_t tipo;      ///< Tipo de respuesta.
    uint16_t codigo;    ///< Codigo que provee mas datos sobre la respuesta.
    char data[128];     ///< Datos exta.
} Resp;

#pragma pack(pop)
#endif
