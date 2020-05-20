//
//  Utils.hpp
//  walls3d
//
//  Created by Brian Dolan on 4/28/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

//#include <algorithm>
#include <math.h>

class Utils
{
public:
    Utils() = delete;
    ~Utils() = delete;
    
    template <typename T>
    static void Clamp(T& n, const T& min, const T& max)
    {
        if (n < min) n = min;
        else if (n > max) n = max;
    };
    
    template <typename T>
    static void NormalizeAngle(T& a)
    {
        constexpr T TwoPi {static_cast<T>(2) * M_PI};
        while (a > TwoPi)
            a -= TwoPi;
        while (a < static_cast<T>(0))
            a += TwoPi;
    };
};

#endif /* Utils_hpp */
