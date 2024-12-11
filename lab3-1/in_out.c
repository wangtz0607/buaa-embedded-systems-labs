#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>
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
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input-pin> <output-pin>\n", argv[0]);
        goto cleanup1;
    }
    int input_pin = atoi(argv[1]);
    int output_pin = atoi(argv[2]);
    if (gpio_export(input_pin) == -1) {
        fprintf(stderr, "gpio_export: %s\n", strerror(errno));
        goto cleanup1;
    }
    if (gpio_set_direction(input_pin, DIRECTION_IN) == -1) {
        fprintf(stderr, "gpio_set_direction: %s\n", strerror(errno));
        goto cleanup2;
    }
    if (gpio_set_edge(input_pin, EDGE_BOTH) == -1) {
        fprintf(stderr, "gpio_set_edge: %s\n", strerror(errno));
        goto cleanup2;
    }
    if (gpio_export(output_pin) == -1) {
        fprintf(stderr, "gpio_export: %s\n", strerror(errno));
        goto cleanup2;
    }
    if (gpio_set_direction(output_pin, DIRECTION_OUT) == -1) {
        fprintf(stderr, "gpio_set_direction: %s\n", strerror(errno));
        goto cleanup3;
    }
    char path[256];
    sprintf(path, "/sys/class/gpio/gpio%d/value", input_pin);
    int input_fd = open(path, O_RDONLY);
    if (input_fd == -1) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        goto cleanup3;
    }
    sprintf(path, "/sys/class/gpio/gpio%d/value", output_pin);
    int output_fd = open(path, O_WRONLY);
    if (output_fd == -1) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        goto cleanup4;
    }
    for (;;) {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(input_fd, &set);
        if (select(input_fd + 1, NULL, NULL, &set, NULL) == -1) {
            if (errno == EINTR) break;
            fprintf(stderr, "select: %s\n", strerror(errno));
            goto cleanup5;
        }
        if (FD_ISSET(input_fd, &set)) {
            if (lseek(input_fd, 0, SEEK_SET) == -1) {
                fprintf(stderr, "lseek: %s\n", strerror(errno));
                goto cleanup5;
            }
            char buf[2];
            if (read(input_fd, buf, 2) == -1) {
                fprintf(stderr, "read: %s\n", strerror(errno));
                goto cleanup5;
            }
            printf("gpio%d: %c\n", input_pin, buf[0]);
            if (write(output_fd, buf, 2) == -1) {
                fprintf(stderr, "write: %s\n", strerror(errno));
                goto cleanup5;
            }
        }
    }
cleanup5:
    close(output_fd);
cleanup4:
    close(input_fd);
cleanup3:
    gpio_unexport(output_pin);
cleanup2:
    gpio_unexport(input_pin);
cleanup1:
    exit(1);
}
