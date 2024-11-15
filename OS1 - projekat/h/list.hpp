//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_LIST_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_LIST_HPP
#include "../h/MemoryAllocator.h"

class TCB;
template <typename T>
class List
{
private:
    struct Elem
    {
        T *data;
        Elem *next;

        Elem(TCB *data, Elem *next) : data(data), next(next) {}
    };

    Elem *head = nullptr;
    Elem *tail = nullptr;
    int counter = 0;
public:

    List()  {}

    List(const List<T> &) = delete;

    List<T> &operator=(const List<T> &) = delete;

    int getCounter() const {return counter;}

    void setPointers(){head = nullptr; tail = nullptr; counter = 0;}
    void addFirst(T *data)
    {
        counter++;
        Elem *elem = (Elem*)MemoryAllocator::getInstance().mem_alloc(sizeof(Elem));
        elem->data = data;
        elem->next = head;

        head = elem;
        if (!tail) { tail = head; }
    }

    void addLast(T *data)
    {
        counter++;
        Elem *elem = (Elem*) MemoryAllocator::getInstance().mem_alloc(sizeof(Elem));
        elem->data = data;
        elem->next = 0;
        if (tail)
        {
            tail->next = elem;
            tail = elem;
        } else
        {
            head = tail = elem;
        }
    }

    T *removeFirst()
    {

        if (!head) { return 0; }

        Elem *elem = head;
        head = head->next;
        if (!head) { tail = 0; }

        TCB *ret = elem->data;
        MemoryAllocator::getInstance().mem_free(elem);
        counter--;
        //delete elem;
        return ret;
    }

    T *peekFirst()
    {
        if (!head) { return 0; }
        return head->data;
    }

    T *removeLast()
    {

        if (!head) { return 0; }

        Elem *prev = 0;
        for (Elem *curr = head; curr && curr != tail; curr = curr->next)
        {
            prev = curr;
        }

        Elem *elem = tail;
        if (prev) { prev->next = 0; }
        else { head = 0; }
        tail = prev;

        TCB *ret = elem->data;
        MemoryAllocator::getInstance().mem_free(elem);
        counter--;
     //   delete elem;
        return ret;
    }

    T *peekLast()
    {
        if (!tail) { return 0; }
        return tail->data;
    }
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_LIST_HPP
