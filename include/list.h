#ifndef LIST_H_
#define LIST_H_

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
 * @param *List* donde se añade el nuevo elemento
 * @param *ListNode* a añadir
 * @return -1 si la alocacion falla
 *         index del nuevo elemento
 */
int List_push(List, ListNode);

/**
 * Busca un nodo por ip.
 *
 * @param *List* donde se busca el *ListNode*
 * @param *ip* correspondiente a un *ListNode*
 *
 * @return index del *ListNode* si se encuentra
 *         -1 caso contrario
 */
int List_search_by_id(List, int);

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

/**
 * Añade datos al buffer de un nodo de la lista
 *
 * @param List donde se encuentra el nodo
 * @param int id del nodo
 * @param long int timestamp en segundos
 * @param char datos
 *
 * @return 1 si puede guardar los datos
 *        -1 si no puede guardar los datos
 */

int List_add_data_to_node_buffer(List, int, long int, char *);

/**
 *
 * Registra un nuevo consumidor a una fuente
 *
 * @param lista donde estan almacenadas las fuentes
 * @param id de la fuente
 * @param ip del consumidor
 * @param puerto del consumidor
 * @param timestamp de incio de envio de datos (opcional)
 * @param timestamp de fin de envio de datos (opcional)
 *
 * @return id del consumidor o FAIL en caso de que ocurra un error
 */
int List_registrar_consumidor(List, int, char*, unsigned short); 

/**
 * Busca una fuente dado la ip y el puerto del mismo. 
 *
 * @param lista donde estan almacenadas las fuentes
 * @param ip de la fuente
 * @param port de la fuente
 *
 * @return id del consumidor o FAIL en caso de que ocurra un error
 */
int List_search_by_ip_port(List, char*, char*);

/**
 * Retorna todos los consumidores de determinada fuente. 
 *
 * @param lista donde estan almacenadas las fuentes
 * @param ip de la fuente
 *
 * @return array de consumidores
 */
struct Consumidor **List_get_consumidores(List, int);

/**
 * Busca un consumidor por id. 
 *
 * @param lista donde estan almacenadas las fuentes
 * @param id de la fuente
 * @param id del consumidor
 *
 * @return puntero a un Consumidor
 */
struct Consumidor *List_get_consumidor(List, int, int);

/**
 * Guarda en el buffer un dato con su correspondiente timestamp. 
 *
 * @param lista donde estan almacenadas las fuentes
 * @param id de la fuente
 * @param id del consumidor
 * @param buffer
 * @param timeline de inicio
 * @param timeline de fin
 *
 * @return status del pedido
 */
int List_get_node_data(List, int, int, char*, unsigned int, unsigned int);
#endif
