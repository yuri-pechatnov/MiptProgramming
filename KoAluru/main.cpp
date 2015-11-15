#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>

#include "koaluru.hpp"

using namespace std;

int cur = 0;
char data[200000000];

void *operator new(size_t n) {
    cur += n;
    return data + cur - n;
}

void operator delete(void *g) {
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
    for (int i = 0; i < _s.size(); i++)
        s[i] = _s[i] - 'a' + 2;
    s.back() = 1;
    KoAluruSuffixArray::stringToSuffixArray(s, arr);
    buildLCP(s, arr, lcp);
    int ans = s.size() * (s.size() - 1) / 2;
    for (int i = 0; i < (int)lcp.size(); i++)
        ans -= lcp[i];
    return ans - 1;
}


int main()
{
    #ifdef DEBUG
    freopen("001", "rt", stdin);
    //freopen("out", "wt", stderr);
    #endif // DEBUG


    if (1) {
        string s;
        cin >> s;
        vector <int> str(s.begin(), s.end()), suf;
        KoAluruSuffixArray::stringToSuffixArray(str, suf);
        for (int i = 0; i < (int)suf.size(); i++)
            cerr << suf[i] << endl;
        exit(0);
    }

    int n;
    string ss, s;
    cin >> n >> s;
    ss = s + s;
    int ncur = cur;
    for (int i = 0; i < s.size(); i++) {
        cur = ncur;
        cout << f(ss.substr(i, n)) << " ";
    }
    return 0;
}
