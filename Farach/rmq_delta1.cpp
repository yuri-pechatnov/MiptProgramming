#include "rmq_delta1.hpp"
#include "sparse_table.hpp"
#include <cassert>
#include <iostream>



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
    blockSize = std::max(1, SparseTable::extractDegree(array.size()) / 2);
    while (array.size() % blockSize) {
        array.push_back(array.back() + 1);
    }
    blockCount = (array.size() + blockSize - 1) / blockSize;
    typeCount = (1 << (blockSize - 1));
    blockAnswerData.resize(typeCount * blockSize * blockSize);
    Vector <int> full(blockSize);
    for (int mask = 0; mask < typeCount; mask++) {
        full.assign(blockSize, 0);
        for (int position = 1; position < blockSize; position++) {
            full[position] = full[position - 1] +
                ((mask >> (blockSize - 1 - position)) & 1) * 2 - 1;
        }
        for (int left = 0; left < blockSize; left++) {
            int answer = full[left], answerPosition = left;
            for (int right = left; right < blockSize; right++) {
                if (answer > full[right]) {
                    answer = full[right];
                    answerPosition = right;
                }
                blockAnswer(mask, left, right).first = answer;
                blockAnswer(mask, left, right).second = answerPosition;
            }
        }
    }
    blockType.resize(blockCount);
    blockMinimum.resize(blockCount);
    for (int i = 0; i < blockCount; i++) {
        blockType[i] = calculateType(array.begin() + i * blockSize,
                                     array.begin() + (i + 1) * blockSize);
        blockMinimum[i] = array[i * blockSize]
            + blockAnswer(blockType[i], 0, blockSize - 1).first;
    }
    table.setArray(blockMinimum);
    assert(blockAnswerData.size() < array.size() + 10);
}

int RMQDelta1::calculateMinimumPosition(int leftBorder, int rightBorder) {
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
        if (blockMinimum[block] < value) {
            value = blockMinimum[block];
            position = block * blockSize + blockAnswer(
                blockType[block], 0, blockSize - 1).second;
        }
    }
    return position;
}
RMQDelta1::RMQDelta1() {
}
