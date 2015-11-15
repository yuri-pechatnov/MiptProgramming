#include "koaluru.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>

using std::cerr;
using std::endl;
using std::setw;

typedef KoAluruSuffixArray::IntVector IntVector;
typedef KoAluruSuffixArray::IntVectorReference IntVectorReference;
typedef KoAluruSuffixArray::IntVectorConstReference IntVectorConstReference;

namespace KoAluruSuffixArrayAssistantClasses {
    LessBucket::LessBucket(int left, int right): leftBound(left),
                                                rightBound(right) {};
};

void KoAluruSuffixArray::swapSuffixes(int firstPosition, int secondPosition) {
    int &firstSuffix = positionToSuffix[firstPosition],
        &secondSuffix = positionToSuffix[secondPosition];
    std::swap(suffixToPosition[firstSuffix], suffixToPosition[secondSuffix]);
    std::swap(firstSuffix, secondSuffix);
}


void KoAluruSuffixArray::swapLess(int firstLess, int secondLess) {
    int &firstSuffix = lessPositionToSuffix[firstLess],
        &secondSuffix = lessPositionToSuffix[secondLess];
    std::swap(suffixToLessPosition[firstSuffix], suffixToLessPosition[secondSuffix]);
    std::swap(firstSuffix, secondSuffix);
}



void KoAluruSuffixArray::calculateAlphabetSize() {
    alphabetSize = 1;
    for (int i = 0; i < (int)string.size(); i++)
        if (alphabetSize <= string[i])
            alphabetSize = string[i] + 1;
}

void KoAluruSuffixArray::calculateTypes() {
    isLessType.assign(string.size(), 0);
    isGreaterType.assign(string.size(), 0);
    isLessType.end()[-1] = isGreaterType.end()[-1] = 1;
    lessCount = 1;
    for (int i = string.size() - 2; i >= 0; i--) {
        if (string[i] == string[i + 1]) {
            isLessType[i] = isLessType[i + 1];
            isGreaterType[i] = isGreaterType[i + 1];
        } else {
            if (string[i] < string[i + 1]) {
                isLessType[i] = 1;
            } else {
                isGreaterType[i] = 1;
            }
        }
        lessCount += isLessType[i];
    }
}

void KoAluruSuffixArray::reverseAlphabetIfNeeded() {
    if (lessCount * 2 - 1 > (int)string.size()) {
        isReversed = true;
        for (int i = 0; i < (int)string.size(); i++) {
            string[i] = alphabetSize - 1 - string[i];
        }
        calculateTypes();
    } else {
        isReversed = false;
    }
}

void KoAluruSuffixArray::generateInitialSuffixArray() {
    suffixToPosition.resize(string.size());
    positionToSuffix.resize(string.size());
    for (int i = 0; i < (int)string.size(); i++) {
        suffixToPosition[i] =
            positionToSuffix[i] = i;
    }
}

void KoAluruSuffixArray::divideInBuckets() {
    bucketRightBound.assign(alphabetSize, 0);
    for (int i = 0; i < (int)string.size(); i++) {
        ++bucketRightBound[string[i]];
    }
    for (int i = 1; i < alphabetSize; i++)
        bucketRightBound[i] += bucketRightBound[i - 1];
    bucketLeftBound.assign(alphabetSize, 0);
    for (int i = 1; i < alphabetSize; i++)
        bucketLeftBound[i] = bucketRightBound[i - 1];
    bucketInsertPosition = bucketLeftBound;
    for (int i = 0; i < (int)string.size(); i++) {
        while (i < bucketLeftBound[string[positionToSuffix[i]]]
                || i >= bucketRightBound[string[positionToSuffix[i]]]) {
            swapSuffixes(i, bucketInsertPosition[string[
                    positionToSuffix[i]]]++);
        }
    }
}


void KoAluruSuffixArray::calculateLessDistance() {
    lessDistance.assign(string.size(), 0);
    int previousLess = -1;
    for (int i = 0; i < (int)string.size(); i++) {
        if (previousLess != -1) {
            lessDistance[i] = i - previousLess;
        }
        if (isLessType[i])
            previousLess = i;
    }
}


void KoAluruSuffixArray::calculateLessDistanceLists() {
    int lessDistanceMax = 0;
    for (int i = 0; i < (int)string.size(); i++) {
        if (lessDistance[i] > lessDistanceMax) {
            lessDistanceMax = lessDistance[i];
        }
    }
    lessDistanceLists.assign(lessDistanceMax + 1, IntVector(0));
    for (int i = 0; i < (int)string.size(); i++) {
        lessDistanceLists[lessDistance[positionToSuffix[i]]]
            .push_back(positionToSuffix[i]);
    }
    lessDistanceLists[0] = lessPositionToSuffix;
}


void KoAluruSuffixArray::calculateDivideLists() {
    divideLists.clear();
    for (int i = 0; i < (int)lessDistanceLists.size(); i++) {
        int equalsBegin = 0;
        int newListsBegin = divideLists.size();
        for (int j = 1; j < (int)lessDistanceLists[i].size(); j++) {
            if (string[lessDistanceLists[i][j]]
                    != string[lessDistanceLists[i][j - 1]]) {
                divideLists.push_back(IntVector(
                    lessDistanceLists[i].begin() + equalsBegin,
                    lessDistanceLists[i].begin() + j));
                equalsBegin = j;
            }
        }
        if (lessDistanceLists[i].size() > 0) {
            divideLists.push_back(IntVector(
                lessDistanceLists[i].begin() + equalsBegin,
                lessDistanceLists[i].end()));
        }
        for (int j = newListsBegin; j < (int)divideLists.size(); j++) {
            for (int k = 0; k < divideLists[j].size(); k++) {
                divideLists[j][k] -= i;
            }
        }
    }
}

void KoAluruSuffixArray::calculateLessPositions() {
    suffixToLessPosition.assign(string.size(), -1);
    for (int i = 0; i < (int)string.size(); i++) {
        if (isLessType[positionToSuffix[i]]) {
            suffixToLessPosition[positionToSuffix[i]] =
                lessPositionToSuffix.size();
            lessPositionToSuffix.push_back(positionToSuffix[i]);
        }
    }
}

void KoAluruSuffixArray::calculateLessTypeDivision() {
    lessBuckets.push_back(LessBucket(0, lessPositionToSuffix.size()));
    lessToBucket.assign(lessPositionToSuffix.size(), lessBuckets.begin());
    for (int i = 0; i < divideLists.size(); i++) {
        for (int j = 0; j < divideLists[i].size(); j++) {
            std::list <LessBucket>::iterator bucketIterator =
                    lessToBucket[suffixToLessPosition[ divideLists[i][j] ]];
            bucketIterator->needPullFront.push_back(divideLists[i][j]);
        }
        for (int j = 0; j < divideLists[i].size(); j++) {
            std::list <LessBucket>::iterator bucketIterator =
                    lessToBucket[suffixToLessPosition[ divideLists[i][j] ]],
                    newBucketIterator;
            IntVector &needPullFront = bucketIterator->needPullFront;
            if (needPullFront.size() > 0) {
                lessBuckets.insert(bucketIterator,
                    LessBucket(bucketIterator->leftBound,
                        bucketIterator->leftBound + needPullFront.size()));
                --(newBucketIterator = bucketIterator);
                int &insertBound = bucketIterator->leftBound;
                for (IntVector::iterator it = needPullFront.begin();
                        it != needPullFront.end(); it++) {
                    lessToBucket[insertBound] = newBucketIterator;
                    swapLess(suffixToLessPosition[*it], insertBound);
                    insertBound++;
                }
                needPullFront.clear();
            }
        }
    }
    lessBucketNum.resize(lessPositionToSuffix.size());
    int current = 1;
    for (std::list <LessBucket>::iterator it = lessBuckets.begin();
            it != lessBuckets.end(); it++) {
        for (int i = it->leftBound; i < it->rightBound; i++) {
            lessBucketNum[i] = current;
        }
        current += (it->leftBound != it->rightBound);
    }
}

void KoAluruSuffixArray::sortLessType() {
    IntVector recursiveString;
    IntVector recursiveConnection;
    for (int i = 0; i < (int)string.size(); i++) {
        if (isLessType[i]) {
            recursiveConnection.push_back(i);
            recursiveString.push_back(lessBucketNum[suffixToLessPosition[i]]);
        }
    }
    stringToSuffixArray(recursiveString, sortedLess);
    for (int i = 0; i < (int)sortedLess.size(); i++)
        sortedLess[i] = recursiveConnection[sortedLess[i]];
}

void KoAluruSuffixArray::putLessTypeOnPlace() {
    bucketInsertPosition = bucketRightBound;
    for (int i = sortedLess.size() - 1; i >= 0; i--) {
        int currentPosition = suffixToPosition[sortedLess[i]],
            destinationPosition = --bucketInsertPosition[
                string[sortedLess[i]]
            ];
        swapSuffixes(currentPosition, destinationPosition);
    }
}

void KoAluruSuffixArray::putGreaterTypeOnPlace() {
    bucketInsertPosition = bucketLeftBound;
    for (int i = 0; i < (int)string.size(); i++) {
        int current = positionToSuffix[i] - 1;
        if (current >= 0 && isGreaterType[current]) {
            int currentPosition = suffixToPosition[current],
                destinationPosition = bucketInsertPosition[
                    string[current]
                ]++;
            swapSuffixes(currentPosition, destinationPosition);
        }
    }
}

void KoAluruSuffixArray::generateSuffixArray() {
    suffixArray.clear();
    suffixArray.insert(suffixArray.begin(),
        positionToSuffix.begin() + 1, positionToSuffix.end());
}

void KoAluruSuffixArray::unreverseAlphabetIfNeeded() {
    if (isReversed) {
        std::reverse(suffixArray.begin(), suffixArray.end());
    }
}

void KoAluruSuffixArray::test() {
    int str[] = {2, 1, 4, 4, 1, 4, 4, 1, 3, 3, 1};
    IntVector strv;
    strv.insert(strv.begin(), str + 0, str + 11);

    setString(strv);
    calculateAlphabetSize();
    cerr << "alphabetSize = " << alphabetSize << endl;
    calculateTypes();
    for (int i = 0; i < string.size(); i++)
        cerr << setw(3) << string[i];
    cerr << endl;
    for (int i = 0; i < string.size(); i++)
        cerr << setw(3) << (isLessType[i] == 0 ? ' ' : 'L');
    cerr << endl;
    for (int i = 0; i < string.size(); i++)
        cerr << setw(3) << (isGreaterType[i] == 0 ? ' ' : 'G');
    cerr << endl;
    cerr << endl;

    generateInitialSuffixArray();
    divideInBuckets();
    calculateLessDistance();
    calculateLessPositions();
    calculateLessDistanceLists();
    calculateDivideLists();

    for (int i = 0; i < string.size(); i++) {
        if (suffixToPosition[positionToSuffix[i]] != i) {
            cerr << "Error of suffixToPosition in " << i << endl;
        }
    }

    for (int i = 0; i < alphabetSize; i++) {
        cerr << "bucket #" << i << endl;
        for (int j = bucketLeftBound[i]; j < bucketRightBound[i]; j++) {
            cerr << setw(3) << positionToSuffix[j] <<  "(LessD = "
                    << lessDistance[positionToSuffix[j]] << ") :";
            for (int k = positionToSuffix[j]; k < (int)string.size(); k++) {
                cerr << setw(3) << string[k];
            }
            cerr << endl;
        }
    }
    cerr << endl;
    for (int i = 0; i < lessDistanceLists.size(); i++) {
        cerr << "lessDistanceList #" << setw(3) << i << ":";
        for (int j = 0; j < lessDistanceLists[i].size(); j++) {
            cerr << setw(3) << lessDistanceLists[i][j];
        }
        cerr << endl;
    }
    cerr << endl;
    for (int i = 0; i < divideLists.size(); i++) {
        cerr << "divideList #" << setw(3) << i << ":";
        for (int j = 0; j < divideLists[i].size(); j++) {
            cerr << setw(3) << divideLists[i][j];
        }
        cerr << endl;
    }
    cerr << endl;
    calculateLessTypeDivision();
    for (int i = 0; i < string.size(); i++)
        cerr << setw(3) << string[i];
    cerr << endl;
    for (int i = 0; i < string.size(); i++)
        if (isLessType[i] == 1)
            cerr << setw(3) << lessBucketNum[suffixToLessPosition[i]];
        else
            cerr << setw(3) << ' ';
    cerr << endl;
    sortLessType();
    cerr << endl;
    for (int j = 0; j < (int)sortedLess.size(); j++) {
        cerr << "SortedLess #" << setw(2) << j <<  " :";
        for (int k = sortedLess[j]; k < (int)string.size(); k++) {
            cerr << setw(3) << string[k];
        }
        cerr << endl;
    }
    cerr << endl;

    cerr << "Put less type on place" << endl;

    putLessTypeOnPlace();

    for (int i = 0; i < alphabetSize; i++) {
        cerr << "bucket #" << i << endl;
        for (int j = bucketLeftBound[i]; j < bucketRightBound[i]; j++) {
            cerr << setw(3) << positionToSuffix[j] <<  "(LessD = "
                    << lessDistance[positionToSuffix[j]] << ") :";
            for (int k = positionToSuffix[j]; k < (int)string.size(); k++) {
                cerr << setw(3) << string[k];
            }
            cerr << endl;
        }
    }
    cerr << endl;

    cerr << "Put greater type on place" << endl;

    putGreaterTypeOnPlace();

    for (int i = 0; i < alphabetSize; i++) {
        cerr << "bucket #" << i << endl;
        for (int j = bucketLeftBound[i]; j < bucketRightBound[i]; j++) {
            cerr << setw(3) << positionToSuffix[j] <<  "(LessD = "
                    << lessDistance[positionToSuffix[j]] << ") :";
            for (int k = positionToSuffix[j]; k < (int)string.size(); k++) {
                cerr << setw(3) << string[k];
            }
            cerr << endl;
        }
    }
    cerr << endl;

}

void KoAluruSuffixArray::setString(IntVectorConstReference stringArg) {
    string = stringArg;
    string.push_back(0);
}

void KoAluruSuffixArray::calculate() {
    if (string.size() > 2) {
        calculateAlphabetSize();
        calculateTypes();
        generateInitialSuffixArray();
        divideInBuckets();
        calculateLessDistance();
        calculateLessPositions();
        calculateLessDistanceLists();
        calculateDivideLists();
        calculateLessTypeDivision();
        sortLessType();
        putLessTypeOnPlace();
        putGreaterTypeOnPlace();
        generateSuffixArray();
    }
    else {
        suffixArray = IntVector(string.size() - 1, 0);
    }
}

IntVectorConstReference KoAluruSuffixArray::getSuffixArray() {
    return suffixArray;
}

KoAluruSuffixArray::KoAluruSuffixArray() {

}

void KoAluruSuffixArray::stringToSuffixArray(IntVectorConstReference string,
                    IntVectorReference suffixArray) {
    KoAluruSuffixArray calculator;
    calculator.setString(string);
    calculator.calculate();
    suffixArray = calculator.getSuffixArray();
}

