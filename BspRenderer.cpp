//
//  BspRenderer.cpp
//  walls3duino
//
//  Created by Brian Dolan on 5/15/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#include <string.h>
#include "BspRenderer.hpp"
#include "GeomUtils.hpp"

BspRenderer::BspRenderer(uint8_t* pPixelBuf,
                         uint32_t screenWidth,
                         uint32_t screenHeight,
                         ColRenderedCbType colRenderedCb,
                         const Camera& camera):
    Renderer(pPixelBuf, screenWidth, screenHeight, colRenderedCb, camera),
    cameraNodeIndex{-1},
    pHeightBuffer{new uint8_t[screenWidth]}
{
}

BspRenderer::~BspRenderer()
{
    delete[] pHeightBuffer;
}

void BspRenderer::LoadBin(const uint8_t* bytes)
{
    bspTree.LoadBin(bytes);
    cameraNodeIndex = bspTree.Find(camera.location);
}

void BspRenderer::RenderScene()
{
    BeginRender();
    memset(pHeightBuffer, 0, screenWidth * sizeof(uint8_t));
    bspTree.TraverseRender(camera.location, RenderWallStatic, this);
    for (uint32_t x = 0; x < screenWidth; x++)
      RenderColumn(x, pHeightBuffer[x]);
    EndRender();
    
    // technically this is not "rendering", but... sue me
    cameraNodeIndex = bspTree.Find(camera.location);
}

void BspRenderer::RenderWallStatic(const Wall& wall, const BspTree::BspNodeDebugInfo& debugInfo, void* bspRenderer)
{
    static_cast<BspRenderer*>(bspRenderer)->RenderWall(wall, debugInfo);
}

void BspRenderer::RenderWall(const Wall &wall, const BspTree::BspNodeDebugInfo& debugInfo)
{
    // but this doesn't mean that the wall is "in front of" the camera
    // as per the camera's view direction
    // cull any walls that are entirely behind the camera to reduce processing
    if (!camera.IsBehind(wall.seg))
    {
        // at this point, we can safely assume that the p2 vertex of the wall is to the right
        // of the p1 vertex in screen coordinates!
        
        bool p1IsOnScreen {false}, p2IsOnScreen {false};
        uint32_t screenXP1, screenXP2;
        double distP1, distP2;
        
        // get properties for screen x and distance for each vertex, with clipping
        p1IsOnScreen = ClipAndGetAttributes(true, wall.seg, screenXP1, distP1);
        p2IsOnScreen = ClipAndGetAttributes(false, wall.seg, screenXP2, distP2);
        
        // if both (clipped) vertices are on screen, fill in the middle
        if (p1IsOnScreen && p2IsOnScreen)
        {
            double columnHeightP1 {GetColumnHeightByDistance(distP1)};
            double columnHeightP2 {GetColumnHeightByDistance(distP2)};
            
            double columnHeight {columnHeightP1};
            uint32_t screenXDifference {screenXP2 - screenXP1};
            double columnHeightIncrement {(columnHeightP2 - columnHeightP1) / static_cast<double>(screenXDifference)};
            
            for (uint32_t screenX = screenXP1; screenX <= screenXP2; screenX++)
            {
                if (pHeightBuffer[screenX] == 0)
                    pHeightBuffer[screenX] = GetClippedHeight(columnHeight);

                columnHeight += columnHeightIncrement;
            }
        }
        // else the wall is in front of the camera, but entirely outside the field of view to the right
        // or the left
    }
}

// (angleFromCamera could be figured out, but it is passed in for efficiency, as it has already been calculated)
double BspRenderer::GetPerpendicularDistanceFromCameraByAngle(const Vec2& point, double angleFromCamera)
{
    double euclideanDistance {(point - camera.location).Mag()};
    return euclideanDistance * cos(angleFromCamera);
}

double BspRenderer::GetAngleFromCamera(const Vec2& location)
{
    Vec2 diffVectN {(location - camera.location).Norm()};
    double absAngleFromCameraDir {GeomUtils::AngleBetweenNorm(camera.dirN, diffVectN)};
    
    // we need to do the cross product in order to tell if this angle
    // is to the left of or the right of the camera's direction
    return (camera.dirN.cross(diffVectN) > 0 ? absAngleFromCameraDir : -absAngleFromCameraDir);
}

uint32_t BspRenderer::GetScreenXFromAngle(double angle)
{
    // (see notebook for the math)
    double opp {camera.viewPlaneDist * tan(angle)};
    double percentWidth {opp / (camera.viewPlaneWidth / 2.0f)};
    uint32_t screenX {static_cast<uint32_t>(percentWidth * static_cast<double>(screenWidth/2)) + screenWidth/2};
    
    return screenX;
}

// this is kind of a silly function, but it must exist when dealing with unsigned numbers...
int32_t BspRenderer::UnsignedSub(uint32_t n1, uint32_t n2)
{
    return (static_cast<int32_t>(n1) - static_cast<int32_t>(n2));
}

bool BspRenderer::ClipAndGetAttributes(bool leftSide, const Line& wallSeg, uint32_t& screenX, double& dist)
{
    bool pIsOnScreen {false};
    Vec2 p {(leftSide ? wallSeg.p1 : wallSeg.p2)};
    double angle {GetAngleFromCamera(p)};
    
    // TODO: is it accurate enough to use camera.leftmostVisibleAngle, vs. the angle
    // from a line drawn from camera.location through the "middle" of the first pixel on the
    // left side of the screen?
    
    // if point is in the field of view
    if (angle >= camera.leftmostVisibleAngle && angle <= camera.rightmostVisibleAngle)
    {
        pIsOnScreen = true;
        screenX = GetScreenXFromAngle(angle);
    }
    else
    {
        // perform clipping if necessary
        Line ray;
        if (leftSide) // clip to the left of the camera's field of view
        {
            angle = camera.leftmostVisibleAngle;
            ray = {camera.location, camera.leftmostViewPlaneEnd};
        }
        else // clip to the right of the camera's field of view
        {
            angle = camera.rightmostVisibleAngle;
            ray = {camera.location, camera.rightmostViewPlaneEnd};
        }
        
        double dummy;
        if (GeomUtils::FindRayLineSegIntersection(ray, wallSeg, p, dummy))
        {
            pIsOnScreen = true;
            screenX = (leftSide ? 0 : (screenWidth - 1));
        }
        // else no clipping was performed - the point is out of the field of
        // view and the wall does not lie in the path of the edges of the field
        // of view
    }
    
    if (pIsOnScreen)
        dist = GetPerpendicularDistanceFromCameraByAngle(p, angle);
    
    return pIsOnScreen;
}
