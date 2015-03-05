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
#include <signal.h>
#include "tcpDataStreaming.h"
#include "structures.h"
#include "list.h"
#include "flags.h"
#include "utils.h"

pthread_mutex_t mutex_list;
pthread_cond_t cond_list;

struct pth_param_t {
    List fuentes;
    int *sdf;
    struct sockaddr_in cliente;
};


struct pth_param_t Crear_estructura_pthread();

void * request_handler();

void sigint_handler(int signum) {
    printf("\n\n------ME INTERRUMPIERON------\n\n");
    exit(signum);
}

int main(int argc, char *argv[]){
    if (argc != 4) {
        printf("usage: %s [HOST] [PORT] [QLEN]\n", argv[0]);
        printf("Example ./server 127.0.0.1 8888 10\n");
        exit(1);
    }
 
    signal(SIGINT, sigint_handler);
    char *end;
    char *host = argv[1];
    int port = strtol(argv[2], &end, 10);
    int qlen = strtol(argv[3], &end, 10);

    List fuentes = List_create(); 
    int sd = passiveTCPSocket(host, port, qlen);
    int sdf, lon;
    struct sockaddr_in cliente;
    
    
    printf("|||| Servidor DataStreaming - v0.1 ||||\n");
    printf("---------------------------------------\n");
    
    pthread_mutex_init(&mutex_list, NULL);
    pthread_cond_init(&cond_list, NULL);
    

    lon = sizeof(struct sockaddr_in);
    printf("- Aguardando connect -\n");
    while (sdf = accept(sd, (struct sockaddr *) &cliente, &lon)) {
        printf("Recibí connect desde: %s port %d\n", inet_ntoa(cliente.sin_addr), cliente.sin_port);
         
        pthread_t tid;

        struct pth_param_t estructura;
        estructura.fuentes = fuentes;
        estructura.sdf = &sdf;
        estructura.cliente = cliente;

        pthread_create(&tid, NULL, request_handler, &estructura);
    }
    
    return 0;

    close(sd);
}

void * request_handler(struct pth_param_t *pth_struct) {
        int sdf = *(pth_struct->sdf);
        pth_struct->fuentes;
        struct sockaddr_in cliente = pth_struct->cliente;
        short strike = 0;
        int sdf_open = 1;
        Header *header = NULL;

        while (sdf_open == 1) {
            header = Mensaje_recibir_header(sdf);
            Sus *paquete_sus = NULL;
            Resp *paquete_resp = NULL;
            Post *paquete_post = NULL;
            Get *paquete_get = NULL;

            printf("Recibi un mensaje %d con un DLEN de %d Bytes\n", header->opcode, header->dlen);
        
            //sleep(1);                  // Simulo tiempo de procesamiento
            switch(header->opcode){
                case GET:
                    paquete_get = Mensaje_recibir_get(sdf, header->dlen);
                    char *data;
                    int tipo;
                    int codigo;

                    printf("Mensaje GET:\n");
                    printf("Operacion: %d. \t",paquete_get->op );
                    printf("Id Fuente: %d.\t", paquete_get->idFuente);
                    printf("Id Destino: %d\t",paquete_get->idDestino);
                    printf("Data: %s\n",paquete_get->data);
                    if (paquete_get->idFuente == 0) {
                        if ((data = List_to_csv(pth_struct->fuentes)) != NULL) { ; 
                            tipo = RESP_TIPO_OK;
                            codigo = RESP_CODIGO_102;
                        } else {
                            tipo = RESP_TIPO_FAIL;
                            codigo = RESP_CODIGO_206;
                        }
                        paquete_resp = Mensaje_crear_resp(tipo, codigo, data);
                        Mensaje_enviar_resp(sdf, paquete_resp);
                    } else {
                        unsigned int tminicio = 0;
                        unsigned int tmfin = 0;
                        int salida;

                        if (paquete_get->op == GET_OP_TM) {
                            char **datos = wrapstrsep(paquete_get->data, ";");
                            tminicio = atoi(datos[0]);
                            tmfin = atoi(datos[1]);
                        }
                        
                        do {
                            salida = List_get_node_data(pth_struct->fuentes, paquete_get->idFuente, 
                                    paquete_get->idDestino, data, tminicio, tmfin);
                            switch (salida) {
                                case SUCCESS:
                                    tipo = RESP_TIPO_OK;
                                    codigo = RESP_CODIGO_104;
                                    printf("Enviando Datos a consumidor de id: %d\n", paquete_get->idDestino);
                                    break;
                                case OVER:              // TODO: tratar errores
                                case LISTNULL:
                                case NODENULL:
                                case DESTNULL:
                                case TMINV:
                                    tipo = RESP_TIPO_FAIL;
                                    codigo = RESP_CODIGO_201;
                                    break;
                                }
                        
                            paquete_resp = Mensaje_crear_resp(tipo, codigo, data);
                            Mensaje_enviar_resp(sdf, paquete_resp);
                        } while (salida == SUCCESS);
                    }

                    break;
                case POST:
                    paquete_post = Mensaje_recibir_post(sdf, header->dlen);
                    printf("Mensaje POST. Id de la fuente = %d\n", paquete_post->idFuente);
                    printf("Recibi estos datos: %s\t", paquete_post->data);
                    printf("Con este timestamp: %d\n", paquete_post->timestamp);
                    
                    if (List_search_by_id(pth_struct->fuentes, paquete_post->idFuente) != FAIL) {
                        List_add_data_to_node_buffer(pth_struct->fuentes, paquete_post->idFuente,
                                paquete_post->timestamp, paquete_post->data);
                        
                        paquete_resp = Mensaje_crear_resp(RESP_TIPO_OK, RESP_CODIGO_103, "");
                    } else {
                        paquete_resp = Mensaje_crear_resp(RESP_TIPO_FAIL, RESP_CODIGO_202, "");
                    }
                    Mensaje_enviar_resp(sdf, paquete_resp);
                    printf("---------------------------------------\n");
                    break;
                case SUS:
                    paquete_sus = Mensaje_recibir_sus(sdf, header->dlen);
                    printf("Operacion Mensaje SUS = %d\n", paquete_sus->op);
                    printf("Recibi estos datos: %s\n", paquete_sus->data);
                    
                    char *ip = inet_ntoa(cliente.sin_addr);
                    char *portF = malloc(6);
                    char *id_str = malloc(6);
                    sprintf(portF, "%d", cliente.sin_port);
                   
                    if (paquete_sus->op == SUS_OP_FUENTE){
                        char **datos = wrapstrsep(paquete_sus->data, ";");
                        
                        if (List_search_by_ip_port(pth_struct->fuentes, ip, portF) == FAIL) {
                            ListNode fuente_node = ListNode_create(datos[0], datos[1], ip, portF);
                            if (fuente_node == NULL) {
                                printf("fallo la alocacion");
                            }
                            pthread_mutex_lock(&mutex_list);     
                            int id = List_push(pth_struct->fuentes, fuente_node);
                            pthread_cond_signal(&cond_list);
                            pthread_mutex_unlock(&mutex_list);

                            printf("Se asigna a la fuente de ip: %s, el id %d\n", ip, id);  
                            sprintf(id_str, "%d", id);
                            paquete_resp = Mensaje_crear_resp(RESP_TIPO_OK, RESP_CODIGO_101, id_str);
                        }
                        else {
                            paquete_resp = Mensaje_crear_resp(RESP_TIPO_FAIL, RESP_CODIGO_201, "");
                        }

                        Mensaje_enviar_resp(sdf, paquete_resp);            
                        printf("---------------------------------------\n");
                    }
                    if (paquete_sus->op == SUS_OP_CONS){
                        int idSolicitado = atoi(paquete_sus->data);
                        pthread_mutex_lock(&mutex_list);
                        int idDestino = List_registrar_consumidor(pth_struct->fuentes, idSolicitado,
                                inet_ntoa(cliente.sin_addr), cliente.sin_port);
                        pthread_cond_signal(&cond_list);
                        pthread_mutex_unlock(&mutex_list);
                        if (idDestino != FAIL) {
                            char * idDestino_str = malloc(6);
                            sprintf(idDestino_str, "%d", idDestino);
                            paquete_resp = Mensaje_crear_resp(RESP_TIPO_OK, RESP_CODIGO_101, idDestino_str);
                            free(idDestino_str);
                        } else {
                            paquete_resp = Mensaje_crear_resp(RESP_TIPO_FAIL, RESP_CODIGO_202, "Id fuente inexistente");
                        }

                        Mensaje_enviar_resp(sdf,paquete_resp);
                    }
                    if (paquete_sus->op == SUS_OP_DEL){
                        int id;
                        if ((id = List_search_by_ip_port(pth_struct->fuentes, ip, portF)) != FAIL) {
                            pthread_mutex_lock(&mutex_list);
                            List_delete_by_id(pth_struct->fuentes, id);
                            pthread_cond_signal(&cond_list);
                            pthread_mutex_unlock(&mutex_list);
                            paquete_resp = Mensaje_crear_resp(RESP_TIPO_OK, RESP_CODIGO_101, "");
                            printf("La Fuente con ID %d se DESUSCRIBIO!\n", id);
                        } else {
                            paquete_resp = Mensaje_crear_resp(RESP_TIPO_OK, RESP_CODIGO_201, "");
                            printf("Mensaje de desuscripcion con Id inexistente..\n",id );
                            printf("---------------------------------------\n");
                        }

                        Mensaje_enviar_resp(sdf, paquete_resp);
                        printf("---------------------------------------\n");
                    }
                    break;
                case RESP:
                    paquete_resp = Mensaje_recibir_resp(sdf, header->dlen);
                    printf("Operacion del mensaje RESP = %d | tipo = %d\n", paquete_resp->codigo, paquete_resp->tipo);
                    if (paquete_resp->tipo == RESP_TIPO_OK && paquete_resp->codigo == RESP_CODIGO_101) {
                        sdf_open = 0;
                        close(sdf);
                        printf("ACK recibido.\n");
                        printf("-------------------------------------------\n");
                        pthread_exit(NULL);
                    }
                    break;
                default:
                    strike++;
                    printf("Paquete Desconocido. %d Strike\n", strike);
                    if (strike == 3) {
                        printf("Desconectando la conexion\n");
                        close(sdf);
                        sdf_open = 0;
                        pthread_exit(NULL);
                    }
                    break;
            }
            free_wrapp(paquete_resp);
            free_wrapp(paquete_sus);
            free_wrapp(paquete_get);
            free_wrapp(header);
        }
}

