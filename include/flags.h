#ifndef FLAGS_H_
#define FLAGS_H_

#define SUCCESS 0
#define FAIL -1
#define LISTNULL 2
#define NODENULL 3
#define DESTNULL 4
#define TMINV 5
#define OVER 6

//Paquetes
#define GET 0
#define POST 1
#define SUS 2
#define RESP 3

//Longitud Header y Carga no util de cada tipo de paquete
#define LONG_HEADER 4
#define LONG_GET 6
#define LONG_POST 6
#define LONG_SUS 2
#define LONG_RESP 4

#define GET_OP_NORMAL 0
#define GET_OP_TM 1

#define RESP_TIPO_OK 1
#define RESP_TIPO_FAIL 2

#define RESP_CODIGO_101 1
#define RESP_CODIGO_102 2
#define RESP_CODIGO_103 3
#define RESP_CODIGO_104 4
#define RESP_CODIGO_201 1
#define RESP_CODIGO_202 2
#define RESP_CODIGO_203 3
#define RESP_CODIGO_204 4
#define RESP_CODIGO_205 5
#define RESP_CODIGO_206 6       ///> Sin fuentes disponibles

#define SUS_OP_FUENTE 0
#define SUS_OP_CONS 1
#define SUS_OP_DEL 2

#endif
