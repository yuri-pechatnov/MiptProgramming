#ifndef STRFUNC_HPP_INCLUDED
#define STRFUNC_HPP_INCLUDED

#include <string>
#include <vector>
#include <algorithm>

typedef std::vector<int> IntVector;
typedef std::string String;

class StringFunction {
  protected:
    static const char specialSymbol = '~';
  public:
    virtual void calculateAndWriteToVector(String string,
                IntVector &stringFunction) = 0;
    template <typename OutputIterator>
    void calculateAndWrite(String string, OutputIterator outputIterator);
};

class FindOccurenceHandler {
  public:
    virtual void operator()(int position) const = 0;
};

class SuitableForFindingOccurences {
  public:
    virtual void findOccurences(String text, String substring,
                const FindOccurenceHandler &findOccurenceHandler) = 0;
};

class SuitableForFindingMaximalPalindromes {
  public:
    virtual void findMaximalPalindrom(String string, int &palindromeBegin,
                int &palindromeEnd) = 0;
};

template <typename OutputIterator>
void StringFunction::calculateAndWrite(String string,
            OutputIterator outputIterator) {
    IntVector stringFunction;
    calculateAndWriteToVector(string, stringFunction);
    std::copy(stringFunction.begin(), stringFunction.end(), outputIterator);
};

class PalindromeFunction : public StringFunction,
            public SuitableForFindingMaximalPalindromes {
  public:
    virtual void calculateAndWriteToVector(String string,
                IntVector &stringFunction);
    virtual void findMaximalPalindrom(String string, int &palindromeBegin,
                int &palindromeEnd);
    PalindromeFunction();
};

class PrefixFunction : public StringFunction,
            public SuitableForFindingOccurences {
  public:
    virtual void calculateAndWriteToVector(String string,
                IntVector &stringFunction);
    virtual void findOccurences(String text, String substring,
        const FindOccurenceHandler &findOccurenceHandler);
    PrefixFunction();
};

class ZFunction : public StringFunction,
            public SuitableForFindingOccurences {
  public:
    virtual void calculateAndWriteToVector(String string,
                IntVector &stringFunction);
    virtual void findOccurences(String text, String substring,
        const FindOccurenceHandler &findOccurenceHandler);
    ZFunction();
};


#endif // STRFUNC_HPP_INCLUDED
