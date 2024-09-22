#pragma once

#include <stdlib.h>
#include "colors.h"
#include "parse.h"

typedef struct {
    char *color;
    char *font;
    int shape;
} Styles;

Styles* NewStyles(enum Themes theme);
void FreeStyles(Styles *s);
