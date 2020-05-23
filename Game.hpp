//
//  Game.hpp
//  walls3d
//
//  Created by Brian Dolan on 5/12/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Game_hpp
#define Game_hpp

#include <stdint.h>
#include "Wall.hpp"
#include "Line.hpp"
#include "Camera.hpp"
#include "BspRenderer.hpp"
#include "Raycaster.hpp"

class Game
{
public:
    Game(uint8_t* pPixelBuf,
         uint32_t screenWidth,
         uint32_t screenHeight,
         Renderer::ColRenderedCbType colRenderedCb);
    void ProcessFrame();
    void ToggleRenderers();
    void RotateCamera(double angleRad);
    void MoveCamera(double distance);
    void StrafeCamera(double distanceToRight);
    
private:
    static constexpr Wall walls[]
    {
        // area bounding box
        { { {  10.0f,  10.0f }, { 210.0f,  10.0f } } },
        { { { 210.0f,  10.0f }, { 210.0f, 210.0f } } },
        { { { 210.0f, 210.0f }, {  10.0f, 210.0f } } },
        { { {  10.0f, 210.0f }, {  10.0f,  10.0f } } },
        // standalone wall
        { { { 190.0f, 190.0f }, { 140.0f, 170.0f } } },
        // interior "box"
        { { {  20.0f,  60.0f }, {  40.0f,  90.0f } } },
        { { {  40.0f,  90.0f }, {  70.0f,  70.0f } } },
        { { {  70.0f,  70.0f }, {  50.0f,  40.0f } } },
        { { {  50.0f,  40.0f }, {  20.0f,  60.0f } } },
    };

    Camera camera;
    BspRenderer bspr;
    //Raycaster rc;
};

#endif /* Game_hpp */
