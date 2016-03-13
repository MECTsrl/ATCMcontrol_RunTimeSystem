#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * strtok_csv(char *string, const char *separators, char **savedptr)
{
    char * p, *s;
    int i;

    if (separators == NULL || savedptr == NULL) {
        return NULL;
    }    
    if (string == NULL) {
        p = *savedptr;
        if (p == NULL) {
            return NULL;
        }
    } else {
        p = string;
    }

    s = strstr(p, separators);
    if (s == NULL) {
        *savedptr = NULL;
        return p;
    }
    *s = 0;
    *savedptr = s + 1;

    while (isspace(*p) && p < s) {
        ++p;
    }
    
    return p;
}

int main(int argc, char *argv[])
{
    char *s, *x, *p, *r;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s string delim\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    s = argv[1];
    x = argv[2];

    p = strtok_csv(s, x, &r);
    while (p != NULL) {
        printf("\t\"%s\"\n", p);
        p = strtok_csv(NULL, x, &r);
    }

    exit(EXIT_SUCCESS);
}

