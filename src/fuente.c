#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcpDataStreaming.h"
#include "structures.h"

int main(){
	
	int sdf=connectTCP();
	msj_t paquete;
	int len;
	char data1[40]="text;text;medicion temperatura";
	

	pack(2, &data1, &paquete);

	len = sizeof(paquete);

	sendall(sdf, (char *)&paquete, &len);


	close(sdf);

	return 0;

}
