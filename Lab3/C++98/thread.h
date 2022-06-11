#ifndef MAIN_CPP_THREAD_H
#define MAIN_CPP_THREAD_H
#pragma once
#include "Windows.h"

struct thread
{
    int *arr;
    int size;
    int num;
    HANDLE start;
    HANDLE stop;
    HANDLE* terminate;
    thread(void);
    thread(int* const &, const int&, const int&, const HANDLE&,
           const HANDLE&, HANDLE*&);
    thread& operator =(const thread&);
};

thread::thread(void)
{
    this->size = 0;
    this->arr = NULL;
    this->num = -1;
    this->start = NULL;
    this->stop = NULL;
    this->terminate = NULL;
}

thread::thread(int* const & arr, const int& size,
               const int& num, const HANDLE& Start,
               const HANDLE& Stop, HANDLE*& Terminate)
               :arr(arr), size(size), num(num),start(Start),
               stop(Stop), terminate(Terminate){}

thread& thread::operator= (const thread& t)
{
    this->arr = t.arr;
    this->size = t.size;
    this->num = t.num;
    this->start = t.start;
    this->stop = t.stop;
    this->terminate = t.terminate;
    return *this;
}
#endif //MAIN_CPP_THREAD_H
