//
//  fixed.h
//  walls3d
//
//  Created by Brian Dolan on 4/18/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef fixed_h
#define fixed_h

#include <stdint.h>

#define ACTUALLY_FIXED 0

#if ACTUALLY_FIXED

// TODO: if this was a class rather than a typedef, we could explicitly disallow implicit casts to double (developer mistakes)
// also disallow implicit multiplication and division
typedef int32_t fixed_t;

// resolution of a fixed-point number (the amount that the LSB in the fractional part represents)
constexpr double fixedRes { static_cast<double>(static_cast<int32_t>(1) << 16) };

constexpr double UNFIXED(fixed_t n)
{
    return (static_cast<double>(n) / fixedRes);
}

constexpr fixed_t FIXED(double n)
{
    // round() would be better than truncation, but can't easily in a constexpr function
    // would be good to assert n is not out of bounds
    return static_cast<fixed_t>(n * fixedRes);
}

constexpr fixed_t FIXED_MULT(fixed_t n1, fixed_t n2)
{
    // TODO: apply UNFIXED() to the number that will yield better precision
    return (UNFIXED(n1) * n2);
}

constexpr fixed_t FIXED_DIV(fixed_t n1, fixed_t n2)
{
    return (n1 / UNFIXED(n2));
}

#else

typedef double fixed_t;
constexpr double UNFIXED(fixed_t n) { return n; }
constexpr fixed_t FIXED(double n) { return n; }
constexpr fixed_t FIXED_MULT(fixed_t n1, fixed_t n2) { return (n1 * n2); }
constexpr fixed_t FIXED_DIV(fixed_t n1, fixed_t n2) { return (n1 / n2); }

#endif

#endif /* fixed_h */
