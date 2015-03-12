#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

char *strip(char *str) {
    size_t ln = strlen(str) - 1;
    if (str[ln] == '\n') {
        str[ln] = '\0';
    }

    return str;
}

char *str_tolower(char *str) {
    unsigned short i;
    for(i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }

    return str;
}

void free_wrapp(int num, ...) {
    va_list a_list;
    int i;
    va_start(a_list, num);
    
    for (i = 0; i < num; i++) {
        void *ptr = va_arg(a_list, void *);
        if (ptr != NULL) {
            free(ptr);
        }
    }
    va_end(a_list);
}

size_t getNroTokens(const char *str, const char *delim) {
    unsigned int nroTokens, i;

    for (i = 0; i < strlen(str); i++) {
        if (str[i] == *delim) {
            nroTokens += 1;
        }
    }
    return nroTokens;
}

char **wrapstrsep(const char* str, const char* delim) {
    char *s = strdup(str);
    size_t tokens_alloc = getNroTokens(str, delim);
    size_t tokens_used = 0;

    if (tokens_alloc == 0) {
        return NULL;
    }

    char **tokens = calloc(tokens_alloc, sizeof(char *));
    char *token, *rest = s;

    while ((token = strsep(&rest, delim)) != NULL) {
        tokens[tokens_used++] = strdup(token);
    }

    return tokens;
}

