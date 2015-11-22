#ifndef FARACH_HPP_INCLUDED
#define FARACH_HPP_INCLUDED

#include <vector>
#include <iostream>
#include <iomanip>

#include "rmq_delta1.hpp"

template<class T>
using Vector = std::vector<T>;


template<class T1, class T2>
using Pair = std::pair<T1, T2>;

class PairIntFirstGetter {
  public:
    typedef Pair<int, int> Type;
    inline int size() const;
    inline int getField(const Type &pairSymbol, int field) const;
};

class TrieNode;

class TrieEdge {
    friend class Trie;
    friend class FarachSuffixTrie;

    int firstLetter;
    TrieNode* to;
    TrieEdge(int firstLetter, TrieNode *to);
};

class TrieNode {
    friend class Trie;
    friend class FarachSuffixTrie;
    TrieNode *parent;
    int distance, id;
    int suffix;
    Vector <TrieEdge> edges;

    TrieNode(TrieNode *parent, int distance, int suffix);
  public:
    TrieNode();
};

class Trie {
    friend class FarachSuffixTrie;

    typedef Vector <int> IntVector;

    static const int outSpaceWidth = 4;
    void operator=(Trie b);
    Vector <TrieNode> nodes;
    int currentNodeNum;
    Vector<TrieNode*> idToNode;
    IntVector positionToId, positionToDepth,
        enterTime, idToEvenSuffix, idToOddSuffix, suffixToId,
        selfLCP;
    RMQDelta1 rmq;
    void eulerTourPrepare(TrieNode *root, int *position, int depth);
    void copySubtree(TrieNode *destination, TrieNode *source);

    void deployLinearLCA();
    int getLCAId(int firstNodeId, int secondNodeId);
    int getLCP(int firstSuffix, int secondSuffix);

  public:

    TrieNode *root;
    const IntVector *stringPointer;

    TrieNode *newTrieNode(TrieNode *parent, int distance, int suffix);

    void setSubtreeIds(TrieNode *root, int *currentId);
    void writeSubtreeSuffixArray(TrieNode *root, IntVector &suffixArray);
    void writeSuffixArray(IntVector &suffixArray);
    void setString(const IntVector &string);
    void buildFromSuffixArrayAndLCP(
            const IntVector &array, const IntVector &lcp);
    void showSubtree(TrieNode *root, std::ostream &out, int level);
    void swap(Trie &trie);
    void copyFrom(Trie &trie);
    friend std::ostream& operator<<(std::ostream &out, Trie &trie);
    Trie();
    ~Trie();
};


class FarachSuffixTrie {
    typedef Vector <int> IntVector;

    IntVector string;
    IntVector evenString;
    IntVector categoryToPosition;
    bool usedTerminalInEvenString;
    Trie evenTrie, oddTrie, badTrie, trie, goodTrie;
    // badTrie and trie have dangerous structure
    // and rely on evenTrie and oddTrie, don't touch them


    void unpairSubtree(Trie &source, Trie &unpaired,
            TrieNode *sourceRoot, TrieNode *unpairedRoot);
    void mergeSubtriesRoughly(Trie &destinationTrie, TrieNode *destination,
            TrieNode *source1, TrieNode *source2);
    void mergeSubtriesFinally(Trie &destinationTrie, Trie &badTrie,
        TrieNode *destination, TrieNode *source1, TrieNode *source2);



    void buildEvenString();
    void buildEvenTrie();
    void unpairEvenTrie();
    void buildOddSuffixTree();
    void mergeTriesRoughly();
    void mergeTriesFinally();
    void improveTrie();

  public:

    static void suffixArrayToLCP(const IntVector &string,
                    const IntVector &array, IntVector &lcp);
    static void partialSuffixArrayToLCP(const IntVector &string,
                    const IntVector &array, IntVector &lcp);
    void setString(const IntVector &string);
    void calculate();
    Trie &getTrie();
    void testAndShow(const IntVector &string, std::ostream &out);
};


#endif // FARACH_HPP_INCLUDED
