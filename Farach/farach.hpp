#ifndef FARACH_HPP_INCLUDED
#define FARACH_HPP_INCLUDED


class FarachSuffixTree {
    typedef std::vector <int> IntVector;

    IntVector string;
    IntVector evenstring;


    void buildEvenString();
    void buildEvenTree();


  public:
    void setString(const IntVector &string)
};


#endif // FARACH_HPP_INCLUDED
