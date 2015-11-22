#include <iostream>
#include <algorithm>

#include "farach.hpp"

using namespace std;

int f(string _s) {
    vector <int> s(_s.size() + 1), arr, lcp;
    for (int i = 0; i < (int)_s.size(); i++)
        s[i] = _s[i] - 'a' + 2;
    s.back() = 1;
    FarachSuffixTrie farach;
    farach.testAndShow(s, cerr);
    exit(0);
    farach.setString(s);
    farach.calculate();
    farach.getTrie().writeSuffixArray(arr);
    arr.erase(arr.begin());
    if (arr.size() != s.size())
        cerr << _s << endl;
    FarachSuffixTrie::suffixArrayToLCP(s, arr, lcp);
    int ans = s.size() * (s.size() - 1) / 2;
    for (int i = 0; i + 1 < (int)lcp.size(); i++)
        ans -= lcp[i];
    return ans;
}

int main()
{
    if (1) {
        #ifdef DEBUG
        freopen("001", "rt", stdin);
        freopen("out", "wt", stderr);
        #endif
        int n;
        string ss, s;
        cin >> n >> s;
        ss = s + s;
        for (int i = 0; i < (int)s.size(); i++) {
            cout << f(ss.substr(i, n)) << " ";
        }
        return 0;
    }

    if (0) {
        //         0  1  2  3  4  5  6  7  8  9 10 11 12
        int a[] = {1, 2, 3, 4, 5, 4, 3, 4, 3, 2, 3, 2, 1};
        vector <int> b(a, a + 13);
        RMQDelta1 table;
        table.setArray(b);
        cout << table.calculateMinimumPosition(3, 10) << endl;

        return 0;
    }
    freopen("out", "wt", stderr);
    //int a[] = {1,2,1,1,1,2,2,1,2,2,2,1,9};
    int a[] = {2,1,1,2,1,1,2,1,2,1,2,1,1,1};
    //10010010101000
    vector <int> b(a, a + 12);
    FarachSuffixTrie().testAndShow(b, cerr);
    return 0;
}
