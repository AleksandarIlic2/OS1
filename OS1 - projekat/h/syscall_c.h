//
// Created by os on 7/10/23.
//

#ifndef PROJECT_BASE_V1_1_1_SYSCALL_C_H
#define PROJECT_BASE_V1_1_1_SYSCALL_C_H
#include "../lib/hw.h"
#include "../h/tcb.hpp"
#include "../h/_sem.hpp"



typedef TCB* thread_t; // rucka iz teksta projekta

typedef unsigned long time_t;

void* mem_alloc(size_t size);

int mem_free(void*);

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg);



void  thread_dispatch();

void thread_join(thread_t handle);

int thread_exit();

class _sem;

typedef _sem*  sem_t;

int sem_open(sem_t* handle, unsigned init);

int sem_close(sem_t handle);


int sem_wait(sem_t id);

int sem_signal(sem_t id);

char getc();

void putc(char);

int time_sleep(time_t t);
#endif //PROJECT_BASE_V1_1_1_SYSCALL_C_H
