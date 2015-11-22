#ifndef SPARSE_TABLE_HPP_INCLUDED
#define SPARSE_TABLE_HPP_INCLUDED

#include <vector>

class SparseTable {
    int* table[30];
    int* tablePositions[30];
    int tableSize, levelsCount;
  public:
    inline static int extractDegree(int value);
    int calculateMinimumPosition(int leftBorder, int rightBorder);
    void setArray(std::vector <int> array);
    void clear();
    SparseTable();
    ~SparseTable();
};

#endif // FARACH_HPP_INCLUDED
