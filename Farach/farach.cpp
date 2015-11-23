#include <iostream>
#include <iomanip>
#include <cassert>

#include "symbol_pair_matcher.hpp"

#include "rmq_delta1.hpp"
#include "farach.hpp"
#include "radix_sort.hpp"

using std::cout;
using std::endl;
using std::setw;
using std::ostream;
using std::swap;


inline int PairIntFirstGetter::size() const {
    return 1;
}
inline int PairIntFirstGetter::getField(const Type &object, int field) const {
    return object.first;
}


TrieEdge::TrieEdge(int firstLetter_, TrieNode *to_):
        firstLetter(firstLetter_), to(to_) {}

TrieNode::TrieNode(TrieNode *parent_, int distance_, int suffix_):
    parent(parent_), distance(distance_), id(-1), suffix(suffix_) {}


TrieNode::TrieNode():
    parent(NULL), distance(-1), id(-1) {}


TrieNode* Trie::newTrieNode(TrieNode *parent, int distance, int suffix) {
    assert(currentNodeNum < (int)nodes.size() && currentNodeNum >= 0);
    return &(nodes[currentNodeNum++] = TrieNode(parent, distance, suffix));
}

void Trie::setSubtreeIds(TrieNode *root, int *currentId) {
    root->id = (*currentId)++;
    for (int i = 0; i < (int)root->edges.size(); i++) {
        setSubtreeIds(root->edges[i].to, currentId);
    }
}


void Trie::eulerTourPrepare(TrieNode *root, int *position, int depth) {
    if (root->distance + root->suffix == (int)stringPointer->size()) {
        suffixToId[root->suffix] = root->id;
    }
    enterTime[root->id] = *position;
    positionToDepth[*position] = depth;
    positionToId[*position] = root->id;
    (*position)++;
    for (int i = 0; i < (int)root->edges.size(); i++) {
        eulerTourPrepare(root->edges[i].to, position, depth + 1);
        positionToDepth[*position] = depth;
        positionToId[*position] = root->id;
        (*position)++;
    }
}



void Trie::deployLinearLCA() {
    int nodeCount = 0, eulerSize;
    setSubtreeIds(root, &nodeCount);
    eulerSize = nodeCount * 2 - 1;
    positionToId.resize(eulerSize);
    positionToDepth.resize(eulerSize);
    enterTime.resize(nodeCount);
    suffixToId.assign(stringPointer->size(), -1);
    selfLCP.assign(nodeCount, -1);
    selfLCP[root->id] = 0;
    int eulerCurrentSize = 0;
    eulerTourPrepare(root, &eulerCurrentSize, 0);
    assert(eulerSize == eulerCurrentSize);
    rmq.setArray(positionToDepth);
    for (int i = 0; i < (int)stringPointer->size(); i++) {
        assert(suffixToId[i] >= 0);
    }
}
int Trie::getLCAId(int first, int second) {
    int firstTime = enterTime[first], secondTime = enterTime[second];
    if (firstTime > secondTime) {
        std::swap(firstTime, secondTime);
    }
    return positionToId[rmq.calculateMinimumPosition(
        firstTime, secondTime + 1
    )];
}

int Trie::getLCP(int firstSuffix, int secondSuffix) {
    if ((*stringPointer)[firstSuffix] != (*stringPointer)[secondSuffix]) {
        return 0;
    }
    int first = suffixToId[firstSuffix], second = suffixToId[secondSuffix];
    int lca = getLCAId(first, second);
    if (selfLCP[lca] != -1)
        return selfLCP[lca];
    return selfLCP[lca] = 1 + getLCP(firstSuffix + 1, secondSuffix + 1);
}

void Trie::writeSubtreeSuffixArray(TrieNode *root, IntVector&suffixArray) {
    if (root->edges.size() == 0) {
        suffixArray.push_back(root->suffix);
    } else {
        for (int i = 0; i < (int)root->edges.size(); i++) {
            writeSubtreeSuffixArray(root->edges[i].to, suffixArray);
        }
    }
}

void Trie::writeSuffixArray(IntVector &suffixArray) {
    writeSubtreeSuffixArray(root, suffixArray);
}

void Trie::setString(const IntVector &string) {
    stringPointer = &string;
    nodes.resize(string.size() * 3);
    currentNodeNum = 0;
    root = newTrieNode(NULL, 0, 0);
}

void Trie::buildFromSuffixArrayAndLCP(
                    const IntVector &array, const IntVector &lcp) {
    assert(stringPointer);
    const IntVector &string = *stringPointer;
    TrieNode *current = root, *inserted, *parent;
    for (int i = 0; i < (int)array.size(); i++) {
        int lcpWithPrevious = 0;
        if (i != 0) {
            lcpWithPrevious = lcp[i - 1];
        }
        while (current->distance != 0
                && current->distance != lcpWithPrevious) {
            parent = current->parent;
            if (current->parent->distance >= lcpWithPrevious) {
                current = parent;
                continue;
            }
            inserted = newTrieNode(parent, lcpWithPrevious, array[i]);
            parent->edges.back().to = inserted;
            inserted->edges.push_back(TrieEdge(
                string[current->suffix + inserted->distance], current));
            current->parent = inserted;
            current = inserted;
        }
        inserted = newTrieNode(current, string.size() - array[i], array[i]);
        current->edges.push_back(
            TrieEdge(string[array[i] + current->distance], inserted));
        current = inserted;
    }
}

void Trie::showSubtree(TrieNode *root, ostream &out, int level) {
    out << setw(level * outSpaceWidth) << ' ';
    int begin = root->suffix;
    if (root->parent != NULL) {
        begin += root->parent->distance;
    }
    for (int i = begin; i < root->suffix + root->distance; i++) {
        out << (*stringPointer)[i];
    }
    out << ": id = " << root->id << ", suffix = " << root->suffix
            << ", distance = " << root->distance
            << ", parent = " << (root->parent ? root->parent->id : -1);
    out << endl;
    for (int i = 0; i < (int)root->edges.size(); i++) {
        //out << root->edges[i].firstLetter << endl;
        showSubtree(root->edges[i].to, out, level + 1);
    }
}

ostream& operator<<(ostream &out, Trie &trie) {
    out << "Show tree:" << endl;
    int currentId = 0;
    trie.setSubtreeIds(trie.root, &currentId);
    trie.showSubtree(trie.root, out, 0);
    return out;
}

Trie::Trie() {
    currentNodeNum = 0;
    stringPointer = NULL;
}

Trie::~Trie() {
}

void Trie::copySubtree(TrieNode *destination, TrieNode *source) {
    destination->distance = source->distance;
    destination->id = source->id;
    destination->suffix = source->suffix;
    for (int i = 0; i < (int)source->edges.size(); i++) {
        TrieNode *current = newTrieNode(destination, 0, 0);
        destination->edges.push_back(
            TrieEdge(source->edges[i].firstLetter, current));
        copySubtree(current, source->edges[i].to);
    }
}

void Trie::copyFrom(Trie &trie) {
    setString(*trie.stringPointer);
    copySubtree(root, trie.root);
}

void Trie::swap(Trie &trie) {
    std::swap(nodes, trie.nodes);
    std::swap(currentNodeNum, trie.currentNodeNum);
    std::swap(root, trie.root);
    std::swap(stringPointer, trie.stringPointer);
}

void FarachSuffixTrie::suffixArrayToLCP(const IntVector &string,
                        const IntVector &array, IntVector &lcp) {
    partialSuffixArrayToLCP(string, array, lcp);
   // return;
    int length = string.size();
    lcp.assign(length, 0);
    Vector <int> position(length);
    for (int i = 0; i < length; i++) {
        position[array[i]] = i;
    }
    int current = 0;
    for (int i = 0; i < length; i++) {
        if (current > 0) {
            current--;
        }
        if (position[i] == length - 1) {
            lcp[length - 1] = -1;
            current = 0;
        } else {
            int j = array[position[i] + 1];
            while (std::max(i + current, j + current) < length
                        && string[i + current] == string[j + current]) {
                current++;
            }
            lcp[position[i]] = current;
        }
    }
}

void FarachSuffixTrie::partialSuffixArrayToLCP(const IntVector &string,
                        const IntVector &array, IntVector &lcp) {
    lcp.assign(array.size(), 0);
    Vector <int> position(string.size(), -1);
    for (int i = 0; i < (int)array.size(); i++) {
        position[array[i]] = i;
    }
    int current = 0;
    for (int i = 0; i < (int)string.size(); i++) {
        if (current > 0) {
            current--;
        }
        if (position[i] == -1) {
            continue;
        }
        if (position[i] == (int)array.size() - 1) {
            lcp[array.size() - 1] = -1;
            current = 0;
        } else {
            int j = array[position[i] + 1];
            while (std::max(i + current, j + current) < (int)string.size()
                        && string[i + current] == string[j + current]) {
                current++;
            }
            lcp[position[i]] = current;
        }
    }
}


void FarachSuffixTrie::buildEvenString() {
    Vector <PairSymbol> paired;
    for (int i = 0; i + 1 < (int)string.size(); i += 2) {
        paired.push_back(PairSymbol(string[i], string[i + 1]));
    }
    if (string.size() % 2 == 1) {
        paired.push_back(PairSymbol(*(string.end() - 1), 0));
    }
    SymbolPairMatcher().generateMatch(paired, evenString);
    for (int i = 0; i < (int)evenString.size(); i++) {
        ++evenString[i];
    }
    assert(evenString.size() * 2 < string.size() + 5);
}

void FarachSuffixTrie::buildEvenTrie() {
    IntVector evenLcp;
    if (evenString.size() > 2) {
        FarachSuffixTrie calculator;
        calculator.setString(evenString);
        calculator.calculate();
        calculator.getTrie().writeSuffixArray(evenSuffixArray);
        evenSuffixArray.erase(evenSuffixArray.begin());
    } else {
        evenSuffixArray.assign(evenString.size(), 0);
        if (evenString.size() == 2) {
            if (evenString[0] < evenString[1]) {
                evenSuffixArray[1] = 1;
            } else {
                evenSuffixArray[0] = 1;
            }
        }
        //calculateSuffixArray(evenString, evenSuffixArray);
    }
    for (int i = 0; i < evenSuffixArray.size(); i++) {
        evenSuffixArray[i] *= 2;
        assert(evenSuffixArray[i] < string.size());
    }
    partialSuffixArrayToLCP(string, evenSuffixArray, evenLcp);
    evenTrie.setString(string);
    evenTrie.buildFromSuffixArrayAndLCP(evenSuffixArray, evenLcp);
}

void FarachSuffixTrie::buildOddSuffixTree() {
    IntVector oddSuffixArray, oddLCP;
    Vector<Pair<int, int>> oddSuffixes;
    oddSuffixes.reserve(evenSuffixArray.size());
    for (int i = 0; i < (int)evenSuffixArray.size(); i++) {
        int position = evenSuffixArray[i] - 1;
        if (position >= 0) {
            oddSuffixes.push_back(Pair<int, int>(string[position], position));
        }
    }
    radixSort(oddSuffixes.begin(), oddSuffixes.end(), PairIntFirstGetter());
    oddSuffixArray.resize(oddSuffixes.size());
    for (int i = 0; i < (int)oddSuffixes.size(); i++) {
        oddSuffixArray[i] = oddSuffixes[i].second;
    }
    if (string.size() % 2 == 0) {
        oddSuffixArray.insert(oddSuffixArray.begin(), string.size() - 1);
    }
    partialSuffixArrayToLCP(string, oddSuffixArray, oddLCP);
    oddTrie.setString(string);
    oddTrie.buildFromSuffixArrayAndLCP(oddSuffixArray, oddLCP);
}

void FarachSuffixTrie::mergeSubtriesRoughly(Trie &trie,
        TrieNode *destination, TrieNode *source1, TrieNode *source2) {
    Vector <TrieEdge> &edges1 = source1->edges, &edges2 = source2->edges,
        &destinationEdges = destination->edges;
    Vector <TrieEdge>::iterator edge1 = edges1.begin(), edge2 = edges2.begin();
    while (edge1 != edges1.end() || edge2 != edges2.end()) {
        if (edge1 != edges1.end() && edge2 != edges2.end()
                && edge1->firstLetter == edge2->firstLetter) {
            TrieNode *newSource1 = edge1->to,
                     *newSource2 = edge2->to;
            if (newSource1->distance < newSource2->distance) {
                std::swap(newSource1, newSource2);
            }
            if (newSource1->distance != newSource2->distance) {
                TrieNode *inserted = trie.newTrieNode(
                    NULL, newSource2->distance, newSource2->suffix);
                inserted->edges.push_back(TrieEdge(string[newSource1->suffix
                    + newSource2->distance], newSource1));
                newSource1 = inserted;
            }
            TrieNode *inserted = trie.newTrieNode(
                destination, newSource2->distance, newSource2->suffix);
            if (newSource1->edges.size() == 0) {
                inserted->suffix = newSource1->suffix;
            }
            destinationEdges.push_back(TrieEdge(edge1->firstLetter, inserted));
            mergeSubtriesRoughly(trie, inserted, newSource1, newSource2);
            edge1++;
            edge2++;
        } else if (edge1 != edges1.end() && (edge2 == edges2.end()
                || edge1->firstLetter < edge2->firstLetter)) {
            destinationEdges.push_back(*edge1);
            edge1++;
        } else {
            destinationEdges.push_back(*edge2);
            edge2++;
        }
    }
}


void FarachSuffixTrie::mergeSubtriesFinally(Trie &trie, Trie &badTrie,
        TrieNode *destination, TrieNode *source1, TrieNode *source2) {
    Vector <TrieEdge> &edges1 = source1->edges, &edges2 = source2->edges,
        &destinationEdges = destination->edges;
    Vector <TrieEdge>::iterator edge1 = edges1.begin(), edge2 = edges2.begin();
    while (edge1 != edges1.end() || edge2 != edges2.end()) {
        if (edge1 != edges1.end() && edge2 != edges2.end()
                && edge1->firstLetter == edge2->firstLetter) {
            TrieNode *newSource1 = edge1->to,
                     *newSource2 = edge2->to;
            if (newSource1->distance < newSource2->distance) {
                std::swap(newSource1, newSource2);
            }
            int lcp = badTrie.getLCP(newSource1->suffix, newSource2->suffix);
            assert(lcp >= destination->distance);

            assert(string[newSource1->suffix + lcp]
                != string[newSource2->suffix + lcp]);
            if (lcp < newSource2->distance) {
                TrieNode *inserted = trie.newTrieNode(
                    destination, lcp, newSource2->suffix);
                inserted->edges.push_back(TrieEdge(string[newSource1->suffix
                    + lcp], newSource1));
                inserted->edges.push_back(TrieEdge(string[newSource2->suffix
                    + lcp], newSource2));
                if (inserted->edges[0].firstLetter
                    > inserted->edges[1].firstLetter) {
                    std::swap(inserted->edges[0], inserted->edges[1]);
                }
                destinationEdges.push_back(TrieEdge(edge1->firstLetter, inserted));
                edge1++;
                edge2++;
                continue;
            }

            if (newSource1->distance != newSource2->distance) {
                TrieNode *inserted = trie.newTrieNode(
                    destination, newSource2->distance, newSource2->suffix);
                inserted->edges.push_back(TrieEdge(string[newSource1->suffix
                    + newSource2->distance], newSource1));
                newSource1 = inserted;
            }

            TrieNode *inserted = trie.newTrieNode(
                destination, newSource2->distance, newSource2->suffix);
            destinationEdges.push_back(TrieEdge(edge1->firstLetter, inserted));
            mergeSubtriesFinally(
                trie, badTrie, inserted, newSource1, newSource2);
            edge1++;
            edge2++;
            continue;
        }
        if (edge1 != edges1.end() && (edge2 == edges2.end()
                || edge1->firstLetter < edge2->firstLetter)) {
            destinationEdges.push_back(*edge1);
            edge1++;
        } else {
            destinationEdges.push_back(*edge2);
            edge2++;
        }
    }
}



void FarachSuffixTrie::mergeTriesRoughly() {
    badTrie.setString(string);
    mergeSubtriesRoughly(badTrie, badTrie.root, oddTrie.root, evenTrie.root);
}

void FarachSuffixTrie::mergeTriesFinally() {
    trie.setString(string);
    mergeSubtriesFinally(trie, badTrie, trie.root, oddTrie.root, evenTrie.root);
}

void FarachSuffixTrie::improveTrie() {
    goodTrie.copyFrom(trie);
}


void FarachSuffixTrie::setString(const IntVector &string_) {
    string = string_;
    string.push_back(0);
}

void FarachSuffixTrie::calculate() {
    buildEvenString();
    buildEvenTrie();
    buildOddSuffixTree();
    mergeTriesRoughly();
    badTrie.deployLinearLCA();
    mergeTriesFinally();
    improveTrie();
}

Trie &FarachSuffixTrie::getTrie() {
    return goodTrie;
}

void FarachSuffixTrie::testAndShow(const IntVector &string_, ostream &out) {
    #ifndef DEBUG
        return;
    #endif
    setString(string_);
    buildEvenString();
    for (int i = 0; i < (int)string.size(); i++)
        out << setw(2) << string[i];
    out << endl;
    for (int i = 0; i < (int)evenString.size(); i++)
        out << setw(2) << evenString[i] << setw(2) << ' ';
    out << endl;
    buildEvenTrie();
    out << "EvenTree: " << evenTrie << endl;
    buildOddSuffixTree();
    out << "OddTree: " << oddTrie << endl;
    {
        mergeTriesRoughly();
        Trie ata;
        ata.copyFrom(badTrie);
        out << "BadTrie: " << endl << badTrie << endl;
    }
    badTrie.deployLinearLCA();
    mergeTriesFinally();
    out << "Trie: " << endl << trie << endl;
    improveTrie();
    out << "GoodTrie: " << endl << goodTrie << endl;
    {
        IntVector a, b, str(string.size());
        trie.writeSuffixArray(a);
        out << "Farach:" << endl;
        for (int i = 0; i < (int)a.size(); i++)
            out << setw(3) << a[i] << " ";
        out << endl;
    }
}


