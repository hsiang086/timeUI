#include "clock.h"
#include "styles.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <string.h>

const char* large_digits[10][5] = {
    {"█████", "█   █", "█   █", "█   █", "█████"},
    {"  █  ", " ██  ", "  █  ", "  █  ", "█████"},
    {"█████", "    █", "█████", "█    ", "█████"},
    {"█████", "    █", "█████", "    █", "█████"},
    {"█   █", "█   █", "█████", "    █", "    █"},
    {"█████", "█    ", "█████", "    █", "█████"},
    {"█████", "█    ", "█████", "█   █", "█████"},
    {"█████", "    █", "    █", "    █", "    █"},
    {"█████", "█   █", "█████", "█   █", "█████"},
    {"█████", "█   █", "█████", "    █", "█████"}
};

const char* large_colon[5] = {"   ", " █ ", "   ", " █ ", "   "};

#define MAX_STREAMS 100
#define STREAM_MAX_LENGTH 20
#define TARGET_FPS 30
#define NSEC_PER_SEC 1000000000

typedef struct {
    int x;
    int y;
    int length;
    int speed;
    char letters[STREAM_MAX_LENGTH];
} Stream;

Stream streams[MAX_STREAMS];

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

void initializeStreams(const struct winsize *w) {
    srand(time(NULL));
    for (int i = 0; i < MAX_STREAMS; i++) {
        streams[i].x = rand() % w->ws_col;
        streams[i].y = -(rand() % w->ws_row);
        streams[i].length = rand() % (STREAM_MAX_LENGTH - 5) + 5;
        streams[i].speed = rand() % 3 + 1;
        for (int j = 0; j < streams[i].length; j++) {
            streams[i].letters[j] = rand() % 26 + 'A';
        }
    }
}

void updateAndPrintStreams(const Clock *c) {
    char *screen = calloc(c->w.ws_row * c->w.ws_col, sizeof(char));
    if (screen == NULL) {
        fprintf(stderr, "Failed to allocate memory for screen buffer\n");
        return;
    }

    for (int i = 0; i < MAX_STREAMS; i++) {
        streams[i].y += streams[i].speed;

        if (streams[i].y - streams[i].length > c->w.ws_row) {
            streams[i].y = 0;
            streams[i].x = rand() % c->w.ws_col;
        }

        for (int j = 0; j < streams[i].length; j++) {
            int y = streams[i].y - j;
            if (y >= 0 && y < c->w.ws_row) {
                int index = y * c->w.ws_col + streams[i].x;
                if (index >= 0 && index < c->w.ws_row * c->w.ws_col) {
                    screen[index] = streams[i].letters[j];
                }
            }
        }

        streams[i].letters[streams[i].length - 1] = rand() % 26 + 'A';
    }

    for (int i = 0; i < c->w.ws_row; i++) {
        for (int j = 0; j < c->w.ws_col; j++) {
            int index = i * c->w.ws_col + j;
            if (screen[index] != 0) {
                printf("\033[%d;%dH%c", i + 1, j + 1, screen[index]);
            }
        }
    }

    free(screen);
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

    if (clock->styles->matrix_effect) {
        initializeStreams(&clock->w);
    }

    clock_gettime(CLOCK_MONOTONIC, &clock->fps_controller.last_frame_time);
    clock->fps_controller.frame_duration = NSEC_PER_SEC / TARGET_FPS;
    clock->fps_controller.current_fps = TARGET_FPS;

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

    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    long elapsed = (current_time.tv_sec - c->fps_controller.last_frame_time.tv_sec) * NSEC_PER_SEC +
                   (current_time.tv_nsec - c->fps_controller.last_frame_time.tv_nsec);

    if (elapsed < c->fps_controller.frame_duration) {
        struct timespec sleep_time = {
            .tv_sec = 0,
            .tv_nsec = c->fps_controller.frame_duration - elapsed
        };
        nanosleep(&sleep_time, NULL);
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        elapsed = c->fps_controller.frame_duration;
    }

    c->fps_controller.current_fps = (double)NSEC_PER_SEC / elapsed;

    c->fps_controller.last_frame_time = current_time;

    return true;
}

double GetFPS(const Clock *c) {
    return c->fps_controller.current_fps;
}

void PrintClock(const Clock *c) {
    if (c == NULL || c->styles == NULL || c->pos == NULL) {
        return;
    }
    time_t now;
    struct tm *tm_info;
    char date_str[11];
    char time_str[7];

    time(&now);
    tm_info = localtime(&now);
    if (tm_info == NULL) {
        fprintf(stderr, "Failed to get local time\n");
        return;
    }
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", tm_info);
    strftime(time_str, sizeof(time_str), "%H%M%S", tm_info);

    printf("\033[2J");
    printf("\033[H");
    printf("%s", c->styles->color);

    if (c->styles->matrix_effect) {
        updateAndPrintStreams(c);
    }

    printf("\033[%d;%dH%s", c->pos->y - 7, c->pos->x - 5, date_str);

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

    char fps_str[20];
    snprintf(fps_str, sizeof(fps_str), "FPS: %.2f", c->fps_controller.current_fps);
    printf("\033[%d;%luH%s", c->w.ws_row, c->w.ws_col - strlen(fps_str), fps_str);

    printf("\033[0m");
    fflush(stdout);
}

void FreeClock(Clock *c) {
    if (c != NULL) {
        FreePos(c->pos);
        free(c);
    }
}
