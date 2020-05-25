//
//  BspRenderer.hpp
//  walls3duino
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
                uint8_t screenWidth,
                uint8_t screenHeight,
                ColRenderedCbType colRenderedCb,
                const Camera& camera);
    ~BspRenderer();

    void LoadBin(const uint8_t* bytes);
    
    void RenderScene() override;
    
private:
    static bool RenderWallStatic(const Wall& wall, void* bspRenderer);
    bool RenderWall(const Wall& wall);
    double GetPerpendicularDistanceFromCameraByAngle(const Vec2& point, double angleFromCamera);
    double GetAngleFromCamera(const Vec2& location);
    uint8_t GetScreenXFromAngle(double angle);
    int32_t UnsignedSub(uint32_t n1, uint32_t n2);
    bool ClipAndGetAttributes(bool leftSide, const Line& wallSeg, uint8_t& screenX, double& dist);

    BspTree bspTree;

    // a buffer of wall heights, per screen column, to be drawn
    // used for adapting rendering of walls in random-ish order into a left-to-right sequence
    // for pushing out to the display
    // (a value of 0 means either there is no wall intersection there, or it just hasn't been rendered yet)
    uint8_t* pHeightBuffer;
};

#endif /* BspRenderer_hpp */
