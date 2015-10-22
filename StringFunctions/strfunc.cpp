
#include <algorithm>
#include "strfunc.hpp"


void PrefixFunction::calculateAndWriteToVector(
            String string, IntVector &stringFunction) {
    stringFunction.resize(string.size());
    stringFunction[0] = 0;
    for (int currentPosition = 1; currentPosition < (int)string.size();
                currentPosition++) {
        int currentSuffixLength = stringFunction[currentPosition - 1];
        while (currentSuffixLength > 0 && string[currentPosition]
                != string[currentSuffixLength]) {
            currentSuffixLength = stringFunction[currentSuffixLength - 1];
        }
        stringFunction[currentPosition] = currentSuffixLength
            + (string[currentPosition] == string[currentSuffixLength]);
    }
}

void PrefixFunction::findOccurences(String text, String substring,
        IntVector &occurences) {
    IntVector stringFunction;
    String specialString = substring + specialSymbol + text;
    occurences.clear();
    calculateAndWriteToVector(specialString, stringFunction);
    int textBeginPosition = substring.size() + 1;
    for (int currentTextPosition = 0; currentTextPosition < (int)text.size();
                currentTextPosition++) {
        if (stringFunction[textBeginPosition + currentTextPosition]
                >= (int)substring.size()) {
            occurences.push_back(currentTextPosition - substring.size() + 1);
        }
    }
}

PrefixFunction::PrefixFunction() {}

void ZFunction::calculateAndWriteToVector(
            String string, IntVector &stringFunction) {
    stringFunction.resize(string.size());
    stringFunction[0] = 0;
    int mostRightBound = -1, mostRightBoundBegin = -1;
    for (int currentPosition = 1; currentPosition < (int)string.size();
                currentPosition++) {
        if (currentPosition < mostRightBound) {
            stringFunction[currentPosition] =
                std::min(stringFunction[currentPosition - mostRightBoundBegin],
                mostRightBound - currentPosition);
        } else {
            stringFunction[currentPosition] = 0;
        }
        while (currentPosition + stringFunction[currentPosition]
                < (int)string.size()
                && string[currentPosition + stringFunction[currentPosition]]
                == string[stringFunction[currentPosition]]) {
            stringFunction[currentPosition]++;
        }
        if (currentPosition + stringFunction[currentPosition]
                > mostRightBound) {
            mostRightBound = currentPosition
                + stringFunction[currentPosition];
            mostRightBoundBegin = currentPosition;
        }
    }
}


void ZFunction::findOccurences(String text, String substring,
        IntVector &occurences) {
    IntVector stringFunction;
    String specialString = substring + specialSymbol + text;
    occurences.clear();
    calculateAndWriteToVector(specialString, stringFunction);
    int textBeginPosition = substring.size() + 1;
    for (int currentTextPosition = 0; currentTextPosition < (int)text.size();
                currentTextPosition++) {
        if (stringFunction[textBeginPosition + currentTextPosition]
                >= (int)substring.size()) {
            occurences.push_back(currentTextPosition);
        }
    }
}

ZFunction::ZFunction() {}


void PalindromeFunction::calculateAndWriteToVector(
            String string, IntVector &stringFunction) {
    stringFunction.resize(string.size() * 2 + 1);
    String specialString;
    specialString.push_back(specialSymbol);
    for (int currentPosition = 0; currentPosition < (int)string.size();
                currentPosition++) {
        specialString.push_back(string[currentPosition]);
        specialString.push_back(specialSymbol);
    }
    int mostRightBound = -1, mostRightBoundBegin = -1;
    for (int currentPosition = 0; currentPosition < (int)specialString.size();
                currentPosition++) {
        if (currentPosition < mostRightBound) {
            stringFunction[currentPosition] =
                std::min(stringFunction[2 * mostRightBoundBegin
                - currentPosition], mostRightBound - currentPosition);
        } else {
            stringFunction[currentPosition] = 0;
        }
        while (currentPosition + stringFunction[currentPosition]
                < (int)specialString.size()
                && currentPosition - stringFunction[currentPosition] >= 0
                && specialString[currentPosition
                    + stringFunction[currentPosition]]
                == specialString[currentPosition
                    - stringFunction[currentPosition]]) {
            stringFunction[currentPosition]++;
        }
        if (currentPosition + stringFunction[currentPosition]
                > mostRightBound) {
            mostRightBound = currentPosition
                + stringFunction[currentPosition];
            mostRightBoundBegin = currentPosition;
        }
    }
}


void PalindromeFunction::findMaximalPalindrom(String string,
    int &palindromeBegin, int &palindromeEnd) {
    IntVector stringFunction;
    calculateAndWriteToVector(string, stringFunction);
    int maximalPalindromeLength = 0, maximalPalindromeCenter = 0;
    for (int currentPosition = 0; currentPosition < (int)stringFunction.size();
                currentPosition++) {
        if (stringFunction[currentPosition] > maximalPalindromeLength) {
            maximalPalindromeLength = stringFunction[currentPosition];
            maximalPalindromeCenter = currentPosition;
        }
    }
    palindromeBegin =
        (maximalPalindromeCenter - maximalPalindromeLength + 2) / 2;
    palindromeEnd =
        (maximalPalindromeCenter + maximalPalindromeLength) / 2;
}

PalindromeFunction::PalindromeFunction() {}
