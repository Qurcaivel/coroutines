#include <stdio.h>
#include <stdlib.h>

#include "coroutines.h"

struct tester_args {
    char* name;
    int iters;
};

void tester(void* args)
{
    int i;
    struct tester_args* ta = args;

    for(i = 0; i < ta->iters; i++){
        printf("running task %s: %d\n", ta->name, i);
        rr_coroutine_yield();
    }

    free(ta);
}

void create_test_task(char *name, int iters)
{
    struct tester_args* ta = malloc(sizeof(*ta));
    ta->name = name;
    ta->iters = iters;
    rr_coroutine_create(tester, ta);
}

int main(int argc, char** argv)
{
    create_test_task("first", 10);
    create_test_task("second", 5);
    rr_loop();
    printf("Finished running all tasks!\n");
}