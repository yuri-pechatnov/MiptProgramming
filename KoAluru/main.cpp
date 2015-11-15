#include <iostream>
#include <cstdio>
#include <string>

#include "koaluru.hpp"

using namespace std;

int main()
{
    freopen("001", "rt", stdin);
    //freopen("out", "wt", stderr);

    std::string s;
    cin >> s;
    vector <int> str(s.begin(), s.end()), sufarr;

    KoAluruSuffixArray::stringToSuffixArray(str, sufarr);

    for (int i = 0; i < (int)sufarr.size(); i++)
        cout << sufarr[i] << endl;
    return 0;
}
