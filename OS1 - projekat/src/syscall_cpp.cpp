//
// Created by os on 7/20/23.
//

#include "../h/syscall_cpp.hpp"
#include "../h/tcb.hpp"
#include "../h/syscall_c.h"
#include "../lib/console.h"

Thread::~Thread() { delete myHandle;};
Semaphore::~Semaphore() { sem_close(myHandle);};

int Semaphore::signal() {return sem_signal(myHandle);};

int Semaphore::wait() {return sem_wait(myHandle);}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

void Thread::wrapperFoo(void * threadArgs)
{
 if (threadArgs != nullptr) {
     ((Thread*)threadArgs)->run();

 }
 else
     return;
}

int Thread::sleep(time_t) {
    //printString("sleep \n");
     return 0;}

void Thread::dispatch() {thread_dispatch();}

void Thread::join() {thread_join(myHandle);}



Thread::Thread(void (*body)(void *) , void *arg) {

    uint64  ecallCode = 0x90;
    uint64 *myStack = new uint64[DEFAULT_STACK_SIZE];
    //if (!myStack)printString("Problem alokacije?");
    __asm__ volatile ("mv a4, %[pHolder]" : : [pHolder] "r" (myStack));
    __asm__ volatile ("mv a3, %[pHolder]" : : [pHolder] "r" (arg));
    __asm__ volatile ("mv a2, %[pHolder]" : : [pHolder] "r" (body));
    __asm__ volatile ("mv a1, %[pHolder]" : : [pHolder] "r" (&myHandle));
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));
    __asm__ volatile ("ecall");


}
int Thread::start() {
    uint64 ecallCode = 0x91;
    __asm__ volatile ("mv a1, %[pHolder]" : : [pHolder] "r" (&myHandle));
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));
    __asm__ volatile ("ecall");
    return 0; // TODO treba da vraca nesto drugo?
}



Thread::Thread() {
    uint64  ecallCode = 0x90; //TODO neki kod koji je slobodan
    uint64 *myStack =  (uint64*)(new uint64[DEFAULT_STACK_SIZE]);
   __asm__ volatile ("mv a4, %[pHolder]" : : [pHolder] "r" (myStack));
   __asm__ volatile ("mv a3, %[pHolder]" : : [pHolder] "r" ((Thread*)(this)));
   __asm__ volatile ("mv a2, %[pHolder]" : : [pHolder] "r" (wrapperFoo));
   __asm__ volatile ("mv a1, %[pHolder]" : : [pHolder] "r" (&myHandle));
   __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ecallCode));
   __asm__ volatile ("ecall");


}


char Console::getc() {
    char character = __getc();
    return character;
}

void Console::putc(char c) {
    // moze da se i razresenjem dosega pozve tvoj sistemski poziv iz syscall.c (dve dvotacke)
    __putc(c);
}