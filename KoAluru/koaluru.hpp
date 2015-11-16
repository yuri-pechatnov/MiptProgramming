#ifndef KOALURU_HPP_INCLUDED
#define KOALURU_HPP_INCLUDED

#include <vector>
#include <list>


namespace KoAluruSuffixArrayAssistantClasses {
    class LessBucket {
      public:
        int leftBound, rightBound, editTime;
        LessBucket(int left = 0, int right = 0);
    };
};

class KoAluruSuffixArray {
  public:
    typedef std::vector<int> IntVector;
    typedef std::vector<bool> BoolVector;
    typedef IntVector &IntVectorReference;
    typedef const IntVector &IntVectorConstReference;
    typedef KoAluruSuffixArrayAssistantClasses::LessBucket LessBucket;
  protected:
    IntVector string; // index - suffix
    int alphabetSize;
    bool isReversed;
    BoolVector isGreaterType, isLessType; // index - suffix
    IntVector lessPositionToSuffix;
    int lessCount;
    IntVector suffixToLessPosition; // index - suffix
                            // defined only for less type
                            // less ordered by first letter
    IntVector suffixToPosition; // index - suffix
    IntVector positionToSuffix; // index - position
    IntVector bucketLeftBound, bucketInsertPosition,
            bucketRightBound; //index - bucket
    IntVector sortedLessType;
    IntVector lessDistance; // index - position
    std::vector <IntVector> lessDistanceLists,
            divideLists; // unimportant indexes, important order
    IntVector lessType;
    IntVector sortedLess;
    IntVector suffixArray;

    void swapSuffixes(int firstPosition, int secondPosition);
    void swapLess(int firstPosition, int secondPosition);


    void calculateAlphabetSize();
    void calculateTypes();
    void reverseAlphabetIfNeeded();
    void generateInitialSuffixArray();
    void divideInBuckets();
    void calculateLessDistance();
    void calculateLessDistanceLists();
    void calculateLessPositions();
    void calculateDivideLists();
    void calculateLessTypeDivision();
    void sortLessType();
    void putLessTypeOnPlace();
    void putGreaterTypeOnPlace();
    void generateSuffixArray();

  public:
    void testAndShow(IntVector string);

    void setString(IntVectorConstReference string);
    void calculate();
    IntVectorConstReference getSuffixArray();

    KoAluruSuffixArray();

    static void stringToSuffixArray(IntVectorConstReference string,
                    IntVectorReference suffixArray);
};

#endif // KOALURU_HPP_INCLUDED
