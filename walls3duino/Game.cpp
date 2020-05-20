//
//  Game.cpp
//  walls3d
//
//  Created by Brian Dolan on 5/12/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#include "Game.hpp"

constexpr Line Game::worldBounds[];
constexpr Wall Game::walls[];

Game::Game(uint8_t* pPixelBuf,
           uint32_t screenWidth,
           uint32_t screenHeight):
    camera({60.0f, 15.0f}),
    //bspr(pPixelBuf, screenWidth, screenHeight, camera, worldBounds, 4),
    rc(pPixelBuf, screenWidth, screenHeight, camera, walls, 9, worldBounds, 4),
    pActiveRenderer{&rc}
{
    //bspr.ProcessWalls(walls, 9);
}

void Game::ProcessFrame()
{
    pActiveRenderer->RenderScene();
}

void Game::ToggleRenderers()
{
//    if (pActiveRenderer == &bspr)
//        pActiveRenderer = &rc;
//    else
//        pActiveRenderer = &bspr;
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
