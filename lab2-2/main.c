#include <stdint.h>
#include <stdio.h>

#include <error.h>
#include <pthread.h>

void *routine(void *arg) {
    int i = (int)(intptr_t)arg;
    printf("Thread %d\n", i);
    return NULL;
}

int main(void) {
    int ret;
    pthread_t threads[5];
    for (int i = 0; i < 5; ++i) {
        printf("main: Creating thread %d\n", i);
        if ((ret = pthread_create(&threads[i], NULL, routine, (void *)(intptr_t)i)) != 0) {
            error(1, ret, "pthread_create");
        }
    }
    printf("main: Exiting\n");
    pthread_exit(NULL);
}
