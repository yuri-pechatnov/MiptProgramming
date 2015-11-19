#ifndef RADIX_SORT_HPP_INCLUDED
#define RADIX_SORT_HPP_INCLUDED

/**
class RadixSortFieldGetter {
  public:
    typename ... Type;
    int size(); // amount of fields
    int getField(const &Type, int field); // field getter
}
*/

#include <vector>
#include <algorithm>

template <typename SortIterator, typename FieldGetterClass>
void radixSort(SortIterator begin, SortIterator end,
                const FieldGetterClass &fieldGetter) {
    int size = end - begin;
    int alphabetSize = 0;
    for (int k = 0; k < fieldGetter.size(); k++) {
        for (int i = 0; i < size; i++) {
            int curSymbol = fieldGetter.getField(*(begin + i), k);
            if (alphabetSize <= curSymbol) {
                alphabetSize = curSymbol + 1;
            }
        }
    }
    std::vector <int> count;
    std::vector <typename FieldGetterClass::Type> order(size);

    for (int k = 0; k < fieldGetter.size(); k++) {
        count.assign(alphabetSize, 0);
        for (int i = 0; i < size; i++) {
            int curSymbol = fieldGetter.getField(*(begin + i), k);
            ++count[curSymbol];
        }
        for (int i = 1; i < alphabetSize; i++) {
            count[i] += count[i - 1];
        }
        for (int i = size - 1; i >= 0; i--) {
            int curSymbol = fieldGetter.getField(*(begin + i), k);
            order[--count[curSymbol]] = *(begin + i);
        }
        std::copy(order.begin(), order.end(), begin);
    }
}

#endif // RADIX_SORT_HPP_INCLUDED
