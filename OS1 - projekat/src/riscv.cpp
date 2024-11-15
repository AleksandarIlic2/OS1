//
// Created by os on 7/11/23.
//

#include "../h/riscv.h"

#include "../h/MemoryAllocator.h"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../h/_sem.hpp"


void Riscv::backToSystem()
{
    uint64 code = 0x92;
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (code));
    __asm__ volatile ("ecall");
}

void Riscv::backToUser()
{
    uint64 code = 0x93;
    __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (code));
    __asm__ volatile ("ecall");
}


void Riscv::popSppSpie() {


    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret"); // samo sret moze da popuje te bite i da poniste ono dato ulaskom u prekidnu rutinu
}


void Riscv::handleSupervisorTrap() {
    long ecallCode;
    __asm__ volatile ("mv %[pHolder], a0 " : [pHolder] "=r" (ecallCode));
    MemoryAllocator m1 = MemoryAllocator::getInstance();
    uint64 volatile sstatusVar = r_sstatus();
    uint64 scause = r_scause();
    uint64 volatile sepcVar;
    __asm__ volatile ("csrr %[pHolder], sepc" : [pHolder] "=r" (sepcVar));
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL )
    {

        if (ecallCode == 0x01)
        {
            uint64 numOfBlocks;
            __asm__ volatile ("ld %[pHolder], 11 * 8 (fp) " : [pHolder] "=r" (numOfBlocks));
            size_t bytes = numOfBlocks * MEM_BLOCK_SIZE;
            void* ptr = m1.mem_alloc(bytes);

            __asm__ volatile("sd %[pHolder], 10 * 8(fp)" : : [pHolder] "r" (ptr));
            __asm__ volatile ("mv a0, %[pHolder]" : : [pHolder] "r" (ptr));
        }
        else if (ecallCode == 0x02)
        {
            void* pointerToFree;
            __asm__ volatile("ld %[pHolder], 11 * 8 (fp)" : [pHolder] "=r" (pointerToFree));
            int tempVal = m1.mem_free(pointerToFree);
            __asm__ volatile ("sd %[pHolder], 10 * 8  (fp)" : : [pHolder] "r" (tempVal));
        }
        else if (ecallCode == 0x11) { // Threads

            uint64* myStack;
            void *threadArgs;
            TCB::Body threadBody;
            TCB** threadHandle; // mozda reinterpret kast?
            __asm__ volatile("ld %[pHolder], 14 * 8 (fp)" : [pHolder] "=r" (myStack));
            __asm__ volatile("ld %[pHolder], 13 * 8 (fp)" : [pHolder] "=r" (threadArgs));
            __asm__ volatile ("ld %[pHolder], 12 * 8 (fp)" : [pHolder] "=r" (threadBody));
            __asm__ volatile ("ld %[pHolder], 11 * 8 (fp)" : [pHolder] "=r" (threadHandle));

            int returnVal = TCB::createThread(threadHandle,threadBody, threadArgs, myStack);
            __asm__ volatile ("sd %[pHolder], 10 * 8(fp)" : : [pHolder] "r" (returnVal));
        }

        else if (ecallCode == 0x12)
        {
    //TODO mozda bolje napraviti (staticku - usluznu)metodu u TCB(TCB::exit);
            TCB::running->threadState = TCB::ThreadState::FINISHED;
            TCB::running->setState(TCB::ThreadState::FINISHED);
            TCB::running->setFinished(true);
            TCB::dispatch(); // ne cuvamo je vise nigde i ona je izgubljena (ugasena);

        }
        else if (ecallCode == 0x13)
        {
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
        }
        else if (ecallCode == 0x14)
        {
            TCB* handle;
            __asm__ volatile ("ld %[pHolder], 11* 8 (fp)" : [pHolder] "=r" (handle));
            if (!handle->isFinished())
            {
                handle->joinFlag = true;
                TCB::join(handle);
            }
        }
        else if(ecallCode == 0x21) //  semafori
        {
            _sem** semHandle;
            uint64 semHandleConv;
            unsigned init;
            __asm__ volatile ("ld %[pHolder], 12* 8 (fp)" : [pHolder] "=r" (init));
            __asm__ volatile ("ld %[pHolder], 11* 8 (fp)" : [pHolder] "=r" (semHandleConv));
            semHandle = (_sem**)semHandleConv;
            int retVal = _sem::sem_open(semHandle, init);
            __asm__ volatile ("sd %[pHolder], 10 * 8(fp)" : : [pHolder] "r" (retVal));
        }
        else if (ecallCode == 0x22)
        {
            _sem* sem;
            __asm__ volatile ("ld %[pHolder], 11* 8 (fp)" : [pHolder] "=r" (sem));
            int retVal = _sem::sem_close(sem);
            __asm__ volatile ("sd %[pHolder], 10 * 8(fp)" : : [pHolder] "r" (retVal));
        }
        else if (ecallCode == 0x23)
        {
            _sem* sem;
            __asm__ volatile ("ld %[pHolder], 11* 8 (fp)" : [pHolder] "=r" (sem));
            int temp = _sem::sem_wait(sem);
            __asm__ volatile ("sd %[pHolder], 10 * 8(fp)" : : [pHolder] "r" (temp));
        }
        else if (ecallCode == 0x24)
        {
            _sem* sem;
            __asm__ volatile ("ld %[pHolder], 11* 8 (fp)" : [pHolder] "=r" (sem));
            int temp = _sem::sem_signal(sem);
            __asm__ volatile ("sd %[pHolder], 10 * 8(fp)" : : [pHolder] "r" (temp));
        }
        else if (ecallCode == 0x41)
        {
            char character = __getc();
            __asm__ volatile ("sd %[pHolder], 10 * 8(fp)" : : [pHolder] "r" (character));
        }
        else if (ecallCode == 0x42)
        {
            char temp;
            __asm__ volatile ("ld %[pHolder], 11* 8 (fp)" : [pHolder] "=r" (temp));
            __putc(temp);
        }
        else if (ecallCode == 0x90)
        {
            uint64* myStack;
            __asm__ volatile("ld %[pHolder], 14 * 8 (fp)" : [pHolder] "=r" (myStack));
            void *threadArgs = nullptr;
            __asm__ volatile("ld %[pHolder], 13 * 8 (fp)" : [pHolder] "=r" (threadArgs));
            TCB::Body threadBody = nullptr;
            __asm__ volatile ("ld %[pHolder], 12 * 8 (fp)" : [pHolder] "=r" (threadBody));
            TCB** threadHandle;
            __asm__ volatile ("ld %[pHolder], 11 * 8 (fp)" : [pHolder] "=r" (threadHandle));
            uint64 flag = 1;
            TCB::createThread(threadHandle,threadBody, threadArgs, myStack, flag);

        }

        else if (ecallCode == 0x91)
        {
            TCB** threadHandle;
           uint64 temp;
           __asm__ volatile ("ld %[pHolder], 11 * 8 (fp) " : [pHolder] "=r" (temp));

            threadHandle = (TCB**)temp;
            TCB::startThread(*threadHandle);

        }
        else if (ecallCode == 0x92)
        {
            Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
            sstatusVar = r_sstatus();

        }
        else if (ecallCode == 0x93)
        {
            Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
            sstatusVar = r_sstatus();

        }
        else {
         // error
        }

        sepcVar = sepcVar + 4;
        __asm__ volatile ("csrw sstatus, %[pHolder]" : : [pHolder] "r" (sstatusVar));

    }
    else if (scause == 0x8000000000000001UL)
    {

        mc_sip(SIP_SSIP);
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            uint64 volatile sepc = r_sepc();
            uint64 volatile sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }

    }
    else if (scause == 0x8000000000000009UL)
    {
        console_handler();
    }
    else
    {
       // printInt(scause);

        //error
    }
    __asm__ volatile ("csrw sepc, %[pHolder]" : : [pHolder] "r" (sepcVar));

}