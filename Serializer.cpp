//
//  Serializer.cpp
//  walls3duino
//
//  Created by Brian Dolan on 5/21/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef SDLSim // should be set as a compiler flag on simulation builds
#include <avr/pgmspace.h>
#endif
#include "Serializer.hpp"

Serializer::Fixed::Fixed(double number):
    // TODO: round() would be better than truncation
    // TODO: assert n is not out of bounds
    encoding{static_cast<int32_t>(number * res)}
{
}

Serializer::Fixed::Fixed(int32_t encoding):
    encoding{encoding}
{
}

double Serializer::Fixed::Unfixed() const
{
    return Unfixed(encoding);
}

double Serializer::DeSerDouble(const uint8_t* bytes, size_t& offset)
{
    return Fixed(DeSerInt(bytes, offset)).Unfixed();
}

int32_t Serializer::DeSerInt(const uint8_t* bytes, size_t& offset)
{
    return static_cast<int32_t>(DeSerUint(bytes, offset));
}

uint32_t Serializer::DeSerUint(const uint8_t* bytes, size_t& offset)
{
    uint32_t ret = FromBytes(bytes + offset);
    offset += 4;
    return ret;
}

int32_t Serializer::PeekInt(const uint8_t* bytes, size_t offset)
{
    return FromBytes(bytes + offset);
}

uint32_t Serializer::FromBytes(const uint8_t* bytes)
{
#ifdef SDLSim
    const uint8_t* ramBytes = bytes;
#else
    uint8_t ramBytes[4];
    for (uint32_t i = 0; i < 4; i++)    
        ramBytes[i] = pgm_read_byte_near(bytes + i);
#endif
    
    return
        (static_cast<uint32_t>(ramBytes[3]) << 0)  |
        (static_cast<uint32_t>(ramBytes[2]) << 8)  |
        (static_cast<uint32_t>(ramBytes[1]) << 16) |
        (static_cast<uint32_t>(ramBytes[0]) << 24);
}
