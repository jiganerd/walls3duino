//
//  Renderer.cpp
//  walls3d
//
//  Created by Brian Dolan on 5/12/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#include <string.h>
#include "Renderer.hpp"
#include "GeomUtils.hpp"
#include "Utils.hpp"

constexpr uint8_t Renderer::ditherPattern8bit[];

Renderer::Renderer(uint8_t* pPixelBuf,
                   uint32_t screenWidth,
                   uint32_t screenHeight,
                   ColRenderedCbType colRenderedCb,
                   const Camera& camera):
    pPixelBuf{pPixelBuf},
    screenWidth{screenWidth},
    screenHeight{screenHeight},
    colRenderedCb{colRenderedCb},
    camera{camera}
{
}

void Renderer::BeginRender()
{
}

void Renderer::EndRender()
{
}

double Renderer::GetColumnHeightByDistance(double dist)
{
    return (30.0f / dist * static_cast<double>(screenHeight));
}

void Renderer::RenderColumn(uint32_t screenX, double height)
{
    double clippedHeight {(height <= screenHeight ? height : screenHeight)};
    double y1Float {static_cast<double>(screenHeight/2) - (clippedHeight / 2.0f)};
    
    uint32_t y1 {static_cast<uint32_t>(Rast(y1Float))};
    uint32_t y2 {static_cast<uint32_t>(Rast(y1Float + clippedHeight))};
    
    // figure out where in the pixel buffer to start, and only do
    // additions from there (avoid multiplication in the drawing loop)
    size_t pixelBufferOffset = 0;
    
    // draw a vertical line all the way through the column
    
    uint8_t ditherPatternIndex = (8 * clippedHeight / screenHeight);
    if (ditherPatternIndex > 7) ditherPatternIndex = 7;
    static uint8_t ditherPatternOffset = 0;
    uint16_t ditherPattern8 = ditherPattern8bit[ditherPatternIndex];
    uint16_t ditherPattern16 = (ditherPattern8 << 8) | ditherPattern8;
    uint8_t ditherPatternFinal = ditherPattern16 >> (ditherPatternOffset % 8);
    ditherPatternOffset += 5; // a weird odd/prime number here that doesn't easily match up with 8 makes for less pattern-y artifacts
    
    uint32_t pixel;
    uint8_t y = 0;
    uint32_t screenHeightPages = screenHeight / 8;
    for (uint32_t pageNum = 0; pageNum < screenHeightPages; pageNum++)
    {
        uint8_t pageData = 0;
        for (uint32_t i = 0; i < 8; i++)
        {
            if (y >= y1 && y < y2)
                pixel = 1;
            else
                pixel = 0;
            
            pageData |= (pixel << i);
            
            y++;
        }
        
        pageData &= ditherPatternFinal;
        pPixelBuf[pixelBufferOffset] = pageData;
        
        pixelBufferOffset++;
    }
    
    colRenderedCb();
}

// maps a range of [0.0, 1.0] to [0, rangeHigh]
// clamps at rangeHigh to account for floating point error
uint32_t Renderer::MapPercentageToRange(double percentage, uint32_t rangeHigh)
{
    uint32_t retVal {static_cast<uint32_t>(percentage * static_cast<double>(rangeHigh + 1))};
    if (retVal > rangeHigh) retVal = rangeHigh;
    return retVal;
}
