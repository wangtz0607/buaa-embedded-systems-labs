#include "gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <unistd.h>

int gpio_export(int pin) {
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1) {
        return -1;
    }
    char buf[16];
    sprintf(buf, "%d", pin);
    if (write(fd, buf, sizeof(buf)) == -1) {
        return -1;
    }
    close(fd);
    return 0;
}

int gpio_unexport(int pin) {
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1) {
        return -1;
    }
    char buf[16];
    sprintf(buf, "%d", pin);
    if (write(fd, buf, sizeof(buf)) == -1) {
        return -1;
    }
    close(fd);
    return 0;
}

int gpio_set_direction(int pin, enum Direction direction) {
    char path[256];
    sprintf(path, "/sys/class/gpio/gpio%d/direction", pin);
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        return -1;
    }
    const char *buf;
    switch (direction) {
        case DIRECTION_IN: buf = "in"; break;
        case DIRECTION_OUT: buf = "out"; break;
        default: abort();
    }
    if (write(fd, buf, strlen(buf)) == -1) {
        return -1;
    }
    close(fd);
    return 0;
}

int gpio_set_edge(int pin, enum Edge edge) {
    char path[256];
    sprintf(path, "/sys/class/gpio/gpio%d/edge", pin);
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        return -1;
    }
    const char *buf;
    switch (edge) {
        case EDGE_NONE: buf = "none"; break;
        case EDGE_RISING: buf = "rising"; break;
        case EDGE_FALLING: buf = "falling"; break;
        case EDGE_BOTH: buf = "both"; break;
        default: abort();
    }
    if (write(fd, buf, strlen(buf)) == -1) {
        return -1;
    }
    close(fd);
    return 0;
}
