//
//  BspTree.hpp
//  walls3duino
//
//  Created by Brian Dolan on 5/9/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

// TODO:
// could even templatize on wall and point types (e.g. for expanding to real 3d)

#ifndef BspTree_hpp
#define BspTree_hpp

#include <stdint.h>
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
        BspNode(BspTree* pOwnerTree, const uint8_t* bytes, size_t& offset, uint32_t index);
        ~BspNode();
        
        uint32_t GetIndex() { return debugInfo.nodeIndex; }
        int32_t Find(const Vec2& p);
        void TraverseRender(const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr);
        void TraverseDebug(TraversalCbType debugFunc, void* ptr);

    public:
        BspTree* pOwnerTree;
        Wall wall;
        BspNodeDebugInfo debugInfo;
        BspNode* pBackNode;
        BspNode* pFrontNode;
    };

public:
    BspTree();
    ~BspTree() = default;

    void LoadBin(const uint8_t* bytes);

    int32_t Find(const Vec2& p);
    void TraverseRender(const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr);
    void TraverseDebug(TraversalCbType debugFunc, void* ptr);
    
private:
    BspNode* ParseNode(const uint8_t* bytes, size_t& offset);
    
    uint32_t numNodes;
    BspNode* pRootNode;
        
    // this value represents a "null node" in the serialized data, and is chosen to
    // not conflict with any reasonable value for what is otherwise a fixed-point
    // representation of an x coordinate in the case of a "real" node
    static constexpr int32_t SerNullNode {static_cast<int32_t>(0x7FFFFFFF)};
};

#endif /* BspTree_hpp */
