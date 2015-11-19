#ifndef SYMBOL_PAIR_MATCHER_HPP_INCLUDED
#define SYMBOL_PAIR_MATCHER_HPP_INCLUDED

#include <vector>

class PairSymbol {
  public:
    int first, second, initialPosition;
    PairSymbol(int first_ = 0, int second_ = 0);
};

class SymbolPairFieldGetter {
  public:
    typedef PairSymbol Type;
    inline int size() const;
    inline int getField(const Type &pairSymbol, int field) const;
};

class SymbolPairMatcher {
  public:
    void generateMatch(std::vector <PairSymbol> source,
            std::vector <int> positionToCategory);
};

#endif // SYMBOL_PAIR_MATCHER_HPP_INCLUDED
