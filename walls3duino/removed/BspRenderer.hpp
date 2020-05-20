//
//  BspRenderer.hpp
//  walls3d
//
//  Created by Brian Dolan on 5/15/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef BspRenderer_hpp
#define BspRenderer_hpp

#include "Renderer.hpp"
#include "BspTree.hpp"

class BspRenderer : public Renderer
{
public:
    BspRenderer(uint8_t* pPixelBuf,
                uint32_t screenWidth,
                uint32_t screenHeight,
                const Camera& camera,
                const Line worldBounds[],
                size_t numWorldBounds);
    ~BspRenderer() = default;

    void ProcessWalls(const Wall walls[], size_t numWalls);
    
    void RenderScene() override;
    
private:
    static void RenderWallStatic(const Wall& wall, const BspTree::BspNodeDebugInfo& debugInfo, void* bspRenderer);
    void RenderWall(const Wall& wall, const BspTree::BspNodeDebugInfo& debugInfo);
    double GetPerpendicularDistanceFromCameraByAngle(const Vec2& point, double angleFromCamera);
    double GetAngleFromCamera(const Vec2& location);
    uint32_t GetScreenXFromAngle(double angle);
    int32_t UnsignedSub(uint32_t n1, uint32_t n2);
    bool ClipAndGetAttributes(bool leftSide, const Line& wallSeg, uint32_t& screenX, double& dist);

    BspTree bspTree;
    int32_t cameraNodeIndex;

    BspRenderer* bspRenderer;
};

#endif /* BspRenderer_hpp */
