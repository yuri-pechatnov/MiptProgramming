#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <cassert>

#include "farach.hpp"

using namespace std;



int n, len;
int pos;

FarachSuffixTrie farach;
vector <int> s, arr, lcp;

class MyClass {
  public:
    bool operator()(int i, int j) {
        int lcp = farach.getTrie().getLCP(i, j);
        if (lcp >= n + pos - max(i, j))
            return i > j;
        return s[i + lcp] < s[j + lcp];
    }
    MyClass() {}
};



int f(string _s) {
    vector <int> s(_s.size() + 1), arr, lcp;
    for (int i = 0; i < (int)_s.size(); i++)
        s[i] = _s[i] - 'a' + 2;
    s.back() = 1;
    FarachSuffixTrie farach;
    //farach.testAndShow(s, cerr);
    //exit(0);
    farach.setString(s);
    //calculateSuffixArray(s, arr);
    //FarachSuffixTrie::suffixArrayToLCP(s, arr, lcp);
    //Trie trie;
    //trie.setString(s);
    //trie.buildFromSuffixArrayAndLCP(arr, lcp);
    //cout << trie << endl;
    farach.calculate();
    //arr.clear(); lcp.clear();
    farach.getTrie().writeSuffixArray(arr);
    arr.erase(arr.begin());
    if (arr.size() != s.size())
        cerr << _s << endl;
    FarachSuffixTrie::suffixArrayToLCP(s, arr, lcp);
    int ans = s.size() * (s.size() - 1) / 2;
    for (int i = 0; i + 1 < (int)lcp.size(); i++)
        ans -= lcp[i];
    //exit(0);
    return ans;
}

int main()
{
    if (0) {
        int LIMIT = 10000;
        fstream out;
        out.open("001", ios::out);
        out << LIMIT << endl;
        for (int i = 0; i < LIMIT; i++) {
            out << rand() % LIMIT << " ";
        }
        out.close();
    }
    if (0) {
        int LIMIT = 3000000;
        int n;
        n = LIMIT;
        vector <int> str(n),  suf;
        for (int i = 0; i < n; i++)
            //cin >> str[i];
            str[i] = rand() % LIMIT;

        cout << "Start" << endl;
        FarachSuffixTrie farach;
        farach.setString(str);
        farach.calculate();
        //calculateSuffixArray(str, suf);
        //for (int i = 0; i < (int)suf.size(); i++)
        //    cerr << suf[i] << endl;
        exit(0);
    }
    if (1) {
        #ifdef DEBUG
        freopen("001", "rt", stdin);
        //freopen("out", "wt", stderr);
        #endif

        string ss;
        cin >> n >> ss;
        len = ss.size();
        ss = ss + ss;
        s.resize(ss.size());
        for (int i = 0; i < (int)ss.size(); i++)
            s[i] = ss[i] - 'a' + 2;
        farach.setString(s);
        farach.calculate();
        farach.getTrie().deployLinearLCA();

        int ans = n * (n + 1) / 2;
        vector <int> q(n);
        for (pos = 0; pos < len; pos++) {
            int current = 0;
            for (int i = 0; i < n; i++)
                q[i] = i + pos;
            //copy(s.begin() + pos, s.begin() + pos + n, q.begin());
            sort(q.begin(), q.end(), MyClass());
            for (int j = 0; j + 1 < n; j++) {
                current += min(n + pos - max(q[j], q[j + 1]), farach.getTrie().getLCP(
                    q[j], q[j + 1]));
            }

            cout << ans - current << " ";
            /*if (ans - current != f(ss.substr(pos, n))) {
                cout << " != " << f(ss.substr(pos, n)) << endl;
            }*/
            /*

            current -= farach.getTrie().getLCP(i, i + 1);
            current -= farach.getTrie().getLCP(i + n - 1, i + n);*/
        }
        return 0;
    }



    if (0) {
        //         0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19
        int a[] = {1, 2, 3, 4, 5, 4, 3, 4, 3, 2, 3, 2, 1, 2, 3, 4, 3, 4, 5, 4};
        vector <int> b(a, a + 20);
        RMQDelta1 table;
        table.setArray(b);
        cout << table.calculateMinimumPosition(4, 16) << endl;

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
