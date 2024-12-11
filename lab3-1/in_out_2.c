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
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <input-pin-1> <input-pin-2> <output-pin-1> <output-pin-2>\n", argv[0]);
        goto cleanup1;
    }
    int input_pins[2], output_pins[2];
    input_pins[0] = atoi(argv[1]);
    input_pins[1] = atoi(argv[2]);
    output_pins[0] = atoi(argv[3]);
    output_pins[1] = atoi(argv[4]);
    if (gpio_export(input_pins[0]) == -1) {
        fprintf(stderr, "gpio_unexport: %s\n", strerror(errno));
        goto cleanup1;
    }
    if (gpio_set_direction(input_pins[0], DIRECTION_IN) == -1) {
        fprintf(stderr, "gpio_set_direction: %s\n", strerror(errno));
        goto cleanup2;
    }
    if (gpio_set_edge(input_pins[0], EDGE_BOTH) == -1) {
        fprintf(stderr, "gpio_set_edge: %s\n", strerror(errno));
        goto cleanup2;
    }
    if (gpio_export(input_pins[1]) == -1) {
        fprintf(stderr, "gpio_unexport: %s\n", strerror(errno));
        goto cleanup2;
    }
    if (gpio_set_direction(input_pins[1], DIRECTION_IN) == -1) {
        fprintf(stderr, "gpio_set_direction: %s\n", strerror(errno));
        goto cleanup3;
    }
    if (gpio_set_edge(input_pins[1], EDGE_BOTH) == -1) {
        fprintf(stderr, "gpio_set_edge: %s\n", strerror(errno));
        goto cleanup3;
    }
    if (gpio_export(output_pins[0]) == -1) {
        fprintf(stderr, "gpio_export: %s\n", strerror(errno));
        goto cleanup3;
    }
    if (gpio_set_direction(output_pins[0], DIRECTION_OUT) == -1) {
        fprintf(stderr, "gpio_set_direction: %s\n", strerror(errno));
        goto cleanup4;
    }
    if (gpio_export(output_pins[1]) == -1) {
        fprintf(stderr, "gpio_export: %s\n", strerror(errno));
        goto cleanup4;
    }
    if (gpio_set_direction(output_pins[1], DIRECTION_OUT) == -1) {
        fprintf(stderr, "gpio_set_direction: %s\n", strerror(errno));
        goto cleanup5;
    }
    char path[256];
    int input_fds[2];
    sprintf(path, "/sys/class/gpio/gpio%d/value", input_pins[0]);
    input_fds[0] = open(path, O_RDONLY);
    if (input_fds[0] == -1) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        goto cleanup5;
    }
    sprintf(path, "/sys/class/gpio/gpio%d/value", input_pins[1]);
    input_fds[1] = open(path, O_RDONLY);
    if (input_fds[1] == -1) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        goto cleanup6;
    }
    int output_fds[2];
    sprintf(path, "/sys/class/gpio/gpio%d/value", output_pins[0]);
    output_fds[0] = open(path, O_WRONLY);
    if (output_fds[0] == -1) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        goto cleanup7;
    }
    sprintf(path, "/sys/class/gpio/gpio%d/value", output_pins[1]);
    output_fds[1] = open(path, O_WRONLY);
    if (output_fds[1] == -1) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        goto cleanup8;
    }
    for (;;) {
        fd_set set;
        FD_ZERO(&set);
        int max_fd = 0;
        for (int i = 0; i < 2; ++i) {
            FD_SET(input_fds[i], &set);
            if (input_fds[i] > max_fd) {
                max_fd = input_fds[i];
            }
        }
        if (select(max_fd + 1, NULL, NULL, &set, NULL) == -1) {
            if (errno == EINTR) {
                break;
            }
            fprintf(stderr, "select: %s\n", strerror(errno));
            goto cleanup9;
        }
        for (int i = 0; i < 2; ++i) {
            if (FD_ISSET(input_fds[i], &set)) {
                if (lseek(input_fds[i], 0, SEEK_SET) == -1) {
                    fprintf(stderr, "lseek: %s\n", strerror(errno));
                    goto cleanup9;
                }
                char buf[2];
                if (read(input_fds[i], buf, 2) == -1) {
                    fprintf(stderr, "read: %s\n", strerror(errno));
                    goto cleanup9;
                }
                printf("gpio%d: %c\n", input_pins[i], buf[0]);
                if (write(output_fds[i], buf, 2) == -1) {
                    fprintf(stderr, "write: %s\n", strerror(errno));
                    goto cleanup9;
                }
            }
        }
    }

cleanup9:
    close(output_fds[1]);
cleanup8:
    close(output_fds[0]);
cleanup7:
    close(input_fds[1]);
cleanup6:
    close(input_fds[0]);
cleanup5:
    gpio_unexport(output_pins[1]);
cleanup4:
    gpio_unexport(output_pins[0]);
cleanup3:
    gpio_unexport(input_pins[1]);
cleanup2:
    gpio_unexport(input_pins[0]);
cleanup1:
    exit(1);
}
