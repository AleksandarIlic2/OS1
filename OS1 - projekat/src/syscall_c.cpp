//
// Created by os on 7/10/23.
//
#include "../h/syscall_c.h"

#include "../h/MemoryAllocator.h"

void* mem_alloc(size_t size)
{

   uint64 temp = size + sizeof(size_t) + sizeof(uint64*);
   uint64 numOfBlocks = (temp / MEM_BLOCK_SIZE + (temp % MEM_BLOCK_SIZE ? 1 : 0));
    uint64 ecallCode = 0x01;
    __asm__ volatile ("mv a1, %[pHolder] " : : [pHolder] "r" (numOfBlocks));
    __asm__ volatile("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));

   __asm__ volatile ("ecall");
   void* retValue;
    __asm__ volatile ("mv a0, %[pHolder]" : [pHolder] "=r" (retValue));

    return retValue;

}

int mem_free(void* ptr)
{
    uint64 ecallCode = 0x02;
    __asm__ volatile ("mv a1, %[pHolder]" : : [pHolder] "r" (ptr));
    __asm__ volatile ("mv a0, %[pHolder] " : : [pHolder] "r" (ecallCode));
    __asm__ volatile ("ecall");
    int retValue;
    __asm__ volatile ("mv %[pHolder], a0  " : [pHolder] "=r" (retValue));

    return retValue;

}

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg)
{

    uint64 ecallCode = 0x11;
   // uint64 stackSize = (DEFAULT_STACK_SIZE   / MEM_BLOCK_SIZE + ((DEFAULT_STACK_SIZE  % MEM_BLOCK_SIZE) ? 1 : 0)) *
                    //   MEM_BLOCK_SIZE;
    uint64 *myStack =  (uint64*)(new uint64[DEFAULT_STACK_SIZE]);

    __asm__ volatile ("mv a4, %[pHolder]" : : [pHolder] "r" (myStack));
    __asm__ volatile ("mv a3, %[pHolder]" : : [pHolder] "r" (arg));
    __asm__ volatile ("mv a2, %[pHolder]" : : [pHolder] "r" (start_routine));
    __asm__ volatile ("mv a1, %[pHolder]" : : [pHolder] "r" (handle));
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));

    __asm__ volatile ("ecall");
    int retValue;
    __asm__ volatile ("mv %[pHolder], a0" : [pHolder] "=r" (retValue));
    return retValue;
}



void  thread_dispatch(){

    uint64 ecallCode = 0x13;
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));

    __asm__ volatile ("ecall");

}

int thread_exit()
{
    uint64 ecallCode = 0x12;
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));
    __asm__ volatile ("ecall");
    uint64 zero = 0;
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (zero));
    return zero;
}

void thread_join(thread_t handle)
{
    uint64 ecallCode = 0x14;
    __asm__ volatile ("mv a1, %[pHolder]" : : [pHolder] "r" (handle));
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));
    __asm__ volatile ("ecall");

}

int sem_open(sem_t* handle, unsigned init)
{
    uint64 ecallCode = 0x21;
    __asm__ volatile ("mv a2, %[pHolder]" : : [pHolder] "r" (init));
    __asm__ volatile ("mv a1, %[pHolder]" : : [pHolder] "r" (handle));
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));
    __asm__ volatile ("ecall");

    int retVal;
    __asm__ volatile ("mv %[pHolder], a0" : [pHolder] "=r" (retVal));
    return retVal;


}

int sem_close(sem_t handle)
{
    uint64 ecallCode = 0x22;
    __asm__ volatile ("mv a1, %[pHolder]" : : [pHolder] "r" (handle));
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));
    __asm__ volatile ("ecall");

    int retVal;
    __asm__ volatile ("mv %[pHolder], a0" : [pHolder] "=r" (retVal));
    return retVal;

}

int sem_wait(sem_t id)
{
    uint64 ecallCode = 0x23;
    __asm__ volatile ("mv a1, %[pHolder]" : : [pHolder] "r" (id));
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));
    __asm__ volatile ("ecall");

    int retVal;
    __asm__ volatile ("mv %[pHolder], a0" : [pHolder] "=r" (retVal));
    return retVal;


}

int sem_signal(sem_t id)
{
    uint64 ecallCode = 0x24;
    __asm__ volatile ("mv a1, %[pHolder]" : : [pHolder] "r" (id));
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));
    __asm__ volatile ("ecall");

    int retVal;
    __asm__ volatile ("mv %[pHolder], a0" : [pHolder] "=r" (retVal));
    return retVal;


}

char getc()
{
    uint64 ecallCode = 0x41;
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));
    __asm__ volatile ("ecall");

    char retChar;
    __asm__ volatile ("mv %[pHolder], a0" : [pHolder] "=r" (retChar));
    return retChar;
}

void putc(char c)
{
    uint64 ecallCode = 0x42;
    __asm__ volatile ("mv a1, %[pHolder]" : : [pHolder] "r" (c));
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));
    __asm__ volatile ("ecall");
}

int time_sleep(time_t t){
    //printString("sleep TODO\n");
    return 0;

}