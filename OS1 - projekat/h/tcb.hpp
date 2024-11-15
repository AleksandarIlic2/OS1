//
// Created by os on 7/12/23.
//

#ifndef PROJECT_BASE_V1_1_1_TCB_HPP
#define PROJECT_BASE_V1_1_1_TCB_HPP
#include "../h/scheduler.hpp"
#include "../lib/hw.h"
#include "../h/MemoryAllocator.h"
#include "../h/syscall_c.h"
class TCB {
public:
    ~TCB() {delete stack;}
    using Body = void (*)(void*);
    static int createThread(TCB**,Body body, void* args, uint64* stack, int flag = 0);
    bool isFinished() const { return finished; }
    void setFinished(bool fin) {finished = fin;}
    uint64 getTimeSlice() const { return timeSlice;}

    static TCB* running;
    static void dispatch();
    static void join(TCB* handle);
    static uint64 idThread;
    uint64 myId ; //TODO getter/setter
    typedef enum ThreadState {CREATED, READY, BLOCKED, FINISHED} ThreadState;
    void setState(ThreadState t);
    uint64 getThreadId() const {return myId;}
    void setThreadId(uint64 id) {this->myId = id;}
    void setArgs(void* args){this->args = args;}
    static void startThread(TCB* handle);
private:


    friend class Thread;
     TCB(Body body, void* args, uint64* stack)
    {
    }



    struct Context{
        uint64 ra;
        uint64 sp;
    };
    Body body;
    uint64* stack;
    Context context;
    uint64 timeSlice;
    bool finished;
    void* args;
    friend class Riscv;
    ThreadState threadState;
    List<TCB> waits;
    bool joinFlag = false;
    static void threadWrapper();

    static void contextSwitch(Context* oldContext, Context* newContext);



    static uint64 timeSliceCounter;


    static uint64 constexpr TIME_SLICE = 2;
};


#endif //PROJECT_BASE_V1_1_1_TCB_HPP
