//
//  BspTree.cpp
//  walls3duino
//
//  Created by Brian Dolan on 5/9/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//
#include <stdlib.h>
#include <math.h>
#ifdef SDLSim
#include <new>
#else
#include <new.h>
#endif
#include "BspTree.hpp"
#include "GeomUtils.hpp"
#include "Serializer.hpp"

BspTree::BspNode::BspNode(const uint8_t* bytes, size_t& offset):
    wall{Line(bytes, offset)},
    backNodeIdx{NullNodeIdx},
    frontNodeIdx{NullNodeIdx}
{
}

// this renders *front to back* (closest walls first), and also performs backface culling
// (walls facing away from the camera are not rendered)
void BspTree::BspNode::TraverseRender(BspNode* nodes, const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr)
{
    // if the camera is in front of this node's wall
    if (GeomUtils::GeomUtils::IsPointInFrontOfLine(wall.seg, cameraLoc))
    {
        // render the nodes in front of this one, then this one, then the ones behind
        // (front to back)
        
        if (frontNodeIdx != NullNodeIdx) nodes[frontNodeIdx].TraverseRender(nodes, cameraLoc, renderFunc, ptr);
        renderFunc(wall, ptr);
        if (backNodeIdx != NullNodeIdx) nodes[backNodeIdx].TraverseRender(nodes, cameraLoc, renderFunc, ptr);
    }
    // ... or in back
    else
    {
        // don't render the current node's wall, since it is not facing the right direction,
        // but do potentially render its children
        // since this wall is facing away from the camera, we render the ones in back of it
        // (closer to the camera) before the ones in front of it (farther from the camera)

        if (backNodeIdx != NullNodeIdx) nodes[backNodeIdx].TraverseRender(nodes, cameraLoc, renderFunc, ptr);
        if (frontNodeIdx != NullNodeIdx) nodes[frontNodeIdx].TraverseRender(nodes, cameraLoc, renderFunc, ptr);
    }
    
    // TODO: handle if camera is exactly on this node's line
    // wikipedia: If that polygon lies in the plane containing P, add it to the *list of polygons* at node N.
    // (I currenly don't have this set up as a list...)
}

BspTree::BspTree():
    numNodes{0},
    rootNodeIdx{BspNode::NullNodeIdx}
{
    static_assert((Serializer::Fixed::Unfixed(SerNullNode) > 10000.0f) || (Serializer::Fixed::Unfixed(SerNullNode) < -10000.0f),
                  "aliasing problem with SerNullNode and a node's (double) coordinate");

    nodes = static_cast<BspNode*>(malloc(sizeof(BspNode) * MaxNodes));
}

BspTree::~BspTree()
{
    delete nodes;
}

void BspTree::LoadBin(const uint8_t* bytes)
{
    // important note:
    // in the original version of this function recursion was used such that each node,
    // upon creation, would parse its own subtree
    // because of memory limitations on the embedded hardware, this function now uses
    // a loop with a stack, rather than recursion

    size_t offset {0};
    NodeStack ns;
    bool done {false};

    ParseAndPush(bytes, offset, ns, rootNodeIdx);
    
    while (!done)
    {
        if (ns.Count() > 0)
        {
            NodeStack::NodeItem& ni = ns.Peek();

            if (ni.nodeIdx != BspNode::NullNodeIdx)
            {
                if (ni.numChildrenProcessed == 0)
                    ParseAndPush(bytes, offset, ns, nodes[ni.nodeIdx].backNodeIdx);
                else if (ni.numChildrenProcessed == 1)
                    ParseAndPush(bytes, offset, ns, nodes[ni.nodeIdx].frontNodeIdx);
                else if (ni.numChildrenProcessed == 2)
                    PopAndIncParent(ns);
            }
            else
            {
                PopAndIncParent(ns);
            }
        }
        else
        {
            done = true;
        }
    }
}

void BspTree::TraverseRender(const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr)
{
    if (rootNodeIdx != BspNode::NullNodeIdx)
        nodes[rootNodeIdx].TraverseRender(nodes, cameraLoc, renderFunc, ptr);
}

uint8_t BspTree::ParseNode(const uint8_t* bytes, size_t& offset)
{
    uint8_t nodeIdx {BspNode::NullNodeIdx};
    
    // "peek" at the data to see whether or not there is a node there
    int32_t identifier {Serializer::PeekInt(bytes, offset)};
    if (identifier != SerNullNode)
    {
        new(&nodes[numNodes]) BspNode(bytes, offset);

        nodeIdx = numNodes;
        numNodes++;
    }
    else
    {
        offset += 4; // make the "peek" official
    }

    return nodeIdx;
}

void BspTree::ParseAndPush(const uint8_t *bytes, size_t &offset, BspTree::NodeStack &ns, uint8_t& nodeIdx)
{
    nodeIdx = ParseNode(bytes, offset);
    ns.Push({nodeIdx, 0});
}

void BspTree::PopAndIncParent(NodeStack& ns)
{
    ns.Pop();
    if (ns.Count() > 0)
        ns.Peek().numChildrenProcessed++;
}

