#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <ctime>
#include <chrono>
#include <ratio>
#include <cmath>
#include <cassert>

#include "futex.h"
#include "SyncQueue.h"

typedef long long int64;

using namespace std;

template <typename MutexT>
void count(int &added, int &s, int S, MutexT &mutex) {
    while (s < S) {
        lock_guard<MutexT> lg(mutex);
        if (s < S) {
            s++;
            added++;
        }
    }
}

template <typename MutexT>
void experiment(int N, int S) {
    chrono::system_clock::time_point begin = chrono::system_clock::now();
    MutexT mutex;
    vector <thread> ts;
    vector <int> ads(N, 0);
    int s = 0;
    for (int i = 0; i < N; i++)
        ts.push_back(thread(count<MutexT>, ref(ads[i]), ref(s), S, ref(mutex)));
    int S2 = 0;
    for (int i = 0; i < N; i++) {
        ts[i].join();
        S2 += ads[i];
    }
    assert(S == S2);
    chrono::system_clock::time_point end = chrono::system_clock::now();
    cout << std::chrono::duration_cast<std::chrono::milliseconds>
            (end-begin).count() / 1000.0 << " seconds" << endl;
}

void testFutex() {
    cout << "Fast mutex unsafe" << endl;
    for (int i = 1; i <= 4; i++) {
        cout << "    " << i << " cores: ";
        experiment<futex_unsafe>(i, 24000000);
    }
    cout << "Fast mutex" << endl;
    for (int i = 1; i <= 4; i++) {
        cout << "    " << i << " cores: ";
        experiment<futex>(i, 24000000);
    }
    cout << "Usual mutex" << endl;
    for (int i = 1; i <= 4; i++) {
        cout << "    " << i << " cores: ";
        experiment<mutex>(i, 24000000);
    }
}

void testQueue() {
    SyncQueue<int> sq;
    thread producer([&]() {
        for (int i = 0; i < 1e4; i++) {
            sq.push(i);
        }
        this_thread::sleep_for(std::chrono::milliseconds(500));
        for (int i = 0; i < 1.5e4; i++) {
            sq.push(i);
        }
        for (int i = 0; i < 0.5e4; i++) {
            sq.pop();
        }
    }), consumer([&]() {
        for (int i = 0; i < 1.5e4; i++)
            sq.pop_and_wait();
    });
    producer.join();
    consumer.join();
    assert(sq.size() == 5e3);
    assert(sq.front() == 10e3);
    cout << "Test passed" << endl;
}


void testQueue(std::memory_order order) {

    chrono::system_clock::time_point begin = chrono::system_clock::now();
    SyncQueue<int> sq(order);
    thread producer([&]() {
        for (int i = 0; i < 1e4; i++) {
            sq.push(i);
        }
        for (int i = 0; i < 1.5e4; i++) {
            sq.push(i);
        }
        this_thread::sleep_for(std::chrono::milliseconds(500));
        for (int i = 0; i < 2.5e4; i++) {
            sq.pop();
        }
    }), consumer([&]() {
        for (int i = 0; i < 5e3; i++)
            sq.push(i);
    });
    producer.join();
    consumer.join();
    assert(sq.size() == 5e3);


    chrono::system_clock::time_point end = chrono::system_clock::now();
    cout << std::chrono::duration_cast<std::chrono::milliseconds>
            (end-begin).count() / 1000.0 << " seconds" << endl;
    cout << "Test passed" << endl;
}

void testMemoryOrders() {

    cerr << "Testing std::memory_order_seq_cst" << endl;
    testQueue(std::memory_order_seq_cst);

    cerr << "Testing std::memory_order_acquire" << endl;
    testQueue(std::memory_order_acquire);

    cerr << "Testing std::memory_order_release" << endl;
    testQueue(std::memory_order_release);

    cerr << "Ничего не видно, работают примерно одинаково =(" << endl;
}

int main() {
    testMemoryOrders();
    //testFutex();
    return 0;
}
