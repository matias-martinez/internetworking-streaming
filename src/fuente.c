#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "tcpDataStreaming.h"
#include "structures.h"

int main() {


	int sdf = connectTCP();
	msj_t *paqueteEnvio;
	msj_t *paqueteRecepcion;
	int len, recibido, enviados;	
	char data2[250];
    memset(&data2,0, sizeof(data2));
	
	//generar data dinamicamente!	
	char data1[40]="0;text/plain;medicion temperatura\0";// |op;codificacion;descripcion| Mensaje SUS. OP = 1.
	//char *payload;
	//strncpy ( payload , data1 , strlen(data1)-1 );->violacion segmento

	pack(2, data1, paqueteEnvio);//check errors! . 2=SUS.
	len = strlen(data1)+3; //size of payload + header

	enviados = sendall(sdf, (char *)paqueteEnvio, len);
	printf("Enviados %d. DLEN  datapaqueteEnvio=%d\n", enviados,paqueteEnvio->dlen);
	//esperar rta
	recibido = receiveall(sdf, data2, 3); //receive. se espera un msj RESP de exito
	printf("Recibi %d Bytes\n", recibido);
	paqueteRecepcion = (msj_t *) &data2;
	printf("Recibi un mensaje %d con un DLEN de %d Bytes\n", paqueteRecepcion->opcode, paqueteRecepcion->dlen);

	//en base a Dlen, hacer un receive de lo que falta
	//paqueteRecepcion = (msj_t *)&data2;
	//printf("Payload Recibida: %s\n", paqueteRecepcion->data);


	close(sdf);

	return 0;

}
