//
// Created by os on 7/12/23.
//
#include "../h/tcb.hpp"
#include "../h/riscv.h"
#include "../h/scheduler.hpp"


TCB* TCB::running = 0;
uint64 TCB::timeSliceCounter = 0;
uint64 TCB::idThread = 1;

void TCB::setState(TCB::ThreadState t) {
    this->threadState = t;
}
int TCB::createThread(TCB** threadHandle, Body body, void* args, uint64* stack, int flag) {

    TCB* tcb = (TCB*)MemoryAllocator::getInstance().mem_alloc(sizeof(TCB));

    if (!tcb) {//printString("Alokacije nije uspela\n");
         return -1;}

    tcb->context = {(uint64) &threadWrapper,
                    body != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0};


    //TODO razmisli, mozda si mogao i da ne delis, nego da uzmes i pozoves new uint64[def_stack_size] ali da
    // preklopljeni new ne zove sys call
    tcb->timeSlice = TIME_SLICE;
    if (body != nullptr)
        tcb->stack = stack;
    tcb->body = body;
    tcb->finished = false;
    tcb->args = args;
    if (flag == 0)
    {
        tcb->threadState = READY;
        if (body != nullptr){Scheduler::put(tcb);}
    }
    else if (flag == 1)
    {
        tcb->setState(TCB::ThreadState::CREATED);
    }
    tcb->myId = ++idThread;
    tcb->waits.setPointers();
    tcb->joinFlag = false;
    *threadHandle = tcb;

    return 0;
}


void TCB::dispatch() {

    TCB* old = running;
    if (!old->finished && running->threadState != BLOCKED && running->threadState != FINISHED) {
        Scheduler::put(old);
    }
    running = Scheduler::get();


  if (old != TCB::running)
      TCB::contextSwitch(&old->context, &running->context);



}

void TCB::threadWrapper() {


    static int temp = 0;
    if (!temp)
        Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
    else
        Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    temp++;

    Riscv::popSppSpie();
    running->body(running->args);
    if (TCB::running->joinFlag)
    {
        TCB* temp = TCB::running->waits.removeFirst();
        while (temp)
        {
            temp->setState(TCB::ThreadState::READY);
            Scheduler::put(temp);
            temp = TCB::running->waits.removeFirst();
        }
    }
    thread_exit();




}

void TCB::startThread(TCB *handle) {
    handle->setState(TCB::ThreadState::READY);

    Scheduler::put(handle);
}
void TCB::join(TCB *handle) {
    TCB::running->setState(TCB::ThreadState::BLOCKED);
    handle->waits.addLast(TCB::running);
    TCB::dispatch();
}


