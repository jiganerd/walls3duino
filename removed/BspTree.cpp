//
//  BspTree.cpp
//  walls3d
//
//  Created by Brian Dolan on 5/9/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#include <stdlib.h>
#include <math.h>
#include "BspTree.hpp"
#include "GeomUtils.hpp"

BspTree::BspNodeDebugInfo::BspNodeDebugInfo(uint32_t nodeIndex):
    nodeIndex(nodeIndex)
{
}

BspTree::BspNode::BspNode(BspTree* pOwnerTree, const Wall& wall, const Vector<Wall>& surroundingWalls,
                          const Vector<Line>& sectionBounds, uint32_t index):
    wall{wall},
    pOwnerTree{pOwnerTree},
    debugInfo{BspNodeDebugInfo(index)}
{
    // append the current wall's line to the boundaries of the children nodes
    Vector<Line> sectionBoundsForChildren {sectionBounds};
    sectionBoundsForChildren.PushBack(wall.seg);
    
    // now do the split for realsies
    Vector<Wall> backWalls, frontWalls;
    BspTree::SplitWalls(wall.seg, surroundingWalls, backWalls, frontWalls);
    
    // now process any/all walls that are in back
    if (backWalls.Size() > 0)
        pBackNode = pOwnerTree->CreateNode(backWalls, sectionBoundsForChildren);
    
    // ... and any/all walls that are in front
    if (frontWalls.Size() > 0)
        pFrontNode = pOwnerTree->CreateNode(frontWalls, sectionBoundsForChildren);
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
}

void BspTree::ProcessWalls(const Vector<Wall>& walls, const Vector<Line>& sectionBounds)
{
    pRootNode = CreateNode(walls, sectionBounds);
}

BspTree::BspNode* BspTree::CreateNode(const Vector<Wall>& walls, const Vector<Line>& sectionBounds)
{
    size_t bestSplitterWallIndex {FindBestSplitterWallIndex(walls)};
    Vector<Wall> wallsWithoutSplitterWall {walls};
    wallsWithoutSplitterWall.Erase(bestSplitterWallIndex);
    
    return new BspNode(this, walls[bestSplitterWallIndex], wallsWithoutSplitterWall, sectionBounds, numNodes++);
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

void BspTree::SplitWalls(const Line& splitterLine, const Vector<Wall>& walls, Vector<Wall>& backWalls, Vector<Wall>& frontWalls)
{
    // loop through all walls in the list, other than the splitter wall
    for (int i = 0; i < walls.Size(); i++)
    {
        const Wall& wall = walls[i];
        
        // see if this wall is bisected by the line formed by this node's wall
        Vec2 intersection;
        double t, u;
        if (GeomUtils::FindLineLineSegIntersection(splitterLine, wall.seg, intersection, t, u) == true)
        {
            // split the wall at the intersection, and store the pieces
            
            // copy over most of the properties of the wall
            // into the two pieces
            Wall backWallPiece {wall};
            Wall frontWallPiece {wall};
            
            bool addBackWallPiece {false};
            bool addFrontWallPiece {false};
            
            // modify the pieces to reflect the intersection point
            // if we think of splitterLine as a "directional ray"
            // starting at splitterLine.p1, then the sign of t represents
            // whether the intersection happened in front of the ray or behind it
            // break up wall into backWallPiece and frontWallPiece accordingly
            //
            // if u == 0, this means the line drawn by the current node's wall
            // touches exactly at the starting end of this wall
            //
            // if u == 1, this means the line drawn by the current node's wall
            // touches exactly at the finishing end of this wall
            
            if (u > 0 && u < 1)
            {
                // the wall needs to be broken up into two
                addBackWallPiece = true;
                addFrontWallPiece = true;
                
                if (GeomUtils::IsPointInFrontOfLine(splitterLine, wall.seg.p1))
                {
                    frontWallPiece.seg.p2 = backWallPiece.seg.p1 = intersection;
                }
                else
                {
                    backWallPiece.seg.p2 = frontWallPiece.seg.p1 = intersection;
                }
            }
            else
            {
                // the ray intersects the wall exactly at the wall start
                if (u == 0)
                {
                    // the wall is entirely to one side of the ray
                    // do a test for where the *end* of the wall is
                    if (GeomUtils::IsPointInFrontOfLine(splitterLine, wall.seg.p2))
                        addFrontWallPiece = true;
                    else
                        addBackWallPiece = true;
                }
                // the ray intersects the wall exactly at the wall end
                else // u == 1
                {
                    // the wall is entirely to one side of the ray
                    // do a test for where the *start* of the wall is
                    if (GeomUtils::IsPointInFrontOfLine(splitterLine, wall.seg.p1))
                        addFrontWallPiece = true;
                    else
                        addBackWallPiece = true;
                }
            }
            
            if (addBackWallPiece)
                backWalls.PushBack(backWallPiece);
            
            if (addFrontWallPiece)
                frontWalls.PushBack(frontWallPiece);
        }
        else
        {
            // if there's no intersection, this wall must lie entirely
            // on one side of the line or the other
            if (GeomUtils::IsLineSegInFrontOfLine(splitterLine, wall.seg))
                frontWalls.PushBack(wall);
            else
                backWalls.PushBack(wall);
        }
    }
}

size_t BspTree::FindBestSplitterWallIndex(const Vector<Wall>& walls)
{
    Vector<Wall> wallsWithoutSplitterWall;
    Vector<Wall> backWalls, frontWalls;
    size_t bestSplitterWallIndex {0};
    uint32_t bestSplitterWallScore {static_cast<uint32_t>(-1)};
    
    for (size_t i = 0; i < walls.Size(); i++)
    {
        backWalls.Clear();
        frontWalls.Clear();
        
        wallsWithoutSplitterWall = walls;
        wallsWithoutSplitterWall.Erase(i);
        
        SplitWalls(walls[i].seg, wallsWithoutSplitterWall, backWalls, frontWalls);
        
        // see 1) how balanced the back/front sides are (good), and 2) how many
        // wall splits occurred (bad)
        // a lower "score" is better
        // we penalize much more harshly for number of splits than we reward for how balanced the tree is - this
        // ratio is something that is done "by feel"
        uint32_t numberOfSplits {static_cast<uint32_t>((backWalls.Size() + frontWalls.Size()) - wallsWithoutSplitterWall.Size()) / 2};
        uint32_t score {abs(static_cast<int32_t>(backWalls.Size()) - static_cast<int32_t>(frontWalls.Size())) + numberOfSplits * 8};
        
        if (score < bestSplitterWallScore)
        {
            bestSplitterWallScore = score;
            bestSplitterWallIndex = i;
        }
    }
    
    return bestSplitterWallIndex;
}
