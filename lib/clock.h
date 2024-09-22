#pragma once

#include "styles.h"
#include <sys/ioctl.h>
#include <stdbool.h>

extern const char* large_digits[10][5];

typedef struct {
    int x;
    int y;
} Pos;

Pos* NewPos(void);
void UpdatePos(Pos *p, const struct winsize *w);
void FreePos(Pos *p);

typedef struct {
    Styles *styles;
    Pos *pos;
    struct winsize w;
} Clock;

Clock* NewClock(Styles *s);
bool UpdateClock(Clock *c);
void PrintClock(const Clock *c);
void FreeClock(Clock *c);
