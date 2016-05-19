#include "futex.h"


void futex_unsafe::lock() {
    while (!try_lock())
        std::this_thread::yield();

}

bool futex_unsafe::try_lock() {
    bool smth = false;
    return owned.compare_exchange_weak(smth, true, order);
    return !owned.exchange(true);
}

void futex_unsafe::unlock() {
    owned = false;
}

futex_unsafe::futex_unsafe(std::memory_order order): owned(false), order(order) {}


void futex::lock() {
    while (!try_lock())
        std::this_thread::yield();
}

bool futex::try_lock() {
    if (mutex.try_lock()) {
        if (owned == true) {
            mutex.unlock();
            return false;
        }
        owned = true;
        owner = std::this_thread::get_id();
        mutex.unlock();
        return true;
    }
    else
        return false;
}

void futex::unlock() {
    if (owned) {
        if (owner == std::this_thread::get_id())
            owned = false;
        else
            throw unlock_error("Can't unlock not owned mutex.");
    }
    else
        throw unlock_error("Can't unlock not locked mutex.");
}

futex::futex(std::memory_order order): mutex(order), owned(false) {}

unlock_error::unlock_error(std::string expl): logic_error(expl) {}
