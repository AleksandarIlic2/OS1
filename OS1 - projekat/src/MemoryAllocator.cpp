//
// Created by os on 6/26/23.
//

#include "../h/MemoryAllocator.h"

#include "../lib/console.h"

void* MemoryAllocator::head = (void*)0;

void* MemoryAllocator:: mem_alloc(size_t size)
{
    if (this->head == 0) // prvi put neko zatrazi alokaciju memorije
    {
        this->head = (void*)HEAP_START_ADDR;
        ((Header*)this->head)->next = nullptr;
        ((Header*)this->head)->sizeOfBlock = (char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR; // sav memorijski prostor
    }
    Header* temp = (Header*)this->head;
    size_t pomVelicina = size + sizeof(Header);
    size_t allocatedSize = (pomVelicina / MEM_BLOCK_SIZE + (pomVelicina % MEM_BLOCK_SIZE ? 1 : 0)) * MEM_BLOCK_SIZE;
    // Slucaj prvi
    if (temp->sizeOfBlock >= allocatedSize)   {
        if (temp->sizeOfBlock > allocatedSize)
        {
            Header* newBlock = (Header*)((char*)temp + allocatedSize);
            newBlock->next = ((Header*)this->head)->next;
            this->head = newBlock;
            newBlock->sizeOfBlock = temp->sizeOfBlock - allocatedSize; // vel novog(preostalog)
            temp->sizeOfBlock = allocatedSize;// velicina stvarno alociranog, upisuje se trebace kod dealokacije
        }
        else if (temp->sizeOfBlock == allocatedSize && temp->sizeOfBlock % MEM_BLOCK_SIZE == 0)
        {

            this->head = ((Header*)(this->head))->next;
            temp->sizeOfBlock = allocatedSize;
        }
        else
        {
            //printString("Potencijalna greska1!");
        }
        return (char*)temp + sizeof(Header);
    }
// Slucaj kada nije prvi element
    while(temp->next) // first fit algoritam
    {
        Header* returnValue = temp->next;
        if (temp->next != nullptr && temp->next->sizeOfBlock >= allocatedSize)
        {
            if (temp->next->sizeOfBlock > allocatedSize)
            {
                Header* newBlock = (Header*)((char*)temp->next + allocatedSize);
                newBlock->next = temp->next->next;
                newBlock->sizeOfBlock = temp->next->sizeOfBlock - allocatedSize;
                temp->next->sizeOfBlock = allocatedSize;
                temp->next = newBlock;
            }
            else if (temp->next->sizeOfBlock == allocatedSize)
            {
                temp->next->sizeOfBlock = allocatedSize;
                temp->next = temp->next->next;
            }
            else
            {
             //   printString("Potencijalna greska2!");
            }
            return (char*)(returnValue) + sizeof(Header);
        }
        temp = temp->next;
    }
   // printString("\nNedovoljno memorije\n");
    return nullptr;
}

int MemoryAllocator::mem_free(void* ptr) {
    if (this->head == nullptr) return -1; // greska!
    if (ptr == nullptr) return -1;
    Header *temp = (Header*) this->head;
    temp = (Header*)this->head;
    Header *blockToFree = (Header*)((char *) ptr - sizeof(Header));
    Header* successor = findSucessor(ptr);
    Header* predecessor = findPredecessor(ptr);
    Header* prev = nullptr;
    while ((char*)temp < (char*)ptr)
    {
        prev = temp;
        temp = temp->next;
    }
    if (predecessor == nullptr && successor == nullptr) // nema spajanja
    {
        blockToFree->next = temp;
        if (prev != nullptr) prev->next = blockToFree;
        if (prev == nullptr) {this->head = blockToFree;}
    }
    else if (successor != nullptr && predecessor == nullptr) // spajamo sa sledbenikom
    {
        blockToFree->sizeOfBlock = blockToFree->sizeOfBlock + successor->sizeOfBlock;
        blockToFree->next = successor->next;
        if (prev != nullptr) prev->next = blockToFree;
        else this->head = blockToFree;
    }
    else if ((successor == nullptr && predecessor != nullptr))
    {
        // temp mora biti jednak prethodniku?
        predecessor->sizeOfBlock = predecessor->sizeOfBlock + blockToFree->sizeOfBlock;
    }
    else if (successor != nullptr && predecessor != nullptr)
    {
        predecessor->sizeOfBlock += blockToFree->sizeOfBlock + successor->sizeOfBlock;
        predecessor->next = predecessor->next->next;
    }
    return 0;
}

MemoryAllocator::Header* MemoryAllocator::findPredecessor(void* ptr) {
    Header* temp = (Header*)this->head;
    if ((char*)temp + temp->sizeOfBlock == (char*)ptr - sizeof(Header)) // Slucaj prvi
    {
        return temp;
    }
    else if ((char*)temp + temp->sizeOfBlock > (char*)ptr - sizeof(Header))
    {
        if ((char*)temp + temp->sizeOfBlock < (char*)ptr - sizeof(Header))
        {
            //printString("Error\n");
        }
        return nullptr;
    }
    while (1)
    {
        if (temp->next != nullptr && (char*)temp->next + temp->next->sizeOfBlock == (char*)ptr - sizeof(Header))
        {
            return temp->next;
        }
        else if (temp->next != nullptr && (char*)temp->next + temp->next->sizeOfBlock > (char*)ptr - sizeof(Header))
        {
            if ((char*)temp->next + temp->next->sizeOfBlock < (char*)ptr - sizeof(Header)) // ako slucajno upada u opseg gde ne bi smela
            {
               // printString("Error \n");
            }
            return nullptr;
        }
        temp = temp->next;
    }
}

MemoryAllocator::Header *MemoryAllocator::findSucessor(void *ptr) {
    Header* temp = (Header*)this->head;
    if ((char*)temp == (char*)ptr - sizeof(Header) + ((Header*)((char*)ptr - sizeof(Header)))->sizeOfBlock) // Slucaj prvi
    {
        return temp;
    }
    else if ((char*)temp > (char*)ptr - sizeof(Header) + ((Header*)((char*)ptr - sizeof(Header)))->sizeOfBlock)
    {
        return nullptr;
    }
    while (1)
    {
        if (temp->next != nullptr && (char*)temp->next == (char*)ptr - sizeof(Header) + ((Header*)((char*)ptr - sizeof(Header)))->sizeOfBlock)
        {
            return temp->next;
        }
        else if (temp->next != nullptr && (char*)temp->next > (char*)ptr - sizeof(Header) + ((Header*)((char*)ptr - sizeof(Header)))->sizeOfBlock)
        {
            return nullptr;
        }
        temp = temp->next;
    }
    return nullptr;
}