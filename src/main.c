#include "clock.h"
#include "parse.h"
#include "styles.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

volatile sig_atomic_t keep_running = true;

void signalHandler(int signal) {
    keep_running = false;
}

void exitHandler(void) {
    printf("\033[H\033[J");
    printf("Ending program...\n");
    printf("\033[?25h");
    fflush(stdout);
}

void hideCursor(void) {
    printf("\033[?25l");
    fflush(stdout);
}

int main(int argc, char **argv) {
    Parser *p = NewParser(argc, argv);
    if (p == NULL) {
        fprintf(stderr, "Failed to create parser\n");
        return 1;
    }

    Result *r = Parse(p);
    if (r == NULL) {
        fprintf(stderr, "Failed to parse arguments\n");
        FreeParser(p);
        return 1;
    }

    if (r->help) {
        printf("Usage: %s [options]\n", argv[0]);
        printf("Options:\n");
        printf("  -h, --help        Show this help message\n");
        printf("  -t, --theme TYPE  Set theme (dark/light/matrix)\n");
        FreeParser(p);
        FreeResult(r);
        return 0;
    }

    Styles *s = NewStyles(r->theme);
    if (s == NULL) {
        fprintf(stderr, "Failed to create styles\n");
        FreeParser(p);
        FreeResult(r);
        return 1;
    }

    Clock *c = NewClock(s);
    if (c == NULL) {
        fprintf(stderr, "Failed to create clock\n");
        FreeParser(p);
        FreeResult(r);
        FreeStyles(s);
        return 1;
    }

    signal(SIGINT, signalHandler);

    hideCursor();
    atexit(exitHandler);

    while (keep_running) {
        if (!UpdateClock(c)) {
            fprintf(stderr, "Failed to update clock\n");
            break;
        }
        PrintClock(c);
    }

    FreeParser(p);
    FreeResult(r);
    FreeStyles(s);
    FreeClock(c);

    return 0;
}
