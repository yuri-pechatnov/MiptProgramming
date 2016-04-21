#include <iostream>
#include <thread>
#include <atomic>

using namespace std;


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


int main()
{
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
     return 0;

}

