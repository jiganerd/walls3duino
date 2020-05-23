//
//  Renderer.hpp
//  walls3d
//
//  Created by Brian Dolan on 5/12/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "Wall.hpp"
#include "Camera.hpp"

// abstract base class for BspRenderer and Raycaster, hosting some common functions
class Renderer
{
public:
    typedef void (*ColRenderedCbType)();

    Renderer(uint8_t* pPixelBuf,
             uint32_t screenWidth,
             uint32_t screenHeight,
             ColRenderedCbType colRenderedCb,
             const Camera& camera);
    ~Renderer() = default;
    
    virtual void RenderScene() = 0;

protected:
    void BeginRender();
    void EndRender();
    double GetColumnHeightByDistance(double dist);
    void RenderColumn(uint32_t screenX, uint8_t height);
    uint32_t MapPercentageToRange(double percentage, uint32_t rangeHigh);
    // this follows triangle rasterization rules described at
    // https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-rasterizer-stage-rules
    //inline static int32_t Rast(double n) { return static_cast<int32_t>(ceil(n - 0.5f)); };
    inline static int32_t Rast(double n) { return static_cast<int32_t>(n); };
    uint8_t GetClippedHeight(double height);
    
    uint8_t* pPixelBuf;
    const Camera& camera;
    
    const uint32_t screenWidth;
    const uint32_t screenHeight;

    ColRenderedCbType colRenderedCb;
    
    // dither pattern - see spreadsheet
    static constexpr uint8_t ditherPattern8bit[] = { /*0x00,*/ 0x80, 0x88, 0x92, 0xAA, 0xD5, 0xDB, 0xFB, 0xFF };
};

#endif /* Renderer_hpp */
