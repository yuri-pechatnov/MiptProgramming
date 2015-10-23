#include <iostream>
#include <string>
#include <cstdio>

#include "strfunc.hpp"

#define USE_ZFUNCTION 1
#define SOLVE_FIND_SUBSTRING_TASK 0
#define SOLVE_FIND_MAXIMAL_SUBPALINDROME_TASK 1

using std::cin;
using std::cout;
using std::endl;

class TypeOccurences : public FindOccurenceHandler {
  public:
    virtual void operator()(int position) const ;
};

void TypeOccurences::operator()(int position) const {
    cout << position << " ";
}

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
        occurencesFinder->findOccurences(text, substring, TypeOccurences());
        cout << endl;
    }
    if (SOLVE_FIND_MAXIMAL_SUBPALINDROME_TASK) {
        String text;
        cin >> text;
        IntVector z;
        int palindromeBegin, palindromeLength;
        PalindromeFunction().findMaximalPalindrom(text,
            palindromeBegin, palindromeLength);
        cout << text.substr(palindromeBegin, palindromeLength) << endl;
    }
    return 0;
}
