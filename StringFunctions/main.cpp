#include <iostream>
#include <string>
#include <cstdio>

#include "strfunc.hpp"

#define USE_ZFUNCTION 0
#define SOLVE_FIND_SUBSTRING_TASK 0
#define SOLVE_FIND_MAXIMAL_SUBPALINDROME_TASK 1

using std::cin;
using std::cout;
using std::endl;

int main()
{
    #ifdef DEBUG
        freopen("001", "rt", stdin);
    #else
        std::ios::sync_with_stdio(false);
    #endif
    if (SOLVE_FIND_SUBSTRING_TASK) {
        String text, substring;
        cin >> substring >> text;
        IntVector resultOccurences;
        SuitableForFindingOccurences *occurencesFinder;
        if (USE_ZFUNCTION) {
            occurencesFinder = new ZFunction();
        } else {
            occurencesFinder = new PrefixFunction();
        }
        occurencesFinder->findOccurences(text, substring, resultOccurences);
        for (IntVector::iterator resultIterator = resultOccurences.begin();
                resultIterator != resultOccurences.end(); resultIterator++) {
            cout << *resultIterator << " ";
        }
        cout << endl;
    }
    if (SOLVE_FIND_MAXIMAL_SUBPALINDROME_TASK) {
        String text;
        cin >> text;
        IntVector z;
        PalindromeFunction().calculateAndWriteToVector(text, z);
        int palindromeBegin, palindromeEnd;
        PalindromeFunction().findMaximalPalindrom(text,
            palindromeBegin, palindromeEnd);
        cout << text.substr(palindromeBegin,
            palindromeEnd - palindromeBegin) << endl;
    }
    return 0;
}
