#include "styles.h"
#include "colors.h"
#include "parse.h"
#include <stdlib.h>

Styles* NewStyles(enum Themes theme) {
    Styles *styles = malloc(sizeof(Styles));
    if (styles == NULL) {
        return NULL;
    }
    switch (theme) {
        case Default:
            styles->color = ANSI_COLOR_CYAN;
            styles->font = "█";
            styles->shape = 0;
            styles->matrix_effect = false;
            break;
        case Dark:
            styles->color = ANSI_COLOR_BLACK;
            styles->font = "█";
            styles->shape = 0;
            styles->matrix_effect = false;
            break;
        case Light:
            styles->color = ANSI_COLOR_WHITE;
            styles->font = "█";
            styles->shape = 0;
            styles->matrix_effect = false;
            break;
        case Matrix:
            styles->color = ANSI_COLOR_GREEN;
            styles->font = "█";
            styles->shape = 0;
            styles->matrix_effect = true;
            break;
    }
    return styles;
}

void FreeStyles(Styles *s) {
    free(s);
}
