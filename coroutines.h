#pragma once

// Round-robin coroutine engine with setjmp/longjmp and unsafe stacks.

void rr_loop();

int rr_coroutine_create(void (*fun)(void*), void* args);
void rr_coroutine_yield(void);
void rr_coroutine_exit(void);