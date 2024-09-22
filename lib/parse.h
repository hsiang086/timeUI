#pragma once

#include <stdbool.h>

enum Themes {
    Default,
    Dark,
    Light,
    Matrix
};

typedef struct {
    int args;
    char **argv;
} Parser;

Parser* NewParser(int args, char **argv);
void FreeParser(Parser *p);

typedef struct {
    bool help;
    enum Themes theme;
} Result;

Result* Parse(Parser *p);
void FreeResult(Result *r);
