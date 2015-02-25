#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h> // sleep
#include "tcpDataStreaming.h"
#include "structures.h"
#include "list.h"

pthread_mutex_t mutex_list;
pthread_cond_t cond_list;

struct pth_param_t {
    List fuentes;
    int sdf;
    struct sockaddr_in fuente;
};

void * request_handler();

struct pth_param_t Crear_estructura_pthread();

int main(int argc, char *argv[]){
    if (argc != 4) {
        printf("usage: %s [HOST] [PORT] [QLEN]\n", argv[0]);
        printf("Example ./server 127.0.0.1 8888 10\n");
        exit(1);
    }
    
    char *end;
    char *host = argv[1];
    int port = strtol(argv[2], &end, 10);
    int qlen = strtol(argv[3], &end, 10);

    List fuentes = List_create(); 
    int sd = passiveTCPSocket(host, port, qlen);
    int sdf, lon;
    struct sockaddr_in fuente;
    pthread_t tid;
    
    
    printf("|||| Servidor DataStreaming - v0.1 ||||\n");
    printf("---------------------------------------\n");

    pthread_mutex_init(&mutex_list, NULL);
    pthread_cond_init(&cond_list, NULL);

    struct pth_param_t estructura;
    estructura.fuentes = fuentes;
    estructura.sdf = sdf;
    estructura.fuente = fuente;

    while (1) {

        lon = sizeof(fuente);
        printf("- Aguardando connect -\n");
        sdf = accept(sd, (struct sockaddr *) &fuente, &lon);
        printf("Recibí connect desde: %s \n", inet_ntoa(fuente.sin_addr));

        pthread_create(&tid, NULL, request_handler, &estructura);
    }
    
    return 0;
}

void * request_handler(struct pth_param_t *pth_struct) {
        int sdf = pth_struct->sdf;
        List fuentes = pth_struct->fuentes;
        struct sockaddr_in fuente = pth_struct->fuente;

        Header *header;
        Sus *paquete_sus = NULL;
        Resp *paquete_resp;
        Post *paquete_post;
        char aux[128];

        header = Mensaje_recibir_header(sdf);

        printf("Recibi un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);
        
        sleep(2);                  // Simulo tiempo de procesamiento
        switch(header->opcode){
            case 1:
                paquete_post = Mensaje_recibir_post(sdf, header->dlen);
                printf("Mensaje POST. Id de la fuente = %d\n", paquete_post->idFuente);
                printf("Recibi estos datos: %s\t", paquete_post->data);
                printf("Con este timestamp: %d\n", paquete_post->timestamp);
                
                if (List_search_by_id(fuentes, paquete_post->idFuente) != -1) {
                    // TODO Agregar datos al buffer de la fuente
                    paquete_resp = Mensaje_crear_resp(0, 13, "");
                    Mensaje_enviar_resp(sdf, paquete_resp);
                } else {
                    paquete_resp = Mensaje_crear_resp(1, 22, "");
                    Mensaje_enviar_resp(sdf, paquete_resp);
                }
                printf("---------------------------------------\n");
                break;
            case 2:
                paquete_sus = Mensaje_recibir_sus(sdf, header->dlen);
                printf("Operacion Mensaje SUS = %d\n", paquete_sus->op);
                printf("Recibi estos datos: %s\n", paquete_sus->data);
                
                if (paquete_sus->op == 0){
                    char *ip = inet_ntoa(fuente.sin_addr);
                    char **datos = wrapstrsep(paquete_sus->data, ";");
                    if (List_search_by_ip(fuentes, ip) == -1) {
                        ListNode fuente_node = ListNode_create(datos[0], datos[1], ip);
                        if (fuente_node == NULL) {
                            printf("fallo la alocacion");
                        }

                        pthread_mutex_lock(&mutex_list);     
                        int id = List_push(fuentes, fuente_node);
                        pthread_cond_signal(&cond_list);
                        pthread_mutex_unlock(&mutex_list);

                        printf("Se asigna a la fuente de ip: %s, el id %d\n", ip, id);  
                        sprintf(aux, "%d", id);
                        paquete_resp = Mensaje_crear_resp(0, 11, aux);
                        Mensaje_enviar_resp(sdf, paquete_resp);             //tipo=0;Codigo=11;data=IDFUENTE   

                    }
                    else {
                        paquete_resp = Mensaje_crear_resp(1, 21, "");         //Tipo=1;Codigo=21;Data=NULL
                        Mensaje_enviar_resp(sdf,paquete_resp);
                    }
                    printf("---------------------------------------\n");
                }
                if (paquete_sus->op == 1){
                    //TODO: Solicitud de Sucripcion de Consumidor. Proxima Version.
                }
                if (paquete_sus->op == 2){
                    char *ip = inet_ntoa(fuente.sin_addr);
                    int id;
                    if ((id = List_search_by_ip(fuentes, ip)) != -1) {
                        pthread_mutex_lock(&mutex_list);
                        List_delete_by_id(fuentes, id);
                        pthread_cond_signal(&cond_list);
                        pthread_mutex_unlock(&mutex_list);
                        paquete_resp = Mensaje_crear_resp(0, 11, "");
                        printf("La Fuente con ID %d se DESUSCRIBIO!\n",id );
                    } else {
                        paquete_resp = Mensaje_crear_resp(1, 21, "");
                        printf("Mensaje de desuscripcion con Id inexistente..\n",id );
                        printf("---------------------------------------\n");
                    }
                    printf("---------------------------------------\n");
                }
                break;
        }
        close(sdf);
}

