#include "sparse_table.hpp"

#include <algorithm>
#include <cassert>



int SparseTable::extractDegree(register int value) {
    register int answer = 0;
    if (value >= (1 << 16)) {
        answer += 16;
        value >>= 16;
    }
    if (value >= (1 << 8)) {
        answer += 8;
        value >>= 8;
    }
    if (value >= (1 << 4)) {
        answer += 4;
        value >>= 4;
    }
    if (value >= (1 << 2)) {
        answer += 2;
        value >>= 2;
    }
    if (value >= (1 << 1)) {
        answer += 1;
        value >>= 1;
    }
    return answer;
}

int SparseTable::calculateMinimumPosition(int leftBorder, int rightBorder) {
    assert(leftBorder != rightBorder);
    int level = extractDegree(rightBorder - leftBorder);
    if (table[level][leftBorder] < table[level][rightBorder - (1 << level)]) {
        return tablePositions[level][leftBorder];
    } else {
        return tablePositions[level][rightBorder - (1 << level)];
    }
}

void SparseTable::setArray(std::vector <int> array) {
    assert(table[0] == NULL);
    int tableSize = array.size();
    for (levelsCount = 0; (1 << levelsCount) < tableSize; levelsCount++) {
        table[levelsCount] = new int[tableSize];
        tablePositions[levelsCount] = new int[tableSize];
    }
    std::copy(array.begin(), array.end(), table[0]);
    for (int i = 0; i < tableSize; i++) {
        tablePositions[0][i] = i;
    }

    for (int k = 1; k < levelsCount; k++) {
        for (int i = 0; i < tableSize; i++) {
            table[k][i] = table[k - 1][i];
            tablePositions[k][i] = tablePositions[k - 1][i];
            int shift = (1 << (k - 1));
            if (i + shift < tableSize &&
                    table[k][i] > table[k - 1][i + shift]) {
                table[k][i] = table[k - 1][i + shift];
                tablePositions[k][i] = tablePositions[k - 1][i + shift];
            }
        }
    }
}
SparseTable::SparseTable() {}

void SparseTable::clear() {
    if (table[0] != NULL) {
        for (int k = 0; k < levelsCount; k++) {
            delete table[k];
            delete tablePositions[k];
        }
        table[0] = NULL;
    }
}

SparseTable::~SparseTable() {
    clear();
}

