#include "coroutines.h"
#include <setjmp.h>
#include <stdlib.h>
#include <assert.h>

// Coroutine task representation

struct coroutine {
    enum {
        ST_CREATED,
        ST_RUNNING,
    }
    status;
    jmp_buf context;
    void (*routine)(void*);
    void* args;
    void* ss;
    void* sp;
    size_t ssize;
};

// Round-robin linked list implementation

struct rr_node {
    struct rr_node* prev;
    struct rr_node* next;
    struct coroutine crt;
};

struct {
    struct rr_node* current;
    jmp_buf context;
} rr;

static void rr_push_back(struct rr_node* node)
{
    struct rr_node* curr = rr.current;

    if(!curr){
        node->prev = node;
        node->next = node;
        rr.current = node;
    }
    else {
        node->prev = curr->prev;
        node->next = curr;
        curr->prev = node;
        node->prev->next = node;
    }
}

static void rr_pop_front(void)
{
    struct rr_node* curr = rr.current;
    assert(curr);
    struct rr_node* next = curr->next;
    struct rr_node* prev = curr->prev;

    if(curr == next){
        rr.current = NULL;
    }
    else {
        prev->next = next;
        next->prev = prev;
        rr.current = next;
    }
}

// Private implementation

enum {
    RT_INIT,
    RT_SHED,
    RT_EXIT,
};

static void rr_proc_coroutine()
{
    if(!rr.current){
        return;
    }

    rr.current = rr.current->next;

    struct rr_node* node = rr.current;
    struct coroutine* cr = &node->crt;

    if(cr->status == ST_CREATED){
        // switch stack
        register void* sp = cr->sp;

        asm volatile(
            "mov %[rs], %%rsp\n"
            : [rs] "+r" (sp) ::
        );

        cr->status = ST_RUNNING;
        cr->routine(cr->args);
        rr_coroutine_exit();
    }
    else {
        longjmp(cr->context, 1);
    }
}

static void rr_drop_coroutine()
{
    struct rr_node* node = rr.current;
    rr_pop_front();

    free(node->crt.ss);
    free(node);
}

// Interface implementation

void rr_loop()
{
    switch(setjmp(rr.context)){
        case RT_EXIT:
            rr_drop_coroutine();
            break;

        case RT_INIT:
        case RT_SHED:
            rr_proc_coroutine();
            break;

        default:
            assert(0);
    }
}

void rr_coroutine_create(void (*routine)(void*), void* args)
{
    struct rr_node* node = malloc(sizeof(*node));   // TODO: comp with NULL
    struct coroutine* cr = &node->crt;
    cr->status = ST_CREATED;
    cr->routine = routine;
    cr->args = args;
    cr->ssize = 4 * 4096;                           // TODO: prep COROUTINE_SSIZE
    cr->ss = malloc(cr->ssize);                     // TODO: comp with NULL
    cr->sp = cr->ss + cr->ssize;
    rr_push_back(node);
}

void rr_coroutine_yield()
{
    if(setjmp(rr.current->crt.context)){
        return;
    }
    else {
        longjmp(rr.context, RT_SHED);
    }
}

void rr_coroutine_exit()
{
    longjmp(rr.context, RT_EXIT);
}