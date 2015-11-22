#include "symbol_pair_matcher.hpp"
#include "radix_sort.hpp"



PairSymbol::PairSymbol(int first_, int second_):
        first(first_), second(second_), initialPosition(-1) {}


int SymbolPairFieldGetter::size() const { return 2; }
int SymbolPairFieldGetter::getField(const Type &pairSymbol, int field) const {
    if (field == 0) {
        return pairSymbol.second;
    }
    return pairSymbol.first;
}

void SymbolPairMatcher::generateMatch(std::vector <PairSymbol> &source,
            std::vector <int> &positionToCategory) {
    positionToCategory.resize(source.size());
    for (int i = 0; i < (int)source.size(); i++) {
        source[i].initialPosition = i;
    }
    radixSort(source.begin(), source.end(), SymbolPairFieldGetter());
    int currentCategory = 0;
    for (int i = 0; i < (int)source.size(); i++) {
        currentCategory += (i != 0 && (source[i].first != source[i - 1].first
                    || source[i].second != source[i - 1].second));
        positionToCategory[source[i].initialPosition] = currentCategory;
    }
}
