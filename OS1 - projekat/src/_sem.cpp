//
// Created by os on 7/19/23.
//
#include "../h/_sem.hpp"

int _sem::sem_open(_sem **sem, unsigned int init) { // nema this, ovo je staticka tj. usluzna metoda;
    MemoryAllocator m1 = MemoryAllocator::getInstance();
    _sem* mySem = (_sem*)m1.mem_alloc(sizeof(_sem));
    if (mySem == nullptr) return -1;

    mySem->val = init;
    mySem->deallocated = false;
    mySem->blockedThreads.setPointers();
    *sem = mySem;
    return 0;

}

int _sem::sem_close(_sem *sem) {
    if (!sem || sem->deallocated) return -1;
    TCB* cur = sem->blockedThreads.removeFirst();
    while (cur != nullptr)
    {
        cur->setState(TCB::ThreadState::READY);
        Scheduler::put(cur);
        cur = sem->blockedThreads.removeFirst();
    }
    int pom = MemoryAllocator::getInstance().mem_free(sem);
    sem->deallocated = true;
    return pom;
}

int _sem::sem_wait(_sem *sem) {

    if (sem == nullptr || sem->deallocated) return -1;
    sem->val = sem->val - 1;

    if ((sem->val) < 0)
    {
        Riscv::pushRegisters();
        TCB::running->setState(TCB::ThreadState::BLOCKED);
        sem->blockedThreads.addLast(TCB::running);

        TCB::dispatch();//TODO-razmisljanja kad god zoves dispatch odnekud, velike su sanse da ce ti trebati pushRegisters i
        //TODO popRegisters, neka optimizacija se radi neki registri se privremeni (ti..) koriste i moras da ih sacuvas zbog
        // toga pre promene konteksta izgleda
        Riscv::popRegisters();
    }
    //TODO return -1 kad je neuspeh/zatvoren semafor
    return 0;

}
int _sem::sem_signal(_sem *sem) {
    if (sem == nullptr || sem->deallocated) return -1;
    sem->val = sem->val + 1;
    if ((sem->val) <= 0)
    {
        TCB* temp = sem->blockedThreads.removeFirst();
        temp->setState(TCB::ThreadState::READY);
        Scheduler::put(temp);
    }
    return 0;  //TODO return -1 kad je neuspeh/zatvoren semafor
}

