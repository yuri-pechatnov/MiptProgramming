#ifndef INT24VECTOR_HPP_INCLUDED
#define INT24VECTOR_HPP_INCLUDED


#include <cstdlib>
#include <stdint.h>
#include <vector>

namespace Int24VectorClasses {

    typedef unsigned char UnsignedChar;
    typedef std::vector <UnsignedChar> UnsignedCharVector;
    class Int24Assigner {
        friend class Int24Vector;
        UnsignedChar *elementPointer;
      public:
        int operator=(int x);
        operator int() const;
      private:
        Int24Assigner(UnsignedChar* pointer);
    };

    class Int24Vector {
        UnsignedCharVector data;
      public:
        Int24Assigner operator[](int position);
        void assign(size_t newSize, int element);
        Int24Vector();
    };
};

using Int24VectorClasses::Int24Vector;



#endif // INT24VECTOR_HPP_INCLUDED
