#include "coroutines.h"
#include <setjmp.h>
#include <stdlib.h>

struct coroutine {
    enum {
        ST_CREATED,
        ST_RUNNING,
        ST_WAITING,
    }
    status;
    int id;
    jmp_buf context;
    void (*routine)(void*);
    void* args;
    void* ss;
    void* sp;
    size_t ssize;
};

struct rr_node {
    struct rr_node* prev;
    struct rr_node* next;
    struct coroutine crt;
};

struct rr_list {
    struct rr_node* head;
    struct rr_node* tail;
};

struct {
    struct rr_list coroutines;
    jmp_buf context;
    struct rr_node* current;
} rr;

static void rr_push_back(struct rr_node* node)      // TODO: make unit-tests
{
    struct rr_node* head = rr.coroutines.head;
    struct rr_node* tail = rr.coroutines.tail;

    if(head){
        head->prev = node;
        tail->next = node;
    }
    else {
        head = node;
        tail = node;
        rr.coroutines.head = node;
    }

    node->prev = tail;
    node->next = head;
    rr.coroutines.tail = node;
}

static void rr_drop_node(struct rr_node* node)      // TODO: make unit-tests
{
    struct rr_node* head = rr.coroutines.head;      // TODO: replace with macro insert
    struct rr_node* tail = rr.coroutines.tail;      // TODO: replace with macro insert

    if(head == tail){                               // TODO: ensure head == node
        rr.coroutines.head = NULL;
        rr.coroutines.tail = NULL;
    }
    else if(node == head){
        rr.coroutines.head = node->next;
    }
    else if(node == tail){
        rr.coroutines.tail = node->prev;
    }

    (node->next)->prev = (node->prev);
    (node->prev)->next = (node->next);
    free(node);
}