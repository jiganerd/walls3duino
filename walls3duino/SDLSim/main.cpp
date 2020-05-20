//
//  main.cpp
//  walls3d
//
//  Created by Brian Dolan on 5/9/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

// TODO:
// use "horizontal center of column" for x scans in both bsp and raycast modes and "pre-step" into angles

#include <iostream>
#include "Game.hpp"
#include "Graphics.hpp"
#include "Input.hpp"
#include "FrameRateMgr.hpp"

int main(int argc, const char * argv[])
{
    Graphics graphics;
    Game game(graphics.GetScreenBuffer(), graphics.ScreenWidth, graphics.ScreenHeight);
    Input input;
    FrameRateMgr frm(true);
    
    bool quit {false};
    while (!quit)
    {
        quit = input.ProcessKeys();
        
        if (!quit)
        {
            graphics.BeginFrame();
            game.ProcessFrame();
            graphics.EndFrame();
            
            double moveSpeed {frm.GetFrameTimeSecs() * 75.0f};
            double rotSpeed {frm.GetFrameTimeSecs() * M_PI / 2.0f};
            
            if (input.GetMoveForward())  game.MoveCamera(moveSpeed);
            if (input.GetMoveBackward()) game.MoveCamera(-moveSpeed);
            if (input.GetRotateLeft())   game.RotateCamera(-rotSpeed);
            if (input.GetRotateRight())  game.RotateCamera(rotSpeed);
            if (input.GetStrafeLeft())   game.StrafeCamera(-moveSpeed);
            if (input.GetStrafeRight())  game.StrafeCamera(moveSpeed);
            
            if (input.GetTabFirstPressed())
                game.ToggleRenderers();
            
            frm.Mark();
        }
    }
    
    return 0;
}