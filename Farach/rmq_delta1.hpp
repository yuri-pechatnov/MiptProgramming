#ifndef RMQ_DELTA1_HPP_INCLUDED
#define RMQ_DELTA1_HPP_INCLUDED

#include <vector>
#include "sparse_table.hpp"

class RMQDelta1 {
    template <typename T>
    using Vector = std::vector<T>;

    template <typename T1, typename T2>
    using Pair = std::pair<T1, T2>;

    SparseTable table;
    int blockSize, blockCount, typeCount;
    Vector<int> array, blockType, blockMinimum;
    Vector<Pair<int, int>> blockAnswerData;
    inline Pair<int, int> &blockAnswer(int type, int left, int right);
    int calculateType(Vector<int>::iterator begin, Vector<int>::iterator end);

  public:
    void setArray(const Vector<int> &array);
    int calculateMinimumPosition(int leftBorder, int rightBorder);
    RMQDelta1();
};


#endif // RMQ_DELTA1_HPP_INCLUDED
