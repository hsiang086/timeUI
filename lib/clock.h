#pragma once

#include "styles.h"
#include <sys/ioctl.h>
#include <stdbool.h>
#include <time.h>

extern const char* large_digits[10][5];
extern const char* large_colon[5];

typedef struct {
    int x;
    int y;
} Pos;

typedef struct {
    struct timespec last_frame_time;
    long frame_duration;
    double current_fps;
} FPSController;

typedef struct {
    Styles *styles;
    Pos *pos;
    struct winsize w;
    FPSController fps_controller;
} Clock;

Pos* NewPos(void);
void UpdatePos(Pos *p, const struct winsize *w);
void FreePos(Pos *p);

Clock* NewClock(Styles *s);
bool UpdateClock(Clock *c);
void PrintClock(const Clock *c);
void FreeClock(Clock *c);

double GetFPS(const Clock *c);
void initializeStreams(const struct winsize *w);
