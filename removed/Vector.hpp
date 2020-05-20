//
//  Vector.hpp
//  walls3duino
//
//  Created by Brian Dolan on 5/19/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Vector_hpp
#define Vector_hpp

#include <stddef.h>
#include <string.h>

template <typename T>
class Vector
{
public:
    Vector():
        size{0},
        capacity{10},
        pData{new T[capacity]}
    {}
    Vector(const Vector<T>& rhs):
        size{rhs.size},
        capacity{rhs.capacity},
        pData{new T[capacity]}
    {
        // TODO: revisit when implementing resizing
        memcpy(pData, rhs.pData, sizeof(T) * rhs.capacity);
    }
    ~Vector()
    {
        delete[] pData;
    }
    void PushBack(const T& value)
    {
        //if (size == capacity)
        //    throw "uh oh";
        
        //    capacity *= 2;
        // realloc()
        
        pData[size++] = value;
    }
    //void PushBack(T&& value);
    size_t Size() const { return size; }
    void Erase(size_t i)
    {
        for (size_t j = i; j < (size - 1); j++)
            pData[j] = pData[j + 1];
        
        size--;
    }
    void Clear()
    {
        size = 0;
    }
    T& operator[](size_t i) const
    {
        return pData[i];
    }
    Vector<T>& operator=(const Vector<T>& rhs)
    {
        // TODO: revisit when implementing resizing
        memcpy(pData, rhs.pData, sizeof(T) * rhs.capacity);
        
        size = rhs.size;
        capacity = rhs.capacity;
        
        return *this;
    }
    
private:
    size_t size;
    size_t capacity;
    T* pData;
};

#endif /* Vector_hpp */
