#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "flags.h"

#define INCREMENT 10

struct Consumidor {
    unsigned short port;
    char *ip;
    long int tm_inicio;
    long int tm_fin;
    unsigned long int data_enviada;
};

struct Buffer {
    long int tm;
    char *data;
};

struct ListNode {
    char *id;
    char *type;
    char *description;
    char *ip;
    char *port;
    int cant_data;
    struct Consumidor **consumidores;
    struct Buffer **buffer;
};

struct List {
    int count;
    int max;
    ListNode *elements;
};

List List_create() {
    List l = malloc(sizeof(struct List));
    if (l == NULL) { return NULL; }

    l->count = 0;
    l->max = INCREMENT;
    l->elements = calloc(INCREMENT, sizeof(struct ListNode));

    return l;
}

ListNode ListNode_create(char *type, char *description, char *ip, char *port) {
    ListNode ln = malloc(sizeof(struct ListNode));
    if (ln == NULL) { return NULL; }
    ln->id = NULL;
    ln->type = type;
    ln->description = description;
    ln->ip = ip;
    ln->port = port;
    ln->buffer = (struct Buffer **) malloc(sizeof(struct Buffer));
    ln->consumidores = (struct Consumidor **) malloc(sizeof(struct Consumidor));
    ln->cant_data = 0;

    return ln;
}

static void increment_array_size(List list) {
    list->max = INCREMENT * 2;
    list->elements = realloc(list->elements, list->max * sizeof(struct ListNode));
    // TODO: check allocation
}

static int get_null(List list) {
    int i;
    for (i = 1; i <= list->count; i++) {
        if (list->elements[i] == NULL) {
            return i;
        }
    }
    return FAIL;
}

int List_push(List list, ListNode node) {
    if (list == NULL || node == NULL) { return FAIL; }

    int blank_space = get_null(list);
    int id;
    if (blank_space == -1) {
        if (list->count >= list->max) {
            increment_array_size(list);
        }
        id = list->count + 1;
        list->count++;
    } else {
        id = blank_space;
    }
  
    node->id = malloc(2);
    sprintf(node->id, "%d", id);
    list->elements[id] = (ListNode) malloc(sizeof(struct ListNode));
    memcpy(list->elements[id], node, sizeof(struct ListNode));
    
    return id;
}

int List_search_by_ip_port(List list, char * ip, char * port) {
    if (list == NULL) return -1;
    
    unsigned int i;
    for (i = 1; i <= list->count; i++) {
        if (list->elements[i] != NULL) {
            if (strcmp(list->elements[i]->ip, ip) == 0 && strcmp(list->elements[i]->port, port)==0) {
                return i;
            }
        }
    }

    return FAIL;
} 

int List_search_by_id(List list, int id) {
    if (list == NULL || list->elements[id] == NULL) {
        return FAIL;
    } else
        return id;
}

char * List_to_csv(List list) {
    if (list == NULL || list->count == 0) { return NULL; }

    int i;
    char *csv = malloc(150);
    strcat(csv, "id,type,description,ip,port;");
    
    for (i = 1; i <= list->count; i++) {
        if (list->elements[i] != NULL) {
            char id[5];
            strcpy(id, list->elements[i]->id);
            char *type = (char *) list->elements[i]->type;
            char *ip = list->elements[i]->ip;
            char *description = list->elements[i]->description;
            char *port = list->elements[i]->port;
            
            int len_of_current_node = strlen(id) + strlen(type) + strlen(ip) + strlen(description) + strlen(port)+ 4; // 4 = separadores
            int len_of_csv = strlen(csv);
                    
            void *new_csv = realloc(csv, len_of_current_node + len_of_csv + 1);
            if (new_csv == NULL) {
                printf("Error");
                exit(1);
            } else {
                csv = new_csv;
            }
            strcat(csv, id);
            strcat(csv, ",");
            strcat(csv, type);
            strcat(csv, ",");
            strcat(csv, description);
            strcat(csv, ",");
            strcat(csv, ip);
            strcat(csv, ",");
            strcat(csv, port);
            strcat(csv, ";");
        }
    }

    return csv;
}

void List_delete_by_id(List list, int id) {
    //list->elements[id] = NULL;
}

int List_add_data_to_node_buffer(List list, int id, long int tm, char *data) {
    if (List_search_by_id(list, id) != FAIL) {
        struct ListNode *node = list->elements[id];
        int size = node->cant_data;
        
        node->buffer = (struct Buffer **) realloc(node->buffer, sizeof(struct Buffer) * (size + 1));
        node->buffer[size] = (struct Buffer *) malloc(sizeof(struct Buffer));
        node->buffer[size]->tm = tm;
        node->buffer[size]->data = (char *) malloc(strlen(data));
        strcpy(node->buffer[size]->data, data);
        node->cant_data++;
    
        return SUCCESS;
    }
    return FAIL;
}

int List_registrar_consumidor(List list, int id, char *ip, unsigned short port) {
    if (List_search_by_id(list, id) != FAIL) {
    
        struct ListNode *node = list->elements[id];
        struct Consumidor *consumidor;
        size_t size = sizeof(node->consumidores) / sizeof(struct Consumidor);

        node->consumidores = (struct Consumidor **) realloc(node->consumidores, sizeof(struct Consumidor) * (size + 1));
        node->consumidores[size] = (struct Consumidor *) malloc(sizeof(struct Consumidor));
        consumidor = node->consumidores[size];
        consumidor->port = port;
        consumidor->ip = (char *) malloc(strlen(ip));
        consumidor->tm_inicio = 0;
        consumidor->tm_fin = 0;
        consumidor->data_enviada = 0;
        strcpy(node->consumidores[size]->ip, ip);
    
        return size;
    }
    return FAIL;
}

struct Consumidor **List_get_consumidores(List list, int idFuente) {
    if (List_search_by_id(list, idFuente) != FAIL) {
        return list->elements[idFuente]->consumidores;
    }
    return NULL;
}

struct Consumidor *List_get_consumidor(List list, int idFuente, int idDestino) {
    if (List_search_by_id(list, idFuente) != FAIL) {
        struct Consumidor *con;
        if ((con = list->elements[idFuente]->consumidores[idDestino]) != NULL) {
            return con;
        };
    }
    return NULL;  
}

int List_get_node_data(List list, int idFuente, int idDestino, char *data) {
    if (list == NULL) { return LISTNULL; }
    if (list->elements[idFuente] == NULL ) { return NODENULL; }
    
    struct Consumidor *con = List_get_consumidor(list, idFuente, idDestino);

    if (con == NULL) { return DESTNULL; }
    
    struct ListNode *node = list->elements[idFuente];

    if (node->cant_data > con->data_enviada) {
        struct Buffer *buffer = node->buffer[con->data_enviada];
        sprintf(data, "%ld", buffer->tm);
        strncat(data, ";", 1);
        strncat(data, buffer->data, strlen(buffer->data));
        con->data_enviada++;
    } else {
        return OVER;
    }

    return SUCCESS;
}
