#include <iostream>
#include <algorithm>

#include "radix_sort.hpp"

using namespace std;

class baka {
public:
    typedef int Type;
    int size() const { return 1; }
    int getField(int a, int b) const { return a; }
};

class haka {
public:
    typedef pair<int, int> Type;
    int size() const { return 2; }
    int getField(Type a, int b) const { return (b == 0 ? a.second : a.first); }
};

int main()
{

    int a[] = {9, 8, 5, 7, 6, 3, 2, 4, 0, 1};
    radixSort(a, a + 10, baka());
    for (int i = 0; i < 10; i++)
        cerr << a[i] << endl;
    vector <haka::Type> tt;
    tt.push_back(make_pair(1, 1));
    tt.push_back(make_pair(2, 2));
    tt.push_back(make_pair(5, 5));
    tt.push_back(make_pair(6, 4));
    tt.push_back(make_pair(7, 9));
    tt.push_back(make_pair(7, 1));
    tt.push_back(make_pair(7, 12));
    tt.push_back(make_pair(3, 3));
    tt.push_back(make_pair(4, 6));
    tt.push_back(make_pair(8, 8));
    tt.push_back(make_pair(9, 7));
    radixSort(tt.begin(), tt.end(), haka());
    for (int i = 0; i < tt.size(); i++)
        cerr << tt[i].first << " " << tt[i].second << endl;

    cout << "Hello world!" << endl;
    return 0;
}
