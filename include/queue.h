#ifndef List_h
#define List_h

struct ListNode;

typedef struct ListNode {
    struct ListNode * next;
    void *value;
} ListNode

typedef struct List {
    int count;
    ListNode *first;
    ListNode *last;
}

List *List_create();
void *List_remove(List *list, ListNode *node);
int *List_push(List *list, ListNode *node); 
int *List_count(List *list);
ListNode *List_first(List *list);
ListNode *List_last(List *list);

#endif
