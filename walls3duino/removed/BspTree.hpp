//
//  BspTree.hpp
//  walls3d
//
//  Created by Brian Dolan on 5/9/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

// TODO:
// could even templatize on wall and point types (e.g. for expanding to real 3d)

#ifndef BspTree_hpp
#define BspTree_hpp

#include <stdint.h>
#include "Vector.hpp"
#include "Vec2.hpp"
#include "Wall.hpp"

class BspTree
{
public:
    class BspNodeDebugInfo
    {
    public:
        BspNodeDebugInfo(uint32_t nodeIndex);
        
        uint32_t nodeIndex;
    };

    typedef void (*TraversalCbType)(const Wall&, const BspNodeDebugInfo&, void* ptr);

private:
    class BspNode
    {
    public:
        BspNode(BspTree* pOwnerTree, const Wall& wall, const Vector<Wall>& surroundingWalls,
                const Vector<Line>& sectionBounds, uint32_t index);
        ~BspNode();
        
        uint32_t GetIndex() { return debugInfo.nodeIndex; }
        int32_t Find(const Vec2& p);
        void TraverseRender(const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr);
        void TraverseDebug(TraversalCbType debugFunc, void* ptr);

    private:
        Wall wall;
        BspNode* pBackNode {nullptr};
        BspNode* pFrontNode {nullptr};
        BspNodeDebugInfo debugInfo;
        
        BspTree* pOwnerTree;
    };

public:
    BspTree();
    ~BspTree() = default;

    void ProcessWalls(const Vector<Wall>& walls, const Vector<Line>& sectionBounds);

    int32_t Find(const Vec2& p);
    void TraverseRender(const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr);
    void TraverseDebug(TraversalCbType debugFunc, void* ptr);
    
private:
    // for "compiling" tree
    BspNode* CreateNode(const Vector<Wall>& walls, const Vector<Line>& sectionBounds);
    static void SplitWalls(const Line& splitterLine, const Vector<Wall>& walls, Vector<Wall>& backWalls, Vector<Wall>& frontWalls);
    static size_t FindBestSplitterWallIndex(const Vector<Wall>& walls);
    
    uint32_t numNodes;
    BspNode* pRootNode;
};

#endif /* BspTree_hpp */
