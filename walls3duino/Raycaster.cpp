//
//  Raycaster.cpp
//  walls3d
//
//  Created by Brian Dolan on 5/15/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#include "Raycaster.hpp"
#include "GeomUtils.hpp"

Raycaster::Raycaster(uint8_t* pPixelBuf,
                     uint32_t screenWidth,
                     uint32_t screenHeight,
                     const Camera& camera,
                     const Wall walls[],
                     size_t numWalls,
                     const Line worldBounds[],
                     size_t numWorldBounds):
    Renderer(pPixelBuf, screenWidth, screenHeight, camera, worldBounds, numWorldBounds),
    walls{walls},
    numWalls{numWalls}
{
}

void Raycaster::RenderScene()
{
    BeginRender();
    
    // do this once, as it is actually a pretty costly call
    static constexpr double infinity {99999999999.9f};

    // -1 to 1 as we draw across the screen
    double percentWidth {-1.0f};
    const double percentWidthIncrement {2.0f / screenWidth};
    
    // loop through columns on the screen
    for (uint32_t column {0}; column < screenWidth; column++)
    {
        // for this current column, figure out the corresponding point
        // on the view plane
        // TODO: optimize performance - don't have to do multiplication here every pass through loop
        Vec2 vectorToRayPoint {camera.halfViewPlane * percentWidth};
        Vec2 viewPlanePoint {camera.viewPlaneMiddle + vectorToRayPoint};

        Line ray {camera.location, viewPlanePoint};
        
        // find intersections with all walls, and find the closest wall
        const Wall* closestWallPtr {nullptr};
        double closestWallDistance {infinity};
        Vec2 closestWallIntersection;

        for (size_t i {0}; i < numWalls; i++)
        {
            const Wall& wall = walls[i];
            
            Vec2 intersection;
            double u;
            if (GeomUtils::FindRayLineSegIntersection(ray, wall.seg, intersection, u))
            {
                double distanceToIntersection {GetPerpendicularDistanceFromCamera(intersection, percentWidth)};
                
                // draw this wall intersection only if it is closer than any other
                // wall intersections drawn so far
                // (prevent overdrawing)
                // (this method works because all walls are the same height, and
                // closer walls will always obscure farther walls entirely)
                if (distanceToIntersection < closestWallDistance)
                {
                    // save off information for this particular (closest) wall intersection
                    closestWallDistance = distanceToIntersection;
                    closestWallPtr = &wall;
                    closestWallIntersection = intersection;
                }
            }
        }
        
        // draw the closest wall
        if (closestWallPtr)
            RenderColumn(column,
                         GetColumnHeightByDistance(closestWallDistance));
        // (this allows us to not have to spend time clearing the screen on every frame,
        // and have dithering be consistent from frame to frame)
        else
            RenderColumn(column, 0.0f);
        
        percentWidth += percentWidthIncrement;
    }
    
    EndRender();
}

double Raycaster::GetPerpendicularDistanceFromCamera(const Vec2& point, double percentWidth)
{
    // see my notebook notes for the math here
    // this "perpendicular distance" equation prevents the fisheye effect that
    // would be seen with the Euclidean (real) distance
    
    Vec2 cameraToIntersection {point - camera.location};
    double opp {percentWidth * camera.viewPlaneWidth / 2.0f};
    double adj {camera.viewPlaneDist};
    double ratio {opp / adj};
    return sqrt((cameraToIntersection.x * cameraToIntersection.x + cameraToIntersection.y * cameraToIntersection.y) / (1 + ratio * ratio));
}
