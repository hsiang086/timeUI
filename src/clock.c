#include "clock.h"
#include "styles.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>

const char* large_digits[10][5] = {
    {"█████", "█   █", "█   █", "█   █", "█████"}, // 0
    {"  █  ", " ██  ", "  █  ", "  █  ", "█████"}, // 1
    {"█████", "    █", "█████", "█    ", "█████"}, // 2
    {"█████", "    █", "█████", "    █", "█████"}, // 3
    {"█   █", "█   █", "█████", "    █", "    █"}, // 4
    {"█████", "█    ", "█████", "    █", "█████"}, // 5
    {"█████", "█    ", "█████", "█   █", "█████"}, // 6
    {"█████", "    █", "    █", "    █", "    █"}, // 7
    {"█████", "█   █", "█████", "█   █", "█████"}, // 8
    {"█████", "█   █", "█████", "    █", "█████"}  // 9
};

const char* large_colon[5] = {"   ", " █ ", "   ", " █ ", "   "};

Pos* NewPos(void) {
    Pos *p = malloc(sizeof(Pos));
    if (p == NULL) {
        return NULL;
    }
    p->x = 0;
    p->y = 0;
    return p;
}

void UpdatePos(Pos *p, const struct winsize *w) {
    if (p == NULL || w == NULL) {
        return;
    }
    p->x = w->ws_col / 2;
    p->y = w->ws_row / 2;
}

void FreePos(Pos *p) {
    free(p);
}

Clock* NewClock(Styles *s) {
    if (s == NULL) {
        return NULL;
    }
    Clock *clock = malloc(sizeof(Clock));
    if (clock == NULL) {
        return NULL;
    }
    clock->styles = s;
    clock->pos = NewPos();
    if (clock->pos == NULL) {
        free(clock);
        return NULL;
    }
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &clock->w) == -1) {
        FreePos(clock->pos);
        free(clock);
        return NULL;
    }
    return clock;
}

bool UpdateClock(Clock *c) {
    if (c == NULL) {
        return false;
    }
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &c->w) == -1) {
        return false;
    }
    UpdatePos(c->pos, &c->w);
    return true;
}


void PrintClock(const Clock *c) {
    if (c == NULL || c->styles == NULL || c->pos == NULL) {
        return;
    }
    time_t now;
    struct tm *tm_info;
    char date_str[11];  // YYYY-MM-DD\0
    char time_str[7];   // HHMMSS\0 (without colons)

    time(&now);
    tm_info = localtime(&now);
    if (tm_info == NULL) {
        fprintf(stderr, "Failed to get local time\n");
        return;
    }
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", tm_info);
    strftime(time_str, sizeof(time_str), "%H%M%S", tm_info);

    // Clear screen and set text color
    printf("\033[H\033[J");
    printf("%s", c->styles->color);

    // Print date
    printf("\033[%d;%dH%s", c->pos->y - 7, c->pos->x - 5, date_str);

    // Print large digital clock
    for (int row = 0; row < 5; row++) {
        printf("\033[%d;%dH", c->pos->y - 5 + row, c->pos->x - 17);
        for (int i = 0; i < 8; i++) {
            if (i == 2 || i == 5) {
                printf("%s ", large_colon[row]);
            } else {
                int index = i;
                if (i > 5) index -= 2;
                else if (i > 2) index -= 1;
                
                int digit = time_str[index] - '0';
                if (digit < 0 || digit > 9) {
                    fprintf(stderr, "Invalid digit: %d at index %d\n", digit, index);
                    continue;
                }
                printf("%s ", large_digits[digit][row]);
            }
        }
        printf("\n");
    }

    // Reset text color
    printf("\033[0m");
    fflush(stdout);
}

void FreeClock(Clock *c) {
    if (c != NULL) {
        FreePos(c->pos);
        free(c);
    }
}
