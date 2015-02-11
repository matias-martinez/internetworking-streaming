#ifndef List_h
#define List_h

typedef struct ListNode *ListNode;

typedef struct List *List;


/**
 * Crea una nueva lista.
 *
 * @return NULL si la alocacion falla 
 */
List List_create(void);

/**
 * Agrega un elemento a la lista.
 *
 * @param una *Lista* donde se añade el nuevo elemento
 * @param un *ListNode* a añadir
 * @return -1 si la alocacion falla
 *         index del nuevo elemento
 */
int List_push(List, ListNode);

/**
 * Crea un nuevo elemento.
 * @return NULL si la alocacion falla
 */
ListNode ListNode_create();

/**
 * Convierte los elementos de una lista a csv.
 */
char * List_to_csv(List);

/**
 * Borra un elemento de la lista por id.
 */
void List_delete_by_id(List, int);

#endif
