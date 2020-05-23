//
//  BspTree.cpp
//  walls3duino
//
//  Created by Brian Dolan on 5/9/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//
#include <stdlib.h>
#include <math.h>
#include "BspTree.hpp"
#include "GeomUtils.hpp"
#include "Serializer.hpp"

BspTree::BspNodeDebugInfo::BspNodeDebugInfo(uint32_t nodeIndex):
    nodeIndex(nodeIndex)
{
}

BspTree::BspNode::BspNode(BspTree* pOwnerTree, const uint8_t* bytes, size_t& offset, uint32_t index):
    pOwnerTree{pOwnerTree},
    wall{Line(bytes, offset)},
    debugInfo{BspNodeDebugInfo(index)},
    pBackNode{pOwnerTree->ParseNode(bytes, offset)},
    pFrontNode{pOwnerTree->ParseNode(bytes, offset)}
{
}

BspTree::BspNode::~BspNode()
{
    if (pBackNode) delete pBackNode;
    if (pFrontNode) delete pFrontNode;
}

int32_t BspTree::BspNode::Find(const Vec2& p)
{
    if (GeomUtils::GeomUtils::IsPointInFrontOfLine(wall.seg, p))
        return (pFrontNode ? pFrontNode->Find(p) : debugInfo.nodeIndex);
    else
        return (pBackNode ? pBackNode->Find(p) : debugInfo.nodeIndex);
}

// this renders *front to back* (closest walls first), and also performs backface culling
// (walls facing away from the camera are not rendered)
void BspTree::BspNode::TraverseRender(const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr)
{
    // if the camera is in front of this node's wall
    if (GeomUtils::GeomUtils::IsPointInFrontOfLine(wall.seg, cameraLoc))
    {
        // render the nodes in front of this one, then this one, then the ones behind
        // (front to back)
        
        if (pFrontNode) pFrontNode->TraverseRender(cameraLoc, renderFunc, ptr);
        renderFunc(wall, debugInfo, ptr);
        if (pBackNode) pBackNode->TraverseRender(cameraLoc, renderFunc, ptr);
    }
    // ... or in back
    else
    {
        // don't render the current node's wall, since it is not facing the right direction,
        // but do potentially render its children
        // since this wall is facing away from the camera, we render the ones in back of it
        // (closer to the camera) before the ones in front of it (farther from the camera)

        if (pBackNode) pBackNode->TraverseRender(cameraLoc, renderFunc, ptr);
        if (pFrontNode) pFrontNode->TraverseRender(cameraLoc, renderFunc, ptr);
    }
    
    // TODO: handle if camera is exactly on this node's line
    // wikipedia: If that polygon lies in the plane containing P, add it to the *list of polygons* at node N.
    // (I currenly don't have this set up as a list...)
}

void BspTree::BspNode::TraverseDebug(BspTree::TraversalCbType debugFunc, void* ptr)
{
    if (pBackNode) pBackNode->TraverseDebug(debugFunc, ptr);
    debugFunc(wall, debugInfo, ptr);
    if (pFrontNode) pFrontNode->TraverseDebug(debugFunc, ptr);
}

BspTree::BspTree():
    numNodes{0},
    pRootNode(nullptr)
{
    static_assert((Serializer::Fixed::Unfixed(SerNullNode) > 10000.0f) || (Serializer::Fixed::Unfixed(SerNullNode) < -10000.0f),
                  "aliasing problem with SerNullNode and a node's (double) coordinate");
}

void BspTree::LoadBin(const uint8_t* bytes)
{
    size_t offset {0};
    pRootNode = ParseNode(bytes, offset);
}

int32_t BspTree::Find(const Vec2& p)
{
    return (pRootNode ? pRootNode->Find(p) : -1);
}

void BspTree::TraverseRender(const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr)
{
    if (pRootNode)
        pRootNode->TraverseRender(cameraLoc, renderFunc, ptr);
}

void BspTree::TraverseDebug(BspTree::TraversalCbType debugFunc, void* ptr)
{
    if (pRootNode)
        pRootNode->TraverseDebug(debugFunc, ptr);
}

BspTree::BspNode* BspTree::ParseNode(const uint8_t* bytes, size_t& offset)
{
    BspNode* pNode {nullptr};
    
    // "peek" at the data to see whether or not there is a node there
    int32_t identifier {Serializer::PeekInt(bytes, offset)};
    if (identifier != SerNullNode)
        pNode = new BspNode(this, bytes, offset, numNodes++);
    else
        offset += 4; // make the "peek" official
    
    return pNode;
}
