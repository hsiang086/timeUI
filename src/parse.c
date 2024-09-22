#include "parse.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Parser* NewParser(int args, char **argv) {
    Parser *p = malloc(sizeof(Parser));
    if (p == NULL) {
        return NULL;
    }
    p->args = args;
    p->argv = argv;
    return p;
}

void FreeParser(Parser *p) {
    free(p);
}

Result* Parse(Parser *p) {
    if (p == NULL || p->argv == NULL) {
        return NULL;
    }

    Result *r = malloc(sizeof(Result));
    if (r == NULL) {
        return NULL;
    }

    r->help = false;
    r->theme = Default;

    for (int i = 1; i < p->args; i++) {
        if (p->argv[i] == NULL) {
            fprintf(stderr, "Warning: Null argument at index %d\n", i);
            continue;
        }

        if (p->argv[i][0] != '-') {
            continue;  // Skip non-option arguments
        }

        if (strcmp(p->argv[i], "-h") == 0 || strcmp(p->argv[i], "--help") == 0) {
            r->help = true;
        } else if ((strcmp(p->argv[i], "-t") == 0 || strcmp(p->argv[i], "--theme") == 0) && i + 1 < p->args) {
            if (strcmp(p->argv[i + 1], "dark") == 0) {
                r->theme = Dark;
            } else if (strcmp(p->argv[i + 1], "light") == 0) {
                r->theme = Light;
            }
            i++;  // Skip the theme argument
        } else {
            fprintf(stderr, "Warning: Unknown option %s\n", p->argv[i]);
        }
    }

    return r;
}

void FreeResult(Result *r) {
    free(r);
}
