//
//  Game.cpp
//  walls3d
//
//  Created by Brian Dolan on 5/12/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#include "Game.hpp"
#include "BspTreeBin.hpp"

constexpr Wall Game::walls[];

Game::Game(uint8_t* pPixelBuf,
           uint8_t screenWidth,
           uint8_t screenHeight,
           Renderer::ColRenderedCbType colRenderedCb):
    camera({60.0f, 15.0f}),
    bspr(pPixelBuf, screenWidth, screenHeight, colRenderedCb, camera)
    //rc(pPixelBuf, screenWidth, screenHeight, colRenderedCb, camera, walls, 9)
{
    //bspr.LoadBin(basicAreaBspTree);
    bspr.LoadBin(smileyFaceBspTree);
}

void Game::ProcessFrame()
{
    bspr.RenderScene();
    //rc.RenderScene();
}

void Game::RotateCamera(double angleRad)
{
    camera.Rotate(angleRad);
}

void Game::MoveCamera(double distance)
{
    camera.MoveForward(distance);
}

void Game::StrafeCamera(double distanceToRight)
{
    camera.Strafe(distanceToRight);
}
