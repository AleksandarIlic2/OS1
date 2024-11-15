//
// Created by os on 6/26/23.
//

#include "../h/MemoryAllocator.h"

#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"
#include "../h/riscv.h"




void userMain();
extern void backToSystem();
extern void backToUser();



void wrapperFoo(void* a)
{
    userMain();
}


int main()
{

    Riscv::w_stvec((uint64) &Riscv::ivt | 1);

    TCB* mainThread = nullptr;
    thread_create(&mainThread, nullptr, nullptr);
    TCB::running = mainThread;

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);


    Thread* t1 = new Thread(wrapperFoo, nullptr);
    t1->start();
    t1->join();

    return 0;
}