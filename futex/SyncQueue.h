#ifndef SYNCQUEUE_H_INCLUDED
#define SYNCQUEUE_H_INCLUDED

#include <deque>
#include <mutex>
#include <condition_variable>
#include "futex.h"

template<typename T>
class SyncQueue {
  protected:
    typedef futex_unsafe MutexT;
    std::memory_order order;
    std::deque<T> data;
    mutable MutexT mtx;
    mutable std::condition_variable cond;
  public:
    typedef T &Reference;
    typedef const T &ConstReference;
    explicit SyncQueue(std::memory_order order = std::memory_order_acquire);
    bool empty() const;
    size_t size() const;
    T front() const;
    T back() const;
    void push(ConstReference x);
    void pop();
    void pop_and_wait();
};

#include "SyncQueue.cpp"

#endif // SYNCQUEUE_H_INCLUDED
