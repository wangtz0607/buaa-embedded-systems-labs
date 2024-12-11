#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include "gpio.h"

void handler(int signum) {
    (void)signum;
}

int main(int argc, char *argv[]) {
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "sigaction: %s\n", strerror(errno));
        goto cleanup1;
    }
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pins>...\n", argv[0]);
        goto cleanup1;
    }
    int n = argc - 1;
    int pins[64];
    for (int i = 0; i < n; ++i) {
        pins[i] = atoi(argv[i + 1]);
    }
    for (int i = 0; i < n; ++i) {
        if (gpio_export(pins[i]) == -1) {
            fprintf(stderr, "gpio_export: %s\n", strerror(errno));
            goto cleanup2;
        }
    }
    int fds[64];
    char path[256];
    for (int i = 0; i < n; ++i) {
        sprintf(path, "/sys/class/gpio/gpio%d/value", pins[i]);
        fds[i] = open(path, O_WRONLY);
        if (fds[i] == -1) {
            fprintf(stderr, "open: %s\n", strerror(errno));
            goto cleanup3;
        }
    }
    for (int i = 0;; ++i) {
        for (int j = 0; j < n; ++j) {
            if (write(fds[j], "1", 1) == -1) {
                fprintf(stderr, "write: %s\n", strerror(errno));
                goto cleanup3;
            }
        }
        for (int j = 0; j < 2; ++j) {
            if (write(fds[i % n], "0", 1) == -1) {
                fprintf(stderr, "write: %s\n", strerror(errno));
                goto cleanup3;
            }
            if (usleep(250000) == -1) goto cleanup3;
            if (write(fds[i % n], "1", 1) == -1) {
                fprintf(stderr, "write: %s\n", strerror(errno));
                goto cleanup3;
            }
            if (usleep(250000) == -1) goto cleanup3;
        }
    }
cleanup3:
    for (int i = 0; i < n; ++i) {
        close(fds[i]);
    }
cleanup2:
    for (int i = 0; i < n; ++i) {
        gpio_unexport(pins[i]);
    }
cleanup1:
    exit(1);
}
