//
// Created by os on 7/19/23.
//

#ifndef PROJECT_BASE_V1_1_1__SEM_HPP
#define PROJECT_BASE_V1_1_1__SEM_HPP
#include "../h/MemoryAllocator.h"

#include "../h/tcb.hpp"
#include "../h/riscv.h"



class _sem {

public:

    static int sem_open(_sem** sem, unsigned init);

    static int sem_wait(_sem* sem);

    static int sem_signal(_sem* sem);

    static int sem_close(_sem* sem);

    int getVal() const {return val;}



private:

    int val;
    bool deallocated = false;
    List<TCB> blockedThreads;

};


#endif //PROJECT_BASE_V1_1_1__SEM_HPP
