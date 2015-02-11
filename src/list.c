#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define INCREMENT 10

struct ListNode {
    char *id;
    char *type;
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
    l->elements = calloc(INCREMENT, sizeof(ListNode));

    return l;
}

ListNode ListNode_create() {
    ListNode ln = malloc(sizeof(struct ListNode));
    if (ln == NULL) { return NULL; }
    ln->id = calloc(10, sizeof(char));
    ln->type = NULL;

    return ln;
}

static void increment_array_size(List list) {
    list->max = INCREMENT * 2;
    list->elements = realloc(list->elements, list->max * sizeof(ListNode));
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
    

    sprintf(node->id, "%d", id);
    list->elements[id] = node;
    
    return id;
}

char * List_to_csv(List list) {
    int i;
    char *tmp = malloc((list->count)*sizeof(ListNode));
    strcat(tmp, "id,type;\n");
    
    for (i = 0; i < list->count; i++) {
        if (list->elements[i] != NULL) {
            char *id = list->elements[i]->id;
            char *type = list->elements[i]->type;
            if (sizeof(tmp) <= sizeof(id) + sizeof(type)+ sizeof(",;\n")) {
                tmp = realloc(tmp, 100 * sizeof(tmp));
            } 
            strcat(tmp, id);
            strcat(tmp, ",");
            strcat(tmp, type);
            strcat(tmp, ";\n");
        }
    }

    return tmp;
}

void List_delete_by_id(List list, int id) {
    list->elements[id] = NULL;
}

int main() {
    // Ejemplo de uso
    List l = List_create();
    ListNode ln; 
    int i;

    for (i = 0; i < 15; i++) {
        ln = ListNode_create();
        ln->type = "asasddddasasdqweqsdasddd";
        List_push(l, ln);
    }
    
    printf("%s\n", List_to_csv(l));
    
    List_delete_by_id(l, 2);
    printf("%s\n", List_to_csv(l));

    ln = ListNode_create();
    ln->type = "asas";
    List_push(l, ln);

    printf("%s\n", List_to_csv(l));

    return 0;
}   
