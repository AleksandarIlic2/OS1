//
// Created by os on 6/26/23.
//
#include "../lib/hw.h"
#ifndef PROJECT_BASE_V1_1_1_MEMORYALLOCATOR_H
#define PROJECT_BASE_V1_1_1_MEMORYALLOCATOR_H

class MemoryAllocator{
private:
    struct Header{ // interna lista za slob/zauzete blokove;
        size_t sizeOfBlock; //velicina racunajuci i zaglavlje za sad
        Header* next;
    };
    Header* findPredecessor(void* ptr);
    Header* findSucessor(void* ptr);
    static void* head;
  //  static Header zaglavljeGlaveListe ;
  MemoryAllocator(){};

public:
    void* mem_alloc(size_t size);

    int mem_free(void*);


    static MemoryAllocator& getInstance()
    {
        static MemoryAllocator myInstance;
        return myInstance;
    }

};




#endif //PROJECT_BASE_V1_1_1_MEMORYALLOCATOR_H
