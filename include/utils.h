#ifndef UTILS_H_
#define UTILS_H_


/**
 * Elimina el caracter de nueva linea (\n) de una cadena.
 *
 * @param cadena a modificar.
 * @return la cadena modificada.
 */
char *strip(char *); 

/**
 * Transforma los caracteres de una cadena a lower case.
 *
 * @param cadena a modificar.
 * @return la cadena modificada.
 */
char *str_tolower(char *);

/**
 * Libera los punteros pasados en la ellipsis
 *
 * @param cantidad de punteros.
 * @param punteros
 *
 * @return la cadena modificada.
 */
void free_wrapp(int, ...);

/**
 * Calcula la cantidad de Tokens que hay en una cadena. 
 *
 * @param la cadena a separar.
 * @param el delimitador.
 *
 * @return un array de tokens
 */
size_t getNroTokens(const char *, const char *);

/**
 * Separa la cadena *str* en tokens usando como delimitador *delim*
 *
 * @param la cadena a separar.
 * @param el delimitador.
 *
 * @return un array de tokens
 */
char **wrapstrsep(const char* str, const char* delim);

#endif
