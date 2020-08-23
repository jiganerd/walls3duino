//
//  BspTreeBin.hpp
//  walls3duino
//
//  Created by Brian Dolan on 5/22/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef BspTreeBin_hpp
#define BspTreeBin_hpp

#ifdef SDLSim // should be set as a compiler flag on simulation builds
#define PROGMEM
#else
#include <avr/pgmspace.h>
#endif

extern const unsigned char smileyFaceBspTree[] PROGMEM;

#endif /* BspTreeBin_hpp */
