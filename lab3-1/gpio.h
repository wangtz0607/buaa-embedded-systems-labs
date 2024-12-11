#pragma once

enum Direction {
    DIRECTION_IN,
    DIRECTION_OUT,
};

enum Edge {
    EDGE_NONE,
    EDGE_RISING,
    EDGE_FALLING,
    EDGE_BOTH,
};

int gpio_export(int pin);
int gpio_unexport(int pin);
int gpio_set_direction(int pin, enum Direction direction);
int gpio_set_edge(int pin, enum Edge edge);
