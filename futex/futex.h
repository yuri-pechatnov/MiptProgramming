#ifndef FUTEX_H_INCLUDED
#define FUTEX_H_INCLUDED

#include <thread>
#include <exception>
#include <atomic>

class futex_unsafe {
    std::atomic_bool owned;
    std::memory_order order;
  public:
    void lock();
    bool try_lock();
    void unlock();
    futex_unsafe(std::memory_order order = std::memory_order_seq_cst);
};

class futex {
    typedef std::thread::id id_type;
    futex_unsafe mutex;
    id_type owner;
    bool owned;
  public:
    void lock();
    bool try_lock();
    void unlock();
    futex(std::memory_order order = std::memory_order_seq_cst);
};

class unlock_error : public std::logic_error {
  public:
    unlock_error(std::string explanation = "");
};

#endif // FUTEX_H_INCLUDED
