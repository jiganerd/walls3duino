//
//  Line.hpp
//  walls3d
//
//  Created by Brian Dolan on 5/11/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Line_hpp
#define Line_hpp

#include "Vec2.hpp"
#include "Serializer.hpp"

class Line
{
public:
    constexpr Line():
        p1{0.0f, 0.0f},
        p2{0.0f, 0.0f}
    {
    }
    constexpr Line(const Vec2& p1, const Vec2& p2):
        p1{p1},
        p2{p2}
    {
    }
    Line(const uint8_t* bytes, size_t& offset):
        p1{Serializer::DeSerDouble(bytes, offset), Serializer::DeSerDouble(bytes, offset)},
        p2{Serializer::DeSerDouble(bytes, offset), Serializer::DeSerDouble(bytes, offset)}
    {
    }
    Line operator+(const Vec2& rhs) const
    {
        return {p1 + rhs, p2 + rhs};
    }
    Line& operator+=(const Vec2& rhs)
    {
        *this = *this + rhs;
        return *this;
    }
    Line operator-(const Vec2& rhs) const
    {
        return {p1 - rhs, p2 - rhs};
    }
    Line& operator-=(const Vec2& rhs)
    {
        *this = *this - rhs;
        return *this;
    }
    double Mag() const
    {
        return ((p2 - p1).Mag());
    }
    
    Vec2 p1;
    Vec2 p2;
};

#endif /* Line_hpp */
