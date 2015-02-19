#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define INCREMENT 10

struct ListNode {
    char *id;
    char *type;
    char *description;
    char *ip;
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

ListNode ListNode_create(char *type, char *description, char *ip) {
    ListNode ln = malloc(sizeof(struct ListNode));
    if (ln == NULL) { return NULL; }
    ln->id = NULL;
    ln->type = type;
    ln->description = description;
    ln->ip = ip;

    return ln;
}

static void increment_array_size(List list) {
    list->max = INCREMENT * 2;
    list->elements = realloc(list->elements, list->max * sizeof(struct ListNode));
    // TODO: check allocation
}

static int get_null(List list) {
    int i;
    for (i = 0; i < list->count; i++) {
        if (list->elements[i] == NULL) {
            return i;
        }
    }
    return -1;
}

int List_push(List list, ListNode node) {
    if (list == NULL || node == NULL) { return -1; }

    int blank_space = get_null(list);
    int id;
    if (blank_space == -1) {
        if (list->count >= list->max) {
            increment_array_size(list);
        }
        id = list->count;
        list->count++;
    } else {
        id = blank_space;
    }
    
    list->elements[id] = node;
    
    return id;
}

int List_search_by_ip(List list, char * ip) {
    if (list == NULL) return -1;
    
    unsigned int i;
    for (i = 0; i < list->count; i++) {
        if (strcmp(list->elements[0]->ip, ip) == 0) {
            return i;
        }
    }

    return -1;
}

char * List_to_csv(List list) {
    int i;
    char *csv = malloc(150);
    strcat(csv, "id,type,description,ip;");
    
    for (i = 0; i < list->count; i++) {
        if (list->elements[i] != NULL) {
            char id[5];
            sprintf(id, "%d", list->elements[i]->id);
            char *type = (char *) list->elements[i]->type;
            char *ip = list->elements[i]->ip;
            char *description = list->elements[i]->description;
            
            int len_of_current_node = strlen(id) + strlen(type) + strlen(ip) + strlen(description) + 4; // 4 = separadores
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
            strcat(csv, ";");
        }
    }

    return csv;
}

void List_delete_by_id(List list, int id) {
    list->elements[id] = NULL;
}

