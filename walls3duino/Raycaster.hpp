//
//  Raycaster.hpp
//  walls3d
//
//  Created by Brian Dolan on 5/15/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Raycaster_hpp
#define Raycaster_hpp

#include "Renderer.hpp"

class Raycaster : public Renderer
{
public:
    Raycaster(uint8_t* pPixelBuf,
              uint32_t screenWidth,
              uint32_t screenHeight,
              const Camera& camera,
              const Wall walls[],
              size_t numWalls,
              const Line worldBounds[],
              size_t numWorldBounds);
    ~Raycaster() = default;

    void RenderScene() override;

private:
    double GetPerpendicularDistanceFromCamera(const Vec2& point, double percentWidth);
    
    const Wall* walls;
    size_t numWalls;
};

#endif /* Raycaster_hpp */
