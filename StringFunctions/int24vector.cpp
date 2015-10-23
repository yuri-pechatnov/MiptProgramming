#include "int24vector.hpp"

namespace Int24VectorClasses {

    int Int24Assigner::operator=(int signedElement) {
        unsigned int element = signedElement;
        elementPointer[0] = element & 255;
        elementPointer[1] = (element >> 8) & 255;
        elementPointer[2] = (element >> 16) & 255;
        return signedElement;
    }


    Int24Assigner::operator int() const {
        return
            (int)elementPointer[0]
            | ((int)elementPointer[1] << 8)
            | ((int)elementPointer[2] << 16);
    }


    Int24Assigner::Int24Assigner(UnsignedChar* pointer): elementPointer(pointer) {}


    Int24Assigner Int24Vector::operator[](int position) {
        return Int24Assigner(data.data() + position * 3);
    }


    void Int24Vector::assign(size_t newSize, int element) {
        data.assign(newSize * 3, 0);
        for (int currentPosition = 0; currentPosition < (int)newSize;
                    currentPosition++) {
            (*this)[currentPosition] = element;
        }
    }


    Int24Vector::Int24Vector() {}

};
