#include <iostream>
#include <thread>
#include <atomic>
#include <vector>

using namespace std;

char data[(int)1e6];
char *dt = data;

// Переопределеяем выделение памяти, чтобы никто умный не мешал (в частности не мешал попадать на границу кэш-линий)
void* operator new(size_t n) {
    dt += n + rand() % 3;
    return dt - n;
}

void operator delete(void*) {
}

std::atomic<int> a[100000];

void threadFunction(int pos)
{
     for (int i = 0; i < 1e7; i++)
        a[pos] = a[pos] + a[pos];
}


double estimate(int pos1, int pos2) {

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    thread thr(threadFunction, pos2);
     for (int i = 0; i < 1e7; i++)
        a[0] = a[0] * a[pos1];
     thr.join();
    end = std::chrono::system_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>
                             (end-start).count();
}



// Создает указанные атомики и щелкает их двумя потоками в перемешку
template <typename T>
double estimate2(int pos1, int pos2) {
    T a[1000];
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    thread thr([&a, pos2]() {
        for (int i = 0; i < 1e7; i++)
            a[pos2].exchange(1);
    });
    for (int i = 0; i < 1e7; i++)
        a[pos1].exchange(0);
    thr.join();
    end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>
                             (end-start).count();
}


void testPingPong() {
   while (true) {
        cout << "Ping pong: " << estimate(0, 1) << endl;
        cout << "Optimal:   " << estimate(0, 8) << endl;
    }
    /*
        time 10000 vs 1500
        (Ping-Pong vs usual)

        cache-line size 8 * sizeof(atomic<int>) = 32
        processor Core i5-4210U

        Architecture:          i686
        CPU op-mode(s):        32-bit, 64-bit
        Byte Order:            Little Endian
        CPU(s):                4
        On-line CPU(s) list:   0-3
        Thread(s) per core:    2
        Core(s) per socket:    2
        Socket(s):             1
        Vendor ID:             GenuineIntel
        CPU family:            6
        Model:                 69
        Stepping:              1
        CPU MHz:               768.000
        BogoMIPS:              4789.03
        Virtualization:        VT-x
        L1d cache:             32K
        L1i cache:             32K
        L2 cache:              256K
        L3 cache:              3072K
    */
}

struct Levelled : public atomic<int> {
    int t[16];
};


struct Intersective {
    int t: 4*31;
    std::atomic<int> a;
    int r: 4*31;
    void exchange(int t) {
        a.exchange(t);
    }
};



int main()
{
   for (int i = 0; i < 3; i++) {
        // задача 4.2 и 4.3.3
        cout << "Usual (one cache line): " << estimate2<atomic<int> >(0, 1) << endl;
        cout << "Usual (two cache line): " << estimate2<atomic<int> >(0, 16) << endl;
        // Выровненные и невыровненные переменные 4.3.1
        cout << "Levelled:   " << estimate2<Levelled>(0, 1) << endl;
        cout << "Intersective:   " << estimate2<Intersective>(0, 1) << endl;
    }
    // Попытаемся пересечь атомиком кэшлинию 4.3.2
    // несколько должны выбиваться
    for (int i = 0; i < 200; i++) {
        cout << "Try to be in two cache-lines:   " << estimate2<atomic<long long> >(i, i) << endl;
    }
    // Действительно видно что случаются выбросы
    return 0;
}

