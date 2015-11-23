#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <algorithm>

#include "koaluru.hpp"

using namespace std;

int cur = 0;
char *data[500000000];

void *operator new(size_t n) {
    cur += n;
    return &data[cur - n];
}

void operator delete(void*) {

}

void buildLCP(vector <int> &str, vector <int> &suf, vector <int> &lcp) {
    int len = str.size();
    lcp.assign(len, 0);
    vector <int> pos(len);
    for (int i = 0; i < len; i++)
        pos[suf[i]] = i;
    int k = 0;
    for (int i = 0; i < len; i++) {
        if (k > 0)
            k--;
        if (pos[i] == len - 1) {
            lcp[len - 1] = -1;
            k = 0;
        } else {
            int j = suf[pos[i] + 1];
            while (max(i + k, j + k) < len && str[i + k] == str[j + k])
                k++;
            lcp[pos[i]] = k;
        }
    }
}

int f(string _s) {
    vector <int> s(_s.size() + 1), arr, lcp;
    for (int i = 0; i < (int)_s.size(); i++)
        s[i] = _s[i] - 'a' + 2;
    s.back() = 1;
    calculateSuffixArray(s, arr);
    buildLCP(s, arr, lcp);
    int ans = s.size() * (s.size() - 1) / 2;
    for (int i = 0; i < (int)lcp.size(); i++)
        ans -= lcp[i];
    return ans - 1;
}


int main()
{

    if (1) {
        int LIMIT = 1000000;
        fstream out;
        out.open("001", ios::out);
        out << LIMIT << endl;
        for (int i = 0; i < LIMIT; i++) {
            out << rand() % LIMIT << " ";
        }
        out.close();
    }

    #ifdef DEBUG
    freopen("001", "rt", stdin);
    #endif // DEBUG


    if (0) {
        freopen("out", "wt", stderr);
        KoAluruSuffixArray koala;
        int str[] = {2, 1, 4, 4, 1, 4, 4, 1, 3, 3, 1};
        vector <int> strv;
        strv.insert(strv.begin(), str + 0, str + 4);
        koala.testAndShow(strv);
        exit(0);
    }

    if (1) {
        int n;
        cin >> n;
        vector <int> str(n),  suf;
        for (int i = 0; i < n; i++)
            cin >> str[i];
        calculateSuffixArray(str, suf);
        //for (int i = 0; i < (int)suf.size(); i++)
        //    cerr << suf[i] << endl;
        exit(0);
    }

    int n;
    string ss, s;
    cin >> n >> s;
    ss = s + s;
    for (int i = 0; i < (int)s.size(); i++) {
        cout << f(ss.substr(i, n)) << " ";
    }
    return 0;
}
