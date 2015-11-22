#include "rmq_delta1.hpp"
#include <cassert>



RMQDelta1::Pair<int, int> &RMQDelta1::blockAnswer(
        int type, int left, int right) {
    return blockAnswerData[type + (left * blockSize + right) * typeCount];
}

int RMQDelta1::calculateType(Vector<int>::iterator begin,
                             Vector<int>::iterator end) {
    assert(end - begin == blockSize);
    int mask = 0;
    for (Vector<int>::iterator it = begin; it + 1 != end; it++) {
        mask = (mask << 1) | (*it < *(it + 1));
    }
    return mask;
}

void RMQDelta1::setArray(const Vector<int> &array_) {
    array = array_;
    blockSize = 4;
    blockSize = std::max(1, SparseTable::extractDegree(array.size()) / 2);
    while (array.size() % blockSize) {
        array.push_back(array.back());
    }
    blockCount = (array.size() + blockSize - 1) / blockSize;
    typeCount = (1 << (blockSize - 1));
    blockType.resize(blockCount);
    for (int i = 0; i < blockCount; i++) {
        blockType[i] = calculateType(array.begin() + i * blockSize,
                                     array.begin() + (i + 1) * blockSize);
    }
    blockAnswerData.resize(typeCount * blockSize * blockSize);
    for (int mask = 0; mask < typeCount; mask++) {
        Vector <int> full(blockSize, 0);
        for (int position = 1; position < blockSize; position++) {
            full[position] = full[position - 1] +
                ((mask >> (blockSize - 1 - position)) & 1) * 2 - 1;
        }
        for (int left = 0; left < blockSize; left++) {
            for (int right = left; right < blockSize; right++) {
                int answer;
                answer = full[left];
                for (int i = left; i <= right; i++) {
                    answer = std::min(answer, full[i]);
                }
                for (int i = left; i <= right; i++) {
                    if (answer == full[i]) {
                        blockAnswer(mask, left, right).second = i;
                        break;
                    }
                }
                blockAnswer(mask, left, right).first = answer;
            }
        }
    }
    blockMinimum.resize(blockCount);
    for (int i = 0; i < blockCount; i++) {
        blockMinimum[i] = blockAnswer(blockType[i], 0, blockSize - 1).first;
    }
    table.setArray(blockMinimum);
}

int RMQDelta1::calculateMinimumPosition(int leftBorder, int rightBorder) {
    if (1) {
        int value = array[leftBorder], position = leftBorder;
        for (int i = leftBorder + 1; i < rightBorder; i++)
            if (array[i] < value) {
                value = array[i];
                position = i;
            }
        return position;
    }
    --rightBorder;
    int value = array[leftBorder], position = leftBorder;
    int leftBorderBlock = leftBorder / blockSize;
    int rightBorderBlock = rightBorder / blockSize;
    int leftBorderInside = leftBorder - leftBorderBlock * blockSize;
    int rightBorderInside = rightBorder - rightBorderBlock * blockSize;
    if (leftBorderBlock == rightBorderBlock) {
        return leftBorderBlock * blockSize +
            blockAnswer(blockType[leftBorderBlock],
            leftBorderInside, rightBorderInside).second;
    }
    /* left block */ {
        int leftBorderInside = leftBorder - leftBorderBlock * blockSize;
        Pair<int, int> &answer = blockAnswer(blockType[leftBorderBlock],
            leftBorderInside, blockSize - 1);
        if (array[leftBorderBlock * blockSize] + answer.first < value) {
            value = array[leftBorderBlock * blockSize] + answer.first;
            position = leftBorderBlock * blockSize + answer.second;
        }
    }
    /* right block */ {
        Pair<int, int> &answer = blockAnswer(blockType[rightBorderBlock],
            0, rightBorderInside);
        if (array[rightBorderBlock * blockSize] + answer.first < value) {
            value = array[rightBorderBlock * blockSize] + answer.first;
            position = rightBorderBlock * blockSize + answer.second;
        }
    }
    if (leftBorderBlock + 1 < rightBorderBlock) {
        int block = table.calculateMinimumPosition(
            leftBorderBlock + 1, rightBorderBlock);
        if (array[block * blockSize] + blockMinimum[block] < value) {
            value = blockMinimum[block];
            position = block * blockSize + blockAnswer(
                blockType[block], 0, blockSize - 1).second;
        }
    }
    return position;
}
RMQDelta1::RMQDelta1() {
}
